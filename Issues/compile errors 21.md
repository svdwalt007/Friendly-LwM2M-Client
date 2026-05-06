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
-- Configuring done (2.4s)
-- Generating done (3.7s)
-- Build files have been written to: /mnt/d/Dev/Friendly-LwM2M-Client/_build
[  1%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUpdater.cpp.o
[  2%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwDownloader.cpp.o
[  3%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwInit.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/WppRegistry.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwInit.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwInit.cpp:7:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h: In member function ‘wpp::Instance* wpp::ObjectImpl<T>::createInstance(wpp::ID_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:79:9: error: there are no arguments to ‘lwm2m_update_registration’ that depend on a template parameter, so a declaration of ‘lwm2m_update_registration’ must be available [-fpermissive]
   79 |         lwm2m_update_registration(&getContext(), 0, false, true);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:79:9: note: (if you use ‘-fpermissive’, G++ will accept your code, but allowing the use of an undeclared name is deprecated)
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h: At global scope:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:127:9: error: ‘lwm2m_client_state_t’ does not name a type; did you mean ‘lwm2m_client_t’?
  127 |         lwm2m_client_state_t getState();
      |         ^~~~~~~~~~~~~~~~~~~~
      |         lwm2m_client_t
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:107: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwInit.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/WppRegistry.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h: In member function ‘wpp::Instance* wpp::ObjectImpl<T>::createInstance(wpp::ID_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:79:9: error: there are no arguments to ‘lwm2m_update_registration’ that depend on a template parameter, so a declaration of ‘lwm2m_update_registration’ must be available [-fpermissive]
   79 |         lwm2m_update_registration(&getContext(), 0, false, true);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:79:9: note: (if you use ‘-fpermissive’, G++ will accept your code, but allowing the use of an undeclared name is deprecated)
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h: At global scope:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:127:9: error: ‘lwm2m_client_state_t’ does not name a type; did you mean ‘lwm2m_client_t’?
  127 |         lwm2m_client_state_t getState();
      |         ^~~~~~~~~~~~~~~~~~~~
      |         lwm2m_client_t
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:93: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwDownloader.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/WppRegistry.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h: In member function ‘wpp::Instance* wpp::ObjectImpl<T>::createInstance(wpp::ID_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:79:9: error: there are no arguments to ‘lwm2m_update_registration’ that depend on a template parameter, so a declaration of ‘lwm2m_update_registration’ must be available [-fpermissive]
   79 |         lwm2m_update_registration(&getContext(), 0, false, true);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:79:9: note: (if you use ‘-fpermissive’, G++ will accept your code, but allowing the use of an undeclared name is deprecated)
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h: At global scope:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:127:9: error: ‘lwm2m_client_state_t’ does not name a type; did you mean ‘lwm2m_client_t’?
  127 |         lwm2m_client_state_t getState();
      |         ^~~~~~~~~~~~~~~~~~~~
      |         lwm2m_client_t
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:79: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUpdater.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1189: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/all] Error 2
make: *** [Makefile:136: all] Error 2