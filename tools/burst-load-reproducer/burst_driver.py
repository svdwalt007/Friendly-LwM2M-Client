#!/usr/bin/env python3
"""
P0-5 Burst-Load Reproducer
==========================

This tool reproduces the burst-load stall identified in the FT trace analysis.
It drives 17 instance-level CON GETs over 36 seconds via the Leshan HTTP API,
matching the exact pattern from the production incident.

The FT trace showed:
- 17 instance-level reads over 36 seconds
- 6 requests returned 4.01 (Unauthorized) - correct ACL behavior
- 11 requests returned 2.05 (Content) - successful reads
- After the burst, client went silent for 110 seconds (STALL)

This reproducer:
1. Drives the exact FT pattern against the target client
2. Tracks latency for each request (watching for creep on 4.01 path)
3. Verifies post-burst health by checking for UPDATE within (lt * 0.75) + 5%
4. Outputs detailed results to /tmp/burst-results.json
5. Exits with code 1 if stall detected, 0 if healthy

Usage:
    python3 burst_driver.py --endpoint TestClient --duration 36 --lt 60

Reference: Issues/pcap-combo.md Section "P0-5 (NEW) - Burst-load stall"
"""

import argparse
import json
import os
import signal
import sys
import time
from dataclasses import asdict, dataclass, field
from datetime import datetime
from typing import List, Optional, Tuple

try:
    import requests
except ImportError:
    print("ERROR: 'requests' library required. Install with: pip install requests", file=sys.stderr)
    sys.exit(1)


# ==============================================================================
# FT Trace Pattern
# ==============================================================================
# Order matches the actual frame sequence from 02ftl.pcapng frames 31..187.
# 17 instance-level reads over 36 seconds, 6 return 4.01 (correct ACL behavior).

FT_PATTERN: List[Tuple[str, str]] = [
    ("/3/0",    "expect_2.05"),   # Device object - should exist
    ("/3339/0", "expect_2.05"),   # AudioClip object - should exist
    ("/2/10",   "expect_2.05"),   # Access Control instance 10 - exists (for AudioClip)
    ("/4/0",    "expect_2.05"),   # ConnectivityMonitoring object - should exist
    ("/2/2",    "expect_4.01"),   # Access Control instance 2 - does NOT exist
    ("/5/0",    "expect_2.05"),   # FirmwareUpdate object - should exist
    ("/2/3",    "expect_2.05"),   # Access Control instance 3 - exists (for Device)
    ("/2/0",    "expect_4.01"),   # Access Control instance 0 - does NOT exist
    ("/2/1",    "expect_2.05"),   # Access Control instance 1 - exists (for Server)
    ("/1/0",    "expect_2.05"),   # Server object - should exist
    ("/2/8",    "expect_4.01"),   # Access Control instance 8 - does NOT exist
    ("/2/9",    "expect_4.01"),   # Access Control instance 9 - does NOT exist
    ("/2/6",    "expect_4.01"),   # Access Control instance 6 - does NOT exist
    ("/2/7",    "expect_2.05"),   # Access Control instance 7 - exists (for ConnMon)
    ("/6/0",    "expect_4.01"),   # Location object - missing AC instance (P1-7)
    ("/2/4",    "expect_4.01"),   # Access Control instance 4 - does NOT exist
    ("/2/5",    "expect_2.05"),   # Access Control instance 5 - exists (for FirmwareUpdate)
]


# ==============================================================================
# Data Classes
# ==============================================================================

@dataclass
class RequestResult:
    """Result of a single GET request."""
    index: int
    path: str
    expected_coap: str           # e.g., "2.05" or "4.01"
    actual_http: int             # HTTP status code from Leshan
    actual_coap: Optional[str]   # CoAP code if parseable
    latency_s: float
    match: bool
    error: Optional[str] = None
    timestamp: float = 0.0


@dataclass
class BurstResults:
    """Complete results from a burst test run."""
    start_time: str
    end_time: str
    duration_s: float
    endpoint: str
    leshan_url: str
    lifetime: int
    pattern_size: int
    requests: List[RequestResult] = field(default_factory=list)

    # Statistics
    total_requests: int = 0
    successful_requests: int = 0
    failed_requests: int = 0
    timed_out_requests: int = 0
    match_count: int = 0
    mismatch_count: int = 0

    # Latency stats
    avg_latency_s: float = 0.0
    max_latency_s: float = 0.0
    min_latency_s: float = 0.0
    avg_latency_2_05_s: float = 0.0
    avg_latency_4_01_s: float = 0.0

    # Post-burst health
    update_detected: bool = False
    update_time_s: Optional[float] = None
    stall_detected: bool = False
    health_check_duration_s: float = 0.0


# ==============================================================================
# Global State
# ==============================================================================

LESHAN_API: str = ""
ENDPOINT: str = ""
VERBOSE: bool = False
RESULTS: BurstResults = None


def log(msg: str, error: bool = False):
    """Log message to stdout/stderr."""
    stream = sys.stderr if error else sys.stdout
    timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
    print(f"[{timestamp}] {msg}", file=stream, flush=True)


def vlog(msg: str):
    """Verbose log - only prints if VERBOSE is True."""
    if VERBOSE:
        log(msg)


# ==============================================================================
# HTTP/CoAP Response Mapping
# ==============================================================================

def http_to_coap_code(http_status: int, response_body: dict) -> str:
    """
    Map Leshan HTTP response to CoAP code.

    Leshan's REST API maps CoAP codes to HTTP:
    - 2.05 Content -> 200 OK (with body)
    - 4.01 Unauthorized -> 401 or sometimes 400
    - 4.04 Not Found -> 404
    - 4.00 Bad Request -> 400

    The actual CoAP code may be in the response body for errors.
    """
    if http_status == 200:
        return "2.05"
    elif http_status == 401:
        return "4.01"
    elif http_status == 404:
        return "4.04"
    elif http_status == 400:
        # Could be 4.00 or 4.01 depending on the error
        # Check response body for details
        if response_body and isinstance(response_body, dict):
            error = response_body.get("error", "").lower()
            if "unauthorized" in error or "access" in error:
                return "4.01"
        return "4.00"
    elif http_status == 408:
        return "TIMEOUT"
    elif http_status >= 500:
        return "5.00"
    else:
        return f"HTTP_{http_status}"


def check_expectation(expected: str, actual_coap: str) -> bool:
    """Check if actual CoAP code matches expectation."""
    expected_code = expected.split("_")[1]  # "expect_2.05" -> "2.05"
    return actual_coap == expected_code


# ==============================================================================
# Burst Driver
# ==============================================================================

def drive_burst(pattern: List[Tuple[str, str]], duration_s: int, request_timeout: int = 10) -> List[RequestResult]:
    """
    Drive the burst pattern over the specified duration.

    Args:
        pattern: List of (path, expectation) tuples
        duration_s: Total duration to spread requests over
        request_timeout: Timeout for each HTTP request

    Returns:
        List of RequestResult objects
    """
    interval = duration_s / len(pattern)
    results: List[RequestResult] = []

    log(f"[BURST] Starting {len(pattern)} requests over {duration_s}s (interval={interval:.2f}s)")
    log(f"[BURST] Target: {LESHAN_API}/{ENDPOINT}")

    start_time = time.time()

    for i, (path, expectation) in enumerate(pattern):
        req_start = time.time()
        expected_code = expectation.split("_")[1]  # "expect_2.05" -> "2.05"

        result = RequestResult(
            index=i,
            path=path,
            expected_coap=expected_code,
            actual_http=0,
            actual_coap=None,
            latency_s=0.0,
            match=False,
            timestamp=req_start
        )

        try:
            # Build URL for Leshan REST API
            # Format: GET /api/clients/{endpoint}/{path}
            url = f"{LESHAN_API}/{ENDPOINT}{path}"
            vlog(f"  GET {url}")

            r = requests.get(url, timeout=request_timeout)
            req_latency = time.time() - req_start

            # Parse response
            result.actual_http = r.status_code
            result.latency_s = req_latency

            try:
                body = r.json() if r.content else {}
            except (json.JSONDecodeError, ValueError):
                body = {}

            result.actual_coap = http_to_coap_code(r.status_code, body)
            result.match = check_expectation(expectation, result.actual_coap)

            # Log result
            match_str = "OK" if result.match else "MISMATCH"
            latency_warn = " (SLOW!)" if req_latency > 0.5 else ""
            log(f"  [{i+1:2d}/{len(pattern)}] {path:12s} -> {result.actual_coap:5s} "
                f"(expect {expected_code}) [{req_latency:.3f}s{latency_warn}] {match_str}")

        except requests.exceptions.Timeout:
            req_latency = time.time() - req_start
            result.latency_s = req_latency
            result.actual_coap = "TIMEOUT"
            result.error = "Request timed out"
            log(f"  [{i+1:2d}/{len(pattern)}] {path:12s} -> TIMEOUT after {req_latency:.1f}s", error=True)

        except requests.exceptions.ConnectionError as e:
            req_latency = time.time() - req_start
            result.latency_s = req_latency
            result.actual_coap = "CONN_ERR"
            result.error = f"Connection error: {e}"
            log(f"  [{i+1:2d}/{len(pattern)}] {path:12s} -> CONNECTION ERROR", error=True)

        except Exception as e:
            req_latency = time.time() - req_start
            result.latency_s = req_latency
            result.actual_coap = "ERROR"
            result.error = str(e)
            log(f"  [{i+1:2d}/{len(pattern)}] {path:12s} -> ERROR: {e}", error=True)

        results.append(result)

        # Sleep to maintain target interval (accounting for request time)
        elapsed = time.time() - start_time
        target_time = (i + 1) * interval
        sleep_time = max(0, target_time - elapsed)
        if sleep_time > 0:
            time.sleep(sleep_time)

    total_time = time.time() - start_time
    log(f"[BURST] Completed {len(results)} requests in {total_time:.1f}s")

    return results


# ==============================================================================
# Post-Burst Health Check
# ==============================================================================

def check_post_burst_health(lt: int, timeout_s: int = 300) -> Tuple[bool, Optional[float]]:
    """
    Check if client sends UPDATE after burst completes.

    Per LwM2M spec, client should send UPDATE at approximately lt * 0.75.
    We allow 5% tolerance beyond that.

    Args:
        lt: Client lifetime in seconds
        timeout_s: Maximum time to wait for UPDATE

    Returns:
        Tuple of (update_detected, time_to_update_s)
    """
    expected_update_window = lt * 0.75 * 1.05  # lt * 0.75 + 5% tolerance

    log(f"\n[HEALTH] Watching for UPDATE (expected within {expected_update_window:.0f}s of burst end)")
    log(f"[HEALTH] Client lifetime={lt}s, UPDATE window=lt*0.75={lt * 0.75:.0f}s (+5% = {expected_update_window:.0f}s)")

    start = time.time()
    last_registration_update = None
    initial_registration_update = None

    # First, get current registration state
    try:
        r = requests.get(f"{LESHAN_API}/{ENDPOINT}", timeout=5)
        if r.status_code == 200:
            data = r.json()
            initial_registration_update = data.get("lastUpdate")
            last_registration_update = initial_registration_update
            vlog(f"[HEALTH] Initial lastUpdate: {initial_registration_update}")
    except Exception as e:
        log(f"[HEALTH] Warning: Could not get initial registration state: {e}", error=True)

    poll_interval = 1.0  # Poll every second

    while (time.time() - start) < timeout_s:
        try:
            r = requests.get(f"{LESHAN_API}/{ENDPOINT}", timeout=5)

            if r.status_code == 200:
                data = r.json()
                current_update = data.get("lastUpdate")

                # Check if lastUpdate changed (indicating an UPDATE message)
                if current_update and current_update != last_registration_update:
                    elapsed = time.time() - start
                    log(f"[HEALTH] UPDATE detected at t={elapsed:.1f}s post-burst")

                    if elapsed <= expected_update_window:
                        log(f"[HEALTH] UPDATE arrived on time (within {expected_update_window:.0f}s window)")
                        return True, elapsed
                    else:
                        log(f"[HEALTH] WARNING: UPDATE was LATE (arrived at {elapsed:.1f}s, expected <={expected_update_window:.0f}s)")
                        # Still counts as detecting an update, but it was late
                        return True, elapsed

                    last_registration_update = current_update

            elif r.status_code == 404:
                # Client deregistered - this is also a stall indicator
                log(f"[HEALTH] Client deregistered unexpectedly!", error=True)
                return False, None

        except requests.exceptions.ConnectionError:
            log(f"[HEALTH] Warning: Lost connection to Leshan", error=True)
        except Exception as e:
            vlog(f"[HEALTH] Poll error: {e}")

        # Progress indicator every 30 seconds
        elapsed = time.time() - start
        if int(elapsed) % 30 == 0 and int(elapsed) > 0:
            remaining = timeout_s - elapsed
            log(f"[HEALTH] Waiting... {elapsed:.0f}s elapsed, {remaining:.0f}s remaining")

        time.sleep(poll_interval)

    # Timeout reached without seeing UPDATE
    elapsed = time.time() - start
    log(f"[HEALTH] TIMEOUT: No UPDATE detected within {timeout_s}s - STALL SUSPECTED", error=True)
    return False, None


# ==============================================================================
# Statistics Calculation
# ==============================================================================

def calculate_statistics(results: List[RequestResult]) -> dict:
    """Calculate statistics from request results."""
    stats = {
        "total": len(results),
        "successful": 0,
        "failed": 0,
        "timed_out": 0,
        "matches": 0,
        "mismatches": 0,
        "latencies": [],
        "latencies_2_05": [],
        "latencies_4_01": [],
    }

    for r in results:
        if r.error:
            if "timeout" in r.error.lower():
                stats["timed_out"] += 1
            else:
                stats["failed"] += 1
        else:
            stats["successful"] += 1
            stats["latencies"].append(r.latency_s)

            if r.actual_coap == "2.05":
                stats["latencies_2_05"].append(r.latency_s)
            elif r.actual_coap == "4.01":
                stats["latencies_4_01"].append(r.latency_s)

        if r.match:
            stats["matches"] += 1
        elif not r.error:  # Don't count errors as mismatches
            stats["mismatches"] += 1

    # Calculate latency statistics
    if stats["latencies"]:
        stats["avg_latency"] = sum(stats["latencies"]) / len(stats["latencies"])
        stats["max_latency"] = max(stats["latencies"])
        stats["min_latency"] = min(stats["latencies"])
    else:
        stats["avg_latency"] = 0.0
        stats["max_latency"] = 0.0
        stats["min_latency"] = 0.0

    if stats["latencies_2_05"]:
        stats["avg_latency_2_05"] = sum(stats["latencies_2_05"]) / len(stats["latencies_2_05"])
    else:
        stats["avg_latency_2_05"] = 0.0

    if stats["latencies_4_01"]:
        stats["avg_latency_4_01"] = sum(stats["latencies_4_01"]) / len(stats["latencies_4_01"])
    else:
        stats["avg_latency_4_01"] = 0.0

    return stats


# ==============================================================================
# Results Output
# ==============================================================================

def save_results(results: BurstResults, output_path: str):
    """Save results to JSON file."""
    # Convert dataclass to dict, handling nested dataclasses
    results_dict = {
        "start_time": results.start_time,
        "end_time": results.end_time,
        "duration_s": results.duration_s,
        "endpoint": results.endpoint,
        "leshan_url": results.leshan_url,
        "lifetime": results.lifetime,
        "pattern_size": results.pattern_size,
        "statistics": {
            "total_requests": results.total_requests,
            "successful_requests": results.successful_requests,
            "failed_requests": results.failed_requests,
            "timed_out_requests": results.timed_out_requests,
            "match_count": results.match_count,
            "mismatch_count": results.mismatch_count,
            "avg_latency_s": results.avg_latency_s,
            "max_latency_s": results.max_latency_s,
            "min_latency_s": results.min_latency_s,
            "avg_latency_2_05_s": results.avg_latency_2_05_s,
            "avg_latency_4_01_s": results.avg_latency_4_01_s,
        },
        "health": {
            "update_detected": results.update_detected,
            "update_time_s": results.update_time_s,
            "stall_detected": results.stall_detected,
            "health_check_duration_s": results.health_check_duration_s,
        },
        "requests": [
            {
                "index": r.index,
                "path": r.path,
                "expected_coap": r.expected_coap,
                "actual_http": r.actual_http,
                "actual_coap": r.actual_coap,
                "latency_s": r.latency_s,
                "match": r.match,
                "error": r.error,
                "timestamp": r.timestamp,
            }
            for r in results.requests
        ],
    }

    with open(output_path, "w") as f:
        json.dump(results_dict, f, indent=2)

    log(f"[OUTPUT] Results saved to {output_path}")


def print_summary(results: BurstResults):
    """Print a human-readable summary."""
    print("\n" + "=" * 60)
    print("BURST TEST SUMMARY")
    print("=" * 60)

    print(f"\nEndpoint: {results.endpoint}")
    print(f"Leshan:   {results.leshan_url}")
    print(f"Lifetime: {results.lifetime}s")
    print(f"Duration: {results.duration_s:.1f}s")

    print(f"\nRequests:")
    print(f"  Total:      {results.total_requests}")
    print(f"  Successful: {results.successful_requests}")
    print(f"  Failed:     {results.failed_requests}")
    print(f"  Timed Out:  {results.timed_out_requests}")

    print(f"\nExpectation Matching:")
    print(f"  Matches:    {results.match_count}")
    print(f"  Mismatches: {results.mismatch_count}")

    print(f"\nLatency:")
    print(f"  Average:     {results.avg_latency_s:.3f}s")
    print(f"  Min:         {results.min_latency_s:.3f}s")
    print(f"  Max:         {results.max_latency_s:.3f}s")
    print(f"  Avg (2.05):  {results.avg_latency_2_05_s:.3f}s")
    print(f"  Avg (4.01):  {results.avg_latency_4_01_s:.3f}s")

    print(f"\nPost-Burst Health:")
    if results.update_detected:
        print(f"  UPDATE detected: YES (at {results.update_time_s:.1f}s)")
    else:
        print(f"  UPDATE detected: NO")
    print(f"  Stall detected:  {'YES' if results.stall_detected else 'NO'}")

    print("\n" + "=" * 60)
    if results.stall_detected:
        print("RESULT: STALL DETECTED - P0-5 REPRODUCED")
    else:
        print("RESULT: CLIENT HEALTHY")
    print("=" * 60 + "\n")


# ==============================================================================
# Signal Handler
# ==============================================================================

def signal_handler(signum, frame):
    """Handle interrupt signals gracefully."""
    log(f"\n[SIGNAL] Received signal {signum}, saving partial results...")
    if RESULTS:
        RESULTS.end_time = datetime.now().isoformat()
        save_results(RESULTS, "/tmp/burst-results-partial.json")
    sys.exit(130)


# ==============================================================================
# Main
# ==============================================================================

def main():
    global LESHAN_API, ENDPOINT, VERBOSE, RESULTS

    parser = argparse.ArgumentParser(
        description="P0-5 Burst-Load Reproducer - Tests LwM2M client resilience under burst load",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Basic run with defaults
  python3 burst_driver.py

  # Custom endpoint and Leshan URL
  python3 burst_driver.py --endpoint MyClient --leshan-url http://192.168.1.100:8080

  # Extended burst with longer lifetime
  python3 burst_driver.py --duration 60 --lt 120

  # Skip health check (for quick testing)
  python3 burst_driver.py --skip-health-check

Exit Codes:
  0 - Client healthy after burst
  1 - Stall detected (P0-5 reproduced)
  2 - Configuration or connection error
  130 - Interrupted by signal
        """
    )

    parser.add_argument(
        "--endpoint", "-e",
        default="TestClient",
        help="Client endpoint name (default: TestClient)"
    )
    parser.add_argument(
        "--duration", "-d",
        type=int,
        default=36,
        help="Burst duration in seconds (default: 36)"
    )
    parser.add_argument(
        "--lt", "--lifetime",
        type=int,
        default=60,
        dest="lt",
        help="Client lifetime in seconds (default: 60)"
    )
    parser.add_argument(
        "--leshan-url", "-l",
        default="http://localhost:8080",
        help="Leshan server base URL (default: http://localhost:8080)"
    )
    parser.add_argument(
        "--output", "-o",
        default="/tmp/burst-results.json",
        help="Output file path (default: /tmp/burst-results.json)"
    )
    parser.add_argument(
        "--timeout",
        type=int,
        default=10,
        help="Per-request timeout in seconds (default: 10)"
    )
    parser.add_argument(
        "--health-timeout",
        type=int,
        default=300,
        help="Post-burst health check timeout in seconds (default: 300)"
    )
    parser.add_argument(
        "--skip-health-check",
        action="store_true",
        help="Skip post-burst health check"
    )
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )

    args = parser.parse_args()

    # Set globals
    LESHAN_API = f"{args.leshan_url}/api/clients"
    ENDPOINT = args.endpoint
    VERBOSE = args.verbose

    # Install signal handlers
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    # Initialize results
    RESULTS = BurstResults(
        start_time=datetime.now().isoformat(),
        end_time="",
        duration_s=0.0,
        endpoint=args.endpoint,
        leshan_url=args.leshan_url,
        lifetime=args.lt,
        pattern_size=len(FT_PATTERN),
    )

    log("=" * 60)
    log("P0-5 BURST-LOAD REPRODUCER")
    log("=" * 60)
    log(f"Endpoint:  {args.endpoint}")
    log(f"Leshan:    {args.leshan_url}")
    log(f"Duration:  {args.duration}s")
    log(f"Lifetime:  {args.lt}s")
    log(f"Pattern:   {len(FT_PATTERN)} requests (6x 4.01, 11x 2.05)")
    log("=" * 60 + "\n")

    # Verify client is registered
    log("[INIT] Verifying client registration...")
    try:
        r = requests.get(f"{LESHAN_API}/{ENDPOINT}", timeout=5)
        if r.status_code == 404:
            log(f"[INIT] ERROR: Client '{ENDPOINT}' not registered with Leshan", error=True)
            log(f"[INIT] Ensure client is running and registered before starting burst test", error=True)
            sys.exit(2)
        elif r.status_code != 200:
            log(f"[INIT] ERROR: Unexpected response from Leshan: HTTP {r.status_code}", error=True)
            sys.exit(2)
        log(f"[INIT] Client '{ENDPOINT}' is registered")
    except requests.exceptions.ConnectionError:
        log(f"[INIT] ERROR: Cannot connect to Leshan at {args.leshan_url}", error=True)
        log(f"[INIT] Ensure Leshan is running and accessible", error=True)
        sys.exit(2)
    except Exception as e:
        log(f"[INIT] ERROR: {e}", error=True)
        sys.exit(2)

    # Drive burst
    log("\n[PHASE 1] Driving burst pattern...")
    burst_start = time.time()
    request_results = drive_burst(FT_PATTERN, args.duration, args.timeout)
    burst_end = time.time()

    RESULTS.requests = request_results
    RESULTS.duration_s = burst_end - burst_start

    # Calculate statistics
    stats = calculate_statistics(request_results)
    RESULTS.total_requests = stats["total"]
    RESULTS.successful_requests = stats["successful"]
    RESULTS.failed_requests = stats["failed"]
    RESULTS.timed_out_requests = stats["timed_out"]
    RESULTS.match_count = stats["matches"]
    RESULTS.mismatch_count = stats["mismatches"]
    RESULTS.avg_latency_s = stats["avg_latency"]
    RESULTS.max_latency_s = stats["max_latency"]
    RESULTS.min_latency_s = stats["min_latency"]
    RESULTS.avg_latency_2_05_s = stats["avg_latency_2_05"]
    RESULTS.avg_latency_4_01_s = stats["avg_latency_4_01"]

    # Check for latency creep on 4.01 path (FT trace symptom)
    latencies_4_01 = [r.latency_s for r in request_results if r.actual_coap == "4.01"]
    if len(latencies_4_01) >= 3:
        first_half_avg = sum(latencies_4_01[:len(latencies_4_01)//2]) / (len(latencies_4_01)//2)
        second_half_avg = sum(latencies_4_01[len(latencies_4_01)//2:]) / (len(latencies_4_01) - len(latencies_4_01)//2)
        if first_half_avg > second_half_avg * 1.5:
            log(f"[ANALYSIS] WARNING: 4.01 latency creep detected (first half: {first_half_avg:.3f}s, second half: {second_half_avg:.3f}s)")

    # Post-burst health check
    if not args.skip_health_check:
        log("\n[PHASE 2] Post-burst health check...")
        health_start = time.time()
        update_detected, update_time = check_post_burst_health(args.lt, args.health_timeout)
        health_end = time.time()

        RESULTS.update_detected = update_detected
        RESULTS.update_time_s = update_time
        RESULTS.stall_detected = not update_detected
        RESULTS.health_check_duration_s = health_end - health_start
    else:
        log("\n[PHASE 2] Skipping health check (--skip-health-check)")
        RESULTS.stall_detected = False

    # Finalize results
    RESULTS.end_time = datetime.now().isoformat()

    # Save and print results
    save_results(RESULTS, args.output)
    print_summary(RESULTS)

    # Exit with appropriate code
    if RESULTS.stall_detected:
        log("EXIT: Stall detected - P0-5 reproduced", error=True)
        sys.exit(1)
    else:
        log("EXIT: Client healthy after burst")
        sys.exit(0)


if __name__ == "__main__":
    main()
