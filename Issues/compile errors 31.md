make -j$(nproc)
[  1%] Building CXX object CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o
[  1%] Building CXX object CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o
[  2%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[  2%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o
[  4%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o
[  4%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o
[  5%] Building CXX object CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
[  5%] Building CXX object CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o
[  6%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/dtls.c.o
[  6%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/dtls.c.o
[  8%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/crypto.c.o
[  8%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/crypto.c.o
[  9%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/ccm.c.o
[  9%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/ccm.c.o
[ 11%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/hmac.c.o
[ 11%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/hmac.c.o
[ 12%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/dtls_debug.c.o
[ 12%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/dtls_debug.c.o
[ 13%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/netq.c.o
[ 13%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/netq.c.o
[ 15%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/peer.c.o
[ 15%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/peer.c.o
[ 16%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/dtls_prng.c.o
[ 16%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/dtls_prng.c.o
[ 17%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/dtls_time.c.o
[ 19%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/session.c.o
[ 19%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/dtls_time.c.o
[ 19%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/session.c.o
[ 20%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/aes/rijndael.c.o
[ 20%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/aes/rijndael_wrap.c.o
[ 20%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/aes/rijndael.c.o
[ 21%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/aes/rijndael_wrap.c.o
[ 22%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/sha2/sha2.c.o
[ 23%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/sha2/sha2.c.o
[ 24%] Building C object CMakeFiles/test_dtls_integration.dir/2305-Wakaama/examples/shared/tinydtls/ecc/ecc.c.o
[ 24%] Building C object CMakeFiles/test_dtls_providers.dir/2305-Wakaama/examples/shared/tinydtls/ecc/ecc.c.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:21,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:15:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:26: note: previous declaration as typedef struct session_t session_t’
   24 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:21,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:15:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:26: note: previous declaration as typedef struct session_t session_t’
   24 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:21,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:21,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:26: note: previous declaration as typedef struct session_t session_t’
   24 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:26: note: previous declaration as typedef struct session_t session_t’
   24 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual void* TinyDtlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:132:44: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  132 |     session_t* new_session = new session_t();
      |                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:137:34: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  137 |     memcpy(new_session, session, sizeof(session_t));
      |                                  ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual void TinyDtlsProvider::destroySession(void*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:143:9: warning: possible problem detected in invocation of ‘operator delete’ [-Wdelete-incomplete]
  143 |         delete static_cast<session_t*>(dtls_session);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:143:16: warning: invalid use of incomplete type ‘struct session_t’
  143 |         delete static_cast<session_t*>(dtls_session);
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘struct session_t’
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:143:9: note: neither the destructor nor the class-specific ‘operator delete’ will be called, even if they are declared when the class is defined
  143 |         delete static_cast<session_t*>(dtls_session);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::setPSK(void*, const char*, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:148:36: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  148 | int TinyDtlsProvider::setPSK(void* ctx, const char* identity,
      |                              ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:148:53: warning: unused parameter ‘identity’ [-Wunused-parameter]
  148 | int TinyDtlsProvider::setPSK(void* ctx, const char* identity,
      |                                         ~~~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:149:45: warning: unused parameter ‘key’ [-Wunused-parameter]
  149 |                              const uint8_t* key, size_t key_len) {
      |                              ~~~~~~~~~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:149:57: warning: unused parameter ‘key_len’ [-Wunused-parameter]
  149 |                              const uint8_t* key, size_t key_len) {
      |                                                  ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::setRPK(void*, const uint8_t*, size_t, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:36: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                              ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:56: warning: unused parameter ‘priv_key’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                         ~~~~~~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:73: warning: unused parameter ‘priv_len’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                                                  ~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:165:45: warning: unused parameter ‘pub_key’ [-Wunused-parameter]
  165 |                              const uint8_t* pub_key, size_t pub_len) {
      |                              ~~~~~~~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:165:61: warning: unused parameter ‘pub_len’ [-Wunused-parameter]
  165 |                              const uint8_t* pub_key, size_t pub_len) {
      |                                                      ~~~~~~~^~~~~~~
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:181:72: warning: unused parameter ‘ca_file’ [-Wunused-parameter]
  181 |                                      const char* key_file, const char* ca_file) {
      |                                                            ~~~~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::send(void*, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:187:34: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  187 | int TinyDtlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
      |                            ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:187:63: warning: unused parameter ‘data’ [-Wunused-parameter]
  187 | int TinyDtlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
      |                                                ~~~~~~~~~~~~~~~^~~~
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
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual void* TinyDtlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:132:44: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  132 |     session_t* new_session = new session_t();
      |                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:137:34: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  137 |     memcpy(new_session, session, sizeof(session_t));
      |                                  ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual void TinyDtlsProvider::destroySession(void*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:143:9: warning: possible problem detected in invocation of ‘operator delete’ [-Wdelete-incomplete]
  143 |         delete static_cast<session_t*>(dtls_session);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:143:16: warning: invalid use of incomplete type ‘struct session_t’
  143 |         delete static_cast<session_t*>(dtls_session);
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘struct session_t’
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:143:9: note: neither the destructor nor the class-specific ‘operator delete’ will be called, even if they are declared when the class is defined
  143 |         delete static_cast<session_t*>(dtls_session);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::setPSK(void*, const char*, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:148:36: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  148 | int TinyDtlsProvider::setPSK(void* ctx, const char* identity,
      |                              ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:148:53: warning: unused parameter ‘identity’ [-Wunused-parameter]
  148 | int TinyDtlsProvider::setPSK(void* ctx, const char* identity,
      |                                         ~~~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:149:45: warning: unused parameter ‘key’ [-Wunused-parameter]
  149 |                              const uint8_t* key, size_t key_len) {
      |                              ~~~~~~~~~~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:149:57: warning: unused parameter ‘key_len’ [-Wunused-parameter]
  149 |                              const uint8_t* key, size_t key_len) {
      |                                                  ~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::setRPK(void*, const uint8_t*, size_t, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:36: warning: unused parameter ‘ctx’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                              ~~~~~~^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:56: warning: unused parameter ‘priv_key’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                         ~~~~~~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:164:73: warning: unused parameter ‘priv_len’ [-Wunused-parameter]
  164 | int TinyDtlsProvider::setRPK(void* ctx, const uint8_t* priv_key, size_t priv_len,
      |                                                                  ~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:165:45: warning: unused parameter ‘pub_key’ [-Wunused-parameter]
  165 |                              const uint8_t* pub_key, size_t pub_len) {
      |                              ~~~~~~~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:165:61: warning: unused parameter ‘pub_len’ [-Wunused-parameter]
  165 |                              const uint8_t* pub_key, size_t pub_len) {
      |                                                      ~~~~~~~^~~~~~~
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:181:72: warning: unused parameter ‘ca_file’ [-Wunused-parameter]
  181 |                                      const char* key_file, const char* ca_file) {
      |                                                            ~~~~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual int TinyDtlsProvider::send(void*, const uint8_t*, size_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:187:34: warning: unused parameter ‘dtls_session’ [-Wunused-parameter]
  187 | int TinyDtlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
      |                            ~~~~~~^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:187:63: warning: unused parameter ‘data’ [-Wunused-parameter]
  187 | int TinyDtlsProvider::send(void* dtls_session, const uint8_t* data, size_t len) {
      |                                                ~~~~~~~~~~~~~~~^~~~
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:318:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  318 |     return new MbedTlsProvider();
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:117:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  117 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:247:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  247 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:254:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  254 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:335:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  335 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:341:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  341 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:318:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  318 |     return new MbedTlsProvider();
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:117:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  117 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:247:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  247 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:254:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  254 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:335:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  335 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:341:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  341 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:156: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:156: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:142: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:142: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual bool MbedTlsProvider::isCIDNegotiated(void*) const’:
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In member function ‘virtual bool MbedTlsProvider::isCIDNegotiated(void*) const’:
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToString(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:687:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  687 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:689:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  689 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:690:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  690 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static int MbedTlsProvider::staticRecvTimeoutCallback(void*, unsigned char*, size_t, uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:656:16: error: ‘MBEDTLS_ERR_NET_RECV_FAILED’ was not declared in this scope; did you mean ‘MBEDTLS_ERR_RSA_RNG_FAILED’?
  656 |         return MBEDTLS_ERR_NET_RECV_FAILED;
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                MBEDTLS_ERR_RSA_RNG_FAILED
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:691:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  691 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:693:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  693 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:654:58: warning: unused parameter ‘timeout’ [-Wunused-parameter]
  654 |                                                 uint32_t timeout) {
      |                                                 ~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToString(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:687:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  687 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:689:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  689 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:690:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  690 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:691:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  691 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:693:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  693 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
[ 33%] Built target friendly_lwm2m
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:170: CMakeFiles/test_dtls_integration.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:170: CMakeFiles/test_dtls_providers.dir/examples/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
[ 38%] Building CXX object CMakeFiles/test_blockwise_transfer.dir/test/test_blockwise_transfer.cpp.o
[ 38%] Building CXX object CMakeFiles/test_delta_algorithms.dir/test/test_delta_algorithms.cpp.o
[ 38%] Building CXX object CMakeFiles/test_rollback_manager.dir/test/test_rollback_manager.cpp.o
[ 38%] Building CXX object CMakeFiles/test_cid_integration.dir/tests/test_cid_integration.cpp.o
[ 38%] Building CXX object CMakeFiles/test_cid_extensions.dir/tests/test_cid_extensions.cpp.o
[ 39%] Building CXX object CMakeFiles/test_cid_aad.dir/tests/test_cid_aad.cpp.o
[ 39%] Building CXX object CMakeFiles/test_nat_rebinding.dir/tests/test_nat_rebinding.cpp.o
[ 39%] Building CXX object CMakeFiles/test_cid_record_format.dir/tests/test_cid_record_format.cpp.o
[ 39%] Building CXX object CMakeFiles/test_firmware_update_object.dir/test/test_firmware_update_object.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/TinyDtlsProvider.h:21,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:17:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_integration.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_nat_rebinding.cpp:21:14: fatal error: global.h: No such file or directory
   21 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:13:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:24:26: note: previous declaration as ‘typedef struct session_t session_t’
   24 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
make[2]: *** [CMakeFiles/test_cid_integration.dir/build.make:79: CMakeFiles/test_cid_integration.dir/tests/test_cid_integration.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_nat_rebinding.dir/build.make:79: CMakeFiles/test_nat_rebinding.dir/tests/test_nat_rebinding.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1499: CMakeFiles/test_cid_integration.dir/all] Error 2
make[1]: *** Waiting for unfinished jobs....
make[1]: *** [CMakeFiles/Makefile2:1531: CMakeFiles/test_nat_rebinding.dir/all] Error 2
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_extensions.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_aad.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/TinyDtlsProvider.h:21,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
compilation terminated.
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:24:26: note: previous declaration as ‘typedef struct session_t session_t’
   24 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
make[2]: *** [CMakeFiles/test_cid_extensions.dir/build.make:79: CMakeFiles/test_cid_extensions.dir/tests/test_cid_extensions.cpp.o] Error 1
make[2]: *** [CMakeFiles/test_cid_aad.dir/build.make:79: CMakeFiles/test_cid_aad.dir/tests/test_cid_aad.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1403: CMakeFiles/test_cid_extensions.dir/all] Error 2
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
make[1]: *** [CMakeFiles/Makefile2:1467: CMakeFiles/test_cid_aad.dir/all] Error 2
/mnt/d/Dev/Friendly-LwM2M-Client/tests/test_cid_record_format.cpp:20:14: fatal error: global.h: No such file or directory
   20 |     #include "global.h"
      |              ^~~~~~~~~~
compilation terminated.
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: warning: ISO C++ forbids flexible array member ‘cookie’ [-Wpedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:21:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
make[2]: *** [CMakeFiles/test_cid_record_format.dir/build.make:79: CMakeFiles/test_cid_record_format.dir/tests/test_cid_record_format.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1435: CMakeFiles/test_cid_record_format.dir/all] Error 2
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:24:
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘session_t* DtlsIntegrationTest::createMockSession()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:90:26: error: aggregate ‘session_t mockSession’ has incomplete type and cannot be defined
   90 |         static session_t mockSession;
      |                          ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp: In member function ‘virtual void MbedTlsProviderTest::SetUp()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_providers.cpp:206:40: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  206 |         provider = new MbedTlsProvider();
      |                                        ^
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:117:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  117 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:247:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  247 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:254:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  254 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:335:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  335 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:341:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  341 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_HandshakeInitiation_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:141:9: warning: unused variable ‘result’ [-Wunused-variable]
  141 |     int result = provider->connect(session);
      |         ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_MultipleSessionCreation_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:278:20: error: invalid use of incomplete type ‘session_’ {aka ‘struct session_t’}
  278 |         mockSession->addr.sin.sin_port = htons(5684 + i);
      |                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void DtlsIntegrationTest_SessionCreationPerformance_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:379:20: error: invalid use of incomplete type ‘session_’ {aka ‘struct session_t’}
  379 |         mockSession->addr.sin.sin_port = htons(5684 + (i % 100));
      |                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp: In member function ‘virtual void MbedTlsIntegrationTest_CertificateMode_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_dtls_integration.cpp:416:21: error: cannot declare variable ‘provider’ to be of abstract type ‘MbedTlsProvider’
  416 |     MbedTlsProvider provider;
      |                     ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:117:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  117 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:247:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  247 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:254:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  254 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:335:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  335 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/../examples/platform/connection_dtls/DtlsProvider.h:341:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  341 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
make[2]: *** [CMakeFiles/test_dtls_providers.dir/build.make:128: CMakeFiles/test_dtls_providers.dir/test/test_dtls_providers.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1563: CMakeFiles/test_dtls_providers.dir/all] Error 2
make[2]: *** [CMakeFiles/test_dtls_integration.dir/build.make:128: CMakeFiles/test_dtls_integration.dir/test/test_dtls_integration.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1627: CMakeFiles/test_dtls_integration.dir/all] Error 2
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_rollback_manager.cpp: In member function ‘virtual lwm2m::platform::PlatformResult lwm2m::firmware::test::MockPartitionManager::verifyPartition(lwm2m::platform::PartitionSlot, const std::vector<unsigned char>&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_rollback_manager.cpp:98:70: warning: unused parameter ‘slot’ [-Wunused-parameter]
   98 |     platform::PlatformResult verifyPartition(platform::PartitionSlot slot,
      |                                              ~~~~~~~~~~~~~~~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_rollback_manager.cpp:99:75: warning: unused parameter ‘expectedChecksum’ [-Wunused-parameter]
   99 |                                               const std::vector<uint8_t>& expectedChecksum) override {
      |                                               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~
[ 40%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o
[ 40%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o
[ 41%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o
[ 42%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o
[ 43%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/WppPlatform.cpp.o
[ 43%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o
[ 44%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/bindings/WppBindings.cpp.o
[ 45%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/main.cpp.o
[ 45%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o
[ 46%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/objects.cpp.o
[ 47%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/Connection.cpp.o
[ 47%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/Guard.cpp.o
[ 49%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/Platform.cpp.o
[ 49%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/FirmwareUpdater/FirmwareUpdater.cpp.o
[ 49%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o
[ 50%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/MbedTlsProvider.cpp.o
[ 51%] Building CXX object examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o
[ 51%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/dtls.c.o
[ 52%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/crypto.c.o
[ 53%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/ccm.c.o
[ 53%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/hmac.c.o
[ 54%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/dtls_debug.c.o
[ 55%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/netq.c.o
[ 55%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/peer.c.o
[ 56%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/dtls_prng.c.o
[ 57%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/dtls_time.c.o
[ 57%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/session.c.o
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:33:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   33 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/utils/task_queue/WppTaskQueue.cpp:60:126: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
   60 |                 WPP_LOGE(TAG_WPP_TASK, "Can't add task becouse task id is WPP_ERR_TASK_ID, looks like all task ids are busy.");
      |                                                                                                                              ^
[ 58%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/aes/rijndael.c.o
[ 59%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/aes/rijndael_wrap.c.o
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/WppRegistry.cpp:8:55: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
    8 |     WPP_LOGD(TAG_WPP_REG, "Creating registry instance");
      |                                                       ^
[ 60%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/sha2/sha2.c.o
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
[ 60%] Building C object examples/CMakeFiles/friendly_example_client.dir/__/2305-Wakaama/examples/shared/tinydtls/ecc/ecc.c.o
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
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:312:68: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  312 |                 WPP_LOGE(TAG, "Security object instance is not set");
      |                                                                    ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:320:81: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  320 |                 WPP_LOGD(TAG, "Server reset state machine through PACKAGE_URI_1");
      |                                                                                 ^
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp:349:77: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  349 |                 WPP_LOGD(TAG, "Server reset state machine through PACKAGE_0");
      |                                                                             ^
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:21,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:15:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:26: note: previous declaration as typedef struct session_t session_t’
   24 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:30,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:32,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.h:21,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:16:
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/session.h:102:3: error: conflicting declaration typedef struct session_t session_t’
  102 | } session_t;
      |   ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:26: note: previous declaration as typedef struct session_t session_t’
   24 | typedef struct session_t session_t;
      |                          ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual void* TinyDtlsProvider::createSession(void*, const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:132:44: error: invalid use of incomplete type ‘session_t’ {aka ‘struct session_t’}
  132 |     session_t* new_session = new session_t();
      |                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:137:34: error: invalid application of ‘sizeof’ to incomplete type ‘session_t’
  137 |     memcpy(new_session, session, sizeof(session_t));
      |                                  ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp: In member function ‘virtual void TinyDtlsProvider::destroySession(void*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:143:9: error: possible problem detected in invocation of ‘operator delete’ [-Werror=delete-incomplete]
  143 |         delete static_cast<session_t*>(dtls_session);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:143:16: error: invalid use of incomplete type ‘struct session_t’ [-Werror]
  143 |         delete static_cast<session_t*>(dtls_session);
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘struct session_t’
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/TinyDtlsProvider.cpp:143:9: note: neither the destructor nor the class-specific ‘operator delete’ will be called, even if they are declared when the class is defined
  143 |         delete static_cast<session_t*>(dtls_session);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/peer.h:51:13: error: field ‘session’ has incomplete type ‘session_t’
   51 |   session_t session;         /**< peer address and local interface */
      |             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/2305-Wakaama/examples/shared/tinydtls/dtls.h:395:9: error: ISO C++ forbids flexible array member ‘cookie’ [-Werror=pedantic]
  395 |   uint8 cookie[];               /**< up to 32 bytes making up the cookie */
      |         ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:20:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:891: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/TinyDtlsProvider.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp: In static member function ‘static DtlsProvider* DtlsProviderFactory::createMbedTLS()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProviderFactory.cpp:318:32: error: invalid new-expression of abstract class type ‘MbedTlsProvider’
  318 |     return new MbedTlsProvider();
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:35:7: note:   because the following virtual functions are pure within ‘MbedTlsProvider’:
   35 | class MbedTlsProvider : public DtlsProvider {
      |       ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:117:19: note:     ‘virtual void* DtlsProvider::getPeer(void*, const session_t*)’
  117 |     virtual void* getPeer(void* ctx, const session_t* session) = 0;
      |                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:247:17: note:     ‘virtual int DtlsProvider::getOurCIDLength(void*) const’
  247 |     virtual int getOurCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:254:17: note:     ‘virtual int DtlsProvider::getPeerCIDLength(void*) const’
  254 |     virtual int getPeerCIDLength(void* dtls_session) const = 0;
      |                 ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:335:25: note:     ‘virtual const char* DtlsProvider::getLastError() const’
  335 |     virtual const char* getLastError() const = 0;
      |                         ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:341:18: note:     ‘virtual void DtlsProvider::setDebugLogging(bool)’
  341 |     virtual void setDebugLogging(bool enable) = 0;
      |                  ^~~~~~~~~~~~~~~
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:919: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/DtlsProviderFactory.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:12:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:126:19: error: field ‘peer_session’ has incomplete type ‘session_t’
  126 |         session_t peer_session;
      |                   ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.h:13:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp: In static member function ‘static std::string MbedTlsProvider::sessionToString(const session_t*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:687:16: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  687 |     if (session->addr.sa.sa_family == AF_INET) {
      |                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:689:36: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  689 |         inet_ntop(AF_INET, &session->addr.sin.sin_addr, addr_str, INET_ADDRSTRLEN);
      |                                    ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:690:48: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  690 |         oss << addr_str << ":" << ntohs(session->addr.sin.sin_port);
      |                                                ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:691:23: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  691 |     } else if (session->addr.sa.sa_family == AF_INET6) {
      |                       ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:693:37: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  693 |         inet_ntop(AF_INET6, &session->addr.sin6.sin6_addr, addr_str, INET6_ADDRSTRLEN);
      |                                     ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/MbedTlsProvider.cpp:694:56: error: invalid use of incomplete type ‘const session_t’ {aka ‘const struct session_t’}
  694 |         oss << "[" << addr_str << "]:" << ntohs(session->addr.sin6.sin6_port);
      |                                                        ^~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/DtlsProvider.h:24:16: note: forward declaration of ‘session_t’ {aka ‘struct session_t’}
   24 | typedef struct session_t session_t;
      |                ^~~~~~~~~
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:905: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/MbedTlsProvider.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:751: examples/CMakeFiles/friendly_example_client.dir/__/wpp/platform/connection/WppConnection.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:555: examples/CMakeFiles/friendly_example_client.dir/__/wpp/client/WppClient.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘bool Connection:isCidNegotiated(wpp::WppConnection::SESSION_T)’:
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp: In member function ‘int Connection::getPeerCidLength(wpp::WppConnection::SESSION_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/platform/connection_dtls/Connection.cpp:587:18: error: ‘dtls_peer_t’ {aka ‘struct dtls_peer_t’} has no member named ‘peer_cid_len’
  587 |     return peer->peer_cid_len;
      |                  ^~~~~~~~~~~~
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:835: examples/CMakeFiles/friendly_example_client.dir/platform/connection_dtls/Connection.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/examples/main.cpp:187:98: error: ISO C++11 requires at least one argument for the "..." in a variadic macro [-Werror]
  187 |                 WPP_LOGD(TAG_WPP_TASK, "Task: Send operation, sending current time to the server");
      |                                                                                                  ^
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:793: examples/CMakeFiles/friendly_example_client.dir/__/wpp/utils/task_queue/WppTaskQueue.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:737: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/WppRegistry.cpp.o] Error 1
[ 61%] Linking CXX executable test_blockwise_transfer
[ 61%] Built target test_blockwise_transfer
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:807: examples/CMakeFiles/friendly_example_client.dir/main.cpp.o] Error 1
[ 62%] Linking CXX executable test_rollback_manager
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:639: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/m_1_lwm2m_server/Lwm2mServer.cpp.o] Error 1
[ 62%] Built target test_rollback_manager
[ 63%] Linking CXX executable test_firmware_update_object
cc1plus: all warnings being treated as errors
make[2]: *** [examples/CMakeFiles/friendly_example_client.dir/build.make:709: examples/CMakeFiles/friendly_example_client.dir/__/wpp/registry/objects/o_5_firmware_update/FirmwareUpdate.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1743: examples/CMakeFiles/friendly_example_client.dir/all] Error 2
[ 63%] Built target test_firmware_update_object
[ 63%] Linking CXX executable test_delta_algorithms
[ 63%] Built target test_delta_algorithms
make: *** [Makefile:166: all] Error 2