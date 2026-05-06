cmake ../examples  # Reconfigure to enable C compiler
-- The C compiler identification is GNU 13.3.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Building for 64 bit system
-- Exceptions are not supported
-- RTTI is not supported
CMake Warning (dev) at /mnt/d/Dev/Friendly-LwM2M-Client/wpp/configs/wpp_config.cmake:142 (set):
  Cannot set "WPP_DEFINITIONS": current scope has no parent.
Call Stack (most recent call first):
  CMakeLists.txt:13 (include)
This warning is for project developers.  Use -Wno-dev to suppress it.

-- WppExample: Endiannes not set, defaulting to little endian
-- libcoap built successfully. Library path: /mnt/d/Dev/Friendly-LwM2M-Client/examples/coap/libcoap/build
-- Using example Firmware Updater
-- Configuring done (3.3s)
-- Generating done (3.7s)
-- Build files have been written to: /mnt/d/Dev/Friendly-LwM2M-Client/_build
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/_build$ make -j$(nproc)
[  1%] Building CXX object CMakeFiles/WppExample.dir/FirmwareUpdater/FirmwareUpdater.cpp.o
[  2%] Building CXX object CMakeFiles/WppExample.dir/main.cpp.o
[  4%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp.o
[  5%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/WppPlatform.cpp.o
[  6%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp.o
[  8%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp.o
[  9%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.cpp.o
[ 10%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/object/Object.cpp.o
[ 12%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/Resource.cpp.o
[ 13%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/ResourceContainer.cpp.o
[ 14%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp.o
[ 16%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o
[ 17%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_3_device/Device.cpp.o
[ 18%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp.o
[ 20%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp.o
[ 21%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o
[ 22%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 24%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_6_location/Location.cpp.o
[ 25%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/bindings/WppBindings.cpp.o
[ 28%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp.o
[ 28%] Building CXX object CMakeFiles/WppExample.dir/objects.cpp.o
[ 29%] Building CXX object CMakeFiles/WppExample.dir/platform/Guard.cpp.o
[ 31%] Building CXX object CMakeFiles/WppExample.dir/platform/Platform.cpp.o
[ 32%] Building CXX object CMakeFiles/WppExample.dir/platform/connection_dtls/Connection.cpp.o
[ 33%] Building C object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/core/bootstrap.c.o
cc1: error: command-line option ‘-fno-rtti’ is valid for C++/D/ObjC++ but not for C [-Werror]
[ 35%] Building C object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/core/discover.c.o
cc1: error: command-line option ‘-fno-rtti’ is valid for C++/D/ObjC++ but not for C [-Werror]
[ 36%] Building C object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/core/liblwm2m.c.o
cc1: error: command-line option ‘-fno-rtti’ is valid for C++/D/ObjC++ but not for C [-Werror]
cc1: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:128: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/core/bootstrap.c.o] Error 1
make[2]: *** Waiting for unfinished jobs....
cc1: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:142: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/core/discover.c.o] Error 1
cc1: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:156: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/core/liblwm2m.c.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1186: CMakeFiles/WppExample.dir/all] Error 2
make: *** [Makefile:91: all] Error 2