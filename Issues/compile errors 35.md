 make -j$(nproc)
[  9%] Built target friendly_lwm2m
[ 11%] Building CXX object CMakeFiles/test_cid_record_format.dir/tests/test_cid_record_format.cpp.o
[ 11%] Building CXX object CMakeFiles/test_nat_rebinding.dir/tests/test_nat_rebinding.cpp.o
[ 11%] Building CXX object CMakeFiles/test_cid_aad.dir/tests/test_cid_aad.cpp.o
[ 11%] Building CXX object CMakeFiles/test_cid_extensions.dir/tests/test_cid_extensions.cpp.o
[ 11%] Building CXX object CMakeFiles/test_cid_integration.dir/tests/test_cid_integration.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_extensions.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/test_cid_extensions.dir/build.make:79: CMakeFiles/test_cid_extensions.dir/tests/test_cid_extensions.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1403: CMakeFiles/test_cid_extensions.dir/all] Error 2
make[1]: *** Waiting for unfinished jobs....
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_integration.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_record_format.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_nat_rebinding.cpp:21:14: fatal error: global.h: No such file or directory
   21 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
compilation terminated.
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_aad.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/test_cid_integration.dir/build.make:79: CMakeFiles/test_cid_integration.dir/tests/test_cid_integration.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_cid_record_format.dir/build.make:79: CMakeFiles/test_cid_record_format.dir/tests/test_cid_record_format.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_nat_rebinding.dir/build.make:79: CMakeFiles/test_nat_rebinding.dir/tests/test_nat_rebinding.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1499: CMakeFiles/test_cid_integration.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:1435: CMakeFiles/test_cid_record_format.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:1531: CMakeFiles/test_nat_rebinding.dir/all] Error 2
make[2]: *** [CMakeFiles/test_cid_aad.dir/build.make:79: CMakeFiles/test_cid_aad.dir/tests/test_cid_aad.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1467: CMakeFiles/test_cid_aad.dir/all] Error 2
[ 13%] Built target test_rollback_manager
[ 17%] Built target test_delta_algorithms
[ 18%] Built target test_firmware_update_object
[ 18%] Built target test_blockwise_transfer
[ 18%] Building CXX object CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o
[ 20%] Building CXX object CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o
[ 20%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[ 20%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[ 21%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o
[ 22%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
[ 22%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o
[ 23%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:318:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  318 |     return new MbedTlsProvider();
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::setPSK(void*, const char*, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:148:36: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  148 | int TinyDtlsProvider::setPSK(void* ctx, const char* identity,
      |                              ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::setPSK(void*, const char*, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:148:36: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  148 | int TinyDtlsProvider::setPSK(void* ctx, const char* identity,
      |                              ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:148:53: warning: unused parameter ‘identity’ [-Wunused-parameter]
  148 | int TinyDtlsProvider::setPSK(void* ctx, const char* identity,
      |                                         ~~~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:148:53: warning: unused parameter ‘identity’ [-Wunused-parameter]
  148 | int TinyDtlsProvider::setPSK(void* ctx, const char* identity,
      |                                         ~~~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:149:45: warning: unused parameter ‘key’ [-Wunused-parameter]
  149 |                              const uint8_t* key, size_t key_len) {
      |                              ~~~~~~~~~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:149:45: warning: unused parameter ‘key’ [-Wunused-parameter]
  149 |                              const uint8_t* key, size_t key_len) {
      |                              ~~~~~~~~~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:149:57: warning: unused parameter ‘key_len’ [-Wunused-parameter]
  149 |                              const uint8_t* key, size_t key_len) {
      |                                                  ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:149:57: warning: unused parameter ‘key_len’ [-Wunused-parameter]
  149 |                              const uint8_t* key, size_t key_len) {
      |                                                  ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::setRPK(void*, const uint8_t*, size_t, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:36: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                              ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::setRPK(void*, const uint8_t*, size_t, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:36: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                              ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:56: warning: unused parameter ‘priv_key’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                         ~~~~~~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:56: warning: unused parameter ‘priv_key’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                         ~~~~~~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:73: warning: unused parameter ‘priv_len’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                                                  ~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:73: warning: unused parameter ‘priv_len’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                                                  ~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:165:45: warning: unused parameter ‘pub_key’ [-Wunused-parameter]
  165 |                              const uint8_t* pub_key, size_t pub_len) {
      |                              ~~~~~~~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:165:45: warning: unused parameter ‘pub_key’ [-Wunused-parameter]
  165 |                              const uint8_t* pub_key, size_t pub_len) {
      |                              ~~~~~~~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:165:61: warning: unused parameter ‘pub_len’ [-Wunused-parameter]
  165 |                              const uint8_t* pub_key, size_t pub_len) {
      |                                                      ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:165:61: warning: unused parameter ‘pub_len’ [-Wunused-parameter]
  165 |                              const uint8_t* pub_key, size_t pub_len) {
      |                                                      ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::setCertificate(void*, const char*, const char*, const char*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:180:44: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  180 | int TinyDtlsProvider::setCertificate(void* ctx, const char* cert_file,
      |                                      ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::setCertificate(void*, const char*, const char*, const char*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:180:44: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  180 | int TinyDtlsProvider::setCertificate(void* ctx, const char* cert_file,
      |                                      ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:180:61: warning: unused parameter ‘cert_file’ [-Wunused-parameter]
  180 | int TinyDtlsProvider::setCertificate(void* ctx, const char* cert_file,
      |                                                 ~~~~~~~~~~~~^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:181:50: warning: unused parameter ‘key_file’ [-Wunused-parameter]
  181 |                                      const char* key_file, const char* ca_file) {
      |                                      ~~~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:180:61: warning: unused parameter ‘cert_file’ [-Wunused-parameter]
  180 | int TinyDtlsProvider::setCertificate(void* ctx, const char* cert_file,
      |                                                 ~~~~~~~~~~~~^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:181:50: warning: unused parameter ‘key_file’ [-Wunused-parameter]
  181 |                                      const char* key_file, const char* ca_file) {
      |                                      ~~~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:181:72: warning: unused parameter ‘ca_file’ [-Wunused-parameter]
  181 |                                      const char* key_file, const char* ca_file) {
      |                                                            ~~~~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:181:72: warning: unused parameter ‘ca_file’ [-Wunused-parameter]
  181 |                                      const char* key_file, const char* ca_file) {
      |                                                            ~~~~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::send(void*, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:187:34: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  187 | int TinyDtlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
      |                            ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::send(void*, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:187:34: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  187 | int TinyDtlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
      |                            ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:187:63: warning: unused parameter ‘data’ [-Wunused-parameter]
  187 | int TinyDtlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
      |                                                ~~~~~~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:187:63: warning: unused parameter ‘data’ [-Wunused-parameter]
  187 | int TinyDtlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
      |                                                ~~~~~~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:187:76: warning: unused parameter ‘len’ [-Wunused-parameter]
  187 | int TinyDtlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
      |                                                                     ~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:187:76: warning: unused parameter ‘len’ [-Wunused-parameter]
  187 | int TinyDtlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
      |                                                                     ~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::recv(void*, uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:194:34: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  194 | int TinyDtlsProvider::recv(void* dtls_session, uint8_t* data, size_t max_len) {
      |                            ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:194:57: warning: unused parameter ‘data’ [-Wunused-parameter]
  194 | int TinyDtlsProvider::recv(void* dtls_session, uint8_t* data, size_t max_len) {
      |                                                ~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:194:70: warning: unused parameter ‘max_len’ [-Wunused-parameter]
  194 | int TinyDtlsProvider::recv(void* dtls_session, uint8_t* data, size_t max_len) {
      |                                                               ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::recv(void*, uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:194:34: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  194 | int TinyDtlsProvider::recv(void* dtls_session, uint8_t* data, size_t max_len) {
      |                            ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:194:57: warning: unused parameter ‘data’ [-Wunused-parameter]
  194 | int TinyDtlsProvider::recv(void* dtls_session, uint8_t* data, size_t max_len) {
      |                                                ~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:194:70: warning: unused parameter ‘max_len’ [-Wunused-parameter]
  194 | int TinyDtlsProvider::recv(void* dtls_session, uint8_t* data, size_t max_len) {
      |                                                               ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::accept(void*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:231:36: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  231 | int TinyDtlsProvider::accept(void* dtls_session) {
      |                              ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual bool TinyDtlsProvider::isHandshakeComplete(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:236:50: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  236 | bool TinyDtlsProvider::isHandshakeComplete(void* dtls_session) const {
      |                                            ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::enableCID(void*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:243:39: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  243 | int TinyDtlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                 ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:243:61: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  243 | int TinyDtlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                                     ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual bool TinyDtlsProvider::isCIDNegotiated(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:253:46: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  253 | bool TinyDtlsProvider::isCIDNegotiated(void* dtls_session) const {
      |                                        ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::getCIDVersion(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:262:43: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  262 | int TinyDtlsProvider::getCIDVersion(void* dtls_session) const {
      |                                     ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::accept(void*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:231:36: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  231 | int TinyDtlsProvider::accept(void* dtls_session) {
      |                              ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual void* TinyDtlsProvider::getPeerByCID(void*, const uint8_t*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:270:77: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  270 | void* TinyDtlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                                     ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual bool TinyDtlsProvider::isHandshakeComplete(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:236:50: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  236 | bool TinyDtlsProvider::isHandshakeComplete(void* dtls_session) const {
      |                                            ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::enableCID(void*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:243:39: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  243 | int TinyDtlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                 ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:243:61: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  243 | int TinyDtlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                                     ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual bool TinyDtlsProvider::isCIDNegotiated(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:253:46: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  253 | bool TinyDtlsProvider::isCIDNegotiated(void* dtls_session) const {
      |                                        ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::getCIDVersion(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:262:43: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  262 | int TinyDtlsProvider::getCIDVersion(void* dtls_session) const {
      |                                     ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual void* TinyDtlsProvider::getPeerByCID(void*, const uint8_t*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:270:77: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  270 | void* TinyDtlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                                     ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In static member function ‘static int TinyDtlsProvider::staticEventCallback(dtls_context_t*, session_t*, dtls_alert_level_t, short unsigned int)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:350:84: warning: unused parameter ‘code’ [-Wunused-parameter]
  350 |                                           dtls_alert_level_t level, unsigned short code) {
      |                                                                     ~~~~~~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::getOurCIDLength(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:451:45: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  451 | int TinyDtlsProvider::getOurCIDLength(void* dtls_session) const {
      |                                       ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::getPeerCIDLength(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:460:46: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  460 | int TinyDtlsProvider::getPeerCIDLength(void* dtls_session) const {
      |                                        ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In static member function ‘static int TinyDtlsProvider::staticEventCallback(dtls_context_t*, session_t*, dtls_alert_level_t, short unsigned int)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:350:84: warning: unused parameter ‘code’ [-Wunused-parameter]
  350 |                                           dtls_alert_level_t level, unsigned short code) {
      |                                                                     ~~~~~~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::getOurCIDLength(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:451:45: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  451 | int TinyDtlsProvider::getOurCIDLength(void* dtls_session) const {
      |                                       ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::getPeerCIDLength(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:460:46: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  460 | int TinyDtlsProvider::getPeerCIDLength(void* dtls_session) const {
      |                                        ~~~~~~^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:142: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:318:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  318 |     return new MbedTlsProvider();
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:142: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:250:50: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  250 |     memcpy(&mbed_session->peer_session, session, sizeof(session_t));
      |                                                  ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:250:50: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  250 |     memcpy(&mbed_session->peer_session, session, sizeof(session_t));
      |                                                  ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::setRPK(void*, const uint8_t*, size_t, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:335:72: warning: unused parameter ‘priv_len’ [-Wunused-parameter]
  335 | int MbedTlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                                                 ~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:336:60: warning: unused parameter ‘pub_len’ [-Wunused-parameter]
  336 |                             const uint8_t* pub_key, size_t pub_len) {
      |                                                     ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::setRPK(void*, const uint8_t*, size_t, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:335:72: warning: unused parameter ‘priv_len’ [-Wunused-parameter]
  335 | int MbedTlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                                                 ~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:336:60: warning: unused parameter ‘pub_len’ [-Wunused-parameter]
  336 |                             const uint8_t* pub_key, size_t pub_len) {
      |                                                     ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::enableCID(void*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:535:38: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  535 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:535:60: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  535 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual bool MbedTlsProvider::isCIDNegotiated(void*) cons’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:559:45: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  559 | bool MbedTlsProvider::isCIDNegotiated(void* dtls_session) const {
      |                                       ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::getCIDVersion(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:583:42: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  583 | int MbedTlsProvider::getCIDVersion(void* dtls_session) const {
      |                                    ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::getPeerByCID(void*, const uint8_t*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:43: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                     ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:63: warning: unused parameter ‘cid’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                ~~~~~~~~~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:76: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::enableCID(void*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:535:38: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  535 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:535:60: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  535 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual bool MbedTlsProvider::isCIDNegotiated(void*) cons’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:559:45: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  559 | bool MbedTlsProvider::isCIDNegotiated(void* dtls_session) const {
      |                                       ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::getCIDVersion(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:583:42: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  583 | int MbedTlsProvider::getCIDVersion(void* dtls_session) const {
      |                                    ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::getPeerByCID(void*, const uint8_t*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:43: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                     ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:63: warning: unused parameter ‘cid’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                ~~~~~~~~~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:76: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticSendCallback(void*, const unsigned char*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:631:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  631 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticSendCallback(void*, const unsigned char*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:631:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  631 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:636:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  636 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:636:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  636 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:643:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  643 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:643:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  643 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticRecvTimeoutCallback(void*, unsigned char*, size_t, uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:656:16: error: ‘MBEDTLS_ERR_NET_RECV_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  656 |         return MBEDTLS_ERR_NET_RECV_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:654:58: warning: unused parameter ‘timeout’ [-Wunused-parameter]
  654 |                                                 uint32_t timeout) {
      |                                                 ~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToStrin(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:687:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  687 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticRecvTimeoutCallback(void*, unsigned char*, size_t, uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:656:16: error: ‘MBEDTLS_ERR_NET_RECV_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  656 |         return MBEDTLS_ERR_NET_RECV_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:654:58: warning: unused parameter ‘timeout’ [-Wunused-parameter]
  654 |                                                 uint32_t timeout) {
      |                                                 ~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToStrin(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:687:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  687 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:689:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  689 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:689:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  689 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:690:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  690 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:690:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  690 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:691:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  691 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:691:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  691 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:693:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  693 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:693:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  693 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:170: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:170: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:17:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:21:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:24:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘session_t* DtlsIntegrationTest::createMockSession()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:90:26: error: aggregate ‘session_t mockSession’ has incomplete type and cannot be defined
   90 |         static session_t mockSession;
      |                          ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_HandshakeInitiation_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:141:9: warning: unused variable ‘result’ [-Wunused-variable]
  141 |     int result = provider->connect(session);
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp: In member function ‘virtual void MbedTlsProviderTest::SetUp()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:206:40: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  206 |         provider = new MbedTlsProvider();
      |                                        ^
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_MultipleSessionCreation_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:278:20: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  278 |         mockSession->addr.sin.sin_port = htons(5684 + i);
      |                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_SessionCreationPerformance_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:379:20: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  379 |         mockSession->addr.sin.sin_port = htons(5684 + (i % 100));
      |                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void MbedTlsIntegrationTest_CertificateMode_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:416:21: error: cannot declare variable ‘provider’ to be of abstract type ‘MbedTlsProvider’
  416 |     MbedTlsProvider provider;
      |                     ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:128: CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:128: CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1563: CMakeFiles/test_dtls_providers.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:1627: CMakeFiles/test_dtls_integration.dir/all] Error 2
[ 24%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o
[ 24%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o
[ 25%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 26%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o
[ 27%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o
[ 28%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o
[ 28%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/main.cpp.o
[ 29%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/Connection.cpp.o
[ 29%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o
[ 30%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/MbedTlsProvider.cpp.o
[ 31%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:33:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   33 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:60:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   60 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:8:55: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
    8 |     WPP_LOGD(TAG_WPP_REG, "Creating registry instance");
      |                                                       ^
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
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:74:38: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   74 |         WPP_LOGD(TAG_WPP_CONN, "Done");
      |                                      ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:81:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   81 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:105:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro -Werror]
  105 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:116:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro -Werror]
  116 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:129:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro -Werror]
  129 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:318:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  318 |     return new MbedTlsProvider();
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:919: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:555: examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:751: examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:250:50: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  250 |     memcpy(&mbed_session->peer_session, session, sizeof(session_t));
      |                                                  ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticSendCallback(void*, const unsigned char*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:631:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  631 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:636:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  636 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:643:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  643 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticRecvTimeoutCallback(void*, unsigned char*, size_t, uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:656:16: error: ‘MBEDTLS_ERR_NET_RECV_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  656 |         return MBEDTLS_ERR_NET_RECV_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToStrin(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:687:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  687 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:689:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  689 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:690:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  690 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:691:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  691 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:693:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  693 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:905: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘bool Connection::isCidNegotiated(wpp::WppConnection::SESSION_T’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:536:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘cid_negotiated’
  536 |     return peer->cid_negotiated != 0;
      |                  ^~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘int Connection::getCidVersion(wpp::WppConnection::SESSION_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:553:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘cid_version’
  553 |     return peer->cid_version;
      |                  ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘int Connection::getOurCidLength(wpp::WppConnection::SESSION_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:570:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘our_cid_len’
  570 |     return peer->our_cid_len;
      |                  ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘int Connection::getPeerCidLength(wpp::WppConnection::SESSION_T’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:587:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘peer_cid_len’
  587 |     return peer->peer_cid_len;
      |                  ^~~~~~~~~~~~
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:835: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/Connection.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:891: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:187:98: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  187 |                 WPP_LOGD(TAG_WPP_TASK, "Task: Send operation, sending current time to the server");
      |                                                                                                  ^
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:793: examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:737: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:807: examples/CMakeFiles/friendly_example_client.dir/main.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:639: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:709: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1743: examples/CMakeFiles/friendly_example_client.dir/all] Error 2
make: *** [Makefile:166: all] Error 2
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$ make -j$(nproc)
[  9%] Built target friendly_lwm2m
[ 11%] Building CXX object CMakeFiles/test_cid_record_format.dir/tests/test_cid_record_format.cpp.o
[ 11%] Building CXX object CMakeFiles/test_nat_rebinding.dir/tests/test_nat_rebinding.cpp.o
[ 11%] Building CXX object CMakeFiles/test_cid_integration.dir/tests/test_cid_integration.cpp.o
[ 11%] Building CXX object CMakeFiles/test_cid_aad.dir/tests/test_cid_aad.cpp.o
[ 11%] Building CXX object CMakeFiles/test_cid_extensions.dir/tests/test_cid_extensions.cpp.o
[ 15%] Built target test_delta_algorithms
[ 16%] Built target test_rollback_manager
[ 16%] Built target test_blockwise_transfer
[ 18%] Built target test_firmware_update_object
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_integration.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_extensions.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_record_format.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_aad.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_nat_rebinding.cpp:21:14: fatal error: global.h: No such file or directory
   21 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/test_cid_aad.dir/build.make:79: CMakeFiles/test_cid_aad.dir/tests/test_cid_aad.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_cid_record_format.dir/build.make:79: CMakeFiles/test_cid_record_format.dir/tests/test_cid_record_format.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_cid_extensions.dir/build.make:79: CMakeFiles/test_cid_extensions.dir/tests/test_cid_extensions.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_cid_integration.dir/build.make:79: CMakeFiles/test_cid_integration.dir/tests/test_cid_integration.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_nat_rebinding.dir/build.make:79: CMakeFiles/test_nat_rebinding.dir/tests/test_nat_rebinding.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1467: CMakeFiles/test_cid_aad.dir/all] Error 2
make[1]: *** Waiting for unfinished jobs....
make[1]: *** [CMakeFiles/Makefile2:1435: CMakeFiles/test_cid_record_format.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:1403: CMakeFiles/test_cid_extensions.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:1531: CMakeFiles/test_nat_rebinding.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:1499: CMakeFiles/test_cid_integration.dir/all] Error 2
[ 19%] Building CXX object CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o
[ 19%] Building CXX object CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o
[ 20%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[ 20%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[ 21%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
[ 21%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:318:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  318 |     return new MbedTlsProvider();
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:318:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  318 |     return new MbedTlsProvider();
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:142: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:142: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:250:50: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  250 |     memcpy(&mbed_session->peer_session, session, sizeof(session_t));
      |                                                  ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:250:50: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  250 |     memcpy(&mbed_session->peer_session, session, sizeof(session_t));
      |                                                  ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::setRPK(void*, const uint8_t*, size_t, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:335:72: warning: unused parameter ‘priv_len’ [-Wunused-parameter]
  335 | int MbedTlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                                                 ~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:336:60: warning: unused parameter ‘pub_len’ [-Wunused-parameter]
  336 |                             const uint8_t* pub_key, size_t pub_len) {
      |                                                     ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::setRPK(void*, const uint8_t*, size_t, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:335:72: warning: unused parameter ‘priv_len’ [-Wunused-parameter]
  335 | int MbedTlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                                                 ~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:336:60: warning: unused parameter ‘pub_len’ [-Wunused-parameter]
  336 |                             const uint8_t* pub_key, size_t pub_len) {
      |                                                     ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::enableCID(void*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:535:38: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  535 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:535:60: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  535 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual bool MbedTlsProvider::isCIDNegotiated(void*) cons’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:559:45: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  559 | bool MbedTlsProvider::isCIDNegotiated(void* dtls_session) const {
      |                                       ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::getCIDVersion(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:583:42: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  583 | int MbedTlsProvider::getCIDVersion(void* dtls_session) const {
      |                                    ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::getPeerByCID(void*, const uint8_t*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:43: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                     ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:63: warning: unused parameter ‘cid’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                ~~~~~~~~~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:76: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::enableCID(void*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:535:38: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  535 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:535:60: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  535 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual bool MbedTlsProvider::isCIDNegotiated(void*) cons’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:559:45: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  559 | bool MbedTlsProvider::isCIDNegotiated(void* dtls_session) const {
      |                                       ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::getCIDVersion(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:583:42: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  583 | int MbedTlsProvider::getCIDVersion(void* dtls_session) const {
      |                                    ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::getPeerByCID(void*, const uint8_t*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:43: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                     ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:63: warning: unused parameter ‘cid’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                ~~~~~~~~~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:591:76: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  591 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticSendCallback(void*, const unsigned char*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:631:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  631 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticSendCallback(void*, const unsigned char*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:631:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  631 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:636:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  636 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:636:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  636 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:643:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  643 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:643:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  643 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticRecvTimeoutCallback(void*, unsigned char*, size_t, uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:656:16: error: ‘MBEDTLS_ERR_NET_RECV_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  656 |         return MBEDTLS_ERR_NET_RECV_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:654:58: warning: unused parameter ‘timeout’ [-Wunused-parameter]
  654 |                                                 uint32_t timeout) {
      |                                                 ~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToStrin(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:687:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  687 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:689:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  689 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:690:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  690 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:691:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  691 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:693:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  693 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticRecvTimeoutCallback(void*, unsigned char*, size_t, uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:656:16: error: ‘MBEDTLS_ERR_NET_RECV_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  656 |         return MBEDTLS_ERR_NET_RECV_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:654:58: warning: unused parameter ‘timeout’ [-Wunused-parameter]
  654 |                                                 uint32_t timeout) {
      |                                                 ~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToStrin(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:687:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  687 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:689:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  689 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:690:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  690 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:691:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  691 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:693:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  693 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:170: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:170: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:17:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:21:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:24:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘session_t* DtlsIntegrationTest::createMockSession()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:90:26: error: aggregate ‘session_t mockSession’ has incomplete type and cannot be defined
   90 |         static session_t mockSession;
      |                          ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_HandshakeInitiation_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:141:9: warning: unused variable ‘result’ [-Wunused-variable]
  141 |     int result = provider->connect(session);
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp: In member function ‘virtual void MbedTlsProviderTest::SetUp()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:206:40: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  206 |         provider = new MbedTlsProvider();
      |                                        ^
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_MultipleSessionCreation_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:278:20: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  278 |         mockSession->addr.sin.sin_port = htons(5684 + i);
      |                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_SessionCreationPerformance_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:379:20: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  379 |         mockSession->addr.sin.sin_port = htons(5684 + (i % 100));
      |                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void MbedTlsIntegrationTest_CertificateMode_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:416:21: error: cannot declare variable ‘provider’ to be of abstract type ‘MbedTlsProvider’
  416 |     MbedTlsProvider provider;
      |                     ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:128: CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1563: CMakeFiles/test_dtls_providers.dir/all] Error 2
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:128: CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1627: CMakeFiles/test_dtls_integration.dir/all] Error 2
[ 22%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o
[ 22%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o
[ 23%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 25%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o
[ 25%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o
[ 26%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o
[ 26%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/main.cpp.o
[ 27%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/Connection.cpp.o
[ 27%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o
[ 28%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/MbedTlsProvider.cpp.o
[ 29%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:33:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   33 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:60:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   60 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:8:55: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
    8 |     WPP_LOGD(TAG_WPP_REG, "Creating registry instance");
      |                                                       ^
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
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:312:68: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  312 |                 WPP_LOGE(TAG, "Security object instance is not set");
      |                                                                    ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:320:81: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  320 |                 WPP_LOGD(TAG, "Server reset state machine through PACKAGE_URI_1");
      |                                                                                 ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:349:77: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  349 |                 WPP_LOGD(TAG, "Server reset state machine through PACKAGE_0");
      |                                                                             ^
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
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:74:38: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   74 |         WPP_LOGD(TAG_WPP_CONN, "Done");
      |                                      ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:81:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   81 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:105:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro -Werror]
  105 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:116:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro -Werror]
  116 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:129:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro -Werror]
  129 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:318:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  318 |     return new MbedTlsProvider();
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:919: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:555: examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:751: examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:250:50: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  250 |     memcpy(&mbed_session->peer_session, session, sizeof(session_t));
      |                                                  ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticSendCallback(void*, const unsigned char*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:631:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  631 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:636:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  636 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:643:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  643 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticRecvTimeoutCallback(void*, unsigned char*, size_t, uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:656:16: error: ‘MBEDTLS_ERR_NET_RECV_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  656 |         return MBEDTLS_ERR_NET_RECV_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToStrin(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:687:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  687 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:689:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  689 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:690:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  690 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:691:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  691 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:693:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  693 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:905: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘bool Connection::isCidNegotiated(wpp::WppConnection::SESSION_T’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:536:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘cid_negotiated’
  536 |     return peer->cid_negotiated != 0;
      |                  ^~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘int Connection::getCidVersion(wpp::WppConnection::SESSION_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:553:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘cid_version’
  553 |     return peer->cid_version;
      |                  ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘int Connection::getOurCidLength(wpp::WppConnection::SESSION_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:570:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘our_cid_len’
  570 |     return peer->our_cid_len;
      |                  ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘int Connection::getPeerCidLength(wpp::WppConnection::SESSION_T’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:587:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘peer_cid_len’
  587 |     return peer->peer_cid_len;
      |                  ^~~~~~~~~~~~
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:835: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/Connection.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:891: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:187:98: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  187 |                 WPP_LOGD(TAG_WPP_TASK, "Task: Send operation, sending current time to the server");
      |                                                                                                  ^
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:793: examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:737: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:807: examples/CMakeFiles/friendly_example_client.dir/main.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:639: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:709: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1743: examples/CMakeFiles/friendly_example_client.dir/all] Error 2
make: *** [Makefile:166: all] Error 2
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/build$ make -j$(nproc)
[  9%] Built target friendly_lwm2m
[ 10%] Building CXX object CMakeFiles/test_cid_record_format.dir/tests/test_cid_record_format.cpp.o
[ 11%] Building CXX object CMakeFiles/test_cid_aad.dir/tests/test_cid_aad.cpp.o
[ 11%] Building CXX object CMakeFiles/test_cid_integration.dir/tests/test_cid_integration.cpp.o
[ 11%] Building CXX object CMakeFiles/test_nat_rebinding.dir/tests/test_nat_rebinding.cpp.o
[ 11%] Building CXX object CMakeFiles/test_cid_extensions.dir/tests/test_cid_extensions.cpp.o
[ 15%] Built target test_delta_algorithms
[ 16%] Built target test_blockwise_transfer
[ 16%] Built target test_rollback_manager
[ 18%] Built target test_firmware_update_object
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_extensions.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_integration.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_record_format.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
compilation terminated.
make[2]: *** [CMakeFiles/test_cid_extensions.dir/build.make:79: CMakeFiles/test_cid_extensions.dir/tests/test_cid_extensions.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_cid_integration.dir/build.make:79: CMakeFiles/test_cid_integration.dir/tests/test_cid_integration.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1403: CMakeFiles/test_cid_extensions.dir/all] Error 2
make[1]: *** Waiting for unfinished jobs....
make[2]: *** [CMakeFiles/test_cid_record_format.dir/build.make:79: CMakeFiles/test_cid_record_format.dir/tests/test_cid_record_format.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1499: CMakeFiles/test_cid_integration.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:1435: CMakeFiles/test_cid_record_format.dir/all] Error 2
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_nat_rebinding.cpp:21:14: fatal error: global.h: No such file or directory
   21 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_aad.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/test_nat_rebinding.dir/build.make:79: CMakeFiles/test_nat_rebinding.dir/tests/test_nat_rebinding.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_cid_aad.dir/build.make:79: CMakeFiles/test_cid_aad.dir/tests/test_cid_aad.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1531: CMakeFiles/test_nat_rebinding.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:1467: CMakeFiles/test_cid_aad.dir/all] Error 2
[ 19%] Building CXX object CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o
[ 19%] Building CXX object CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o
[ 20%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[ 20%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[ 21%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
[ 21%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:14:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:14:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:321:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  321 |     return new MbedTlsProvider();
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:321:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  321 |     return new MbedTlsProvider();
      |                                ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:18:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:18:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:33:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:33:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:142: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:142: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
make[2]: *** Waiting for unfinished jobs....
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h: In constructor ‘MbedTlsProvider::MbedTlsSession::MbedTlsSession()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:132:16: warning: ‘MbedTlsProvider::MbedTlsSession::recv_offset’ will be initialized after [-Wreorder]
  132 |         size_t recv_offset;
      |                ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:20: warning:   ‘session_t* MbedTlsProvider::MbedTlsSession::peer_session’ [-Wreorder]
  126 |         session_t* peer_session;  // Pointer to avoid requiring complete type
      |                    ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:96:1: warning:   when initialized here [-Wreorder]
   96 | MbedTlsProvider::MbedTlsSession::MbedTlsSession() :
      | ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In destructor ‘MbedTlsProvider::MbedTlsSession::~MbedTlsSession()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:115:9: warning: possible problem detected in invocation of ‘operator delete’ [-Wdelete-incomplete]
  115 |         delete peer_session;
      |         ^~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:115:16: warning: invalid use of incomplete type ‘struct session_t’
  115 |         delete peer_session;
      |                ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h: In constructor ‘MbedTlsProvider::MbedTlsSession::MbedTlsSession()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:132:16: warning: ‘MbedTlsProvider::MbedTlsSession::recv_offset’ will be initialized after [-Wreorder]
  132 |         size_t recv_offset;
      |                ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:20: warning:   ‘session_t* MbedTlsProvider::MbedTlsSession::peer_session’ [-Wreorder]
  126 |         session_t* peer_session;  // Pointer to avoid requiring complete type
      |                    ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘struct session_t’
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:115:9: note: neither the destructor nor the class-specific ‘operator delete’ will be called, even if they are declared when the class is defined
  115 |         delete peer_session;
      |         ^~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:96:1: warning:   when initialized here [-Wreorder]
   96 | MbedTlsProvider::MbedTlsSession::MbedTlsSession() :
      | ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In destructor ‘MbedTlsProvider::MbedTlsSession::~MbedTlsSession()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:115:9: warning: possible problem detected in invocation of ‘operator delete’ [-Wdelete-incomplete]
  115 |         delete peer_session;
      |         ^~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:115:16: warning: invalid use of incomplete type ‘struct session_t’
  115 |         delete peer_session;
      |                ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘struct session_t’
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:255:48: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  255 |     mbed_session->peer_session = new session_t();
      |                                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:115:9: note: neither the destructor nor the class-specific ‘operator delete’ will be called, even if they are declared when the class is defined
  115 |         delete peer_session;
      |         ^~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:256:49: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  256 |     memcpy(mbed_session->peer_session, session, sizeof(session_t));
      |                                                 ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::setRPK(void*, const uint8_t*, size_t, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:341:72: warning: unused parameter ‘priv_len’ [-Wunused-parameter]
  341 | int MbedTlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                                                 ~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:342:60: warning: unused parameter ‘pub_len’ [-Wunused-parameter]
  342 |                             const uint8_t* pub_key, size_t pub_len) {
      |                                                     ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:255:48: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  255 |     mbed_session->peer_session = new session_t();
      |                                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:256:49: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  256 |     memcpy(mbed_session->peer_session, session, sizeof(session_t));
      |                                                 ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::setRPK(void*, const uint8_t*, size_t, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:341:72: warning: unused parameter ‘priv_len’ [-Wunused-parameter]
  341 | int MbedTlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                                                 ~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:342:60: warning: unused parameter ‘pub_len’ [-Wunused-parameter]
  342 |                             const uint8_t* pub_key, size_t pub_len) {
      |                                                     ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::enableCID(void*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:541:38: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  541 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:541:60: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  541 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual bool MbedTlsProvider::isCIDNegotiated(void*) cons’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:565:45: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  565 | bool MbedTlsProvider::isCIDNegotiated(void* dtls_session) const {
      |                                       ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::getCIDVersion(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:589:42: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  589 | int MbedTlsProvider::getCIDVersion(void* dtls_session) const {
      |                                    ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::getPeerByCID(void*, const uint8_t*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:597:43: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  597 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                     ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:597:63: warning: unused parameter ‘cid’ [-Wunused-parameter]
  597 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                ~~~~~~~~~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:597:76: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  597 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::enableCID(void*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:541:38: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  541 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:541:60: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  541 | int MbedTlsProvider::enableCID(void* dtls_session, uint8_t cid_len) {
      |                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual bool MbedTlsProvider::isCIDNegotiated(void*) cons’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:565:45: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  565 | bool MbedTlsProvider::isCIDNegotiated(void* dtls_session) const {
      |                                       ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual int MbedTlsProvider::getCIDVersion(void*) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:589:42: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  589 | int MbedTlsProvider::getCIDVersion(void* dtls_session) const {
      |                                    ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::getPeerByCID(void*, const uint8_t*, uint8_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:597:43: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  597 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                     ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:597:63: warning: unused parameter ‘cid’ [-Wunused-parameter]
  597 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                ~~~~~~~~~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:597:76: warning: unused parameter ‘cid_len’ [-Wunused-parameter]
  597 | void* MbedTlsProvider::getPeerByCID(void* ctx, const uint8_t* cid, uint8_t cid_len) {
      |                                                                    ~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticSendCallback(void*, const unsigned char*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:637:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  637 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticSendCallback(void*, const unsigned char*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:637:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  637 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:643:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  643 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:643:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  643 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:650:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  650 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:650:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  650 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticRecvTimeoutCallback(void*, unsigned char*, size_t, uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:663:16: error: ‘MBEDTLS_ERR_NET_RECV_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  663 |         return MBEDTLS_ERR_NET_RECV_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:661:58: warning: unused parameter ‘timeout’ [-Wunused-parameter]
  661 |                                                 uint32_t timeout) {
      |                                                 ~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToStrin(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:696:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  696 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:697:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  697 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:698:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  698 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:700:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  700 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:701:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  701 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticRecvTimeoutCallback(void*, unsigned char*, size_t, uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:663:16: error: ‘MBEDTLS_ERR_NET_RECV_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  663 |         return MBEDTLS_ERR_NET_RECV_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:661:58: warning: unused parameter ‘timeout’ [-Wunused-parameter]
  661 |                                                 uint32_t timeout) {
      |                                                 ~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToStrin(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:696:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  696 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:697:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  697 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:698:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  698 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:700:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  700 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:701:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  701 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:170: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:170: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
  In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:17:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration ‘typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:26:26: note: previous declaration as ‘typedef struct session_t session_t’
   26 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘session_t* DtlsIntegrationTest::createMockSession()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:90:26: error: aggregate ‘session_t mockSession’ has incomplete type and cannot be defined
   90 |         static session_t mockSession;
      |                          ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp: In member function ‘virtual void MbedTlsProviderTest::SetUp()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:206:40: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  206 |         provider = new MbedTlsProvider();
      |                                        ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:21:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_HandshakeInitiation_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:141:9: warning: unused variable ‘result’ [-Wunused-variable]
  141 |     int result = provider->connect(session);
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_MultipleSessionCreation_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:278:20: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  278 |         mockSession->addr.sin.sin_port = htons(5684 + i);
      |                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_SessionCreationPerformance_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:379:20: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  379 |         mockSession->addr.sin.sin_port = htons(5684 + (i % 100));
      |                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void MbedTlsIntegrationTest_CertificateMode_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:416:21: error: cannot declare variable ‘provider’ to be of abstract type ‘MbedTlsProvider’
  416 |     MbedTlsProvider provider;
      |                     ^~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:24:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
v[ 22%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o
[ 22%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o
[ 23%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 24%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o
[ 25%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:128: CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1563: CMakeFiles/test_dtls_providers.dir/all] Error 2
[ 26%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:128: CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1627: CMakeFiles/test_dtls_integration.dir/all] Error 2
[ 26%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/main.cpp.o
[ 27%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/Connection.cpp.o
[ 27%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o
[ 28%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/MbedTlsProvider.cpp.o
[ 29%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:33:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   33 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:60:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   60 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:74:38: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   74 |         WPP_LOGD(TAG_WPP_CONN, "Done");
      |                                      ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:81:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   81 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:105:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro -Werror]
  105 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:116:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro -Werror]
  116 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/platform/connection/WppConnection.cpp:129:67: error: ISO C++11 requires at least one argument for the "..." in a variadic macro -Werror]
  129 |                         WPP_LOGE(TAG_WPP_CONN, "User data is NULL");
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:8:55: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
    8 |     WPP_LOGD(TAG_WPP_REG, "Creating registry instance");
      |                                                       ^
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
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:14:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:321:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  321 |     return new MbedTlsProvider();
      |                                ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:18:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:33:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:119:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  119 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:249:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  249 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:256:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  256 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:337:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  337 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:343:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  343 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:919: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:751: examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h: In constructor ‘MbedTlsProvider::MbedTlsSession::MbedTlsSession()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:132:16: error: ‘MbedTlsProvider::MbedTlsSession::recv_offset’ will be initialized after [-Werror=reorder]
  132 |         size_t recv_offset;
      |                ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:20: error:   ‘session_t* MbedTlsProvider::MbedTlsSession::peer_session’ [-Werror=reorder]
  126 |         session_t* peer_session;  // Pointer to avoid requiring complete type
      |                    ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:96:1: error:   when initialized here [-Werror=reorder]
   96 | MbedTlsProvider::MbedTlsSession::MbedTlsSession() :
      | ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In destructor ‘MbedTlsProvider::MbedTlsSession::~MbedTlsSession()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:115:9: error: possible problem detected in invocation of ‘operator delete’ [-Werror=delete-incomplete]
  115 |         delete peer_session;
      |         ^~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:115:16: error: invalid use of incomplete type ‘struct session_t’ [-Werror]
  115 |         delete peer_session;
      |                ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘struct session_t’
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:115:9: note: neither the destructor nor the class-specific ‘operator delete’ will be called, even if they are declared when the class is defined
  115 |         delete peer_session;
      |         ^~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual void* MbedTlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:255:48: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  255 |     mbed_session->peer_session = new session_t();
      |                                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:256:49: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  256 |     memcpy(mbed_session->peer_session, session, sizeof(session_t));
      |                                                 ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticSendCallback(void*, const unsigned char*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:637:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  637 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:643:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  643 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:650:16: error: ‘MBEDTLS_ERR_NET_SEND_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  650 |         return MBEDTLS_ERR_NET_SEND_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticRecvTimeoutCallback(void*, unsigned char*, size_t, uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:663:16: error: ‘MBEDTLS_ERR_NET_RECV_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  663 |         return MBEDTLS_ERR_NET_RECV_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToStrin(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:696:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  696 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:697:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  697 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:698:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  698 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:700:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  700 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:701:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  701 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:25:8: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   25 | struct session_t;
      |        ^~~~~~~~~
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:555: examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:905: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘bool Connection::isCidNegotiated(wpp::WppConnection::SESSION_T’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:536:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘cid_negotiated’
  536 |     return peer->cid_negotiated != 0;
      |                  ^~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘int Connection::getCidVersion(wpp::WppConnection::SESSION_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:553:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘cid_version’
  553 |     return peer->cid_version;
      |                  ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘int Connection::getOurCidLength(wpp::WppConnection::SESSION_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:570:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘our_cid_len’
  570 |     return peer->our_cid_len;
      |                  ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘int Connection::getPeerCidLength(wpp::WppConnection::SESSION_T’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:587:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘peer_cid_len’
  587 |     return peer->peer_cid_len;
      |                  ^~~~~~~~~~~~
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:835: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/Connection.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:891: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:187:98: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  187 |                 WPP_LOGD(TAG_WPP_TASK, "Task: Send operation, sending current time to the server");
      |                                                                                                  ^
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:793: examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:737: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:807: examples/CMakeFiles/friendly_example_client.dir/main.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:639: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:709: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1743: examples/CMakeFiles/friendly_example_client.dir/all] Error 2
make: *** [Makefile:166: all] Error 2