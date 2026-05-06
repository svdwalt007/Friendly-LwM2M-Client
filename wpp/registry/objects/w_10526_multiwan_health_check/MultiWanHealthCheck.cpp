/*
 * MultiWanHealthCheck Implementation (Object 10526)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_10526_multiwan_health_check/MultiWanHealthCheck.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppLogs.h"

#ifdef OBJ_W_10526_MULTIWAN_HEALTH_CHECK

#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
#include <regex>
#include <algorithm>
#include <cmath>

// Network includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>

#define TAG "MultiWanHealthCheck"

using namespace wpp;

/* Static object methods */
Object& MultiWanHealthCheck::object(WppClient& ctx) {
    return ctx.registry().multiWanHealthCheck();
}

MultiWanHealthCheck* MultiWanHealthCheck::createInst(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().multiWanHealthCheck().createInstance(instId);
    if (!inst) return NULL;
    return static_cast<MultiWanHealthCheck*>(inst);
}

MultiWanHealthCheck* MultiWanHealthCheck::instance(WppClient& ctx, ID_T instId) {
    Instance *inst = ctx.registry().multiWanHealthCheck().instance(instId);
    if (!inst) return NULL;
    return static_cast<MultiWanHealthCheck*>(inst);
}

bool MultiWanHealthCheck::removeInst(WppClient& ctx, ID_T instId) {
    return ctx.registry().multiWanHealthCheck().remove(instId);
}

/* Instance lifecycle */
MultiWanHealthCheck::MultiWanHealthCheck(lwm2m_context_t& context, const OBJ_LINK_T& id)
    : Instance(context, id),
      m_running(false),
      m_probingActive(false) {
    WPP_LOGD(TAG, "MultiWanHealthCheck instance %d created", instId());
}

MultiWanHealthCheck::~MultiWanHealthCheck() {
    WPP_LOGD(TAG, "MultiWanHealthCheck instance %d destroyed", instId());

    // Stop probing thread
    m_running = false;
    m_probingActive = false;
    m_cv.notify_all();

    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

void MultiWanHealthCheck::serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) {
    operationNotify(*this, resLink, type);
}

void MultiWanHealthCheck::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
    if (type == ItemOp::WRITE || type == ItemOp::DELETE) notifyResChanged(resLink.resId, resLink.resInstId);
}

/* Initialize resources */
void MultiWanHealthCheck::resourcesInit() {
    WPP_LOGD(TAG, "Initializing MultiWanHealthCheck resources for instance %d", instId());

    // Default configuration values
    resource(INTERFACE_NAME_0)->set<STRING_T>( "wan");
    resource(PROBE_TYPE_1)->set<INT_T>( PROBE_ICMP);
    resource(TARGET_HOST_2)->set<STRING_T>( "8.8.8.8");
    resource(TARGET_PORT_3)->set<INT_T>( 80);
    resource(PROBE_INTERVAL_4)->set<INT_T>( 10);      // 10 seconds
    resource(PROBE_TIMEOUT_5)->set<INT_T>( 2000);     // 2000 ms (2 seconds)
    resource(FAILURE_THRESHOLD_6)->set<INT_T>( 3);    // 3 consecutive failures
    resource(RECOVERY_THRESHOLD_7)->set<INT_T>( 3);   // 3 consecutive successes

    // Initialize read-only status resources
    resource(CURRENT_STATE_8)->set<INT_T>( STATE_HEALTHY);
    resource(LAST_PROBE_TIME_9)->set<TIME_T>( 0);
    resource(LAST_PROBE_RESULT_10)->set<BOOL_T>( true);
    resource(AVERAGE_LATENCY_11)->set<FLOAT_T>( 0.0);
    resource(MIN_LATENCY_12)->set<FLOAT_T>( 0.0);
    resource(MAX_LATENCY_13)->set<FLOAT_T>( 0.0);
    resource(JITTER_14)->set<FLOAT_T>( 0.0);
    resource(PACKET_LOSS_15)->set<FLOAT_T>( 0.0);
    resource(CONSECUTIVE_FAILURES_16)->set<INT_T>( 0);
    resource(CONSECUTIVE_SUCCESSES_17)->set<INT_T>( 0);
    resource(TOTAL_PROBES_18)->set<INT_T>( 0);
    resource(FAILED_PROBES_19)->set<INT_T>( 0);

    // Set execute handlers
    resource(START_PROBING_20)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {
        return MultiWanHealthCheck::startProbing(inst, resId, data);
    });
    resource(STOP_PROBING_21)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {
        return MultiWanHealthCheck::stopProbing(inst, resId, data);
    });
    resource(FORCE_CHECK_22)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {
        return MultiWanHealthCheck::forceCheck(inst, resId, data);
    });
    resource(RESET_STATS_23)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {
        return MultiWanHealthCheck::resetStats(inst, resId, data);
    });

#ifdef OPENWRT_BUILD
    // Load existing configuration from mwan3 if available
    loadFromMwan3();
#endif

    // Start worker thread
    m_running = true;
    m_workerThread = std::thread(&MultiWanHealthCheck::probeWorker, this);
}

/* Validation */

/* Execute handler: Start Probing */
bool MultiWanHealthCheck::startProbing(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MultiWanHealthCheck& healthCheck = static_cast<MultiWanHealthCheck&>(inst);

    WPP_LOGI(TAG, "Starting health check probing for instance %d", healthCheck.instId());

    std::lock_guard<std::mutex> lock(healthCheck.m_mutex);
    healthCheck.m_probingActive = true;
    healthCheck.m_cv.notify_all();

    return true;
}

/* Execute handler: Stop Probing */
bool MultiWanHealthCheck::stopProbing(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MultiWanHealthCheck& healthCheck = static_cast<MultiWanHealthCheck&>(inst);

    WPP_LOGI(TAG, "Stopping health check probing for instance %d", healthCheck.instId());

    std::lock_guard<std::mutex> lock(healthCheck.m_mutex);
    healthCheck.m_probingActive = false;

    return true;
}

/* Execute handler: Force Check */
bool MultiWanHealthCheck::forceCheck(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MultiWanHealthCheck& healthCheck = static_cast<MultiWanHealthCheck&>(inst);

    WPP_LOGI(TAG, "Forcing immediate health check for instance %d", healthCheck.instId());

    // Execute probe immediately
    healthCheck.executeProbe();

    return true;
}

/* Execute handler: Reset Stats */
bool MultiWanHealthCheck::resetStats(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    MultiWanHealthCheck& healthCheck = static_cast<MultiWanHealthCheck&>(inst);

    WPP_LOGI(TAG, "Resetting statistics for instance %d", healthCheck.instId());

    healthCheck.resetStatistics();

    return true;
}

/* Probing worker thread */
void MultiWanHealthCheck::probeWorker() {
    WPP_LOGD(TAG, "Health check worker thread started for instance %d", instId());

    while (m_running) {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (!m_probingActive) {
            // Wait for probing to be activated
            m_cv.wait(lock, [this]() { return m_probingActive || !m_running; });
            if (!m_running) break;
        }

        lock.unlock();

        // Execute probe
        if (m_probingActive) {
            executeProbe();
        }

        // Wait for next interval
        INT_T interval = resource(PROBE_INTERVAL_4)->get<INT_T>();
        lock.lock();
        m_cv.wait_for(lock, std::chrono::seconds(interval),
                      [this]() { return !m_probingActive || !m_running; });
    }

    WPP_LOGD(TAG, "Health check worker thread stopped for instance %d", instId());
}

/* Execute probe based on configured type */
bool MultiWanHealthCheck::executeProbe() {
    INT_T probeType = resource(PROBE_TYPE_1)->get<INT_T>();
    STRING_T targetHost = resource(TARGET_HOST_2)->get<STRING_T>();
    INT_T targetPort = resource(TARGET_PORT_3)->get<INT_T>();
    INT_T timeout = resource(PROBE_TIMEOUT_5)->get<INT_T>();

    double latency_ms = 0.0;
    bool success = false;

    // Update probe time
    resource(LAST_PROBE_TIME_9)->set<TIME_T>( time(nullptr));

    switch (probeType) {
        case PROBE_ICMP:
            success = probeICMP(targetHost, timeout, latency_ms);
            break;
        case PROBE_HTTP:
            success = probeHTTP(targetHost, targetPort, timeout, latency_ms);
            break;
        case PROBE_DNS:
            success = probeDNS(targetHost, timeout, latency_ms);
            break;
        case PROBE_TCP:
            success = probeTCP(targetHost, targetPort, timeout, latency_ms);
            break;
        default:
            WPP_LOGE(TAG, "Unknown probe type: %lld", probeType);
            return false;
    }

    // Update result
    resource(LAST_PROBE_RESULT_10)->set<BOOL_T>( success);

    // Update statistics and health state
    updateStatistics(success, latency_ms);
    updateHealthState(success);

    WPP_LOGD(TAG, "Probe result: %s, latency: %.2f ms",
             success ? "SUCCESS" : "FAILURE", latency_ms);

    return success;
}

/* ICMP Ping Probe */
bool MultiWanHealthCheck::probeICMP(const std::string& host, int timeout_ms, double& latency_ms) {
    // Resolve hostname to IP
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    if (getaddrinfo(host.c_str(), nullptr, &hints, &result) != 0) {
        WPP_LOGE(TAG, "Failed to resolve host: %s", host.c_str());
        return false;
    }

    // Create raw socket
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        WPP_LOGE(TAG, "Failed to create ICMP socket: %s", strerror(errno));
        freeaddrinfo(result);
        return false;
    }

    // Set socket timeout
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // Prepare ICMP echo request
    struct icmp icmp_hdr;
    memset(&icmp_hdr, 0, sizeof(icmp_hdr));
    icmp_hdr.icmp_type = ICMP_ECHO;
    icmp_hdr.icmp_code = 0;
    icmp_hdr.icmp_id = getpid() & 0xFFFF;
    icmp_hdr.icmp_seq = getInstanceID() & 0xFFFF;

    // Calculate checksum
    icmp_hdr.icmp_cksum = 0;
    uint16_t* ptr = (uint16_t*)&icmp_hdr;
    uint32_t sum = 0;
    for (size_t i = 0; i < sizeof(icmp_hdr) / 2; i++) {
        sum += ptr[i];
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    icmp_hdr.icmp_cksum = ~sum;

    // Send ICMP echo request
    auto start_time = std::chrono::steady_clock::now();

    struct sockaddr_in* addr = (struct sockaddr_in*)result->ai_addr;
    ssize_t sent = sendto(sock, &icmp_hdr, sizeof(icmp_hdr), 0,
                          result->ai_addr, result->ai_addrlen);

    if (sent < 0) {
        WPP_LOGE(TAG, "Failed to send ICMP packet: %s", strerror(errno));
        close(sock);
        freeaddrinfo(result);
        return false;
    }

    // Wait for ICMP echo reply
    char recv_buf[1024];
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);

    ssize_t received = recvfrom(sock, recv_buf, sizeof(recv_buf), 0,
                                (struct sockaddr*)&recv_addr, &addr_len);

    auto end_time = std::chrono::steady_clock::now();
    latency_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    close(sock);
    freeaddrinfo(result);

    if (received < 0) {
        WPP_LOGD(TAG, "ICMP probe timeout");
        return false;
    }

    // Parse ICMP reply
    struct ip* ip_hdr = (struct ip*)recv_buf;
    int ip_hdr_len = ip_hdr->ip_hl * 4;
    struct icmp* icmp_reply = (struct icmp*)(recv_buf + ip_hdr_len);

    if (icmp_reply->icmp_type == ICMP_ECHOREPLY &&
        icmp_reply->icmp_id == icmp_hdr.icmp_id &&
        icmp_reply->icmp_seq == icmp_hdr.icmp_seq) {
        return true;
    }

    return false;
}

/* HTTP/HTTPS Probe */
bool MultiWanHealthCheck::probeHTTP(const std::string& host, int port, int timeout_ms, double& latency_ms) {
    // Resolve hostname
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string port_str = std::to_string(port);
    if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, &result) != 0) {
        WPP_LOGE(TAG, "Failed to resolve host: %s", host.c_str());
        return false;
    }

    // Create socket
    int sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock < 0) {
        WPP_LOGE(TAG, "Failed to create socket: %s", strerror(errno));
        freeaddrinfo(result);
        return false;
    }

    // Set non-blocking mode
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    auto start_time = std::chrono::steady_clock::now();

    // Connect
    int ret = connect(sock, result->ai_addr, result->ai_addrlen);

    if (ret < 0 && errno != EINPROGRESS) {
        WPP_LOGE(TAG, "Failed to connect: %s", strerror(errno));
        close(sock);
        freeaddrinfo(result);
        return false;
    }

    // Wait for connection with timeout
    struct pollfd pfd;
    pfd.fd = sock;
    pfd.events = POLLOUT;

    ret = poll(&pfd, 1, timeout_ms);

    if (ret <= 0) {
        WPP_LOGD(TAG, "HTTP probe timeout or error");
        close(sock);
        freeaddrinfo(result);
        return false;
    }

    // Check if connection succeeded
    int error = 0;
    socklen_t len = sizeof(error);
    getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len);

    if (error != 0) {
        WPP_LOGD(TAG, "HTTP connection failed: %s", strerror(error));
        close(sock);
        freeaddrinfo(result);
        return false;
    }

    // Send HTTP HEAD request
    std::stringstream request;
    request << "HEAD / HTTP/1.0\r\n"
            << "Host: " << host << "\r\n"
            << "Connection: close\r\n\r\n";

    std::string req_str = request.str();
    send(sock, req_str.c_str(), req_str.length(), 0);

    // Receive response
    char recv_buf[256];
    ret = recv(sock, recv_buf, sizeof(recv_buf) - 1, 0);

    auto end_time = std::chrono::steady_clock::now();
    latency_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    close(sock);
    freeaddrinfo(result);

    if (ret > 0) {
        recv_buf[ret] = '\0';
        // Check for HTTP response
        if (strstr(recv_buf, "HTTP/") != nullptr) {
            return true;
        }
    }

    return false;
}

/* DNS Probe */
bool MultiWanHealthCheck::probeDNS(const std::string& host, int timeout_ms, double& latency_ms) {
    auto start_time = std::chrono::steady_clock::now();

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int ret = getaddrinfo(host.c_str(), nullptr, &hints, &result);

    auto end_time = std::chrono::steady_clock::now();
    latency_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    if (ret != 0) {
        WPP_LOGD(TAG, "DNS resolution failed for %s: %s", host.c_str(), gai_strerror(ret));
        return false;
    }

    freeaddrinfo(result);

    // Check if resolution took longer than timeout
    if (latency_ms > timeout_ms) {
        WPP_LOGD(TAG, "DNS probe timeout");
        return false;
    }

    return true;
}

/* TCP Connection Probe */
bool MultiWanHealthCheck::probeTCP(const std::string& host, int port, int timeout_ms, double& latency_ms) {
    // Resolve hostname
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string port_str = std::to_string(port);
    if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, &result) != 0) {
        WPP_LOGE(TAG, "Failed to resolve host: %s", host.c_str());
        return false;
    }

    // Create socket
    int sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock < 0) {
        WPP_LOGE(TAG, "Failed to create socket: %s", strerror(errno));
        freeaddrinfo(result);
        return false;
    }

    // Set non-blocking mode
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    auto start_time = std::chrono::steady_clock::now();

    // Connect
    int ret = connect(sock, result->ai_addr, result->ai_addrlen);

    if (ret < 0 && errno != EINPROGRESS) {
        WPP_LOGE(TAG, "Failed to connect: %s", strerror(errno));
        close(sock);
        freeaddrinfo(result);
        return false;
    }

    // Wait for connection with timeout
    struct pollfd pfd;
    pfd.fd = sock;
    pfd.events = POLLOUT;

    ret = poll(&pfd, 1, timeout_ms);

    auto end_time = std::chrono::steady_clock::now();
    latency_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    if (ret <= 0) {
        WPP_LOGD(TAG, "TCP probe timeout or error");
        close(sock);
        freeaddrinfo(result);
        return false;
    }

    // Check if connection succeeded
    int error = 0;
    socklen_t len = sizeof(error);
    getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len);

    close(sock);
    freeaddrinfo(result);

    if (error != 0) {
        WPP_LOGD(TAG, "TCP connection failed: %s", strerror(error));
        return false;
    }

    return true;
}

/* Update statistics */
void MultiWanHealthCheck::updateStatistics(bool success, double latency_ms) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Update total probes
    INT_T totalProbes = resource(TOTAL_PROBES_18)->get<INT_T>() + 1;
    resource(TOTAL_PROBES_18)->set<INT_T>( totalProbes);

    if (!success) {
        // Update failed probes
        INT_T failedProbes = resource(FAILED_PROBES_19)->get<INT_T>() + 1;
        resource(FAILED_PROBES_19)->set<INT_T>( failedProbes);

        // Update packet loss percentage
        double packetLoss = (static_cast<double>(failedProbes) / totalProbes) * 100.0;
        resource(PACKET_LOSS_15)->set<FLOAT_T>( packetLoss);

        return;
    }

    // Update latency statistics (only for successful probes)
    FLOAT_T currentAvg = resource(AVERAGE_LATENCY_11)->get<FLOAT_T>();
    FLOAT_T minLatency = resource(MIN_LATENCY_12)->get<FLOAT_T>();
    FLOAT_T maxLatency = resource(MAX_LATENCY_13)->get<FLOAT_T>();

    // Update average using exponential moving average
    if (currentAvg == 0.0) {
        resource(AVERAGE_LATENCY_11)->set<FLOAT_T>( latency_ms);
    } else {
        double newAvg = (EMA_ALPHA * latency_ms) + ((1.0 - EMA_ALPHA) * currentAvg);
        resource(AVERAGE_LATENCY_11)->set<FLOAT_T>( newAvg);
    }

    // Update min/max
    if (minLatency == 0.0 || latency_ms < minLatency) {
        resource(MIN_LATENCY_12)->set<FLOAT_T>( latency_ms);
    }
    if (latency_ms > maxLatency) {
        resource(MAX_LATENCY_13)->set<FLOAT_T>( latency_ms);
    }

    // Add to latency history for jitter calculation
    LatencyRecord record;
    record.latency_ms = latency_ms;
    record.timestamp = std::chrono::steady_clock::now();

    m_latencyHistory.push_back(record);

    // Keep history size limited
    while (m_latencyHistory.size() > MAX_LATENCY_HISTORY) {
        m_latencyHistory.pop_front();
    }

    // Calculate jitter
    calculateJitter();

    // Update packet loss percentage
    INT_T failedProbes = resource(FAILED_PROBES_19)->get<INT_T>();
    double packetLoss = (static_cast<double>(failedProbes) / totalProbes) * 100.0;
    resource(PACKET_LOSS_15)->set<FLOAT_T>( packetLoss);
}

/* Update health state based on probe result */
void MultiWanHealthCheck::updateHealthState(bool probeSuccess) {
    INT_T consecutiveFailures = resource(CONSECUTIVE_FAILURES_16)->get<INT_T>();
    INT_T consecutiveSuccesses = resource(CONSECUTIVE_SUCCESSES_17)->get<INT_T>();
    INT_T failureThreshold = resource(FAILURE_THRESHOLD_6)->get<INT_T>();
    INT_T recoveryThreshold = resource(RECOVERY_THRESHOLD_7)->get<INT_T>();
    INT_T currentState = resource(CURRENT_STATE_8)->get<INT_T>();

    if (probeSuccess) {
        // Reset consecutive failures
        consecutiveFailures = 0;
        consecutiveSuccesses++;

        resource(CONSECUTIVE_FAILURES_16)->set<INT_T>( consecutiveFailures);
        resource(CONSECUTIVE_SUCCESSES_17)->set<INT_T>( consecutiveSuccesses);

        // Check for recovery
        if (currentState != STATE_HEALTHY && consecutiveSuccesses >= recoveryThreshold) {
            WPP_LOGI(TAG, "Interface %d transitioning to HEALTHY state", instId());
            resource(CURRENT_STATE_8)->set<INT_T>( STATE_HEALTHY);

#ifdef OPENWRT_BUILD
            // Notify mwan3 track system
            STRING_T ifname = resource(INTERFACE_NAME_0)->get<STRING_T>();
            std::stringstream cmd;
            cmd << "mwan3 track " << ifname << " up";
            system(cmd.str().c_str());
#endif
        }
    } else {
        // Reset consecutive successes
        consecutiveSuccesses = 0;
        consecutiveFailures++;

        resource(CONSECUTIVE_FAILURES_16)->set<INT_T>( consecutiveFailures);
        resource(CONSECUTIVE_SUCCESSES_17)->set<INT_T>( consecutiveSuccesses);

        // Check for degradation
        if (currentState == STATE_HEALTHY && consecutiveFailures >= failureThreshold / 2) {
            WPP_LOGW(TAG, "Interface %d transitioning to DEGRADED state", instId());
            resource(CURRENT_STATE_8)->set<INT_T>( STATE_DEGRADED);
        }

        // Check for failure
        if (consecutiveFailures >= failureThreshold) {
            if (currentState != STATE_UNHEALTHY) {
                WPP_LOGE(TAG, "Interface %d transitioning to UNHEALTHY state", instId());
                resource(CURRENT_STATE_8)->set<INT_T>( STATE_UNHEALTHY);

#ifdef OPENWRT_BUILD
                // Notify mwan3 track system
                STRING_T ifname = resource(INTERFACE_NAME_0)->get<STRING_T>();
                std::stringstream cmd;
                cmd << "mwan3 track " << ifname << " down";
                system(cmd.str().c_str());
#endif
            }
        }
    }
}

/* Reset statistics */
void MultiWanHealthCheck::resetStatistics() {
    std::lock_guard<std::mutex> lock(m_mutex);

    resource(AVERAGE_LATENCY_11)->set<FLOAT_T>( 0.0);
    resource(MIN_LATENCY_12)->set<FLOAT_T>( 0.0);
    resource(MAX_LATENCY_13)->set<FLOAT_T>( 0.0);
    resource(JITTER_14)->set<FLOAT_T>( 0.0);
    resource(PACKET_LOSS_15)->set<FLOAT_T>( 0.0);
    resource(CONSECUTIVE_FAILURES_16)->set<INT_T>( 0);
    resource(CONSECUTIVE_SUCCESSES_17)->set<INT_T>( 0);
    resource(TOTAL_PROBES_18)->set<INT_T>( 0);
    resource(FAILED_PROBES_19)->set<INT_T>( 0);

    m_latencyHistory.clear();

    WPP_LOGI(TAG, "Statistics reset for instance %d", instId());
}

/* Calculate jitter from latency history */
void MultiWanHealthCheck::calculateJitter() {
    if (m_latencyHistory.size() < 2) {
        return;
    }

    // Calculate mean absolute deviation of latency differences
    double sumDiff = 0.0;
    for (size_t i = 1; i < m_latencyHistory.size(); i++) {
        double diff = std::abs(m_latencyHistory[i].latency_ms - m_latencyHistory[i-1].latency_ms);
        sumDiff += diff;
    }

    double jitter = sumDiff / (m_latencyHistory.size() - 1);
    resource(JITTER_14)->set<FLOAT_T>( jitter);
}

/* Load configuration from mwan3 track */
bool MultiWanHealthCheck::loadFromMwan3() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Loading health check configuration from mwan3");

    STRING_T ifname = resource(INTERFACE_NAME_0)->get<STRING_T>();

    // Read track IP
    std::stringstream cmd;
    cmd << "uci get mwan3." << ifname << "_track.track_ip 2>/dev/null";
    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            resource(TARGET_HOST_2)->set<STRING_T>( buffer);
        }
        pclose(pipe);
    }

    // Read track method
    cmd.str("");
    cmd << "uci get mwan3." << ifname << "_track.track_method 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[64];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            std::string method(buffer);
            if (method == "ping") {
                resource(PROBE_TYPE_1)->set<INT_T>( PROBE_ICMP);
            } else if (method == "httping") {
                resource(PROBE_TYPE_1)->set<INT_T>( PROBE_HTTP);
            }
        }
        pclose(pipe);
    }

    // Read reliability (failure threshold)
    cmd.str("");
    cmd << "uci get mwan3." << ifname << "_track.reliability 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[16];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            int reliability = atoi(buffer);
            resource(FAILURE_THRESHOLD_6)->set<INT_T>( reliability);
        }
        pclose(pipe);
    }

    // Read count (recovery threshold)
    cmd.str("");
    cmd << "uci get mwan3." << ifname << "_track.count 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[16];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            int count = atoi(buffer);
            resource(RECOVERY_THRESHOLD_7)->set<INT_T>( count);
        }
        pclose(pipe);
    }

    // Read timeout
    cmd.str("");
    cmd << "uci get mwan3." << ifname << "_track.timeout 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[16];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            int timeout_sec = atoi(buffer);
            resource(PROBE_TIMEOUT_5)->set<INT_T>( timeout_sec * 1000);  // Convert to ms
        }
        pclose(pipe);
    }

    // Read interval
    cmd.str("");
    cmd << "uci get mwan3." << ifname << "_track.interval 2>/dev/null";
    pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[16];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            int interval = atoi(buffer);
            resource(PROBE_INTERVAL_4)->set<INT_T>( interval);
        }
        pclose(pipe);
    }

    WPP_LOGD(TAG, "Loaded health check configuration from mwan3");
    return true;
#else
    return false;
#endif
}

/* Save configuration to mwan3 track */
bool MultiWanHealthCheck::saveToMwan3() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Saving health check configuration to mwan3");

    STRING_T ifname = resource(INTERFACE_NAME_0)->get<STRING_T>();
    std::stringstream cmd;

    // Set track IP
    cmd.str("");
    cmd << "uci set mwan3." << ifname << "_track.track_ip='"
        << resource(TARGET_HOST_2)->get<STRING_T>() << "'";
    system(cmd.str().c_str());

    // Set track method
    cmd.str("");
    INT_T probeType = resource(PROBE_TYPE_1)->get<INT_T>();
    std::string method = (probeType == PROBE_ICMP) ? "ping" : "httping";
    cmd << "uci set mwan3." << ifname << "_track.track_method='" << method << "'";
    system(cmd.str().c_str());

    // Set reliability
    cmd.str("");
    cmd << "uci set mwan3." << ifname << "_track.reliability='"
        << resource(FAILURE_THRESHOLD_6)->get<INT_T>() << "'";
    system(cmd.str().c_str());

    // Set count
    cmd.str("");
    cmd << "uci set mwan3." << ifname << "_track.count='"
        << resource(RECOVERY_THRESHOLD_7)->get<INT_T>() << "'";
    system(cmd.str().c_str());

    // Set timeout (convert from ms to seconds)
    cmd.str("");
    INT_T timeout_ms = resource(PROBE_TIMEOUT_5)->get<INT_T>();
    cmd << "uci set mwan3." << ifname << "_track.timeout='"
        << (timeout_ms / 1000) << "'";
    system(cmd.str().c_str());

    // Set interval
    cmd.str("");
    cmd << "uci set mwan3." << ifname << "_track.interval='"
        << resource(PROBE_INTERVAL_4)->get<INT_T>() << "'";
    system(cmd.str().c_str());

    // Commit changes
    system("uci commit mwan3");

    // Restart mwan3
    system("mwan3 restart");

    WPP_LOGD(TAG, "Saved health check configuration to mwan3");
    return true;
#else
    return false;
#endif
}

/* Get interface device name */
std::string MultiWanHealthCheck::getInterfaceDevice() {
#ifdef OPENWRT_BUILD
    STRING_T ifname = resource(INTERFACE_NAME_0)->get<STRING_T>();
    std::stringstream cmd;
    cmd << "uci get network." << ifname << ".device 2>/dev/null";

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (pipe) {
        char buffer[64];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            buffer[strcspn(buffer, "\n")] = 0;
            pclose(pipe);
            return std::string(buffer);
        }
        pclose(pipe);
    }
#endif
    return "";
}

/* Validate hostname or IP address */
bool MultiWanHealthCheck::isValidHostname(const std::string& host) {
    // Check if it's a valid IPv4 address
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, host.c_str(), &(sa.sin_addr)) == 1) {
        return true;
    }

    // Check if it's a valid hostname
    std::regex hostnameRegex("^([a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?\\.)*[a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?$");
    return std::regex_match(host, hostnameRegex);
}

#endif // OBJ_W_10526_MULTIWAN_HEALTH_CHECK
