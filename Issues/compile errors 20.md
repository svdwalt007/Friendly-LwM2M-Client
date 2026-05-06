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
[  1%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUpdater.cpp.o
[  2%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwDownloader.cpp.o
[  3%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwInit.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwInit.h:14,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwInit.cpp:7:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUriDownloader.h:14:10: fatal error: m_0_lwm2m_security/Lwm2mSecurity.h: No such file or directory
   14 | #include "m_0_lwm2m_security/Lwm2mSecurity.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
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
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:39:42: error: expected class-name before ‘{’ token
   39 | class OpenWrtFwUpdater: public FwUpdater {
      |                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:54:5: error: ‘FwUpdRes’ does not name a type
   54 |     FwUpdRes lastUpdateResult() override;
      |     ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:85:5: error: ‘FwUpdRes’ does not name a type
   85 |     FwUpdRes _lastUpdateResult;         // Last update result
      |     ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:52:10: error: ‘void OpenWrtFwUpdater::startUpdating()’ marked ‘override’, but does not override
   52 |     void startUpdating() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:53:10: error: ‘bool OpenWrtFwUpdater::isUpdated()’ marked ‘override’, but does not override
   53 |     bool isUpdated() override;
      |          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:64:10: error: ‘void OpenWrtFwUpdater::reset()’ marked ‘override’, but does not override
   64 |     void reset() override;
      |          ^~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:41:48: error: expected class-name before ‘{’ token
   41 | class OpenWrtFwDownloader: public FwInternalDl {
      |                                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:55:5: error: ‘FwUpdRes’ does not name a type
   55 |     FwUpdRes downloadResult() override;
      |     ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:70:5: error: ‘FwUpdRes’ does not name a type
   70 |     FwUpdRes _downloadResult;           // Download result
      |     ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:52:10: error: ‘void OpenWrtFwDownloader::downloadIsStarted()’ marked ‘override’, but does not override
   52 |     void downloadIsStarted() override;
      |          ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:53:10: error: ‘bool OpenWrtFwDownloader::saveDownloadedBlock(const wpp::OPAQUE_T&)’ marked ‘override’, but does not override
   53 |     bool saveDownloadedBlock(const OPAQUE_T &dataBlock) override;
      |          ^~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:54:10: error: ‘void OpenWrtFwDownloader::downloadIsCompleted()’ marked ‘override’, but does not override
   54 |     void downloadIsCompleted() override;
      |          ^~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:56:10: error: ‘void OpenWrtFwDownloader::reset()’ marked ‘override’, but does not override
   56 |     void reset() override;
      |          ^~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp: In constructor ‘OpenWrtFwDownloader::OpenWrtFwDownloader(OpenWrtFwUpdater&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:16:7: error: class ‘OpenWrtFwDownloader’ does not have any field named ‘_downloadResul’
   16 |     , _downloadResult(FwUpdRes::R_INITIAL)
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:16:23: error: ‘FwUpdRes’ has not been declared
   16 |     , _downloadResult(FwUpdRes::R_INITIAL)
      |                       ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp: In member function ‘void OpenWrtFwDownloader::downloadIsStarted()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:39:9: error: ‘_downloadResult’ was not declared in this scope
   39 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:39:27: error: ‘FwUpdRes’ has not been declared
   39 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |                           ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:45:5: error: ‘_downloadResult’ was not declared in this scope
   45 |     _downloadResult = FwUpdRes::R_INITIAL;
      |     ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:45:23: error: ‘FwUpdRes’ has not been declared
   45 |     _downloadResult = FwUpdRes::R_INITIAL;
      |                       ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp: In member function ‘bool OpenWrtFwDownloader::saveDownloadedBlock(const wpp::OPAQUE_T&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:55:9: error: ‘_downloadResult’ was not declared in this scope
   55 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:55:27: error: ‘FwUpdRes’ has not been declared
   55 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |                           ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:68:9: error: ‘_downloadResult’ was not declared in this scope
   68 |         _downloadResult = FwUpdRes::R_NOT_ENOUGH_FLASH;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:68:27: error: ‘FwUpdRes’ has not been declared
   68 |         _downloadResult = FwUpdRes::R_NOT_ENOUGH_FLASH;
      |                           ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:77:9: error: ‘_downloadResult’ was not declared in this scope
   77 |         _downloadResult = FwUpdRes::R_NOT_ENOUGH_FLASH;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:77:27: error: ‘FwUpdRes’ has not been declared
   77 |         _downloadResult = FwUpdRes::R_NOT_ENOUGH_FLASH;
      |                           ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:88:13: error: ‘_downloadResult’ was not declared in this scope
   88 |             _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |             ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:88:31: error: ‘FwUpdRes’ has not been declared
   88 |             _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |                               ^~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/WppRegistry.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h: In member function ‘wpp::Instance* wpp::ObjectImpl<T>::createInstance(wpp::ID_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:79:9: error: there are no arguments to ‘lwm2m_update_registration’ that depend on a template parameter, so a declaration of ‘lwm2m_update_registration’ must be available [-fpermissive]
   79 |         lwm2m_update_registration(&getContext(), 0, false, true);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:79:9: note: (if you use ‘-fpermissive’, G++ will accept your code, but allowing the use of an undeclared name is deprecated)
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:100:9: error: ‘_downloadResult’ was not declared in this scope
  100 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:100:27: error: ‘FwUpdRes’ has not been declared
  100 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |                           ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp: At global scope:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:123:1: error: ‘FwUpdRes’ does not name a type
  123 | FwUpdRes OpenWrtFwDownloader::downloadResult() {
      | ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h: At global scope:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:127:9: error: ‘lwm2m_client_state_t’ does not name a type; did you mean ‘lwm2m_client_t’?
  127 |         lwm2m_client_state_t getState();
      |         ^~~~~~~~~~~~~~~~~~~~
      |         lwm2m_client_t
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp: In member function ‘void OpenWrtFwDownloader::reset()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:135:5: error: ‘_downloadResult’ was not declared in this scope
  135 |     _downloadResult = FwUpdRes::R_INITIAL;
      |     ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:135:23: error: ‘FwUpdRes’ has not been declared
  135 |     _downloadResult = FwUpdRes::R_INITIAL;
      |                       ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp: In member function ‘void OpenWrtFwDownloader::finalizeDownload()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:165:9: error: ‘_downloadResult’ was not declared in this scope
  165 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:165:27: error: ‘FwUpdRes’ has not been declared
  165 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |                           ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:180:9: error: ‘_downloadResult’ was not declared in this scope
  180 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:180:27: error: ‘FwUpdRes’ has not been declared
  180 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |                           ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:191:5: error: ‘_downloadResult’ was not declared in this scope
  191 |     _downloadResult = FwUpdRes::R_INITIAL;
      |     ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:191:23: error: ‘FwUpdRes’ has not been declared
  191 |     _downloadResult = FwUpdRes::R_INITIAL;
      |                       ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp: In member function ‘bool OpenWrtFwDownloader::verifyDownload()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:201:9: error: ‘_downloadResult’ was not declared in this scope
  201 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:201:27: error: ‘FwUpdRes’ has not been declared
  201 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |                           ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:208:9: error: ‘_downloadResult’ was not declared in this scope
  208 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:208:27: error: ‘FwUpdRes’ has not been declared
  208 |         _downloadResult = FwUpdRes::R_FW_UPD_FAIL;
      |                           ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:214:9: error: ‘_downloadResult’ was not declared in this scope
  214 |         _downloadResult = FwUpdRes::R_INTEGRITY_CHECK_FAIL;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:214:27: error: ‘FwUpdRes’ has not been declared
  214 |         _downloadResult = FwUpdRes::R_INTEGRITY_CHECK_FAIL;
      |                           ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:39:42: error: expected class-name before ‘{’ token
   39 | class OpenWrtFwUpdater: public FwUpdater {
      |                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:54:5: error: ‘FwUpdRes’ does not name a type
   54 |     FwUpdRes lastUpdateResult() override;
      |     ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:85:5: error: ‘FwUpdRes’ does not name a type
   85 |     FwUpdRes _lastUpdateResult;         // Last update result
      |     ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:221:9: error: ‘_downloadResult’ was not declared in this scope
  221 |         _downloadResult = FwUpdRes::R_UNSUPPORTED_PKG_TYPE;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:221:27: error: ‘FwUpdRes’ has not been declared
  221 |         _downloadResult = FwUpdRes::R_UNSUPPORTED_PKG_TYPE;
      |                           ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:52:10: error: ‘void OpenWrtFwUpdater::startUpdating()’ marked ‘override’, but does not override
   52 |     void startUpdating() override;
      |          ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:53:10: error: ‘bool OpenWrtFwUpdater::isUpdated()’ marked ‘override’, but does not override
   53 |     bool isUpdated() override;
      |          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:64:10: error: ‘void OpenWrtFwUpdater::reset()’ marked ‘override’, but does not override
   64 |     void reset() override;
      |          ^~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp: In constructor ‘OpenWrtFwUpdater::OpenWrtFwUpdater()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:21:7: error: class ‘OpenWrtFwUpdater’ does not have any field named ‘_lastUpdateResult’
   21 |     , _lastUpdateResult(FwUpdRes::R_INITIAL)
      |       ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:21:25: error: ‘FwUpdRes’ has not been declared
   21 |     , _lastUpdateResult(FwUpdRes::R_INITIAL)
      |                         ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp: In member function ‘void OpenWrtFwUpdater::startUpdating()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:55:5: error: ‘_lastUpdateResult’ was not declared in this scope
   55 |     _lastUpdateResult = FwUpdRes::R_INITIAL;
      |     ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:55:25: error: ‘FwUpdRes’ has not been declared
   55 |     _lastUpdateResult = FwUpdRes::R_INITIAL;
      |                         ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:60:29: error: ‘FwUpdRes’ has not been declared
   60 |         _lastUpdateResult = FwUpdRes::R_FW_UPD_FAIL;
      |                             ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:75:29: error: ‘FwUpdRes’ has not been declared
   75 |         _lastUpdateResult = FwUpdRes::R_NOT_ENOUGH_FLASH;
      |                             ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp: At global scope:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:96:1: error: ‘FwUpdRes’ does not name a type
   96 | FwUpdRes OpenWrtFwUpdater::lastUpdateResult() {
      | ^~~~~~~~
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:93: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwDownloader.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp: In member function ‘void OpenWrtFwUpdater::reset()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:139:5: error: ‘_lastUpdateResult’ was not declared in this scope
  139 |     _lastUpdateResult = FwUpdRes::R_INITIAL;
      |     ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:139:25: error: ‘FwUpdRes’ has not been declared
  139 |     _lastUpdateResult = FwUpdRes::R_INITIAL;
      |                         ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp: In member function ‘void OpenWrtFwUpdater::performUpdate()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:155:13: error: ‘_lastUpdateResult’ was not declared in this scope
  155 |             _lastUpdateResult = FwUpdRes::R_FW_UPD_SUCCESS;
      |             ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:155:33: error: ‘FwUpdRes’ has not been declared
  155 |             _lastUpdateResult = FwUpdRes::R_FW_UPD_SUCCESS;
      |                                 ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:166:9: error: ‘_lastUpdateResult’ was not declared in this scope
  166 |         _lastUpdateResult = FwUpdRes::R_FW_UPD_FAIL;
      |         ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:166:29: error: ‘FwUpdRes’ has not been declared
  166 |         _lastUpdateResult = FwUpdRes::R_FW_UPD_FAIL;
      |                             ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp: In member function ‘bool OpenWrtFwUpdater::validateFirmware()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:186:13: error: ‘_lastUpdateResult’ was not declared in this scope
  186 |             _lastUpdateResult = FwUpdRes::R_NOT_ENOUGH_FLASH;
      |             ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:186:33: error: ‘FwUpdRes’ has not been declared
  186 |             _lastUpdateResult = FwUpdRes::R_NOT_ENOUGH_FLASH;
      |                                 ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:189:13: error: ‘_lastUpdateResult’ was not declared in this scope
  189 |             _lastUpdateResult = FwUpdRes::R_UNSUPPORTED_PKG_TYPE;
      |             ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:189:33: error: ‘FwUpdRes’ has not been declared
  189 |             _lastUpdateResult = FwUpdRes::R_UNSUPPORTED_PKG_TYPE;
      |                                 ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:192:13: error: ‘_lastUpdateResult’ was not declared in this scope
  192 |             _lastUpdateResult = FwUpdRes::R_INTEGRITY_CHECK_FAIL;
      |             ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:192:33: error: ‘FwUpdRes’ has not been declared
  192 |             _lastUpdateResult = FwUpdRes::R_INTEGRITY_CHECK_FAIL;
      |                                 ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:194:13: error: ‘_lastUpdateResult’ was not declared in this scope
  194 |             _lastUpdateResult = FwUpdRes::R_FW_UPD_FAIL;
      |             ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:194:33: error: ‘FwUpdRes’ has not been declared
  194 |             _lastUpdateResult = FwUpdRes::R_FW_UPD_FAIL;
      |                                 ^~~~~~~~
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:79: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUpdater.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1189: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/all] Error 2
make: *** [Makefile:136: all] Error 2