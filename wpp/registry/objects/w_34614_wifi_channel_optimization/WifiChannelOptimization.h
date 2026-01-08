/*
 * WifiChannelOptimization (Object 34614)
 * Walt Technologies - OpenWRT One Router
 *
 * WiFi Channel Optimization and Automatic Channel Selection for OpenWRT
 */

#ifndef WPP_W_34614_WIFI_CHANNEL_OPTIMIZATION_H
#define WPP_W_34614_WIFI_CHANNEL_OPTIMIZATION_H

#include "WifiChannelOptimizationConfig.h"
#include "WifiChannelOptimizationInfo.h"
#include "Instance.h"
#include "InstSubject.h"
#include <map>
#include <vector>
#include <string>

namespace wpp {

class WppClient;
class Object;

class WifiChannelOptimization : public Instance {
public:
    enum ID: ID_T {
        INTERFACE_0 = 0,              // Interface name (RW, String)
        CURRENT_CHANNEL_1 = 1,        // Current Channel (R, Integer)
        CURRENT_WIDTH_2 = 2,          // Current Width (R, Integer) - 20/40/80/160 MHz
        AUTO_CHANNEL_3 = 3,           // Auto Channel (RW, Boolean)
        SCAN_INTERVAL_4 = 4,          // Scan Interval (RW, Integer, seconds)
        NEIGHBORING_APS_5 = 5,        // Neighboring APs (R, Integer, count)
        CHANNEL_UTILIZATION_6 = 6,    // Channel Utilization (R, Integer, percentage)
        NOISE_FLOOR_7 = 7,            // Noise Floor (R, Integer, dBm)
        INTERFERENCE_LEVEL_8 = 8,     // Interference Level (R, Integer, 0-100)
        DFS_ENABLED_9 = 9,            // DFS Enabled (RW, Boolean)
        DFS_STATE_10 = 10,            // DFS State (R, String - CAC/Available/NOP)
        RADAR_DETECTED_11 = 11,       // Radar Detected (R, Boolean)
        BEST_2G_CHANNEL_12 = 12,      // Best 2.4G Channel (R, Integer)
        BEST_5G_CHANNEL_13 = 13,      // Best 5G Channel (R, Integer)
        BEST_6G_CHANNEL_14 = 14,      // Best 6G Channel (R, Integer)
        CHANNEL_SURVEY_15 = 15,       // Channel Survey (R, Multiple, String - channel:busy:noise)
        LAST_SCAN_TIME_16 = 16,       // Last Scan Time (R, Time)
        LAST_OPTIMIZATION_17 = 17,    // Last Optimization (R, Time)
        SCAN_NOW_18 = 18,             // Scan Now (E, Execute)
        OPTIMIZE_CHANNEL_19 = 19,     // Optimize Channel (E, Execute)
        SET_CHANNEL_20 = 20,          // Set Channel (E, Execute with argument)
        ENABLE_DFS_21 = 21            // Enable DFS (E, Execute)
    };

    // DFS State Enumeration
    enum DFSState: uint8_t {
        DFS_CAC = 0,           // Channel Availability Check in progress
        DFS_AVAILABLE = 1,     // DFS channel available for use
        DFS_NOP = 2            // Non-Occupancy Period (radar detected)
    };

    // Channel Score Structure
    struct ChannelScore {
        int channel;
        int width;
        double score;
        int neighborCount;
        int utilization;
        int noiseFloor;
        bool dfsRequired;
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId);
    static bool remove(WppClient& client, INST_T instId);

    /* Instance lifecycle */
    WifiChannelOptimization(Object& object, INST_T instId);
    ~WifiChannelOptimization() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool scanNow(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool optimizeChannel(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool setChannel(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool enableDFS(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    bool performChannelScan();
    bool performChannelSurvey();
    bool loadCurrentState();
    bool applyChannelChange(int channel, int width);

    /* Channel analysis methods */
    std::vector<ChannelScore> calculateChannelScores();
    ChannelScore scoreChannel(int channel, int width);
    bool isChannelDFS(int channel);
    std::vector<int> getAvailableChannels();
    int getBandFromChannel(int channel);

    /* Interference detection */
    int detectNeighboringAPs(int channel);
    int measureChannelUtilization();
    int measureNoiseFloor();
    int calculateInterferenceLevel();

    /* DFS support */
    bool checkDFSState();
    bool checkRadarDetection();

    /* OpenWRT integration */
    bool getInterfaceInfo();
    bool setWirelessConfig(int channel, int width);
    bool reloadWireless();

    /* Utility methods */
    std::string executeCommand(const std::string& cmd);
    std::vector<std::string> splitString(const std::string& str, char delimiter);
    bool isValidChannel(int channel);
    bool isValidWidth(int width);

    /* Historical data for ML optimization */
    std::map<int, std::vector<double>> channelPerformanceHistory;
    void updateChannelHistory(int channel, double performance);
    double getHistoricalPerformance(int channel);
};

} // namespace wpp

#endif // WPP_W_34614_WIFI_CHANNEL_OPTIMIZATION_H
