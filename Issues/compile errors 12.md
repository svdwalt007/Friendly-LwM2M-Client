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
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/_build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/_build$
sean@SvdWGPU:/mnt/d/Dev/Friendly-LwM2M-Client/_build$ make -j$(nproc)
[  3%] Building CXX object CMakeFiles/WppExample.dir/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/o_4_connectivity_monitoring/ConnectivityMonitoring.cpp.o
[  7%] Building CXX object CMakeFiles/WppExample.dir/main.cpp.o
[ 11%] Building CXX object CMakeFiles/WppExample.dir/platform/connection_dtls/Connection.cpp.o
[ 15%] Linking CXX executable WppExample
lto-wrapper: warning: using serial compilation of 2 LTRANS jobs
lto-wrapper: note: see the ‘-flto’ option documentation for more information
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `std::_Function_handler<bool (wpp::WppClient&, void*), wpp::Lwm2mServer::resourcesInit()::{lambda(wpp::Instance&, unsigned short, std::vector<unsigned char, std::allocator<unsigned char> > const&)#1}::operator()(wpp::Instance&, unsigned short, std::vector<unsigned char, std::allocator<unsigned char> > const&) const::{lambda(wpp::WppClient&, void*)#1}>::_M_invoke(std::_Any_data const&, wpp::WppClient&, void*&&)':
<artificial>:(.text._ZNSt17_Function_handlerIFbRN3wpp9WppClientEPvEZZNS0_11Lwm2mServer13resourcesInitEvENKUlRNS0_8InstanceEtRKSt6vectorIhSaIhEEE_clES7_tSC_EUlS2_S3_E_E9_M_invokeERKSt9_Any_dataS2_OS3_+0xd): undefined reference to `lwm2m_deregister'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::Lwm2mAccessControl::serverOperationNotifier(wpp::Instance*, wpp::ItemOp::TYPE, wpp::ResLink const&)':
<artificial>:(.text._ZN3wpp18Lwm2mAccessControl23serverOperationNotifierEPNS_8InstanceENS_6ItemOp4TYPEERKNS_7ResLinkE+0x17): undefined reference to `lwm2m_ac_request_update_policy'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::Object::clear()':
<artificial>:(.text._ZN3wpp6Object5clearEv+0x5f): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::Object::remove(unsigned short)':
<artificial>:(.text._ZN3wpp6Object6removeEt+0x71): undefined reference to `lwm2m_list_remove'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object6removeEt+0xb5): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::Instance::notifyResChanged(unsigned short, unsigned short)':
<artificial>:(.text._ZN3wpp8Instance16notifyResChangedEtt+0x76): undefined reference to `lwm2m_resource_value_changed'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::Lwm2mAccessControl::userOperationNotifier(wpp::ItemOp::TYPE, wpp::ResLink const&)':
<artificial>:(.text._ZN3wpp18Lwm2mAccessControl21userOperationNotifierENS_6ItemOp4TYPEERKNS_7ResLinkE+0x2d): undefined reference to `lwm2m_ac_request_update_policy'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::Lwm2mAccessControl::~Lwm2mAccessControl()':
<artificial>:(.text._ZN3wpp18Lwm2mAccessControlD2Ev+0x19): undefined reference to `lwm2m_ac_request_update_policy'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::Instance::resourceWrite(_lwm2m_server_*, wpp::Resource&, _lwm2m_data_t const&, lwm2m_write_type_t)':
<artificial>:(.text._ZN3wpp8Instance13resourceWriteEP14_lwm2m_server_RNS_8ResourceERK13_lwm2m_data_t18lwm2m_write_type_t+0x290): undefined reference to `lwm2m_data_decode_bool'
/usr/bin/ld: <artificial>:(.text._ZN3wpp8Instance13resourceWriteEP14_lwm2m_server_RNS_8ResourceERK13_lwm2m_data_t18lwm2m_write_type_t+0x2bc): undefined reference to `lwm2m_data_decode_time'
/usr/bin/ld: <artificial>:(.text._ZN3wpp8Instance13resourceWriteEP14_lwm2m_server_RNS_8ResourceERK13_lwm2m_data_t18lwm2m_write_type_t+0x2ce): undefined reference to `lwm2m_data_decode_int'
/usr/bin/ld: <artificial>:(.text._ZN3wpp8Instance13resourceWriteEP14_lwm2m_server_RNS_8ResourceERK13_lwm2m_data_t18lwm2m_write_type_t+0x2ff): undefined reference to `lwm2m_data_decode_uint'
/usr/bin/ld: <artificial>:(.text._ZN3wpp8Instance13resourceWriteEP14_lwm2m_server_RNS_8ResourceERK13_lwm2m_data_t18lwm2m_write_type_t+0x3e9): undefined reference to `lwm2m_data_decode_float'
/usr/bin/ld: <artificial>:(.text._ZN3wpp8Instance13resourceWriteEP14_lwm2m_server_RNS_8ResourceERK13_lwm2m_data_t18lwm2m_write_type_t+0x427): undefined reference to `lwm2m_data_decode_objlink'
/usr/bin/ld: <artificial>:(.text._ZN3wpp8Instance13resourceWriteEP14_lwm2m_server_RNS_8ResourceERK13_lwm2m_data_t18lwm2m_write_type_t+0x55f): undefined reference to `lwm2m_data_decode_opaque'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::Object::serverRead_clb(_lwm2m_context_*, _lwm2m_server_*, unsigned short, int*, _lwm2m_data_t**, _lwm2m_object_t*)':
<artificial>:(.text._ZN3wpp6Object14serverRead_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x37f): undefined reference to `lwm2m_data_encode_instances'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object14serverRead_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x3e3): undefined reference to `lwm2m_data_new'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object14serverRead_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x4f6): undefined reference to `lwm2m_data_encode_bool'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object14serverRead_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x50d): undefined reference to `lwm2m_data_encode_time'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object14serverRead_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x524): undefined reference to `lwm2m_data_encode_int'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object14serverRead_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x594): undefined reference to `lwm2m_data_encode_uint'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object14serverRead_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x600): undefined reference to `lwm2m_data_encode_float'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object14serverRead_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x673): undefined reference to `lwm2m_data_encode_objlink'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object14serverRead_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x6a4): undefined reference to `lwm2m_data_encode_opaque'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object14serverRead_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x6d5): undefined reference to `lwm2m_data_encode_string'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object14serverRead_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x6fb): undefined reference to `lwm2m_data_encode_corelink'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::Object::serverDiscover_clb(_lwm2m_context_*, _lwm2m_server_*, unsigned short, int*, _lwm2m_data_t**, _lwm2m_object_t*)':
<artificial>:(.text._ZN3wpp6Object18serverDiscover_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x21f): undefined reference to `lwm2m_data_new'
/usr/bin/ld: <artificial>:(.text._ZN3wpp6Object18serverDiscover_clbEP15_lwm2m_context_P14_lwm2m_server_tPiPP13_lwm2m_data_tP15_lwm2m_object_t+0x2fb): undefined reference to `lwm2m_data_encode_instances'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::Lwm2mServer::serverOperationNotifier(wpp::Instance*, wpp::ItemOp::TYPE, wpp::ResLink const&)':
<artificial>:(.text._ZN3wpp11Lwm2mServer23serverOperationNotifierEPNS_8InstanceENS_6ItemOp4TYPEERKNS_7ResLinkE+0x39): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::Lwm2mServer::userOperationNotifier(wpp::ItemOp::TYPE, wpp::ResLink const&)':
<artificial>:(.text._ZN3wpp11Lwm2mServer21userOperationNotifierENS_6ItemOp4TYPEERKNS_7ResLinkE+0x87): undefined reference to `lwm2m_update_server_lifetime'
/usr/bin/ld: <artificial>:(.text._ZN3wpp11Lwm2mServer21userOperationNotifierENS_6ItemOp4TYPEERKNS_7ResLinkE+0xca): undefined reference to `lwm2m_update_server_mute'
/usr/bin/ld: <artificial>:(.text._ZN3wpp11Lwm2mServer21userOperationNotifierENS_6ItemOp4TYPEERKNS_7ResLinkE+0x10f): undefined reference to `lwm2m_update_server_disable_timeout'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `std::_Function_handler<bool (wpp::Instance&, unsigned short, std::vector<unsigned char, std::allocator<unsigned char> > const&), wpp::Lwm2mServer::resourcesInit()::{lambda(wpp::Instance&, unsigned short, std::vector<unsigned char, std::allocator<unsigned char> > const&)#2}>::_M_invoke(std::_Any_data const&, wpp::Instance&, unsigned short&&, std::vector<unsigned char, std::allocator<unsigned char> > const&)':
<artificial>:(.text._ZNSt17_Function_handlerIFbRN3wpp8InstanceEtRKSt6vectorIhSaIhEEEZNS0_11Lwm2mServer13resourcesInitEvEUlS2_tS7_E0_E9_M_invokeERKSt9_Any_dataS2_OtS7_+0x56): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::ObjectImpl<wpp::Lwm2mAccessControl>::createInstance(unsigned short)':
<artificial>:(.text._ZN3wpp10ObjectImplINS_18Lwm2mAccessControlEE14createInstanceEt+0xec): undefined reference to `lwm2m_list_add'
/usr/bin/ld: <artificial>:(.text._ZN3wpp10ObjectImplINS_18Lwm2mAccessControlEE14createInstanceEt+0x12b): undefined reference to `lwm2m_ac_request_update_policy'
/usr/bin/ld: <artificial>:(.text._ZN3wpp10ObjectImplINS_18Lwm2mAccessControlEE14createInstanceEt+0x160): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::ObjectImpl<wpp::Location>::createInstance(unsigned short)':
<artificial>:(.text._ZN3wpp10ObjectImplINS_8LocationEE14createInstanceEt+0xec): undefined reference to `lwm2m_list_add'
/usr/bin/ld: <artificial>:(.text._ZN3wpp10ObjectImplINS_8LocationEE14createInstanceEt+0x22a): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::ObjectImpl<wpp::ConnectivityMonitoring>::createInstance(unsigned short)':
<artificial>:(.text._ZN3wpp10ObjectImplINS_22ConnectivityMonitoringEE14createInstanceEt+0xf8): undefined reference to `lwm2m_list_add'
/usr/bin/ld: <artificial>:(.text._ZN3wpp10ObjectImplINS_22ConnectivityMonitoringEE14createInstanceEt+0x244): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::ObjectImpl<wpp::Lwm2mSecurity>::createInstance(unsigned short)':
<artificial>:(.text._ZN3wpp10ObjectImplINS_13Lwm2mSecurityEE14createInstanceEt+0xec): undefined reference to `lwm2m_list_add'
/usr/bin/ld: <artificial>:(.text._ZN3wpp10ObjectImplINS_13Lwm2mSecurityEE14createInstanceEt+0x153): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::ObjectImpl<wpp::Lwm2mServer>::createInstance(unsigned short)':
<artificial>:(.text._ZN3wpp10ObjectImplINS_11Lwm2mServerEE14createInstanceEt+0xec): undefined reference to `lwm2m_list_add'
/usr/bin/ld: <artificial>:(.text._ZN3wpp10ObjectImplINS_11Lwm2mServerEE14createInstanceEt+0x159): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::ObjectImpl<wpp::Device>::createInstance(unsigned short)':
<artificial>:(.text._ZN3wpp10ObjectImplINS_6DeviceEE14createInstanceEt+0xec): undefined reference to `lwm2m_list_add'
/usr/bin/ld: <artificial>:(.text._ZN3wpp10ObjectImplINS_6DeviceEE14createInstanceEt+0x158): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::ObjectImpl<wpp::FirmwareUpdate>::createInstance(unsigned short)':
<artificial>:(.text._ZN3wpp10ObjectImplINS_14FirmwareUpdateEE14createInstanceEt+0xec): undefined reference to `lwm2m_list_add'
/usr/bin/ld: <artificial>:(.text._ZN3wpp10ObjectImplINS_14FirmwareUpdateEE14createInstanceEt+0x16c): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::ObjectImpl<wpp::AudioClip>::createInstance(unsigned short)':
<artificial>:(.text._ZN3wpp10ObjectImplINS_9AudioClipEE14createInstanceEt+0xf9): undefined reference to `lwm2m_list_add'
/usr/bin/ld: <artificial>:(.text._ZN3wpp10ObjectImplINS_9AudioClipEE14createInstanceEt+0x250): undefined reference to `lwm2m_update_registration'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `wpp::WppClient::~WppClient()':
<artificial>:(.text._ZN3wpp9WppClientD2Ev+0x106): undefined reference to `lwm2m_close'
/usr/bin/ld: /tmp/cctSfrkM.ltrans0.ltrans.o: in function `main':
<artificial>:(.text.startup.main+0xf48): undefined reference to `dtls_init'
/usr/bin/ld: <artificial>:(.text.startup.main+0xf55): undefined reference to `dtls_new_context'
/usr/bin/ld: <artificial>:(.text.startup.main+0x110b): undefined reference to `lwm2m_init'
/usr/bin/ld: <artificial>:(.text.startup.main+0x1415): undefined reference to `lwm2m_configure'
/usr/bin/ld: <artificial>:(.text.startup.main+0x22c1): undefined reference to `lwm2m_handle_packet'
/usr/bin/ld: <artificial>:(.text.startup.main+0x2301): undefined reference to `lwm2m_observe_step'
/usr/bin/ld: <artificial>:(.text.startup.main+0x2585): undefined reference to `lwm2m_step'
/usr/bin/ld: /tmp/cctSfrkM.ltrans1.ltrans.o: in function `std::_Function_handler<bool (wpp::WppClient&, void*), main::{lambda(wpp::WppClient&, void*)#1}>::_M_invoke(std::_Any_data const&, wpp::WppClient&, void*&&) [clone .lto_priv.0]':
<artificial>:(.text._ZNSt17_Function_handlerIFbRN3wpp9WppClientEPvEZ4mainEUlS2_S3_E_E9_M_invokeERKSt9_Any_dataS2_OS3_.lto_priv.0+0x85): undefined reference to `lwm2m_send_operation'
/usr/bin/ld: /tmp/cctSfrkM.ltrans1.ltrans.o: in function `Connection::sendPacket(wpp::WppConnection::Packet const&)':
<artificial>:(.text._ZN10Connection10sendPacketERKN3wpp13WppConnection6PacketE+0xbd): undefined reference to `dtls_get_peer'
/usr/bin/ld: <artificial>:(.text._ZN10Connection10sendPacketERKN3wpp13WppConnection6PacketE+0xd8): undefined reference to `dtls_reset_peer'
/usr/bin/ld: <artificial>:(.text._ZN10Connection10sendPacketERKN3wpp13WppConnection6PacketE+0xe5): undefined reference to `dtls_connect'
/usr/bin/ld: <artificial>:(.text._ZN10Connection10sendPacketERKN3wpp13WppConnection6PacketE+0x13e): undefined reference to `dtls_write'
/usr/bin/ld: /tmp/cctSfrkM.ltrans1.ltrans.o: in function `Connection::loop()':
<artificial>:(.text._ZN10Connection4loopEv+0x12d): undefined reference to `dtls_handle_message'
/usr/bin/ld: /tmp/cctSfrkM.ltrans1.ltrans.o: in function `wpp::WppRegistry::registerObj(wpp::Object&) [clone .isra.0]':
<artificial>:(.text._ZN3wpp11WppRegistry11registerObjERNS_6ObjectE.isra.0+0x42): undefined reference to `lwm2m_add_object'
/usr/bin/ld: /tmp/cctSfrkM.ltrans1.ltrans.o: in function `wpp::Instance::createEmptyLwm2mDataArray(std::vector<wpp::Resource*, std::allocator<wpp::Resource*> >, _lwm2m_data_t**, int*) [clone .isra.0]':
<artificial>:(.text._ZN3wpp8Instance25createEmptyLwm2mDataArrayESt6vectorIPNS_8ResourceESaIS3_EEPP13_lwm2m_data_tPi.isra.0+0x1d): undefined reference to `lwm2m_data_new'
collect2: error: ld returned 1 exit status
make[2]: *** [CMakeFiles/WppExample.dir/build.make:523: WppExample] Error 1
make[1]: *** [CMakeFiles/Makefile2:1186: CMakeFiles/WppExample.dir/all] Error 2
make: *** [Makefile:91: all] Error 2