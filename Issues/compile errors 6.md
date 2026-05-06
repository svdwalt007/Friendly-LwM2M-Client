make -j$(nproc)
[  2%] Built target submodule_update
[  4%] Performing autoheader step for 'external_tinydtls'
[  6%] No download step for 'external_tinydtls'
[  8%] No update step for 'external_tinydtls'
autoreconf: export WARNINGS=
autoreconf: Entering directory '.'
autoreconf: configure.ac: not using Gettext
[ 10%] No patch step for 'external_tinydtls'
autoreconf: running: aclocal --force
autoreconf: configure.ac: tracing
autoreconf: configure.ac: not using Libtool
autoreconf: configure.ac: not using Intltool
autoreconf: configure.ac: not using Gtkdoc
autoreconf: running: /usr/bin/autoconf --force
autoreconf: running: /usr/bin/autoheader --force
autoreconf: configure.ac: not using Automake
autoreconf: Leaving directory '.'
[ 12%] Performing configure step for 'external_tinydtls'
-- external_tinydtls configure command succeeded.  See also /mnt/d/Dev/Friendly-LwM2M-Client/_build/external_tinydtls-prefix/src/external_tinydtls-stamp/external_tinydtls-configure-*.log
[ 14%] No build step for 'external_tinydtls'
[ 16%] No install step for 'external_tinydtls'
[ 18%] Completed 'external_tinydtls'
[ 20%] Built target external_tinydtls
[ 22%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp.o
[ 25%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/object/Object.cpp.o
[ 27%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.cpp.o
[ 29%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/Resource.cpp.o
[ 31%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/ResourceContainer.cpp.o
[ 33%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp.o
[ 35%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o
[ 37%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_3_device/Device.cpp.o
[ 39%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.cpp.o
[ 41%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.cpp.o
[ 43%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_13_bearer_selection/BearerSelection.cpp.o
[ 45%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp.o
[ 47%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp.o
[ 50%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o
[ 52%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 54%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_6_location/Location.cpp.o
[ 56%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10513_router_management/RouterManagement.cpp.o
[ 58%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10514_ethernet_interface/EthernetInterface.cpp.o
[ 60%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10515_gpio_control/GpioControl.cpp.o
[ 62%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10516_usb_management/UsbManagement.cpp.o
[ 64%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10517_storage_management/StorageManagement.cpp.o
[ 66%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10518_system_monitor/SystemMonitor.cpp.o
[ 68%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10521_firewall_config/FirewallConfig.cpp.o
[ 70%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10522_poe_management/PoeManagement.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_13_bearer_selection/BearerSelection.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_13_bearer_selection/BearerSelection.cpp:9:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_13_bearer_selection/BearerSelectionInfo.h:10:10: fatal error: Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:272: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_13_bearer_selection/BearerSelection.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:258: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10513_router_management/RouterManagement.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10513_router_management/RouterManagement.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10513_router_management/RouterManagementInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10514_ethernet_interface/EthernetInterface.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10514_ethernet_interface/EthernetInterface.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10514_ethernet_interface/EthernetInterfaceInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10517_storage_management/StorageManagement.h:10,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10517_storage_management/StorageManagement.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10517_storage_management/StorageManagementInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
compilation terminated.
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:356: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10513_router_management/RouterManagement.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:370: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10514_ethernet_interface/EthernetInterface.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10518_system_monitor/SystemMonitor.h:10,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10518_system_monitor/SystemMonitor.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10518_system_monitor/SystemMonitorInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10515_gpio_control/GpioControl.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10515_gpio_control/GpioControl.cpp:7:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10515_gpio_control/GpioControlInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10521_firewall_config/FirewallConfig.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10521_firewall_config/FirewallConfig.cpp::
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10521_firewall_config/FirewallConfigInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10516_usb_management/UsbManagement.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10516_usb_management/UsbManagement.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10516_usb_management/UsbManagementInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
compilation terminated.
compilation terminated.
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:412: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10517_storage_management/StorageManagement.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:384: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10515_gpio_control/GpioControl.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:398: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10516_usb_management/UsbManagement.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:426: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10518_system_monitor/SystemMonitor.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:440: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10521_firewall_config/FirewallConfig.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10522_poe_management/PoeManagement.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10522_poe_management/PoeManagement.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10522_poe_management/PoeManagementInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:454: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10522_poe_management/PoeManagement.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/WppLogs.h:4,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/Resource.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/Resource.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/WppPlatform.h: In static member function ‘static void wpp::WppPlatform::print(const char*, ...)’:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/WppPlatform.h:64:9: error: ‘va_start’ was not declared in this scope
   64 |         va_start(ap, msg);
      |         ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/WppPlatform.h:66:9: error: ‘va_end’ was not declared in this scope
   66 |         va_end(ap);
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/WppLogs.h:4,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/Resource.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/ResourceContainer.h:6,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/ResourceContainer.cpp:1:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/WppPlatform.h: In static member function ‘static void wpp::WppPlatform::print(const char*, ...)’:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/WppPlatform.h:64:9: error: ‘va_start’ was not declared in this scope
   64 |         va_start(ap, msg);
      |         ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/WppPlatform.h:66:9: error: ‘va_end’ was not declared in this scope
   66 |         va_end(ap);
      |         ^~~~~~
At global scope:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
make[2]: *** [CMakeFiles/WppExample.dir/build.make:188: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/ResourceContainer.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:202: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:216: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o] Error 1
At global scope:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
make[2]: *** [CMakeFiles/WppExample.dir/build.make:174: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/Resource.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_6_location/Location.cpp:14:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:342: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_6_location/Location.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_3_device/Device.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:230: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_3_device/Device.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/object/Object.cpp:2:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
compilation terminated.
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:146: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/object/Object.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:160: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:314: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:328: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:300: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:286: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:132: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivity.h:1,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:34,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_12_wlan_connectivity/WlanConnectivityInfo.h:10:10: fatal error:Lwm2mObjectBase.h: No such file or directory
   10 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:244: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1189: CMakeFiles/WppExample.dir/all] Error 2
make: *** [Makefile:91: all] Error 2