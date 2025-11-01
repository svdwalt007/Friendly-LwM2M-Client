/*
 * OpenWrtPlatform.cpp
 *
 * OpenWRT-specific platform implementation for LwM2M client
 * Integrates with OpenWRT logging (syslog) and system time
 */

#include "WppPlatform.h"
#include <ctime>
#include <cstdarg>
#include <cstdio>
#include <syslog.h>
#include <unistd.h>

namespace wpp {

// Global flag to control syslog vs. console logging
static bool g_useSyslog = true;
static int g_logLevel = LOG_INFO;

/**
 * @brief Set whether to use syslog or console logging
 * @param enable true to use syslog, false for console
 */
void setUseSyslog(bool enable) {
    g_useSyslog = enable;

    if (enable) {
        // Open syslog connection
        openlog("lwm2m-client", LOG_PID | LOG_CONS, LOG_DAEMON);
    } else {
        // Close syslog connection
        closelog();
    }
}

/**
 * @brief Set log level for syslog
 * @param level Syslog priority level (LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERR)
 */
void setLogLevel(int level) {
    g_logLevel = level;
}

/**
 * @brief Get current time in seconds since Epoch
 *
 * Uses POSIX time() function which returns seconds since Unix Epoch (1970-01-01)
 */
time_t WppPlatform::getTime(void) {
    return time(nullptr);
}

/**
 * @brief Print formatted message
 *
 * Outputs to syslog (when enabled) or stderr (when disabled)
 * Syslog is preferred for OpenWRT daemon operation
 */
void WppPlatform::print(const char* msg, va_list arg) {
    if (g_useSyslog) {
        // Use syslog for daemon operation
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), msg, arg);
        syslog(g_logLevel, "%s", buffer);
    } else {
        // Use stderr for console/debugging
        vfprintf(stderr, msg, arg);
        fflush(stderr);
    }
}

} // namespace wpp
