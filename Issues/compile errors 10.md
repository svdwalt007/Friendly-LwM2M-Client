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
-- Using example Firmware Updater
-- Configuring done (2.6s)
-- Generating done (3.4s)
-- Build files have been written to: /mnt/d/Dev/Friendly-LwM2M-Client/_build
[  3%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp.o
[  7%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/object/Object.cpp.o
[ 11%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.cpp.o
[ 15%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/Resource.cpp.o
[ 19%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/resource/ResourceContainer.cpp.o
[ 23%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp.o
[ 26%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o
[ 30%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_3_device/Device.cpp.o
[ 34%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp.o
[ 38%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp.o
[ 42%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o
[ 46%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 50%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_6_location/Location.cpp.o
[ 53%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp.o
[ 57%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp.o
[ 61%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/WppPlatform.cpp.o
[ 65%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/bindings/WppBindings.cpp.o
[ 69%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp.o
[ 73%] Building CXX object CMakeFiles/WppExample.dir/main.cpp.o
[ 76%] Building CXX object CMakeFiles/WppExample.dir/objects.cpp.o
[ 80%] Building CXX object CMakeFiles/WppExample.dir/platform/connection_dtls/Connection.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/platform/Guard.cpp.o
[ 88%] Building CXX object CMakeFiles/WppExample.dir/platform/Platform.cpp.o
[ 92%] Building CXX object CMakeFiles/WppExample.dir/FirmwareUpdater/FirmwareUpdater.cpp.o
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
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp: In lambda function:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:357:33: error: declaration of ‘pkg’ shadows a previous local [-Werror=shadow]
  357 |                 const OPAQUE_T &pkg = resource(PACKAGE_0)->get<OPAQUE_T>();
      |                                 ^~~
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:346:25: note: shadowed declaration is here
  346 |         const OPAQUE_T &pkg = resource(PACKAGE_0)->get<OPAQUE_T>();
      |                         ^~~
At top level:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:268: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
At top level:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:282: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:10,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:1:
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
At top level:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:408: CMakeFiles/WppExample.dir/platform/connection_dtls/Connection.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.cpp:2:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/cli_options.h: In function ‘bool parseCliOptions(int, char**, CliOptions&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/cli_options.h:429:16: error: switch missing default case [-Werror=switch-default]
  429 |         switch (options.security_mode) {
      |                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.cpp: In function ‘void securityInit(wpp::WppClient&, const CliOptions&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.cpp:66:12: error: switch missing default case [-Werror=switch-default]
   66 |     switch (options.security_mode) {
      |            ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:10,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:5:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
At top level:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:394: CMakeFiles/WppExample.dir/objects.cpp.o] Error 1
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
make[2]: *** [CMakeFiles/WppExample.dir/build.make:380: CMakeFiles/WppExample.dir/main.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1186: CMakeFiles/WppExample.dir/all] Error 2
make: *** [Makefile:91: all] Error 2