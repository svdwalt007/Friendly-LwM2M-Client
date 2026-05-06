make -j$(nproc)
[  3%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o
[  7%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 11%] Building CXX object CMakeFiles/WppExample.dir/main.cpp.o
[ 15%] Building CXX object CMakeFiles/WppExample.dir/objects.cpp.o
[ 19%] Building CXX object CMakeFiles/WppExample.dir/platform/connection_dtls/Connection.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp: In member function ‘bool wpp::ConnectivityMonitoring::checkLinkQuality(uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp:265:32: error: comparison is always true due to limited range of data type [-Werror=type-limits]
  265 |             return linkQuality <= LINK_QUALITY_IEEE_802_15_4_MAX;  // MIN is 0, uint8_t is always >= 0
      |                                ^
At top level:
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:268: CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:10,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:1:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:408: CMakeFiles/WppExample.dir/platform/connection_dtls/Connection.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.h:10,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:5:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
cc1plus: note: unrecognized command-line option ‘-Wno-c99-extensions’ may have been intended to silence earlier diagnostics
cc1plus: note: unrecognized command-line option ‘-Wno-gnu-zero-variadic-macro-arguments’ may have been intended to silence earlier diagnostics
cc1plus: all warnings being treated as errors
make[2]: *** [CMakeFiles/WppExample.dir/build.make:380: CMakeFiles/WppExample.dir/main.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1186: CMakeFiles/WppExample.dir/all] Error 2
make: *** [Makefile:91: all] Error 2