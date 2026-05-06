make -j$(nproc)
[  3%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp.o
[  7%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/object/Object.cpp.o
[ 11%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.cpp.o
[ 14%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp.o
[ 18%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o
[ 22%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_3_device/Device.cpp.o
[ 25%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.cpp.o
[ 29%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp.o
[ 33%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp.o
[ 37%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o
[ 40%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 44%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_6_location/Location.cpp.o
[ 48%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp.o
[ 51%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp.o
[ 55%] Building CXX object CMakeFiles/WppExample.dir/main.cpp.o
[ 62%] Building CXX object CMakeFiles/WppExample.dir/objects.cpp.o
[ 62%] Building CXX object CMakeFiles/WppExample.dir/platform/connection_dtls/Connection.cpp.o
[ 66%] Building CXX object CMakeFiles/WppExample.dir/FirmwareUpdater/FirmwareUpdater.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_3_device/Device.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/object/Object.cpp:2:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:1:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp: In member function ‘bool wpp::ConnectivityMonitoring::checkLinkQuality(uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp:263:53: error: comparison is always true due to limited range of data type [-Werror=type-limits]
  263 |                         return LINK_QUALITY_GSM_MIN <= linkQuality && linkQuality <= LINK_QUALITY_GSM_MAX;
      |                                                     ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp:265:51: error: comparison is always true due to limited range of data type [-Werror=type-limits]
  265 |             return LINK_QUALITY_IEEE_802_15_4_MIN <= linkQuality && linkQuality <= LINK_QUALITY_IEEE_802_15_4_MAX;
      |                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp:265:81: error: comparison is always true due to limited range of data type [-Werror=type-limits]
  265 |             return LINK_QUALITY_IEEE_802_15_4_MIN <= linkQuality && linkQuality <= LINK_QUALITY_IEEE_802_15_4_MAX;
      |                                                                                 ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp: In member function ‘bool wpp::ConnectivityMonitoring::checkCellId(uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp:282:36: error: comparison of unsigned expression in ‘>= 0’ is always true [-Werror=type-limits]
  282 |             return CELL_ID_GSM_MIN <= cellId && cellId <= CELL_ID_GSM_MAX;
      |                                    ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp:285:50: error: comparison of unsigned expression in ‘>= 0’ is always true [-Werror=type-limits]
  285 |                         return CELL_ID_WCDMA_MIN <= cellId && cellId <= CELL_ID_WCDMA_MAX;
      |                                                  ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp: In constructor ‘wpp::WppRegistry::WppRegistry(lwm2m_context_t&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:51:71: error: ‘STARLINK_TERMINAL_OBJ_INFO’ was not declared in this scope; did you mean ‘STARLINK_TERMINAL_H’?
   51 |         _objects.push_back(new ObjectImpl<StarlinkTerminal>(_context, STARLINK_TERMINAL_OBJ_INFO));
      |                                                                       ^~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                                       STARLINK_TERMINAL_H
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:54:63: error: ‘MIKROBUS_OBJ_INFO’ was not declared in this scope
   54 |         _objects.push_back(new ObjectImpl<Mikrobus>(_context, MIKROBUS_OBJ_INFO));
      |                                                               ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp: In member function ‘wpp::Object& wpp::WppRegistry::hardwareWatchdog()’:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:175:32: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
  175 |         return *object(OBJ_ID::HARDWARE_WATCHDOG);
      |                                ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp: In member function ‘wpp::Object& wpp::WppRegistry::starlinkTerminal()’:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:183:32: error: ‘STARLINK_TERMINAL’ is not a member of ‘wpp::OBJ_ID’
  183 |         return *object(OBJ_ID::STARLINK_TERMINAL);
      |                                ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp: In member function ‘wpp::Object& wpp::WppRegistry::mikrobus()’:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:188:32: error: ‘MIKROBUS’ is not a member of ‘wpp::OBJ_ID’
  188 |         return *object(OBJ_ID::MIKROBUS);
      |                                ^~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_6_location/Location.cpp:14:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
At global scope:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
At global scope:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp: In lambda function:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:357:33: error: declaration of ‘pkg’ shadows a previous local [-Werror=shadow]
  357 |                 const OPAQUE_T &pkg = resource(PACKAGE_0)->get<OPAQUE_T>();
      |                                 ^~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:346:25: note: shadowed declaration is here
  346 |         const OPAQUE_T &pkg = resource(PACKAGE_0)->get<OPAQUE_T>();
      |                         ^~~
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
At global scope:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:128: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
make[2]: *** [CMakeFiles/WppExample.dir/build.make:142: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/object/Object.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:156: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:198: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:212: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:226: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_3_device/Device.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:254: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:268: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:282: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:296: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:310: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_6_location/Location.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:324: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:338: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/FirmwareUpdater/FirmwareUpdater.h:4,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/FirmwareUpdater/FirmwareUpdater.cpp:1:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
make[2]: *** [CMakeFiles/WppExample.dir/build.make:464: CMakeFiles/WppExample.dir/FirmwareUpdater/FirmwareUpdater.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.cpp:7:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
make[2]: *** [CMakeFiles/WppExample.dir/build.make:240: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:6,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:1:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:10:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:112:5: error: C++ designated initializers only available with ‘-std=c++20’ or ‘-std=gnu++20’ -Werror=c++20-extensions]
  112 |     .write = send_to_peer,
      |     ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:113:5: error: C++ designated initializers only available with ‘-std=c++20’ or ‘-std=gnu++20’ -Werror=c++20-extensions]
  113 |     .read  = read_from_peer,
      |     ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:114:5: error: C++ designated initializers only available with ‘-std=c++20’ or ‘-std=gnu++20’ -Werror=c++20-extensions]
  114 |     .event = NULL,
      |     ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:116:5: error: C++ designated initializers only available with ‘-std=c++20’ or ‘-std=gnu++20’ -Werror=c++20-extensions]
  116 |     .get_psk_info = get_psk_info,
      |     ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:121:1: error: missing initializer for member ‘dtls_handler_t::get_ecdsa_key’ [-Werror=missing-field-initializers]
  121 | };
      | ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:121:1: error: missing initializer for member ‘dtls_handler_t::verify_ecdsa_key’ [-Werror=missing-field-initializers]
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘Connection::dtls_connection_t* Connection::createNewConn(sockaddr*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:322:15: error: ‘void* memset(void*, int, size_t)’ clearing an object of type ‘struct Connection::dtls_connection_t’ with no trivial copy-assignment; use assignment or value-initialization instead [-Werror=class-memaccess]
  322 |         memset(conn, 0, sizeof(dtls_connection_t));
      |         ~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:29:12: note: ‘struct Connection::dtls_connection_t’ declared here
   29 |     struct dtls_connection_t {
      |            ^~~~~~~~~~~~~~~~~
At global scope:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:422: CMakeFiles/WppExample.dir/platform/connection_dtls/Connection.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.h:4,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.cpp:1:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.cpp:2:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/cli_options.h: In function ‘bool parseCliOptions(int, char**, CliOptions&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/cli_options.h:429:16: error: switch missing default case [-Werror=switch-default]
  429 |         switch (options.security_mode) {
      |                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.cpp: In function ‘void securityInit(wpp::WppClient&, const CliOptions&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.cpp:66:12: error: switch missing default case [-Werror=switch-default]
   66 |     switch (options.security_mode) {
      |            ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:49,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:6,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:5:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdogInfo.h:19:17: error: ‘HARDWARE_WATCHDOG’ is not a member of ‘wpp::OBJ_ID’
   19 |         OBJ_ID::HARDWARE_WATCHDOG,
      |                 ^~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:153:52: error: ‘OBJ_INST_ID_T’ has not been declared
  153 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId = 0);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:180:41: error: ‘OBJ_INST_ID_T’ has not been declared
  180 |     StarlinkTerminal(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                         ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:185:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  185 |     EXECUTE_RESULT executeReboot(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:186:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  186 |     EXECUTE_RESULT executeSpeedTest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:187:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  187 |     EXECUTE_RESULT executeFactoryReset(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:188:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  188 |     EXECUTE_RESULT executeGetObstructionMap(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:189:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  189 |     EXECUTE_RESULT executeRefreshTelemetry(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:158:10: error: ‘void wpp::StarlinkTerminal::resourcesCreate()’ marked ‘override’, but does not override
  158 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:163:10: error: ‘void wpp::StarlinkTerminal::resourcesInit()’ marked ‘overrid’, but does not override
  163 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:58:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:106:52: error: ‘OBJ_INST_ID_T’ has not been declared
  106 |     static Instance* createInst(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                                    ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:133:33: error: ‘OBJ_INST_ID_T’ has not been declared
  133 |     Mikrobus(WppClient& client, OBJ_INST_ID_T instanceId);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:138:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  138 |     EXECUTE_RESULT executeInitClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:139:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  139 |     EXECUTE_RESULT executeResetClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:140:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  140 |     EXECUTE_RESULT executeShutdownClickBoard(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:141:5: error: ‘EXECUTE_RESULT’ does not name a type; did you mean ‘EXECUTE_T’?
  141 |     EXECUTE_RESULT executeReadManifest(Instance& inst, ID_T resId, const OPAQUE_T& data);
      |     ^~~~~~~~~~~~~~
      |     EXECUTE_T
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:168:5: error: ‘OBJ_INST_ID_T’ does not name a type
  168 |     OBJ_INST_ID_T _socketId;
      |     ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:111:10: error: ‘void wpp::Mikrobus::resourcesCreate()’ marked ‘override’, but does not override
  111 |     void resourcesCreate() override;
      |          ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10520_mikrobus/Mikrobus.h:116:10: error: ‘void wpp::Mikrobus::resourcesInit()’ marked ‘override’, but does not override
  116 |     void resourcesInit() override;
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:10:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:7:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/cli_options.h: In function ‘bool parseCliOptions(int, char**, CliOptions&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/cli_options.h:429:16: error: switch missing default case [-Werror=switch-default]
  429 |         switch (options.security_mode) {
      |                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp: In function ‘void wppErrorHandler(wpp::WppClient&, int)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:34:29: error: too few arguments to function ‘void securityInit(wpp::WppClient&, const CliOptions&)’
   34 |                 securityInit(client);
      |                 ~~~~~~~~~~~~^~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.h:21:6: note: declared here
   21 | void securityInit(WppClient &client, const CliOptions& options);
      |      ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:35:27: error: too few arguments to function ‘void serverInit(wpp::WppClient&, const CliOptions&)’
   35 |                 serverInit(client);
      |                 ~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.h:19:6: note: declared here
   19 | void serverInit(WppClient &client, const CliOptions& options);
      |      ^~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp: In function ‘int main(int, char**)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:61:24: error: switch missing default case [-Werror=switch-default]
   61 |                 switch (options.security_mode) {
      |                        ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp: In lambda function:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:171:48: error: declaration of ‘client’ shadows a previous local [-Werror=shadow]
  171 |         WppTaskQueue::addTask(5, [](WppClient &client, void *ctx) {
      |                                     ~~~~~~~~~~~^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:81:20: note: shadowed declaration is here
   81 |         WppClient *client = WppClient::takeOwnershipBlocking();
      |                    ^~~~~~
At global scope:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:408: CMakeFiles/WppExample.dir/objects.cpp.o] Error 1
At global scope:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:394: CMakeFiles/WppExample.dir/main.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1186: CMakeFiles/WppExample.dir/all] Error 2
make: *** [Makefile:91: all] Error 2