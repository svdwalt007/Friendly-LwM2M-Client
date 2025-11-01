/*
 * OpenWRT Location Information Helper
 *
 * Provides location data for OpenWRT One router from available sources:
 * 1. Starlink Terminal GPS (if available at 192.168.100.1:9200)
 * 2. gpsd daemon (if available)
 * 3. Manual configuration from UCI
 *
 * This helper class reads GPS location data that can be used to populate
 * the OMA LWM2M Location object (ID 6).
 */

#ifndef OPENWRT_LOCATION_INFO_H
#define OPENWRT_LOCATION_INFO_H

#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cmath>
#include <vector>
#include <chrono>

namespace wpp {

class OpenWrtLocationInfo {
private:
    // Previous location for speed calculation
    static double _prevLatitude;
    static double _prevLongitude;
    static time_t _prevTimestamp;

    /**
     * @brief Read a file and return its contents
     * @param path File path
     * @return File contents as string, empty if error
     */
    static std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        // Trim whitespace
        size_t start = content.find_first_not_of(" \t\n\r");
        size_t end = content.find_last_not_of(" \t\n\r");
        if (start == std::string::npos || end == std::string::npos) {
            return "";
        }

        return content.substr(start, end - start + 1);
    }

    /**
     * @brief Execute command and return output
     * @param cmd Command to execute
     * @return Command output
     */
    static std::string executeCommand(const std::string& cmd) {
        std::string result;
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) return "";

        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);

        // Trim newline
        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }

        return result;
    }

    /**
     * @brief Check if Starlink Terminal is available
     * @return true if Starlink is reachable
     */
    static bool isStarlinkAvailable() {
        // Check if Starlink dish is reachable at 192.168.100.1
        std::string result = executeCommand("ping -c 1 -W 1 192.168.100.1 2>/dev/null");
        return (result.find("1 received") != std::string::npos);
    }

    /**
     * @brief Get location data from Starlink Terminal via grpcurl
     * This requires grpcurl to be installed on OpenWRT
     * @param latitude Output latitude
     * @param longitude Output longitude
     * @param altitude Output altitude
     * @return true if successful
     */
    static bool getStarlinkLocation(double& latitude, double& longitude, double& altitude) {
        #ifdef OBJ_O_34600_STARLINK_TERMINAL
        // Try to get GPS data from Starlink dish via grpcurl
        // Note: This requires grpcurl and Starlink protobuf definitions
        std::string cmd = "grpcurl -plaintext -d '{\"get_status\":{}}' 192.168.100.1:9200 SpaceX.API.Device.Device/Handle 2>/dev/null";
        std::string result = executeCommand(cmd);

        if (result.empty()) {
            return false;
        }

        // Parse JSON response for GPS coordinates
        // Look for: "gpsStats": { "gpsValid": true, "gpsSats": N, "latitude": X, "longitude": Y, "altitudeMeters": Z }

        // Simple string parsing (in production, use a JSON library)
        size_t latPos = result.find("\"latitude\":");
        size_t lonPos = result.find("\"longitude\":");
        size_t altPos = result.find("\"altitudeMeters\":");
        size_t validPos = result.find("\"gpsValid\":true");

        if (validPos == std::string::npos) {
            return false; // GPS not valid
        }

        if (latPos != std::string::npos && lonPos != std::string::npos) {
            try {
                // Extract latitude
                size_t latStart = latPos + 11;
                size_t latEnd = result.find_first_of(",}", latStart);
                std::string latStr = result.substr(latStart, latEnd - latStart);
                latitude = std::stod(latStr);

                // Extract longitude
                size_t lonStart = lonPos + 12;
                size_t lonEnd = result.find_first_of(",}", lonStart);
                std::string lonStr = result.substr(lonStart, lonEnd - lonStart);
                longitude = std::stod(lonStr);

                // Extract altitude
                if (altPos != std::string::npos) {
                    size_t altStart = altPos + 17;
                    size_t altEnd = result.find_first_of(",}", altStart);
                    std::string altStr = result.substr(altStart, altEnd - altStart);
                    altitude = std::stod(altStr);
                }

                return true;
            } catch (...) {
                return false;
            }
        }
        #endif

        return false;
    }

    /**
     * @brief Get location from gpsd daemon
     * @param latitude Output latitude
     * @param longitude Output longitude
     * @param altitude Output altitude
     * @return true if successful
     */
    static bool getGpsdLocation(double& latitude, double& longitude, double& altitude) {
        // Try to get location from gpsd via gpspipe
        std::string cmd = "gpspipe -w -n 10 2>/dev/null | grep -m 1 TPV";
        std::string result = executeCommand(cmd);

        if (result.empty()) {
            return false;
        }

        // Parse JSON for lat, lon, alt
        size_t latPos = result.find("\"lat\":");
        size_t lonPos = result.find("\"lon\":");
        size_t altPos = result.find("\"alt\":");

        if (latPos != std::string::npos && lonPos != std::string::npos) {
            try {
                // Extract latitude
                size_t latStart = latPos + 6;
                size_t latEnd = result.find_first_of(",}", latStart);
                std::string latStr = result.substr(latStart, latEnd - latStart);
                latitude = std::stod(latStr);

                // Extract longitude
                size_t lonStart = lonPos + 6;
                size_t lonEnd = result.find_first_of(",}", lonStart);
                std::string lonStr = result.substr(lonStart, lonEnd - lonStart);
                longitude = std::stod(lonStr);

                // Extract altitude
                if (altPos != std::string::npos) {
                    size_t altStart = altPos + 6;
                    size_t altEnd = result.find_first_of(",}", altStart);
                    std::string altStr = result.substr(altStart, altEnd - altStart);
                    altitude = std::stod(altStr);
                }

                return true;
            } catch (...) {
                return false;
            }
        }

        return false;
    }

    /**
     * @brief Get manual location from UCI configuration
     * @param latitude Output latitude
     * @param longitude Output longitude
     * @param altitude Output altitude
     * @return true if successful
     */
    static bool getManualLocation(double& latitude, double& longitude, double& altitude) {
        // Try to read from UCI: uci get lwm2m.location.latitude
        std::string latStr = executeCommand("uci get lwm2m.location.latitude 2>/dev/null");
        std::string lonStr = executeCommand("uci get lwm2m.location.longitude 2>/dev/null");
        std::string altStr = executeCommand("uci get lwm2m.location.altitude 2>/dev/null");

        if (!latStr.empty() && !lonStr.empty()) {
            try {
                latitude = std::stod(latStr);
                longitude = std::stod(lonStr);
                if (!altStr.empty()) {
                    altitude = std::stod(altStr);
                }
                return true;
            } catch (...) {
                return false;
            }
        }

        return false;
    }

    /**
     * @brief Calculate distance between two GPS coordinates using Haversine formula
     * @param lat1 Latitude of point 1 (degrees)
     * @param lon1 Longitude of point 1 (degrees)
     * @param lat2 Latitude of point 2 (degrees)
     * @param lon2 Longitude of point 2 (degrees)
     * @return Distance in meters
     */
    static double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
        const double R = 6371000.0; // Earth radius in meters

        // Convert to radians
        double lat1Rad = lat1 * M_PI / 180.0;
        double lat2Rad = lat2 * M_PI / 180.0;
        double deltaLat = (lat2 - lat1) * M_PI / 180.0;
        double deltaLon = (lon2 - lon1) * M_PI / 180.0;

        // Haversine formula
        double a = sin(deltaLat / 2.0) * sin(deltaLat / 2.0) +
                   cos(lat1Rad) * cos(lat2Rad) *
                   sin(deltaLon / 2.0) * sin(deltaLon / 2.0);
        double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

        return R * c;
    }

public:
    /**
     * @brief Get current latitude
     * @return Latitude in decimal degrees (WGS84)
     */
    static double getLatitude() {
        double lat = 0.0, lon = 0.0, alt = 0.0;

        // Try sources in order of preference
        if (getStarlinkLocation(lat, lon, alt)) {
            return lat;
        }
        if (getGpsdLocation(lat, lon, alt)) {
            return lat;
        }
        if (getManualLocation(lat, lon, alt)) {
            return lat;
        }

        return 0.0; // No location available
    }

    /**
     * @brief Get current longitude
     * @return Longitude in decimal degrees (WGS84)
     */
    static double getLongitude() {
        double lat = 0.0, lon = 0.0, alt = 0.0;

        // Try sources in order of preference
        if (getStarlinkLocation(lat, lon, alt)) {
            return lon;
        }
        if (getGpsdLocation(lat, lon, alt)) {
            return lon;
        }
        if (getManualLocation(lat, lon, alt)) {
            return lon;
        }

        return 0.0; // No location available
    }

    /**
     * @brief Get current altitude
     * @return Altitude in meters above sea level
     */
    static double getAltitude() {
        double lat = 0.0, lon = 0.0, alt = 0.0;

        // Try sources in order of preference
        if (getStarlinkLocation(lat, lon, alt)) {
            return alt;
        }
        if (getGpsdLocation(lat, lon, alt)) {
            return alt;
        }
        if (getManualLocation(lat, lon, alt)) {
            return alt;
        }

        return 0.0; // No altitude available
    }

    /**
     * @brief Get location uncertainty radius
     * @return Radius in meters (typical GPS accuracy)
     */
    static double getRadius() {
        // Return typical GPS accuracy based on source
        if (isStarlinkAvailable()) {
            return 10.0; // Starlink GPS is typically accurate to ~10m
        }

        // Check if gpsd is running
        std::string gpsd = executeCommand("pgrep gpsd");
        if (!gpsd.empty()) {
            return 15.0; // Consumer GPS typically 10-15m accuracy
        }

        return 100.0; // Manual configuration - assume low accuracy
    }

    /**
     * @brief Get current timestamp
     * @return Unix timestamp (seconds since epoch)
     */
    static time_t getTimestamp() {
        return std::time(nullptr);
    }

    /**
     * @brief Get current speed
     * @return Speed in meters per second
     */
    static double getSpeed() {
        double lat = getLatitude();
        double lon = getLongitude();
        time_t now = getTimestamp();

        // If we have previous position, calculate speed
        if (_prevTimestamp > 0 && (now > _prevTimestamp)) {
            double distance = calculateDistance(_prevLatitude, _prevLongitude, lat, lon);
            double timeDiff = static_cast<double>(now - _prevTimestamp);

            // Update previous position
            _prevLatitude = lat;
            _prevLongitude = lon;
            _prevTimestamp = now;

            return distance / timeDiff; // m/s
        }

        // First measurement - save position
        _prevLatitude = lat;
        _prevLongitude = lon;
        _prevTimestamp = now;

        return 0.0;
    }

    /**
     * @brief Check if location is available
     * @return true if location data is available
     */
    static bool isLocationAvailable() {
        double lat = 0.0, lon = 0.0, alt = 0.0;
        return (getStarlinkLocation(lat, lon, alt) ||
                getGpsdLocation(lat, lon, alt) ||
                getManualLocation(lat, lon, alt));
    }

    /**
     * @brief Get all location data at once (more efficient)
     * @param lat Output latitude
     * @param lon Output longitude
     * @param alt Output altitude
     * @return true if successful
     */
    static bool getAllLocation(double& lat, double& lon, double& alt) {
        // Try sources in order of preference
        if (getStarlinkLocation(lat, lon, alt)) {
            return true;
        }
        if (getGpsdLocation(lat, lon, alt)) {
            return true;
        }
        if (getManualLocation(lat, lon, alt)) {
            return true;
        }

        return false;
    }
};

// Initialize static members
double OpenWrtLocationInfo::_prevLatitude = 0.0;
double OpenWrtLocationInfo::_prevLongitude = 0.0;
time_t OpenWrtLocationInfo::_prevTimestamp = 0;

} // namespace wpp

#endif // OPENWRT_LOCATION_INFO_H
