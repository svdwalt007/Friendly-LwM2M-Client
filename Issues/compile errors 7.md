cmake ../examples -DCMAKE_BUILD_TYPE=MinSizeRel
-- The CXX compiler identification is GNU 13.3.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Building for 64 bit system
-- Exceptions are not supported
-- RTTI is not supported
CMake Warning (dev) at /mnt/d/Dev/Friendly-LwM2M-Client/wpp/configs/wpp_config.cmake:142 (set):
  Cannot set "WPP_DEFINITIONS": current scope has no parent.
Call Stack (most recent call first):
  CMakeLists.txt:13 (include)
This warning is for project developers.  Use -Wno-dev to suppress it.

-- Found CURL: /usr/lib/x86_64-linux-gnu/libcurl.so (found version "8.5.0")
-- Found OpenSSL: /usr/lib/x86_64-linux-gnu/libcrypto.so (found version "3.0.13")
-- WppExample: Endiannes not set, defaulting to little endian
-- libcoap built successfully. Library path: /mnt/d/Dev/Friendly-LwM2M-Client/examples/coap/libcoap/build
-- Using example Firmware Updater
-- Found Git: /usr/bin/git (found version "2.43.0")
-- Configuring done (13.6s)
-- Generating done (6.6s)
-- Build files have been written to: /mnt/d/Dev/Friendly-LwM2M-Client/_build
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/_build$ make -j$(nproc)
[  7%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp.o
[  7%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/object/Object.cpp.o
[ 11%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.cpp.o
[ 14%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/Resource.cpp.o
[ 18%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/ResourceContainer.cpp.o
[ 22%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_3_device/Device.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_6_location/Location.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/platform/connection_dtls/Connection.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/platform/Platform.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/platform/Guard.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/WppPlatform.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/main.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/objects.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/bindings/WppBindings.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp.o
[ 92%] Building CXX object CMakeFiles/WppExample.dir/FirmwareUpdater/FirmwareUpdater.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp: In lambda function:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:88:99: error: declaration of ‘task’ shadows a previous local [-Werror=shadow]
   88 |         auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [id](TaskInfo *task) { return task->id == id; });
      |                                                                                         ~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:88:14: note: shadowed declaration is here
   88 |         auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [id](TaskInfo *task) { return task->id == id; });
      |              ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp: In lambda function:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:98:99: error: declaration of ‘task’ shadows a previous local [-Werror=shadow]
   98 |         auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [id](TaskInfo *task) { return task->id == id; });
      |                                                                                         ~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:98:14: note: shadowed declaration is here
   98 |         auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [id](TaskInfo *task) { return task->id == id; });
      |              ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp: In lambda function:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:112:99: error: declaration of ‘task’ shadows a previous local [-Werror=shadow]
  112 |         auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [id](TaskInfo *task) { return task->id == id; });
      |                                                                                         ~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:112:14: note: shadowed declaration is here
  112 |         auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [id](TaskInfo *task) { return task->id == id; });
      |              ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp: In lambda function:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:126:99: error: declaration of ‘task’ shadows a previous local [-Werror=shadow]
  126 |         auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [id](TaskInfo *task) { return task->id == id; });
      |                                                                                         ~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:126:14: note: shadowed declaration is here
  126 |         auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [id](TaskInfo *task) { return task->id == id; });
      |              ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp: In lambda function:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:140:99: error: declaration of ‘task’ shadows a previous local [-Werror=shadow]
  140 |         auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [id](TaskInfo *task) { return task->id == id; });
      |                                                                                         ~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:140:14: note: shadowed declaration is here
  140 |         auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [id](TaskInfo *task) { return task->id == id; });
      |              ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp: In lambda function:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:251:110: error: declaration of ‘task’ shadows a previous local [-Werror=shadow]
  251 |                 auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [newId](TaskInfo *task) { return task->id == newId; });
      |                                                                                                    ~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:251:22: note: shadowed declaration is here
  251 |                 auto task = std::find_if(_instance._tasks.begin(), _instance._tasks.end(), [newId](TaskInfo *task) { return task->id == newId; });
      |                      ^~~~
At top level:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:380: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:128: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_6_location/Location.cpp:14:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/object/Object.cpp:2:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:142: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/object/Object.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:212: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:310: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_6_location/Location.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:282: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:156: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:254: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_3_device/Device.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:226: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_3_device/Device.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:240: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10519_hardware_watchdog/HardwareWatchdog.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.h:4,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.cpp:1:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:408: CMakeFiles/WppExample.dir/objects.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:296: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:1:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:198: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp.o] Error 1
make[2]: *** [CMakeFiles/WppExample.dir/build.make:324: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:268: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:338: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:6,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:1:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:422: CMakeFiles/WppExample.dir/platform/connection_dtls/Connection.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:6,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:5:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:394: CMakeFiles/WppExample.dir/main.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.h:55,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/FirmwareUpdater/FirmwareUpdater.h:4,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/FirmwareUpdater/FirmwareUpdater.cpp:1:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_10512_starlink_terminal/StarlinkTerminal.h:20:10: fatal error: data_model/Instance.h: No such file or directory
   20 | #include "data_model/Instance.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/WppExample.dir/build.make:464: CMakeFiles/WppExample.dir/FirmwareUpdater/FirmwareUpdater.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1186: CMakeFiles/WppExample.dir/all] Error 2
make: *** [Makefile:91: all] Error 2