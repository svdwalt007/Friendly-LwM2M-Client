# P0-5 Burst-Load Reproducer

A test harness for reproducing and diagnosing the burst-load stall issue (P0-5) identified in the FT trace analysis.

## Background

The FT trace showed a critical issue: after receiving 17 instance-level CON GET requests over 36 seconds, the LwM2M client went silent for 110+ seconds. The client failed to send scheduled UPDATE messages, indicating an internal stall.

**Key Observations from FT Trace:**
- 17 instance-level reads over 36 seconds (~1 request per 2 seconds)
- 6 requests returned 4.01 (Unauthorized) - correct ACL behavior
- 11 requests returned 2.05 (Content) - successful reads
- Reply latency on 4.01 path showed "creep" (0.73-0.86s vs 0.18s baseline)
- After burst: client sent one UPDATE at t=57s, then went silent

**Reference:** `Issues/pcap-combo.md` Section "P0-5 (NEW) - Burst-load stall"

## Components

### 1. `burst_driver.py`

Python script that drives the exact FT trace pattern against a LwM2M client via Leshan's HTTP REST API.

**Features:**
- Executes 17 instance-level GET requests over 36 seconds
- Matches the exact FT trace pattern (6x 4.01, 11x 2.05)
- Tracks per-request latency (detects latency creep on 4.01 path)
- Verifies post-burst health: checks for UPDATE within `(lt * 0.75) + 5%`
- Outputs detailed results to `/tmp/burst-results.json`

### 2. `docker-compose.yml`

Docker Compose configuration for running the complete test environment:
- **Leshan Server:** Eclipse Leshan demo server (HTTP on 8080, CoAP on 5683)
- **Client:** Friendly-LwM2M-Client built from repository
- **Burst Driver (optional):** Automated test driver

## Quick Start

### Prerequisites

- Docker and Docker Compose
- Python 3.8+ with `requests` library
- Built Friendly-LwM2M-Client (or use Docker)

### Option 1: Docker Compose (Recommended)

```bash
# Navigate to reproducer directory
cd tools/burst-load-reproducer

# Start Leshan and client
docker-compose up -d leshan client

# Wait for registration (check Leshan UI at http://localhost:8080)
sleep 15

# Run burst driver (from host)
pip install requests
python3 burst_driver.py --endpoint TestClient --duration 36 --lt 60

# Check results
cat /tmp/burst-results.json | jq .

# Cleanup
docker-compose down
```

### Option 2: Fully Automated Test

```bash
cd tools/burst-load-reproducer

# Run everything including burst driver
docker-compose --profile auto-test up

# Results will be in ./output/burst-results.json
```

### Option 3: Local Testing

```bash
# Terminal 1: Start Leshan (or use existing server)
docker run -p 8080:8080 -p 5683:5683/udp eclipselwm2m/leshan-server-demo

# Terminal 2: Start client
./friendly_example_client -e TestClient -s coap://127.0.0.1:5683 --lifetime 60

# Terminal 3: Run burst driver
cd tools/burst-load-reproducer
python3 burst_driver.py --endpoint TestClient --duration 36 --lt 60
```

## Usage

### Basic Usage

```bash
python3 burst_driver.py
```

### Full Options

```bash
python3 burst_driver.py \
    --endpoint TestClient \      # Client endpoint name
    --duration 36 \              # Burst duration in seconds
    --lt 60 \                    # Client lifetime in seconds
    --leshan-url http://localhost:8080 \
    --output /tmp/burst-results.json \
    --timeout 10 \               # Per-request timeout
    --health-timeout 300 \       # Post-burst health check timeout
    --verbose                    # Enable verbose output
```

### Skip Health Check

For quick burst pattern testing without waiting for UPDATE:

```bash
python3 burst_driver.py --skip-health-check
```

## Expected Behavior

### Unfixed Client (Stall Expected)

```
[BURST] Starting 17 requests over 36s...
  [ 1/17] /3/0         -> 2.05  (expect 2.05) [0.185s] OK
  [ 2/17] /3339/0      -> 2.05  (expect 2.05) [0.192s] OK
  ...
  [ 5/17] /2/2         -> 4.01  (expect 4.01) [0.734s] OK  <- Latency creep
  ...
  [17/17] /2/5         -> 2.05  (expect 2.05) [0.201s] OK
[BURST] Completed in 36.2s

[HEALTH] Watching for UPDATE (expected within 48s)
[HEALTH] Waiting... 60s elapsed, 240s remaining
[HEALTH] TIMEOUT: No UPDATE detected within 300s - STALL SUSPECTED

RESULT: STALL DETECTED - P0-5 REPRODUCED
```

**Exit code: 1**

### Fixed Client (Healthy)

```
[BURST] Starting 17 requests over 36s...
  [ 1/17] /3/0         -> 2.05  (expect 2.05) [0.185s] OK
  ...
  [17/17] /2/5         -> 2.05  (expect 2.05) [0.201s] OK
[BURST] Completed in 36.1s

[HEALTH] Watching for UPDATE (expected within 48s)
[HEALTH] UPDATE detected at t=42.3s
[HEALTH] UPDATE arrived on time

RESULT: CLIENT HEALTHY
```

**Exit code: 0**

## Interpreting Results

### Output File: `/tmp/burst-results.json`

```json
{
  "start_time": "2026-04-30T10:00:00.000000",
  "end_time": "2026-04-30T10:06:00.000000",
  "duration_s": 36.2,
  "endpoint": "TestClient",
  "statistics": {
    "total_requests": 17,
    "successful_requests": 17,
    "match_count": 17,
    "avg_latency_s": 0.312,
    "avg_latency_2_05_s": 0.189,
    "avg_latency_4_01_s": 0.523
  },
  "health": {
    "update_detected": false,
    "stall_detected": true
  },
  "requests": [...]
}
```

### Key Metrics

| Metric | Healthy Value | Stall Indicator |
|--------|---------------|-----------------|
| `update_detected` | `true` | `false` |
| `update_time_s` | `< lt * 0.75 * 1.05` | `null` or very late |
| `avg_latency_4_01_s` | `~0.2s` (similar to 2.05) | `> 0.5s` with creep |
| `stall_detected` | `false` | `true` |

### Latency Creep Detection

The script warns if early 4.01 responses are significantly slower than later ones:

```
[ANALYSIS] WARNING: 4.01 latency creep detected (first half: 0.78s, second half: 0.45s)
```

This pattern was observed in the FT trace and suggests resource accumulation during 4.01 processing.

## Debugging Stall

### 1. Capture Client Snapshot (with P1-8 instrumentation)

```bash
# During stall, send SIGUSR1 to client
kill -SIGUSR1 $(pgrep friendly_example_client)

# Check snapshot
cat /tmp/ftl-lwm2m-snapshot.json
```

**Look for:**
- `transaction_count > 0` - Transaction leak
- `pending_token_count > 0` - Token/MID leak
- `nvm_inflight_writes > 0` - NVM contention

### 2. Check Resource Usage

```bash
# File descriptors
ls -la /proc/$(pgrep friendly_example_client)/fd | wc -l

# Memory
cat /proc/$(pgrep friendly_example_client)/status | grep -E "VmRSS|VmSize"

# Open files
lsof -p $(pgrep friendly_example_client)
```

### 3. Capture PCAP

```bash
# Start capture before burst
tcpdump -i any -w burst-debug.pcap port 5683

# Run burst
python3 burst_driver.py ...

# Analyze with Wireshark
wireshark burst-debug.pcap
```

## Test Variations

### Sustained Burst Test (Soak Test)

```bash
# 200 requests over 400 seconds (simulating long management session)
python3 burst_driver.py --duration 400 --health-timeout 600

# Modify FT_PATTERN in script or add --custom-pattern option
```

### Stress Test

```bash
# Rapid fire (no delay between requests)
python3 burst_driver.py --duration 1 --health-timeout 120
```

### Different Mix Ratios

Edit `FT_PATTERN` in `burst_driver.py` to test different combinations of accessible/inaccessible resources.

## Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Client healthy after burst |
| 1 | Stall detected (P0-5 reproduced) |
| 2 | Configuration or connection error |
| 130 | Interrupted by signal (SIGINT/SIGTERM) |

## Integration with CI

### GitHub Actions Example

```yaml
jobs:
  burst-test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Start Leshan
        run: docker-compose -f tools/burst-load-reproducer/docker-compose.yml up -d leshan

      - name: Build and Start Client
        run: |
          cmake -B build && cmake --build build
          ./build/friendly_example_client -e TestClient -s coap://127.0.0.1:5683 --lifetime 60 &
          sleep 10

      - name: Run Burst Test
        run: |
          pip install requests
          python3 tools/burst-load-reproducer/burst_driver.py \
            --endpoint TestClient \
            --duration 36 \
            --lt 60

      - name: Upload Results
        if: always()
        uses: actions/upload-artifact@v4
        with:
          name: burst-results
          path: /tmp/burst-results.json
```

## Troubleshooting

### "Client not registered"

```
[INIT] ERROR: Client 'TestClient' not registered with Leshan
```

**Solutions:**
1. Wait longer for client to register (15-30 seconds)
2. Check client logs for registration errors
3. Verify Leshan is accessible: `curl http://localhost:8080/api/clients`

### "Cannot connect to Leshan"

```
[INIT] ERROR: Cannot connect to Leshan at http://localhost:8080
```

**Solutions:**
1. Start Leshan: `docker-compose up -d leshan`
2. Check port: `netstat -an | grep 8080`
3. Try explicit URL: `--leshan-url http://192.168.1.100:8080`

### All requests timeout

**Solutions:**
1. Check CoAP connectivity: `nc -z -u localhost 5683`
2. Increase timeout: `--timeout 30`
3. Check client is responding (not already stalled)

## References

- **Issue Specification:** `Issues/pcap-combo.md` Section "P0-5"
- **Action Plan:** `docs/IMMEDIATE_ACTION_PLAN.md`
- **Leshan API:** https://github.com/eclipse/leshan/wiki/REST-API
- **LwM2M Spec:** OMA-TS-LightweightM2M_Core-V1_1

## License

Same license as the Friendly-LwM2M-Client project.
