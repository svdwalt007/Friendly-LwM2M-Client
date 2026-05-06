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
-- Generating done (3.9s)
-- Build files have been written to: /mnt/d/Dev/Friendly-LwM2M-Client/_build
[  1%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUpdater.cpp.o
[  3%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwDownloader.cpp.o
[  3%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwInit.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwInit.h:14,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwInit.cpp:7:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUriDownloader.h:14:10: fatal error: m_0_lwm2m_security/Lwm2mSecurity.h: No such file or directory
   14 | #include "m_0_lwm2m_security/Lwm2mSecurity.h"
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:107: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwInit.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:15,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h: In function ‘wpp::TYPE_ID wpp::dataTypeToID()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:80:12: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   80 |         if constexpr (std::is_same<T, BOOL_T>::value) typeID = TYPE_ID::BOOL;
      |            ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:81:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   81 |         else if constexpr (std::is_same<T, INT_T>::value) typeID = TYPE_ID::INT;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:82:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   82 |         else if constexpr (std::is_same<T, UINT_T>::value) typeID = TYPE_ID::UINT;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:83:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   83 |         else if constexpr (std::is_same<T, FLOAT_T>::value) typeID = TYPE_ID::FLOAT;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:84:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   84 |         else if constexpr (std::is_same<T, OPAQUE_T>::value) typeID = TYPE_ID::OPAQUE;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:85:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   85 |         else if constexpr (std::is_same<T, OBJ_LINK_T>::value) typeID = TYPE_ID::OBJ_LINK;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:86:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   86 |         else if constexpr (std::is_same<T, STRING_T>::value) typeID = TYPE_ID::STRING;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:87:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   87 |         else if constexpr (std::is_same<T, EXECUTE_T>::value) typeID = TYPE_ID::EXECUTE;
      |                 ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/Object.h:14,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/WppRegistry.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:16:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/observer/ObjSubject.h: In member function ‘void wpp::ObjSubject::opSubscribe(wpp::ObjOpObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/observer/ObjSubject.h:30:18: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   30 |         if (std::find(_opObservers.begin(), _opObservers.end(), observer) == _opObservers.end())
      |                  ^~~~
      |                  bind
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/observer/ObjSubject.h: In member function ‘void wpp::ObjSubject::opUnsubscribe(wpp::ObjOpObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/observer/ObjSubject.h:40:33: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   40 |         _opObservers.erase(std::find(_opObservers.begin(), _opObservers.end(), observer));
      |                                 ^~~~
      |                                 bind
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/Instance.h:14,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/Object.h:15:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h: At global scope:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:54:29: error: ‘variant’ in namespace ‘std’ does not name a template type
   54 |         using DATA_T = std::variant<BOOL_T, INT_T, UINT_T, FLOAT_T, OPAQUE_T, OBJ_LINK_T, STRING_T, EXECUTE_T>;
      |                             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:54:24: note: ‘std::variant’ is only available from C++17 onwards
   54 |         using DATA_T = std::variant<BOOL_T, INT_T, UINT_T, FLOAT_T, OPAQUE_T, OBJ_LINK_T, STRING_T, EXECUTE_T>;
      |                        ^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:61:17: error: ‘DATA_T’ does not name a type
   61 |                 DATA_T data;
      |                 ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:67:38: error: ‘variant’ in namespace ‘std’ does not name a template type
   67 |         using DATA_VERIFIER_T = std::variant<VERIFY_INT_T, VERIFY_UINT_T, VERIFY_FLOAT_T, VERIFY_OPAQUE_T, VERIFY_BOOL_T,
      |                                      ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:67:33: note: ‘std::variant’ is only available from C++17 onwards
   67 |         using DATA_VERIFIER_T = std::variant<VERIFY_INT_T, VERIFY_UINT_T, VERIFY_FLOAT_T, VERIFY_OPAQUE_T, VERIFY_BOOL_T,
      |                                 ^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:135:36: error: ‘DATA_VERIFIER_T’ does not name a type
  135 |         bool setDataVerifier(const DATA_VERIFIER_T &verifier);
      |                                    ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:229:40: error: ‘DATA_VERIFIER_T’ does not name a type
  229 |         bool isDataVerifierValid(const DATA_VERIFIER_T &verifier) const;
      |                                        ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:241:5: error: ‘DATA_VERIFIER_T’ does not name a type
  241 |     DATA_VERIFIER_T _dataVerifier;
      |     ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h: In member function ‘bool wpp::Resource::set(const T&, wpp::ID_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:262:27: error: ‘struct wpp::Resource::ResInst’ has no member named ‘data’
  262 |                 instIter->data = value;
      |                           ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h: In member function ‘bool wpp::Resource::set(T&&, wpp::ID_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:280:27: error: ‘struct wpp::Resource::ResInst’ has no member named ‘data’
  280 |                 instIter->data = std::move(value);
      |                           ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h: In member function ‘const T& wpp::Resource::get(wpp::ID_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:302:38: error: ‘struct wpp::Resource::ResInst’ has no member named ‘data’
  302 |         return std::get<T>(instIter->data);
      |                                      ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h: In member function ‘bool wpp::Resource::isDataValueValid(const T&) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:351:34: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  351 |         if (!isDataVerifierValid(_dataVerifier)) return true;
      |                                  ^~~~~~~~~~~~~
      |                                  setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:353:12: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  353 |         if constexpr (std::is_same<T, BOOL_T>::value) {
      |            ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:354:48: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  354 |                 return std::get<VERIFY_BOOL_T>(_dataVerifier)(data);
      |                                                ^~~~~~~~~~~~~
      |                                                setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:355:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  355 |         } else if constexpr (std::is_same<T, INT_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:356:47: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  356 |                 return std::get<VERIFY_INT_T>(_dataVerifier)(data);
      |                                               ^~~~~~~~~~~~~
      |                                               setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:357:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  357 |         } else if constexpr (std::is_same<T, UINT_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:358:48: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  358 |                 return std::get<VERIFY_UINT_T>(_dataVerifier)(data);
      |                                                ^~~~~~~~~~~~~
      |                                                setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:359:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  359 |         } else if constexpr (std::is_same<T, FLOAT_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:360:49: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  360 |                 return std::get<VERIFY_FLOAT_T>(_dataVerifier)(data);
      |                                                 ^~~~~~~~~~~~~
      |                                                 setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:361:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  361 |         } else if constexpr (std::is_same<T, OPAQUE_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:362:50: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  362 |                 return std::get<VERIFY_OPAQUE_T>(_dataVerifier)(data);
      |                                                  ^~~~~~~~~~~~~
      |                                                  setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:363:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  363 |         } else if constexpr (std::is_same<T, OBJ_LINK_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:364:52: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  364 |                 return std::get<VERIFY_OBJ_LINK_T>(_dataVerifier)(data);
      |                                                    ^~~~~~~~~~~~~
      |                                                    setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:365:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  365 |         } else if constexpr (std::is_same<T, STRING_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:366:50: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  366 |                 return std::get<VERIFY_STRING_T>(_dataVerifier)(data);
      |                                                  ^~~~~~~~~~~~~
      |                                                  setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:367:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  367 |         } else if constexpr (std::is_same<T, EXECUTE_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:368:51: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  368 |                 return std::get<VERIFY_EXECUTE_T>(_dataVerifier)(data);
      |                                                   ^~~~~~~~~~~~~
      |                                                   setDataVerifier
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/Instance.h:17:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h: In member function ‘void wpp::InstSubject::opSubscribe(wpp::InstOpObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h:32:18: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   32 |         if (std::find(_opObservers.begin(), _opObservers.end(), observer) == _opObservers.end())
      |                  ^~~~
      |                  bind
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h: In member function ‘void wpp::InstSubject::opUnsubscribe(wpp::InstOpObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h:42:33: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   42 |         _opObservers.erase(std::find(_opObservers.begin(), _opObservers.end(), observer));
      |                                 ^~~~
      |                                 bind
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h: In member function ‘void wpp::InstSubject::eventSubscribe(wpp::InstEventObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h:54:18: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   54 |         if (std::find(_eventObservers.begin(), _eventObservers.end(), observer) == _eventObservers.end())
      |                  ^~~~
      |                  bind
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h: In member function ‘void wpp::InstSubject::eventUnsubscribe(wpp::InstEventObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h:64:36: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   64 |         _eventObservers.erase(std::find(_eventObservers.begin(), _eventObservers.end(), observer));
      |                                    ^~~~
      |                                    bind
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
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:15,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h: In function ‘wpp::TYPE_ID wpp::dataTypeToID()’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:80:12: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   80 |         if constexpr (std::is_same<T, BOOL_T>::value) typeID = TYPE_ID::BOOL;
      |            ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:81:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   81 |         else if constexpr (std::is_same<T, INT_T>::value) typeID = TYPE_ID::INT;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:82:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   82 |         else if constexpr (std::is_same<T, UINT_T>::value) typeID = TYPE_ID::UINT;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:83:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   83 |         else if constexpr (std::is_same<T, FLOAT_T>::value) typeID = TYPE_ID::FLOAT;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:84:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   84 |         else if constexpr (std::is_same<T, OPAQUE_T>::value) typeID = TYPE_ID::OPAQUE;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:85:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   85 |         else if constexpr (std::is_same<T, OBJ_LINK_T>::value) typeID = TYPE_ID::OBJ_LINK;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:86:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   86 |         else if constexpr (std::is_same<T, STRING_T>::value) typeID = TYPE_ID::STRING;
      |                 ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/types/WppTypes.h:87:17: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
   87 |         else if constexpr (std::is_same<T, EXECUTE_T>::value) typeID = TYPE_ID::EXECUTE;
      |                 ^~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/Object.h:14,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/WppRegistry.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:16:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/observer/ObjSubject.h: In member function ‘void wpp::ObjSubject::opSubscribe(wpp::ObjOpObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/observer/ObjSubject.h:30:18: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   30 |         if (std::find(_opObservers.begin(), _opObservers.end(), observer) == _opObservers.end())
      |                  ^~~~
      |                  bind
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/observer/ObjSubject.h: In member function ‘void wpp::ObjSubject::opUnsubscribe(wpp::ObjOpObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/observer/ObjSubject.h:40:33: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   40 |         _opObservers.erase(std::find(_opObservers.begin(), _opObservers.end(), observer));
      |                                 ^~~~
      |                                 bind
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/Instance.h:14,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/Object.h:15:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h: At global scope:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:54:29: error: ‘variant’ in namespace ‘std’ does not name a template type
   54 |         using DATA_T = std::variant<BOOL_T, INT_T, UINT_T, FLOAT_T, OPAQUE_T, OBJ_LINK_T, STRING_T, EXECUTE_T>;
      |                             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:54:24: note: ‘std::variant’ is only available from C++17 onwards
   54 |         using DATA_T = std::variant<BOOL_T, INT_T, UINT_T, FLOAT_T, OPAQUE_T, OBJ_LINK_T, STRING_T, EXECUTE_T>;
      |                        ^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:61:17: error: ‘DATA_T’ does not name a type
   61 |                 DATA_T data;
      |                 ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:67:38: error: ‘variant’ in namespace ‘std’ does not name a template type
   67 |         using DATA_VERIFIER_T = std::variant<VERIFY_INT_T, VERIFY_UINT_T, VERIFY_FLOAT_T, VERIFY_OPAQUE_T, VERIFY_BOOL_T,
      |                                      ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:67:33: note: ‘std::variant’ is only available from C++17 onwards
   67 |         using DATA_VERIFIER_T = std::variant<VERIFY_INT_T, VERIFY_UINT_T, VERIFY_FLOAT_T, VERIFY_OPAQUE_T, VERIFY_BOOL_T,
      |                                 ^~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:135:36: error: ‘DATA_VERIFIER_T’ does not name a type
  135 |         bool setDataVerifier(const DATA_VERIFIER_T &verifier);
      |                                    ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:229:40: error: ‘DATA_VERIFIER_T’ does not name a type
  229 |         bool isDataVerifierValid(const DATA_VERIFIER_T &verifier) const;
      |                                        ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:241:5: error: ‘DATA_VERIFIER_T’ does not name a type
  241 |     DATA_VERIFIER_T _dataVerifier;
      |     ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h: In member function ‘bool wpp::Resource::set(const T&, wpp::ID_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:262:27: error: ‘struct wpp::Resource::ResInst’ has no member named ‘data’
  262 |                 instIter->data = value;
      |                           ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h: In member function ‘bool wpp::Resource::set(T&&, wpp::ID_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:280:27: error: ‘struct wpp::Resource::ResInst’ has no member named ‘data’
  280 |                 instIter->data = std::move(value);
      |                           ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h: In member function ‘const T& wpp::Resource::get(wpp::ID_T)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:302:38: error: ‘struct wpp::Resource::ResInst’ has no member named ‘data’
  302 |         return std::get<T>(instIter->data);
      |                                      ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h: In member function ‘bool wpp::Resource::isDataValueValid(const T&) const’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:351:34: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  351 |         if (!isDataVerifierValid(_dataVerifier)) return true;
      |                                  ^~~~~~~~~~~~~
      |                                  setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:353:12: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  353 |         if constexpr (std::is_same<T, BOOL_T>::value) {
      |            ^~~~~~~~~
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:354:48: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  354 |                 return std::get<VERIFY_BOOL_T>(_dataVerifier)(data);
      |                                                ^~~~~~~~~~~~~
      |                                                setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:355:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  355 |         } else if constexpr (std::is_same<T, INT_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:356:47: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  356 |                 return std::get<VERIFY_INT_T>(_dataVerifier)(data);
      |                                               ^~~~~~~~~~~~~
      |                                               setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:357:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  357 |         } else if constexpr (std::is_same<T, UINT_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:358:48: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  358 |                 return std::get<VERIFY_UINT_T>(_dataVerifier)(data);
      |                                                ^~~~~~~~~~~~~
      |                                                setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:359:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  359 |         } else if constexpr (std::is_same<T, FLOAT_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:360:49: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  360 |                 return std::get<VERIFY_FLOAT_T>(_dataVerifier)(data);
      |                                                 ^~~~~~~~~~~~~
      |                                                 setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:361:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  361 |         } else if constexpr (std::is_same<T, OPAQUE_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:362:50: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  362 |                 return std::get<VERIFY_OPAQUE_T>(_dataVerifier)(data);
      |                                                  ^~~~~~~~~~~~~
      |                                                  setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:363:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  363 |         } else if constexpr (std::is_same<T, OBJ_LINK_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:364:52: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  364 |                 return std::get<VERIFY_OBJ_LINK_T>(_dataVerifier)(data);
      |                                                    ^~~~~~~~~~~~~
      |                                                    setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:365:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  365 |         } else if constexpr (std::is_same<T, STRING_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:366:50: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  366 |                 return std::get<VERIFY_STRING_T>(_dataVerifier)(data);
      |                                                  ^~~~~~~~~~~~~
      |                                                  setDataVerifier
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:367:19: error: ‘if constexpr’ only available with ‘-std=c++17’ or ‘-std=gnu++17’ [-Werror=c++17-extensions]
  367 |         } else if constexpr (std::is_same<T, EXECUTE_T>::value) {
      |                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/resource/Resource.h:368:51: error: ‘_dataVerifier’ was not declared in this scope; did you mean ‘setDataVerifier’?
  368 |                 return std::get<VERIFY_EXECUTE_T>(_dataVerifier)(data);
      |                                                   ^~~~~~~~~~~~~
      |                                                   setDataVerifier
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/Instance.h:17:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h: In member function ‘void wpp::InstSubject::opSubscribe(wpp::InstOpObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h:32:18: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   32 |         if (std::find(_opObservers.begin(), _opObservers.end(), observer) == _opObservers.end())
      |                  ^~~~
      |                  bind
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h: In member function ‘void wpp::InstSubject::opUnsubscribe(wpp::InstOpObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h:42:33: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   42 |         _opObservers.erase(std::find(_opObservers.begin(), _opObservers.end(), observer));
      |                                 ^~~~
      |                                 bind
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h: In member function ‘void wpp::InstSubject::eventSubscribe(wpp::InstEventObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h:54:18: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   54 |         if (std::find(_eventObservers.begin(), _eventObservers.end(), observer) == _eventObservers.end())
      |                  ^~~~
      |                  bind
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h: In member function ‘void wpp::InstSubject::eventUnsubscribe(wpp::InstEventObserver*)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/instance/observer/InstSubject.h:64:36: error: ‘find’ is not a member of ‘std’; did you mean ‘bind’?
   64 |         _eventObservers.erase(std::find(_eventObservers.begin(), _eventObservers.end(), observer));
      |                                    ^~~~
      |                                    bind
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:98:36: error: exception handling disabled, use ‘-fexceptions’ to enable
   98 |     } catch (const std::exception& e) {
      |                                    ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:99:77: error: ‘e’ was not declared in this scope
   99 |         cout << "OpenWrtFwDownloader: ERROR - Exception while writing: " << e.what() << endl;
      |                                                                             ^
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:221:9: error: ‘_downloadResult’ was not declared in this scope
  221 |         _downloadResult = FwUpdRes::R_UNSUPPORTED_PKG_TYPE;
      |         ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:221:27: error: ‘FwUpdRes’ has not been declared
  221 |         _downloadResult = FwUpdRes::R_UNSUPPORTED_PKG_TYPE;
      |                           ^~~~~~~~
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
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:163:36: error: exception handling disabled, use ‘-fexceptions’ to enable
  163 |     } catch (const std::exception& e) {
      |                                    ^
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:164:74: error: ‘e’ was not declared in this scope
  164 |         cout << "OpenWrtFwUpdater: ERROR - Exception during update: " << e.what() << endl;
      |                                                                          ^
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
cc1plus: all warnings being treated as errors
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:194:13: error: ‘_lastUpdateResult’ was not declared in this scope
  194 |             _lastUpdateResult = FwUpdRes::R_FW_UPD_FAIL;
      |             ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:194:33: error: ‘FwUpdRes’ has not been declared
  194 |             _lastUpdateResult = FwUpdRes::R_FW_UPD_FAIL;
      |                                 ^~~~~~~~
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:93: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwDownloader.cpp.o] Error 1
cc1plus: all warnings being treated as errors
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:79: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUpdater.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1189: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/all] Error 2
make: *** [Makefile:136: all] Error 2