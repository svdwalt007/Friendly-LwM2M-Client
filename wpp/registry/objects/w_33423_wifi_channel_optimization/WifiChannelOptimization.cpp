/*
 * WifiChannelOptimization Implementation (Object 33423)
 * Walt Technologies - OpenWRT One Router
 */

#include "w_33423_wifi_channel_optimization/WifiChannelOptimization.h"
#include "WppClient.h"
#include "WppRegistry.h"
#include "WppLogs.h"
#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_33423_WIFI_CHANNEL_OPTIMIZATION

#include <cstdio>
#include <cstring>
#include <cmath>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <regex>

#define TAG "WifiChannelOptimization"

using namespace wpp;

/* Static object methods */
Object& WifiChannelOptimization::object(WppClient& client) {
    return client.registry().getObject(WIFI_CHANNEL_OPTIMIZATION_OBJECT_ID);
}

Instance* WifiChannelOptimization::createInst(WppClient& client, INST_T instId) {
    return object(client).createInstance(instId);
}

Instance* WifiChannelOptimization::instance(WppClient& client, INST_T instId) {
    return object(client).instance(instId);
}

bool WifiChannelOptimization::remove(WppClient& client, INST_T instId) {
    return object(client).remove(instId);
}

/* Instance lifecycle */
WifiChannelOptimization::WifiChannelOptimization(Object& object, INST_T instId)
    : Instance(object, instId) {
    WPP_LOGD(TAG, "WifiChannelOptimization instance %d created", instId);
}

WifiChannelOptimization::~WifiChannelOptimization() {
    WPP_LOGD(TAG, "WifiChannelOptimization instance %d destroyed", instId());
}

/* Initialize resources */
bool WifiChannelOptimization::initResources(ItemOp *itemOp) {
    WPP_LOGD(TAG, "Initializing WifiChannelOptimization resources for instance %d", instId());

    // Default values
    set<STRING_T>(INTERFACE_0, "radio0");
    set<INT_T>(CURRENT_CHANNEL_1, 0);
    set<INT_T>(CURRENT_WIDTH_2, 20);
    set<BOOL_T>(AUTO_CHANNEL_3, false);
    set<INT_T>(SCAN_INTERVAL_4, 3600);  // 1 hour default
    set<INT_T>(NEIGHBORING_APS_5, 0);
    set<INT_T>(CHANNEL_UTILIZATION_6, 0);
    set<INT_T>(NOISE_FLOOR_7, -95);
    set<INT_T>(INTERFERENCE_LEVEL_8, 0);
    set<BOOL_T>(DFS_ENABLED_9, false);
    set<STRING_T>(DFS_STATE_10, "Available");
    set<BOOL_T>(RADAR_DETECTED_11, false);
    set<INT_T>(BEST_2G_CHANNEL_12, 6);
    set<INT_T>(BEST_5G_CHANNEL_13, 36);
    set<INT_T>(BEST_6G_CHANNEL_14, 0);
    set<TIME_T>(LAST_SCAN_TIME_16, 0);
    set<TIME_T>(LAST_OPTIMIZATION_17, 0);

    // Set execute handlers
    setExecute(SCAN_NOW_18, scanNow);
    setExecute(OPTIMIZE_CHANNEL_19, optimizeChannel);
    setExecute(SET_CHANNEL_20, setChannel);
    setExecute(ENABLE_DFS_21, enableDFS);

#ifdef OPENWRT_BUILD
    // Load current wireless state
    loadCurrentState();
#endif

    return true;
}

/* Validation */
bool WifiChannelOptimization::validate(ID_T resId, const void *data, size_t size) {
    switch (resId) {
        case INTERFACE_0: {
            const STRING_T& iface = *(const STRING_T*)data;
            if (iface.empty()) {
                WPP_LOGE(TAG, "Interface name cannot be empty");
                return false;
            }
            break;
        }
        case SCAN_INTERVAL_4: {
            INT_T interval = *(const INT_T*)data;
            if (interval < 60 || interval > 86400) {  // 1 minute to 24 hours
                WPP_LOGE(TAG, "Invalid scan interval: %lld (must be 60-86400 seconds)", interval);
                return false;
            }
            break;
        }
    }
    return true;
}

/* Execute handler: Scan Now */
bool WifiChannelOptimization::scanNow(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WifiChannelOptimization& wifi = static_cast<WifiChannelOptimization&>(inst);

    WPP_LOGI(TAG, "Starting channel scan for instance %d", wifi.instId());

#ifdef OPENWRT_BUILD
    if (!wifi.performChannelScan()) {
        WPP_LOGE(TAG, "Channel scan failed");
        return false;
    }

    if (!wifi.performChannelSurvey()) {
        WPP_LOGE(TAG, "Channel survey failed");
        return false;
    }

    wifi.set<TIME_T>(LAST_SCAN_TIME_16, time(nullptr));
    WPP_LOGI(TAG, "Channel scan completed successfully");
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot perform scan");
    return false;
#endif
}

/* Execute handler: Optimize Channel */
bool WifiChannelOptimization::optimizeChannel(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WifiChannelOptimization& wifi = static_cast<WifiChannelOptimization&>(inst);

    WPP_LOGI(TAG, "Starting channel optimization for instance %d", wifi.instId());

#ifdef OPENWRT_BUILD
    // Perform scan first
    if (!wifi.performChannelScan()) {
        WPP_LOGE(TAG, "Channel scan failed during optimization");
        return false;
    }

    if (!wifi.performChannelSurvey()) {
        WPP_LOGE(TAG, "Channel survey failed during optimization");
        return false;
    }

    // Calculate best channels
    std::vector<ChannelScore> scores = wifi.calculateChannelScores();

    if (scores.empty()) {
        WPP_LOGE(TAG, "No suitable channels found");
        return false;
    }

    // Sort by score (descending)
    std::sort(scores.begin(), scores.end(),
        [](const ChannelScore& a, const ChannelScore& b) {
            return a.score > b.score;
        });

    // Get best channel for current band
    int currentChannel = wifi.get<INT_T>(CURRENT_CHANNEL_1);
    int currentBand = wifi.getBandFromChannel(currentChannel);

    ChannelScore bestChannel = scores[0];

    // Find best channel in current band
    for (const auto& score : scores) {
        if (wifi.getBandFromChannel(score.channel) == currentBand) {
            bestChannel = score;
            break;
        }
    }

    // Update best channel recommendations
    for (const auto& score : scores) {
        int band = wifi.getBandFromChannel(score.channel);
        if (band == 2 && wifi.get<INT_T>(BEST_2G_CHANNEL_12) == wifi.get<INT_T>(CURRENT_CHANNEL_1)) {
            wifi.set<INT_T>(BEST_2G_CHANNEL_12, score.channel);
        } else if (band == 5 && wifi.get<INT_T>(BEST_5G_CHANNEL_13) == wifi.get<INT_T>(CURRENT_CHANNEL_1)) {
            wifi.set<INT_T>(BEST_5G_CHANNEL_13, score.channel);
        } else if (band == 6 && wifi.get<INT_T>(BEST_6G_CHANNEL_14) == wifi.get<INT_T>(CURRENT_CHANNEL_1)) {
            wifi.set<INT_T>(BEST_6G_CHANNEL_14, score.channel);
        }
    }

    // Apply the best channel if auto-channel is enabled
    if (wifi.get<BOOL_T>(AUTO_CHANNEL_3)) {
        WPP_LOGI(TAG, "Auto-channel enabled, switching to channel %d (score: %.2f)",
                 bestChannel.channel, bestChannel.score);

        if (!wifi.applyChannelChange(bestChannel.channel, bestChannel.width)) {
            WPP_LOGE(TAG, "Failed to apply channel change");
            return false;
        }

        // Update historical data
        wifi.updateChannelHistory(bestChannel.channel, bestChannel.score);
    }

    wifi.set<TIME_T>(LAST_OPTIMIZATION_17, time(nullptr));
    WPP_LOGI(TAG, "Channel optimization completed successfully");
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot optimize channel");
    return false;
#endif
}

/* Execute handler: Set Channel */
bool WifiChannelOptimization::setChannel(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WifiChannelOptimization& wifi = static_cast<WifiChannelOptimization&>(inst);

    WPP_LOGI(TAG, "Setting channel for instance %d", wifi.instId());

    // Parse channel from data (format: "channel:width" or just "channel")
    std::string channelStr(data.begin(), data.end());
    int channel = 0;
    int width = 20;

    size_t colonPos = channelStr.find(':');
    if (colonPos != std::string::npos) {
        channel = std::stoi(channelStr.substr(0, colonPos));
        width = std::stoi(channelStr.substr(colonPos + 1));
    } else {
        channel = std::stoi(channelStr);
        width = wifi.get<INT_T>(CURRENT_WIDTH_2);  // Keep current width
    }

    if (!wifi.isValidChannel(channel)) {
        WPP_LOGE(TAG, "Invalid channel: %d", channel);
        return false;
    }

    if (!wifi.isValidWidth(width)) {
        WPP_LOGE(TAG, "Invalid channel width: %d", width);
        return false;
    }

#ifdef OPENWRT_BUILD
    if (!wifi.applyChannelChange(channel, width)) {
        WPP_LOGE(TAG, "Failed to set channel");
        return false;
    }

    WPP_LOGI(TAG, "Channel set successfully to %d (width: %d MHz)", channel, width);
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot set channel");
    return false;
#endif
}

/* Execute handler: Enable DFS */
bool WifiChannelOptimization::enableDFS(Instance& inst, ID_T resId, const OPAQUE_T& data) {
    WifiChannelOptimization& wifi = static_cast<WifiChannelOptimization&>(inst);

    WPP_LOGI(TAG, "Enabling DFS for instance %d", wifi.instId());

#ifdef OPENWRT_BUILD
    std::string iface = wifi.get<STRING_T>(INTERFACE_0);

    // Enable DFS in UCI configuration
    std::stringstream cmd;
    cmd << "uci set wireless." << iface << ".dfs='1' && uci commit wireless";
    int result = system(cmd.str().c_str());

    if (result != 0) {
        WPP_LOGE(TAG, "Failed to enable DFS in UCI");
        return false;
    }

    wifi.set<BOOL_T>(DFS_ENABLED_9, true);

    // Reload wireless to apply changes
    if (!wifi.reloadWireless()) {
        WPP_LOGE(TAG, "Failed to reload wireless after enabling DFS");
        return false;
    }

    WPP_LOGI(TAG, "DFS enabled successfully");
    return true;
#else
    WPP_LOGW(TAG, "OpenWRT build not enabled, cannot enable DFS");
    return false;
#endif
}

/* Perform channel scan */
bool WifiChannelOptimization::performChannelScan() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Performing channel scan");

    std::string iface = get<STRING_T>(INTERFACE_0);

    // Get the physical interface name
    std::stringstream cmd;
    cmd << "iw dev | grep -A 1 \"" << iface << "\" | grep Interface | awk '{print $2}'";
    std::string phyIface = executeCommand(cmd.str());

    if (phyIface.empty()) {
        WPP_LOGE(TAG, "Could not find physical interface for %s", iface.c_str());
        return false;
    }

    // Perform scan
    cmd.str("");
    cmd << "iw dev " << phyIface << " scan 2>/dev/null";
    std::string scanOutput = executeCommand(cmd.str());

    // Count neighboring APs
    int neighborCount = 0;
    std::stringstream ss(scanOutput);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.find("BSS ") != std::string::npos) {
            neighborCount++;
        }
    }

    set<INT_T>(NEIGHBORING_APS_5, neighborCount);

    // Detect channel-specific neighbors
    int currentChannel = get<INT_T>(CURRENT_CHANNEL_1);
    int channelNeighbors = detectNeighboringAPs(currentChannel);

    WPP_LOGD(TAG, "Found %d neighboring APs (%d on current channel %d)",
             neighborCount, channelNeighbors, currentChannel);

    return true;
#else
    return false;
#endif
}

/* Perform channel survey */
bool WifiChannelOptimization::performChannelSurvey() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Performing channel survey");

    std::string iface = get<STRING_T>(INTERFACE_0);

    // Get the physical interface name
    std::stringstream cmd;
    cmd << "iw dev | grep -A 1 \"" << iface << "\" | grep Interface | awk '{print $2}'";
    std::string phyIface = executeCommand(cmd.str());

    if (phyIface.empty()) {
        WPP_LOGE(TAG, "Could not find physical interface for %s", iface.c_str());
        return false;
    }

    // Get survey dump
    cmd.str("");
    cmd << "iw dev " << phyIface << " survey dump 2>/dev/null";
    std::string surveyOutput = executeCommand(cmd.str());

    // Clear existing survey data
    clearResource(CHANNEL_SURVEY_15);

    // Parse survey data
    std::stringstream ss(surveyOutput);
    std::string line;
    int currentSurveyChannel = 0;
    int busy = 0;
    int noise = -95;

    while (std::getline(ss, line)) {
        if (line.find("frequency:") != std::string::npos) {
            // Extract frequency and convert to channel
            size_t pos = line.find("frequency:") + 10;
            int freq = std::stoi(line.substr(pos));

            // Convert frequency to channel
            if (freq >= 2412 && freq <= 2484) {
                currentSurveyChannel = (freq - 2407) / 5;
            } else if (freq >= 5170 && freq <= 5825) {
                currentSurveyChannel = (freq - 5000) / 5;
            } else if (freq >= 5955 && freq <= 7115) {
                currentSurveyChannel = (freq - 5950) / 5;
            }
        } else if (line.find("channel active time:") != std::string::npos) {
            // Extract active time
        } else if (line.find("channel busy time:") != std::string::npos) {
            // Extract busy time and calculate percentage
            std::regex busyRegex("channel busy time:\\s+(\\d+)\\s+ms");
            std::smatch match;
            if (std::regex_search(line, match, busyRegex)) {
                busy = std::stoi(match[1].str());
            }
        } else if (line.find("noise:") != std::string::npos) {
            // Extract noise floor
            std::regex noiseRegex("noise:\\s+(-?\\d+)\\s+dBm");
            std::smatch match;
            if (std::regex_search(line, match, noiseRegex)) {
                noise = std::stoi(match[1].str());
            }

            // End of survey entry, add to resource
            if (currentSurveyChannel > 0) {
                std::stringstream surveyEntry;
                surveyEntry << currentSurveyChannel << ":" << busy << ":" << noise;
                addToMultiple(CHANNEL_SURVEY_15, surveyEntry.str());
            }
        }
    }

    // Update current channel metrics
    int utilization = measureChannelUtilization();
    int noiseFloor = measureNoiseFloor();
    int interference = calculateInterferenceLevel();

    set<INT_T>(CHANNEL_UTILIZATION_6, utilization);
    set<INT_T>(NOISE_FLOOR_7, noiseFloor);
    set<INT_T>(INTERFERENCE_LEVEL_8, interference);

    WPP_LOGD(TAG, "Channel survey completed: utilization=%d%%, noise=%ddBm, interference=%d",
             utilization, noiseFloor, interference);

    return true;
#else
    return false;
#endif
}

/* Load current state from system */
bool WifiChannelOptimization::loadCurrentState() {
#ifdef OPENWRT_BUILD
    WPP_LOGD(TAG, "Loading current wireless state");

    std::string iface = get<STRING_T>(INTERFACE_0);

    // Get current channel
    std::stringstream cmd;
    cmd << "uci get wireless." << iface << ".channel 2>/dev/null";
    std::string channelStr = executeCommand(cmd.str());

    if (!channelStr.empty() && channelStr != "auto") {
        int channel = std::stoi(channelStr);
        set<INT_T>(CURRENT_CHANNEL_1, channel);
    } else {
        set<BOOL_T>(AUTO_CHANNEL_3, true);
    }

    // Get current channel width
    cmd.str("");
    cmd << "uci get wireless." << iface << ".htmode 2>/dev/null";
    std::string htmode = executeCommand(cmd.str());

    int width = 20;
    if (htmode.find("HT40") != std::string::npos || htmode.find("VHT40") != std::string::npos) {
        width = 40;
    } else if (htmode.find("VHT80") != std::string::npos || htmode.find("HE80") != std::string::npos) {
        width = 80;
    } else if (htmode.find("VHT160") != std::string::npos || htmode.find("HE160") != std::string::npos) {
        width = 160;
    }
    set<INT_T>(CURRENT_WIDTH_2, width);

    // Check DFS status
    cmd.str("");
    cmd << "uci get wireless." << iface << ".dfs 2>/dev/null";
    std::string dfsStr = executeCommand(cmd.str());
    set<BOOL_T>(DFS_ENABLED_9, dfsStr == "1");

    // Get interface info
    getInterfaceInfo();

    // Check DFS state
    checkDFSState();

    return true;
#else
    return false;
#endif
}

/* Apply channel change */
bool WifiChannelOptimization::applyChannelChange(int channel, int width) {
#ifdef OPENWRT_BUILD
    WPP_LOGI(TAG, "Applying channel change: channel=%d, width=%dMHz", channel, width);

    if (!setWirelessConfig(channel, width)) {
        WPP_LOGE(TAG, "Failed to set wireless configuration");
        return false;
    }

    if (!reloadWireless()) {
        WPP_LOGE(TAG, "Failed to reload wireless");
        return false;
    }

    set<INT_T>(CURRENT_CHANNEL_1, channel);
    set<INT_T>(CURRENT_WIDTH_2, width);

    return true;
#else
    return false;
#endif
}

/* Calculate channel scores using ACS algorithm */
std::vector<WifiChannelOptimization::ChannelScore> WifiChannelOptimization::calculateChannelScores() {
    std::vector<ChannelScore> scores;

    std::vector<int> channels = getAvailableChannels();
    int currentWidth = get<INT_T>(CURRENT_WIDTH_2);

    for (int channel : channels) {
        ChannelScore score = scoreChannel(channel, currentWidth);
        scores.push_back(score);
    }

    return scores;
}

/* Score a specific channel */
WifiChannelOptimization::ChannelScore WifiChannelOptimization::scoreChannel(int channel, int width) {
    ChannelScore score;
    score.channel = channel;
    score.width = width;
    score.dfsRequired = isChannelDFS(channel);

    // Get metrics for this channel
    score.neighborCount = detectNeighboringAPs(channel);

    // Get survey data for this channel
    std::vector<STRING_T> surveyData;
    getMultiple(CHANNEL_SURVEY_15, surveyData);

    score.utilization = 0;
    score.noiseFloor = -95;

    for (const auto& entry : surveyData) {
        std::vector<std::string> parts = splitString(entry, ':');
        if (parts.size() >= 3 && std::stoi(parts[0]) == channel) {
            score.utilization = std::stoi(parts[1]);
            score.noiseFloor = std::stoi(parts[2]);
            break;
        }
    }

    // Scoring algorithm (higher is better)
    double baseScore = 100.0;

    // Penalize for neighboring APs (each AP reduces score by 5)
    baseScore -= score.neighborCount * 5.0;

    // Penalize for channel utilization (1:1 penalty)
    baseScore -= score.utilization;

    // Penalize for noise floor (each dB above -95 reduces score by 0.5)
    baseScore -= (score.noiseFloor + 95) * 0.5;

    // Bonus for DFS channels if DFS is enabled (less congested)
    if (score.dfsRequired && get<BOOL_T>(DFS_ENABLED_9)) {
        baseScore += 10.0;
    } else if (score.dfsRequired && !get<BOOL_T>(DFS_ENABLED_9)) {
        // Cannot use DFS channels if DFS is disabled
        baseScore = -100.0;
    }

    // Bonus for historical good performance
    double historicalScore = getHistoricalPerformance(channel);
    baseScore += historicalScore * 0.2;  // 20% weight on historical data

    // Bonus for wider channels in clear spectrum
    if (width >= 80 && score.neighborCount < 2) {
        baseScore += 5.0;
    }

    score.score = std::max(0.0, baseScore);

    return score;
}

/* Check if channel requires DFS */
bool WifiChannelOptimization::isChannelDFS(int channel) {
    // DFS channels in 5GHz band (varies by region, this is ETSI)
    std::vector<int> dfsChannels = {
        52, 56, 60, 64,           // DFS1
        100, 104, 108, 112, 116,  // DFS2
        120, 124, 128, 132, 136,  // DFS3
        140, 144                  // DFS4
    };

    return std::find(dfsChannels.begin(), dfsChannels.end(), channel) != dfsChannels.end();
}

/* Get available channels for current radio */
std::vector<int> WifiChannelOptimization::getAvailableChannels() {
    std::vector<int> channels;

    int currentChannel = get<INT_T>(CURRENT_CHANNEL_1);
    int band = getBandFromChannel(currentChannel);

    if (band == 2) {
        // 2.4 GHz channels (1-14, typically 1-11 in US)
        for (int ch = 1; ch <= 11; ch++) {
            channels.push_back(ch);
        }
    } else if (band == 5) {
        // 5 GHz channels (36-165)
        // UNII-1
        for (int ch = 36; ch <= 48; ch += 4) {
            channels.push_back(ch);
        }
        // UNII-2A (DFS)
        for (int ch = 52; ch <= 64; ch += 4) {
            channels.push_back(ch);
        }
        // UNII-2C (DFS)
        for (int ch = 100; ch <= 144; ch += 4) {
            channels.push_back(ch);
        }
        // UNII-3
        for (int ch = 149; ch <= 165; ch += 4) {
            channels.push_back(ch);
        }
    } else if (band == 6) {
        // 6 GHz channels (1-233)
        for (int ch = 1; ch <= 233; ch += 4) {
            channels.push_back(ch);
        }
    }

    return channels;
}

/* Get band from channel number */
int WifiChannelOptimization::getBandFromChannel(int channel) {
    if (channel >= 1 && channel <= 14) {
        return 2;  // 2.4 GHz
    } else if (channel >= 36 && channel <= 165) {
        return 5;  // 5 GHz
    } else if (channel >= 1 && channel <= 233) {
        return 6;  // 6 GHz (overlaps with 2.4, check context)
    }
    return 0;  // Unknown
}

/* Detect neighboring APs on specific channel */
int WifiChannelOptimization::detectNeighboringAPs(int channel) {
#ifdef OPENWRT_BUILD
    std::string iface = get<STRING_T>(INTERFACE_0);

    std::stringstream cmd;
    cmd << "iw dev | grep -A 1 \"" << iface << "\" | grep Interface | awk '{print $2}'";
    std::string phyIface = executeCommand(cmd.str());

    if (phyIface.empty()) {
        return 0;
    }

    cmd.str("");
    cmd << "iw dev " << phyIface << " scan 2>/dev/null | grep -E \"freq:|channel\" | wc -l";
    std::string result = executeCommand(cmd.str());

    return result.empty() ? 0 : std::stoi(result) / 2;
#else
    return 0;
#endif
}

/* Measure current channel utilization */
int WifiChannelOptimization::measureChannelUtilization() {
#ifdef OPENWRT_BUILD
    std::string iface = get<STRING_T>(INTERFACE_0);

    std::stringstream cmd;
    cmd << "iw dev | grep -A 1 \"" << iface << "\" | grep Interface | awk '{print $2}'";
    std::string phyIface = executeCommand(cmd.str());

    if (phyIface.empty()) {
        return 0;
    }

    cmd.str("");
    cmd << "iw dev " << phyIface << " survey dump 2>/dev/null | grep -A 5 \"in use\" | grep \"channel busy time\" | awk '{print $4}'";
    std::string busyTime = executeCommand(cmd.str());

    cmd.str("");
    cmd << "iw dev " << phyIface << " survey dump 2>/dev/null | grep -A 5 \"in use\" | grep \"channel active time\" | awk '{print $4}'";
    std::string activeTime = executeCommand(cmd.str());

    if (busyTime.empty() || activeTime.empty()) {
        return 0;
    }

    double busy = std::stod(busyTime);
    double active = std::stod(activeTime);

    if (active == 0) {
        return 0;
    }

    return static_cast<int>((busy / active) * 100.0);
#else
    return 0;
#endif
}

/* Measure noise floor */
int WifiChannelOptimization::measureNoiseFloor() {
#ifdef OPENWRT_BUILD
    std::string iface = get<STRING_T>(INTERFACE_0);

    std::stringstream cmd;
    cmd << "iw dev | grep -A 1 \"" << iface << "\" | grep Interface | awk '{print $2}'";
    std::string phyIface = executeCommand(cmd.str());

    if (phyIface.empty()) {
        return -95;
    }

    cmd.str("");
    cmd << "iw dev " << phyIface << " survey dump 2>/dev/null | grep -A 5 \"in use\" | grep \"noise:\" | awk '{print $2}'";
    std::string noise = executeCommand(cmd.str());

    return noise.empty() ? -95 : std::stoi(noise);
#else
    return -95;
#endif
}

/* Calculate interference level (0-100) */
int WifiChannelOptimization::calculateInterferenceLevel() {
    int neighbors = get<INT_T>(NEIGHBORING_APS_5);
    int utilization = get<INT_T>(CHANNEL_UTILIZATION_6);
    int noiseFloor = get<INT_T>(NOISE_FLOOR_7);

    // Composite interference score
    double interference = 0.0;

    // Neighbor contribution (0-40 points)
    interference += std::min(40.0, neighbors * 4.0);

    // Utilization contribution (0-40 points)
    interference += std::min(40.0, utilization * 0.4);

    // Noise contribution (0-20 points)
    // -95 dBm is good (0 points), -75 dBm is bad (20 points)
    double noiseScore = std::max(0.0, (noiseFloor + 95.0));
    interference += std::min(20.0, noiseScore);

    return static_cast<int>(std::min(100.0, interference));
}

/* Check DFS state */
bool WifiChannelOptimization::checkDFSState() {
#ifdef OPENWRT_BUILD
    if (!get<BOOL_T>(DFS_ENABLED_9)) {
        set<STRING_T>(DFS_STATE_10, "Disabled");
        return true;
    }

    std::string iface = get<STRING_T>(INTERFACE_0);

    std::stringstream cmd;
    cmd << "iw dev | grep -A 1 \"" << iface << "\" | grep Interface | awk '{print $2}'";
    std::string phyIface = executeCommand(cmd.str());

    if (phyIface.empty()) {
        return false;
    }

    cmd.str("");
    cmd << "iw dev " << phyIface << " info 2>/dev/null | grep \"channel\"";
    std::string channelInfo = executeCommand(cmd.str());

    if (channelInfo.find("DFS") != std::string::npos) {
        if (channelInfo.find("CAC") != std::string::npos) {
            set<STRING_T>(DFS_STATE_10, "CAC");
        } else if (channelInfo.find("NOP") != std::string::npos) {
            set<STRING_T>(DFS_STATE_10, "NOP");
        } else {
            set<STRING_T>(DFS_STATE_10, "Available");
        }
    } else {
        set<STRING_T>(DFS_STATE_10, "Available");
    }

    return true;
#else
    return false;
#endif
}

/* Check for radar detection */
bool WifiChannelOptimization::checkRadarDetection() {
#ifdef OPENWRT_BUILD
    // Check system logs for radar detection events
    std::string logCheck = executeCommand("logread | grep -i \"radar detected\" | tail -1");

    bool radarDetected = !logCheck.empty();
    set<BOOL_T>(RADAR_DETECTED_11, radarDetected);

    if (radarDetected) {
        WPP_LOGW(TAG, "Radar detected on current channel");
        set<STRING_T>(DFS_STATE_10, "NOP");
    }

    return radarDetected;
#else
    return false;
#endif
}

/* Get interface information */
bool WifiChannelOptimization::getInterfaceInfo() {
#ifdef OPENWRT_BUILD
    std::string iface = get<STRING_T>(INTERFACE_0);

    std::stringstream cmd;
    cmd << "iw dev | grep -A 1 \"" << iface << "\" | grep Interface | awk '{print $2}'";
    std::string phyIface = executeCommand(cmd.str());

    if (phyIface.empty()) {
        WPP_LOGW(TAG, "Could not find physical interface for %s", iface.c_str());
        return false;
    }

    cmd.str("");
    cmd << "iw dev " << phyIface << " info 2>/dev/null";
    std::string info = executeCommand(cmd.str());

    // Parse channel from info
    std::regex channelRegex("channel (\\d+)");
    std::smatch match;
    if (std::regex_search(info, match, channelRegex)) {
        int channel = std::stoi(match[1].str());
        set<INT_T>(CURRENT_CHANNEL_1, channel);
    }

    return true;
#else
    return false;
#endif
}

/* Set wireless configuration */
bool WifiChannelOptimization::setWirelessConfig(int channel, int width) {
#ifdef OPENWRT_BUILD
    std::string iface = get<STRING_T>(INTERFACE_0);

    // Set channel
    std::stringstream cmd;
    cmd << "uci set wireless." << iface << ".channel='" << channel << "'";
    system(cmd.str().c_str());

    // Set channel width
    std::string htmode;
    int band = getBandFromChannel(channel);

    if (band == 2) {
        if (width == 40) {
            htmode = "HT40";
        } else {
            htmode = "HT20";
        }
    } else if (band == 5) {
        if (width == 160) {
            htmode = "VHT160";
        } else if (width == 80) {
            htmode = "VHT80";
        } else if (width == 40) {
            htmode = "VHT40";
        } else {
            htmode = "VHT20";
        }
    } else if (band == 6) {
        if (width == 160) {
            htmode = "HE160";
        } else if (width == 80) {
            htmode = "HE80";
        } else if (width == 40) {
            htmode = "HE40";
        } else {
            htmode = "HE20";
        }
    }

    cmd.str("");
    cmd << "uci set wireless." << iface << ".htmode='" << htmode << "'";
    system(cmd.str().c_str());

    // Commit changes
    system("uci commit wireless");

    return true;
#else
    return false;
#endif
}

/* Reload wireless configuration */
bool WifiChannelOptimization::reloadWireless() {
#ifdef OPENWRT_BUILD
    int result = system("wifi reload");

    if (result != 0) {
        WPP_LOGE(TAG, "Failed to reload wireless");
        return false;
    }

    // Wait for wireless to stabilize
    sleep(5);

    // Reload current state
    return loadCurrentState();
#else
    return false;
#endif
}

/* Execute shell command and return output */
std::string WifiChannelOptimization::executeCommand(const std::string& cmd) {
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        return result;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);

    // Trim trailing newline
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

/* Split string by delimiter */
std::vector<std::string> WifiChannelOptimization::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

/* Validate channel number */
bool WifiChannelOptimization::isValidChannel(int channel) {
    // 2.4 GHz: 1-14
    if (channel >= 1 && channel <= 14) {
        return true;
    }

    // 5 GHz: 36-165
    if (channel >= 36 && channel <= 165) {
        return true;
    }

    // 6 GHz: 1-233
    if (channel >= 1 && channel <= 233) {
        return true;
    }

    return false;
}

/* Validate channel width */
bool WifiChannelOptimization::isValidWidth(int width) {
    return width == 20 || width == 40 || width == 80 || width == 160;
}

/* Update channel performance history */
void WifiChannelOptimization::updateChannelHistory(int channel, double performance) {
    if (channelPerformanceHistory[channel].size() >= 100) {
        // Keep only last 100 entries
        channelPerformanceHistory[channel].erase(channelPerformanceHistory[channel].begin());
    }

    channelPerformanceHistory[channel].push_back(performance);
}

/* Get historical performance for channel */
double WifiChannelOptimization::getHistoricalPerformance(int channel) {
    if (channelPerformanceHistory.find(channel) == channelPerformanceHistory.end() ||
        channelPerformanceHistory[channel].empty()) {
        return 0.0;
    }

    // Calculate average historical performance
    double sum = 0.0;
    for (double perf : channelPerformanceHistory[channel]) {
        sum += perf;
    }

    return sum / channelPerformanceHistory[channel].size();
}

#endif // OBJ_W_33423_WIFI_CHANNEL_OPTIMIZATION
