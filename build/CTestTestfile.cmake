# CMake generated Testfile for 
# Source directory: /home/sean/work/Friendly-LwM2M-Client
# Build directory: /home/sean/work/Friendly-LwM2M-Client/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(DeltaAlgorithms "/home/sean/work/Friendly-LwM2M-Client/build/test_delta_algorithms")
set_tests_properties(DeltaAlgorithms PROPERTIES  _BACKTRACE_TRIPLES "/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;727;add_test;/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;0;")
add_test(BlockwiseTransfer "/home/sean/work/Friendly-LwM2M-Client/build/test_blockwise_transfer")
set_tests_properties(BlockwiseTransfer PROPERTIES  _BACKTRACE_TRIPLES "/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;737;add_test;/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;0;")
add_test(RollbackManager "/home/sean/work/Friendly-LwM2M-Client/build/test_rollback_manager")
set_tests_properties(RollbackManager PROPERTIES  _BACKTRACE_TRIPLES "/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;747;add_test;/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;0;")
add_test(Lwm2mCoseObject "/home/sean/work/Friendly-LwM2M-Client/build/test_lwm2m_cose_object")
set_tests_properties(Lwm2mCoseObject PROPERTIES  _BACKTRACE_TRIPLES "/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;760;add_test;/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;0;")
add_test(MqttServerObject "/home/sean/work/Friendly-LwM2M-Client/build/test_mqtt_server_object")
set_tests_properties(MqttServerObject PROPERTIES  _BACKTRACE_TRIPLES "/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;773;add_test;/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;0;")
add_test(LogRing "/home/sean/work/Friendly-LwM2M-Client/build/test_log_ring")
set_tests_properties(LogRing PROPERTIES  _BACKTRACE_TRIPLES "/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;787;add_test;/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;0;")
add_test(FirmwareUpdateObject "/home/sean/work/Friendly-LwM2M-Client/build/test_firmware_update_object")
set_tests_properties(FirmwareUpdateObject PROPERTIES  _BACKTRACE_TRIPLES "/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;822;add_test;/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;0;")
add_test(DTLSProviders "/home/sean/work/Friendly-LwM2M-Client/build/test_dtls_providers")
set_tests_properties(DTLSProviders PROPERTIES  _BACKTRACE_TRIPLES "/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;962;add_test;/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;0;")
add_test(DTLSIntegration "/home/sean/work/Friendly-LwM2M-Client/build/test_dtls_integration")
set_tests_properties(DTLSIntegration PROPERTIES  _BACKTRACE_TRIPLES "/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;1025;add_test;/home/sean/work/Friendly-LwM2M-Client/CMakeLists.txt;0;")
subdirs("examples")
