cmake ../examples -DUSE_OPENWRT_FW_UPDATER=ON
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
-- Using OpenWRT Firmware Updater
-- OpenWrtFirmwareUpdater library configured
-- Configuring done (2.5s)
-- Generating done (4.0s)
-- Build files have been written to: /mnt/d/Dev/Friendly-LwM2M-Client/_build
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/_build$ make -j$(nproc)
[  2%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUpdater.cpp.o
[  3%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwDownloader.cpp.o
[  3%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwInit.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:12:10: fatal error: WppClient.h: No such file or directory
   12 | #include "WppClient.h"
      |          ^~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:12:10: fatal error: WppClient.h: No such file or directory
   12 | #include "WppClient.h"
      |          ^~~~~~~~~~~~~
compilation terminated.
compilation terminated.
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwInit.cpp:7:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwInit.h:11:10: fatal error: WppClient.h: No such file or directory
   11 | #include "WppClient.h"
      |          ^~~~~~~~~~~~~
compilation terminated.
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:79: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUpdater.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:93: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwDownloader.cpp.o] Error 1
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:107: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwInit.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1189: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/all] Error 2
make: *** [Makefile:136: all] Error 2