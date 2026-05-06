make -j$(nproc)
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
-- Generating done (3.8s)
-- Build files have been written to: /mnt/d/Dev/Friendly-LwM2M-Client/_build
[  1%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUriDownloader.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUriDownloader.cpp: In member function ‘bool OpenWrtFwUriDownloader::downloadWithWget(const std::string&, wpp::Lwm2mSecurity*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUriDownloader.cpp:179:23: error: aggregate ‘std::stringstream cmd’ has incomplete type and cannot be defined
  179 |     std::stringstream cmd;
      |                       ^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUriDownloader.cpp: In member function ‘bool OpenWrtFwUriDownloader::downloadWithCurl(const std::string&, wpp::Lwm2mSecurity*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUriDownloader.cpp:224:23: error: aggregate ‘std::stringstream cmd’ has incomplete type and cannot be defined
  224 |     std::stringstream cmd;
      |                       ^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUriDownloader.cpp: In member function ‘bool OpenWrtFwUriDownloader::downloadWithCoap(const std::string&, wpp::Lwm2mSecurity*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUriDownloader.cpp:268:23: error: aggregate ‘std::stringstream cmd’ has incomplete type and cannot be defined
  268 |     std::stringstream cmd;
      |                       ^~~
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:107: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUriDownloader.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1189: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/all] Error 2
make: *** [Makefile:136: all] Error 2