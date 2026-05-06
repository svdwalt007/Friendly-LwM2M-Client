make -j$(nproc)
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