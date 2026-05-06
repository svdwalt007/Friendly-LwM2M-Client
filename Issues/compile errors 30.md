make -j$(nproc)
-- Building for 64 bit system
-- Exceptions are not supported
-- RTTI is not supported
-- Enabling TinyDTLS provider
-- Enabling mbedTLS provider
-- Could NOT find MbedTLS (missing: MbedTLS_LIBRARY MbedX509_LIBRARY MbedCrypto_LIBRARY MbedTLS_INCLUDE_DIR)
CMake Warning at examples/CMakeLists.txt:93 (message):
  mbedTLS not found, mbedTLS provider will not be available


-- DTLS Provider Sources: platform/connection_dtls/TinyDtlsProvider.cpp;platform/connection_dtls/DtlsProviderFactory.cpp
-- DTLS Provider Libraries:
-- friendly_example_client: Endiannes not set, defaulting to little endian
-- libcoap built successfully. Library path: /mnt/d/Dev/Friendly-LwM2M-Client/examples/coap/libcoap/build
-- Using example Firmware Updater
-- Found Git: /usr/bin/git (found version "2.43.0")
--
-- Friendly LwM2M Client Configuration:
--   Version:          1.2.2
--   Build type:
--   C++ Standard:     17
--
-- Core Features:
--   DTLS:             ON
--   mbedTLS:          ON
--   TinyDTLS:         OFF
--
-- DTLS Multi-Provider (v1.2.2):
--   TinyDTLS Provider: ON
--   mbedTLS Provider:  ON (DEFAULT)
--   OpenSSL Provider:  OFF
--   wolfSSL Provider:  OFF
--   Default Provider:  mbedtls
--   Static Linking:    ON
--   Delta FOTA:       ON
--   BSDIFF:           ON
--   VCDIFF:           ON
--   Courgette:        ON
--   Compression:      ON
--   Rollback:         ON
--   MQTT Transport:   ON
--   Edge AI:          ON
--   TensorFlow Lite:  OFF
--   ONNX Runtime:     OFF
--
-- v1.2.0 Extended Features:
--   OpenWRT Extended: ON
--   LUCI Module:      ON
--   Hotplug Handlers: ON
--   LAN Config:       OFF
--   Routing Table:    OFF
--   VPN Config:       OFF
--   WAN Failover:     OFF
--   Multi-WAN Health: OFF
--   WiFi Client Mgmt: OFF
--   WiFi Channel Opt: OFF
--   Starlink:         ON
--   Starlink gRPC:    ON
--
-- Platform Integrations:
--   prpl Integration: OFF
--   prpl EasyMesh:    OFF
--   prpl Ambiorix:    OFF
--   Matter/Thread:    OFF
--   Matter Bridge:    OFF
--   Thread BR:        OFF
--   Zigbee:           OFF
--   Zigbee EZSP:      OFF
--   Zigbee Z-Stack:   OFF
--
-- Build options:
--   Tests:            ON
--   Examples:         ON
--   Documentation:    ON
--   Coverage:         ON
--   Sanitizers:       ON
--
-- Optional libraries found:
--   BZip2:            TRUE
--   LZMA:             TRUE
--   Zstd:
--   Brotli:
--
-- Configuring done (6.7s)
-- Generating done (5.8s)
-- Build files have been written to: /mnt/d/Dev/Friendly-LwM2M-Client/build
[  2%] Building CXX object CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o
[  2%] Building CXX object CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o
[  4%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o
[  4%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[  4%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[  5%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o
[  7%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
[  7%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:16:10: fatal error: tinydtls.h: No such file or directory
   16 | #include "tinydtls.h"
      |          ^~~~~~~~~~~~
compilation terminated.
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:16:10: fatal error: tinydtls.h: No such file or directory
   16 | #include "tinydtls.h"
      |          ^~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:107: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:107: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:10: fatal error: mbedtls/ssl.h: No such file or directory
   12 | #include "mbedtls/ssl.h"
      |          ^~~~~~~~~~~~~~~
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:10: fatal error: mbedtls/ssl.h: No such file or directory
   12 | #include "mbedtls/ssl.h"
      |          ^~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:121: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:121: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:16:10: fatal error: tinydtls.h: No such file or directory
   16 | #include "tinydtls.h"
      |          ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:16:10: fatal error: tinydtls.h: No such file or directory
   16 | #include "tinydtls.h"
      |          ^~~~~~~~~~~~
compilation terminated.
compilation terminated.
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:93: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:93: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
[  8%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/bootstrap.c.o
[  9%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/discover.c.o
[ 10%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/liblwm2m.c.o
[ 11%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/list.c.o
[ 12%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/management.c.o
[ 12%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/objects.c.o
[ 13%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/observe.c.o
[ 14%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/packet.c.o
[ 16%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/registration.c.o
[ 16%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/uri.c.o
[ 17%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/utils.c.o
[ 18%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/send.c.o
[ 18%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/core/objects/access_control.c.o
[ 19%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/coap/block.c.o
[ 20%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/coap/er-coap-13/er-coap-13.c.o
[ 21%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/coap/transaction.c.o
[ 22%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/data/data.c.o
[ 23%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/data/json.c.o
[ 23%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/data/json_common.c.o
[ 24%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/data/senml_json.c.o
[ 25%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/data/tlv.c.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:17:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/TinyDtlsProvider.h:16:10: fatal error: tinydtls.h: No such file or directory
   16 | #include "tinydtls.h"
      |          ^~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:79: CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1563: CMakeFiles/test_dtls_providers.dir/all] Error 2
make[1]: *** Waiting for unfinished jobs....
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/TinyDtlsProvider.h:16:10: fatal error: tinydtls.h: No such file or directory
   16 | #include "tinydtls.h"
      |          ^~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:79: CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1595: CMakeFiles/test_dtls_integration.dir/all] Error 2
[ 26%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/data/cbor.c.o
[ 27%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/data/senml_cbor.c.o
[ 38%] Built target friendly_lwm2m
[ 39%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/libs/tinycbor/src/cborencoder.c.o
[ 40%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/libs/tinycbor/src/cborencoder_close_container_checked.c.o
[ 40%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/libs/tinycbor/src/cborencoder_float.c.o
[ 41%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/libs/tinycbor/src/cborerrorstrings.c.o
[ 42%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/libs/tinycbor/src/cborparser.c.o
[ 43%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/libs/tinycbor/src/cborparser_dup_string.c.o
[ 44%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/libs/tinycbor/src/cborparser_float.c.o
[ 45%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/libs/tinycbor/src/cborpretty.c.o
[ 45%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/libs/tinycbor/src/cborpretty_stdio.c.o
[ 46%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/libs/tinycbor/src/cbortojson.c.o
[ 47%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/libs/tinycbor/src/cborvalidation.c.o
[ 48%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o
[ 49%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/data_model/object/Object.cpp.o
[ 50%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/data_model/instance/Instance.cpp.o
[ 51%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/data_model/resource/Resource.cpp.o
[ 51%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/data_model/resource/ResourceContainer.cpp.o
[ 52%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_0_lwm2m_security/Lwm2mSecurity.cpp.o
[ 53%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o
[ 54%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_3_device/Device.cpp.o
[ 55%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_2_lwm2m_access_control/Lwm2mAccessControl.cpp.o
[ 56%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_3339_audio_clip/AudioClip.cpp.o
[ 56%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o
[ 57%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 58%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_6_location/Location.cpp.o
[ 59%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o
[ 61%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o
[ 61%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/WppPlatform.cpp.o
[ 62%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/bindings/WppBindings.cpp.o
[ 62%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o
[ 64%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/main.cpp.o
[ 64%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/objects.cpp.o
[ 65%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/Connection.cpp.o
[ 66%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/Guard.cpp.o
[ 67%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/Platform.cpp.o
[ 68%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/FirmwareUpdater/FirmwareUpdater.cpp.o
[ 68%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:25:56: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   25 |         WPP_LOGI(TAG_WPP_CLIENT, "Destroying WppClient");
      |                                                        ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:26:58: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   26 |         WPP_LOGI(TAG_WPP_CLIENT, "Destroying WppRegistry");
      |                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:29:54: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   29 |         WPP_LOGI(TAG_WPP_CLIENT, "Clearing wpp tasks");
      |                                                      ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:31:57: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   31 |         WPP_LOGI(TAG_WPP_CLIENT, "Clearing packet queue");
      |                                                         ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:33:57: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   33 |         WPP_LOGI(TAG_WPP_CLIENT, "Closing lwm2m context");
      |                                                         ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:46:77: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   46 |                 WPP_LOGE(TAG_WPP_CLIENT, "Error during client configuration");
      |                                                                             ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:56:63: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   56 |         WPP_LOGD(TAG_WPP_CLIENT, "Removing WppClient instance");
      |                                                               ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:110:74: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  110 |         WPP_LOGD(TAG_WPP_CLIENT, "Handling server packets if they exists");
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:118:69: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  118 |         WPP_LOGD(TAG_WPP_CLIENT, "Handling wpp tasks if they exists");
      |                                                                     ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:140:63: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  140 |         WPP_LOGI(TAG_WPP_CLIENT, "Unregister with each server");
      |                                                               ^
[ 69%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp:203:98: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  203 |                         WPP_LOGI(TAG, "Deregistration Request Trigger: Deregistration is started");
      |                                                                                                  ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp:209:114: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  209 |                         WPP_LOGI(TAG, "Deregistration Request Trigger: Deregistration is already in the progress");
      |                                                                                                                  ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp:240:96: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  240 |                         WPP_LOGI(TAG, "Bootstrap Request Trigger: Bootstrap request is started");
      |                                                                                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp:246:108: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  246 |                         WPP_LOGI(TAG, "Bootstrap Request Trigger: Bootstrap request is already in progress");
      |                                                                                                            ^
[ 70%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/dtls.c.o
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:312:68: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  312 |                 WPP_LOGE(TAG, "Security object instance is not set");
      |                                                                    ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:320:81: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  320 |                 WPP_LOGD(TAG, "Server reset state machine through PACKAGE_URI_1");
      |                                                                                 ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:349:77: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  349 |                 WPP_LOGD(TAG, "Server reset state machine through PACKAGE_0");
      |                                                                             ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:33:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   33 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:60:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   60 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:8:55: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
    8 |     WPP_LOGD(TAG_WPP_REG, "Creating registry instance");
      |                                                       ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:74:38: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   74 |         WPP_LOGD(TAG_WPP_CONN, "Done");
      |                                      ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:81:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   81 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:105:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  105 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:116:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  116 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:129:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  129 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:604: examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:800: examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:17,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:18:8: note: previous declaration as ‘struct session_t’
   18 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:54:13: error: field ‘session’ has incomplete type ‘session_t’
   54 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:18:8: note: forward declaration of ‘struct session_t’
   18 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:396:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  396 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:94:18: error: ‘vector’ in namespace ‘std’ does not name a template type
   94 |             std::vector<uint8_t> psk_key;
      |                  ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:18:1: note: ‘std::vector’ is defined in header ‘<vector>’; did you forget to ‘#include <vector>’?
   17 | #include "dtls.h"
  +++ |+#include <vector>
   18 | }
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:98:18: error: ‘vector’ in namespace ‘std’ does not name a template type
   98 |             std::vector<uint8_t> priv_key;
      |                  ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:98:13: note: ‘std::vector’ is defined in header ‘<vector>’; did you forget to ‘#include <vector>’?
   98 |             std::vector<uint8_t> priv_key;
      |             ^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:99:18: error: ‘vector’ in namespace ‘std’ does not name a template type
   99 |             std::vector<uint8_t> pub_key;
      |                  ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:99:13: note: ‘std::vector’ is defined in header ‘<vector>’; did you forget to ‘#include <vector>’?
   99 |             std::vector<uint8_t> pub_key;
      |             ^~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/crypto.h:29,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:33:
/usr/include/c++/13/bits/streambuf.tcc: In member function ‘virtual std::streamsize std::basic_streambuf<_CharT, _Traits>::xsgetn(char_type*, std::streamsize)’:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/numeric.h:24:18: error: expected unqualified-id before ‘(’ token
   24 | #define min(A,B) ((A) <= (B) ? (A) : (B))
      |                  ^
/usr/include/c++/13/bits/streambuf.tcc: In member function ‘virtual std::streamsize std::basic_streambuf<_CharT, _Traits>::xsputn(const char_type*, std::streamsize)’:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/numeric.h:24:18: error: expected unqualified-id before ‘(’ token
   24 | #define min(A,B) ((A) <= (B) ? (A) : (B))
      |                  ^
/usr/include/c++/13/bits/istream.tcc: In member function ‘std::streamsize std::basic_istream<_CharT, _Traits>::readsome(char_type*, std::streamsize)’:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/numeric.h:24:18: error: expected unqualified-id before ‘(’ token
   24 | #define min(A,B) ((A) <= (B) ? (A) : (B))
      |                  ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual void* TinyDtlsProvider::createContext()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:91:9: error: C++ designated initializers only available with ‘-std=c++20’ or ‘-std=gnu++20’ [-Werror=c++20-extensions]
   91 |         .write = staticSendCallback,
      |         ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:92:9: error: C++ designated initializers only available with ‘-std=c++20’ or ‘-std=gnu++20’ [-Werror=c++20-extensions]
   92 |         .read = staticReadCallback,
      |         ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:93:9: error: C++ designated initializers only available with ‘-std=c++20’ or ‘-std=gnu++20’ [-Werror=c++20-extensions]
   93 |         .event = staticEventCallback,
      |         ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:95:9: error: C++ designated initializers only available with ‘-std=c++20’ or ‘-std=gnu++20’ [-Werror=c++20-extensions]
   95 |         .get_psk_info = staticGetPskInfo,
      |         ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:96:9: error: C++ designated initializers only available with ‘-std=c++20’ or ‘-std=gnu++20’ [-Werror=c++20-extensions]
   96 |         .get_ecdsa_key = nullptr,
      |         ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:97:9: error: C++ designated initializers only available with ‘-std=c++20’ or ‘-std=gnu++20’ [-Werror=c++20-extensions]
   97 |         .verify_ecdsa_key = nullptr
      |         ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual void* TinyDtlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:130:44: error: invalid use of incomplete type ‘struct session_t’
  130 |     session_t* new_session = new session_t();
      |                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:18:8: note: forward declaration of ‘struct session_t’
   18 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:135:34: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  135 |     memcpy(new_session, session, sizeof(session_t));
      |                                  ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual void TinyDtlsProvider::destroySession(void*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:141:9: error: possible problem detected in invocation of ‘operator delete’ [-Werror=delete-incomplete]
  141 |         delete static_cast<session_t*>(dtls_session);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:141:16: error: invalid use of incomplete type ‘struct session_t’ [-Werror]
  141 |         delete static_cast<session_t*>(dtls_session);
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:18:8: note: forward declaration of ‘struct session_t’
   18 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:141:9: note: neither the destructor nor the class-specific ‘operator delete’ will be called, even if they are declared when the class is defined
  141 |         delete static_cast<session_t*>(dtls_session);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::setPSK(void*, const char*, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:155:19: error: ‘struct TinyDtlsProvider::TinyDtlsContext::<unnamed>’ has no member named ‘psk_key’
  155 |     tiny_ctx->psk.psk_key.assign(key, key + key_len);
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In static member function ‘static int TinyDtlsProvider::staticGetPskInfo(dtls_context_t*, const session_t*, dtls_credentials_type_t, const unsigned char*, size_t, unsigned char*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:385:47: error: ‘struct TinyDtlsProvider::TinyDtlsContext::<unnamed>’ has no member named ‘psk_key’
  385 |             if (result_length < tiny_ctx->psk.psk_key.size()) {
      |                                               ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:389:42: error: ‘struct TinyDtlsProvider::TinyDtlsContext::<unnamed>’ has no member named ‘psk_key’
  389 |             memcpy(result, tiny_ctx->psk.psk_key.data(), tiny_ctx->psk.psk_key.size());
      |                                          ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:389:72: error: ‘struct TinyDtlsProvider::TinyDtlsContext::<unnamed>’ has no member named ‘psk_key’
  389 |             memcpy(result, tiny_ctx->psk.psk_key.data(), tiny_ctx->psk.psk_key.size());
      |                                                                        ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:390:34: error: ‘struct TinyDtlsProvider::TinyDtlsContext::<unnamed>’ has no member named ‘psk_key’
  390 |             return tiny_ctx->psk.psk_key.size();
      |                                  ^~~~~~~
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:940: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/cli_options.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:7:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/version_info.h:49: error: "DTLS_VERSION" redefined [-Werror]
   49 |     #define DTLS_VERSION "N/A"
      |
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:5:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:46: note: this is the location of the previous definition
   46 | #define DTLS_VERSION 0xfefd     /* DTLS v1.2 */
      |
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:187:98: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  187 |                 WPP_LOGD(TAG_WPP_TASK, "Task: Send operation, sending current time to the server");
      |                                                                                                  ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:17,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:18:8: note: previous declaration as ‘struct session_t’
   18 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:54:13: error: field ‘session’ has incomplete type ‘session_t’
   54 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:18:8: note: forward declaration of ‘struct session_t’
   18 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:396:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  396 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createTinyDTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:309:33: error: invalid new-expression of abstract class type ‘TinyDtlsProvider’
  309 |     return new TinyDtlsProvider();
      |                                 ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:27:7: note:   because the following virtual functions are pure within ‘TinyDtlsProvider’:
   27 | class TinyDtlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:110:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  110 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:240:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  240 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:247:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  247 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:328:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  328 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:334:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  334 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:954: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:842: examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:786: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:856: examples/CMakeFiles/friendly_example_client.dir/main.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:688: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:758: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1711: examples/CMakeFiles/friendly_example_client.dir/all] Error 2
make: *** [Makefile:166: all] Error 2
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$ make -j$(nproc)
[  2%] Building CXX object CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o
[  2%] Building CXX object CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o
[  4%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[  4%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[  5%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o
[  5%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o
[  7%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
[  7%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
[ 18%] Built target friendly_lwm2m
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:23:10: fatal error: ../../2305-Wakaama/examples/shared/tinydtls/session.h: No such file or directory
   23 | #include "../../2305-Wakaama/examples/shared/tinydtls/session.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:23:10: fatal error: ../../2305-Wakaama/examples/shared/tinydtls/session.h: No such file or directory
   23 | #include "../../2305-Wakaama/examples/shared/tinydtls/session.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
compilation terminated.
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:15,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:23:10: fatal error: ../../2305-Wakaama/examples/shared/tinydtls/session.h: No such file or directory
   23 | #include "../../2305-Wakaama/examples/shared/tinydtls/session.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:15,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:23:10: fatal error: ../../2305-Wakaama/examples/shared/tinydtls/session.h: No such file or directory
   23 | #include "../../2305-Wakaama/examples/shared/tinydtls/session.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:93: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:93: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
make[2]: *** Waiting for unfinished jobs....
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:107: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:107: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:23:10: fatal error: ../../2305-Wakaama/examples/shared/tinydtls/session.h: No such file or directory
   23 | #include "../../2305-Wakaama/examples/shared/tinydtls/session.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:121: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:23:10: fatal error: ../../2305-Wakaama/examples/shared/tinydtls/session.h: No such file or directory
   23 | #include "../../2305-Wakaama/examples/shared/tinydtls/session.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:121: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
[ 21%] Building CXX object CMakeFiles/test_cid_record_format.dir/tests/test_cid_record_format.cpp.o
[ 22%] Building CXX object CMakeFiles/test_cid_aad.dir/tests/test_cid_aad.cpp.o
[ 22%] Building CXX object CMakeFiles/test_cid_extensions.dir/tests/test_cid_extensions.cpp.o
[ 22%] Building CXX object CMakeFiles/test_nat_rebinding.dir/tests/test_nat_rebinding.cpp.o
[ 22%] Building CXX object CMakeFiles/test_cid_integration.dir/tests/test_cid_integration.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_aad.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_nat_rebinding.cpp:21:14: fatal error: global.h: No such file or directory
   21 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_integration.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_record_format.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_extensions.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
compilation terminated.
make[2]: *** [CMakeFiles/test_cid_aad.dir/build.make:79: CMakeFiles/test_cid_aad.dir/tests/test_cid_aad.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_nat_rebinding.dir/build.make:79: CMakeFiles/test_nat_rebinding.dir/tests/test_nat_rebinding.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_cid_integration.dir/build.make:79: CMakeFiles/test_cid_integration.dir/tests/test_cid_integration.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_cid_extensions.dir/build.make:79: CMakeFiles/test_cid_extensions.dir/tests/test_cid_extensions.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1467: CMakeFiles/test_cid_aad.dir/all] Error 2
make[1]: *** Waiting for unfinished jobs....
make[2]: *** [CMakeFiles/test_cid_record_format.dir/build.make:79: CMakeFiles/test_cid_record_format.dir/tests/test_cid_record_format.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1531: CMakeFiles/test_nat_rebinding.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:1499: CMakeFiles/test_cid_integration.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:1403: CMakeFiles/test_cid_extensions.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:1435: CMakeFiles/test_cid_record_format.dir/all] Error 2
[ 24%] Built target test_rollback_manager
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:23:10: fatal error: ../../2305-Wakaama/examples/shared/tinydtls/session.h: No such file or directory
   23 | #include "../../2305-Wakaama/examples/shared/tinydtls/session.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:23:10: fatal error: ../../2305-Wakaama/examples/shared/tinydtls/session.h: No such file or directory
   23 | #include "../../2305-Wakaama/examples/shared/tinydtls/session.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
[ 26%] Built target test_firmware_update_object
compilation terminated.
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:79: CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1563: CMakeFiles/test_dtls_providers.dir/all] Error 2
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:79: CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1595: CMakeFiles/test_dtls_integration.dir/all] Error 2
[ 29%] Built target test_blockwise_transfer
[ 29%] Built target test_delta_algorithms
[ 30%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o
[ 31%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o
[ 32%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 33%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o
[ 34%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o
[ 34%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o
[ 36%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/main.cpp.o
[ 36%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/objects.cpp.o
[ 36%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o
[ 37%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o
[ 38%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/crypto.c.o
[ 39%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/ccm.c.o
[ 40%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/hmac.c.o
[ 40%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/dtls_debug.c.o
[ 41%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/netq.c.o
[ 42%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/peer.c.o
[ 43%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/dtls_prng.c.o
[ 44%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/dtls_time.c.o
[ 45%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/session.c.o
[ 46%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/aes/rijndael.c.o
[ 46%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/aes/rijndael_wrap.c.o
[ 47%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/sha2/sha2.c.o
[ 48%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/ecc/ecc.c.o
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:33:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   33 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:60:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   60 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp:203:98: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  203 |                         WPP_LOGI(TAG, "Deregistration Request Trigger: Deregistration is started");
      |                                                                                                  ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp:209:114: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  209 |                         WPP_LOGI(TAG, "Deregistration Request Trigger: Deregistration is already in the progress");
      |                                                                                                                  ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp:240:96: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  240 |                         WPP_LOGI(TAG, "Bootstrap Request Trigger: Bootstrap request is started");
      |                                                                                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp:246:108: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  246 |                         WPP_LOGI(TAG, "Bootstrap Request Trigger: Bootstrap request is already in progress");
      |                                                                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:312:68: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  312 |                 WPP_LOGE(TAG, "Security object instance is not set");
      |                                                                    ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:320:81: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  320 |                 WPP_LOGD(TAG, "Server reset state machine through PACKAGE_URI_1");
      |                                                                                 ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:349:77: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  349 |                 WPP_LOGD(TAG, "Server reset state machine through PACKAGE_0");
      |                                                                             ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:8:55: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
    8 |     WPP_LOGD(TAG_WPP_REG, "Creating registry instance");
      |                                                       ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:74:38: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   74 |         WPP_LOGD(TAG_WPP_CONN, "Done");
      |                                      ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:81:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   81 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:105:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  105 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:116:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  116 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:129:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  129 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:25:56: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   25 |         WPP_LOGI(TAG_WPP_CLIENT, "Destroying WppClient");
      |                                                        ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:26:58: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   26 |         WPP_LOGI(TAG_WPP_CLIENT, "Destroying WppRegistry");
      |                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:29:54: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   29 |         WPP_LOGI(TAG_WPP_CLIENT, "Clearing wpp tasks");
      |                                                      ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:31:57: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   31 |         WPP_LOGI(TAG_WPP_CLIENT, "Clearing packet queue");
      |                                                         ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:33:57: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   33 |         WPP_LOGI(TAG_WPP_CLIENT, "Closing lwm2m context");
      |                                                         ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:46:77: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   46 |                 WPP_LOGE(TAG_WPP_CLIENT, "Error during client configuration");
      |                                                                             ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:56:63: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   56 |         WPP_LOGD(TAG_WPP_CLIENT, "Removing WppClient instance");
      |                                                               ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:110:74: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  110 |         WPP_LOGD(TAG_WPP_CLIENT, "Handling server packets if they exists");
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:118:69: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  118 |         WPP_LOGD(TAG_WPP_CLIENT, "Handling wpp tasks if they exists");
      |                                                                     ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/client/WppClient.cpp:140:63: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  140 |         WPP_LOGI(TAG_WPP_CLIENT, "Unregister with each server");
      |                                                               ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:21,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/include/../../2305-Wakaama/examples/shared/tinydtls/dtls.h:396:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  396 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/include/../../2305-Wakaama/examples/shared/tinydtls/crypto.h:29,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/include/../../2305-Wakaama/examples/shared/tinydtls/peer.h:33,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/include/../../2305-Wakaama/examples/shared/tinydtls/dtls.h:32:
/usr/include/c++/13/bits/streambuf.tcc: In member function ‘virtual std::streamsize std::basic_streambuf<_CharT, _Traits>::xsgetn(char_type*, std::streamsize)’:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/include/../../2305-Wakaama/examples/shared/tinydtls/numeric.h:24:18: error: expected unqualified-id before ‘(’ token
   24 | #define min(A,B) ((A) <= (B) ? (A) : (B))
      |                  ^
/usr/include/c++/13/bits/streambuf.tcc: In member function ‘virtual std::streamsize std::basic_streambuf<_CharT, _Traits>::xsputn(const char_type*, std::streamsize)’:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/include/../../2305-Wakaama/examples/shared/tinydtls/numeric.h:24:18: error: expected unqualified-id before ‘(’ token
   24 | #define min(A,B) ((A) <= (B) ? (A) : (B))
      |                  ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:21,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/include/../../2305-Wakaama/examples/shared/tinydtls/dtls.h:396:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  396 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/usr/include/c++/13/bits/istream.tcc: In member function ‘std::streamsize std::basic_istream<_CharT, _Traits>::readsome(char_type*, std::streamsize)’:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/include/../../2305-Wakaama/examples/shared/tinydtls/numeric.h:24:18: error: expected unqualified-id before ‘(’ token
   24 | #define min(A,B) ((A) <= (B) ? (A) : (B))
      |                  ^
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:940: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:800: examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:604: examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:187:98: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  187 |                 WPP_LOGD(TAG_WPP_TASK, "Task: Send operation, sending current time to the server");
      |                                                                                                  ^
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:842: examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:954: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:786: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:856: examples/CMakeFiles/friendly_example_client.dir/main.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:688: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:758: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1711: examples/CMakeFiles/friendly_example_client.dir/all] Error 2
make: *** [Makefile:166: all] Error 2