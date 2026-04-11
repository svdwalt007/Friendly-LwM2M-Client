make -j$(nproc)
[  3%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/platform/openwrt_platform.cpp.o
[  7%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/platform/linux_platform.cpp.o
[ 10%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/firmware/delta_algorithms.cpp.o
[ 14%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/firmware/rollback_manager.cpp.o
[ 17%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/transport/blockwise_transfer.cpp.o
[ 21%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/transport/mqtt_transport.cpp.o
[ 25%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/objects/advanced_firmware_update_object.cpp.o
[ 28%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/objects/mqtt_server_object.cpp.o
[ 35%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/objects/lwm2m_cose_object.cpp.o
[ 35%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/objects/edge_ai_inference_object.cpp.o
[ 42%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/starlink/starlink_grpc_client.cpp.o
[ 42%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/starlink/starlink_data_collector.cpp.o
[ 46%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.cpp.o
[ 50%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10526_multiwan_health_check/MultiWanHealthCheck.cpp.o
[ 53%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.cpp.o
[ 57%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization.cpp.o
[ 60%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10535_lan_configuration/LanConfiguration.cpp.o
[ 64%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10536_routing_table/RoutingTable.cpp.o
[ 67%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10537_vpn_configuration/VpnConfiguration.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicyInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:247: CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagementInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10526_multiwan_health_check/MultiWanHealthCheck.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10526_multiwan_health_check/MultiWanHealthCheck.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10526_multiwan_health_check/MultiWanHealthCheckInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
compilation terminated.
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimizationInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:261: CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10526_multiwan_health_check/MultiWanHealthCheck.cpp.o] Error 1
compilation terminated.
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:275: CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.cpp.o] Error 1
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:289: CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10535_lan_configuration/LanConfiguration.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10535_lan_configuration/LanConfiguration.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10535_lan_configuration/LanConfigurationInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:303: CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10535_lan_configuration/LanConfiguration.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10536_routing_table/RoutingTable.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10536_routing_table/RoutingTable.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10536_routing_table/RoutingTableInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:317: CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10536_routing_table/RoutingTable.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10537_vpn_configuration/VpnConfiguration.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10537_vpn_configuration/VpnConfiguration.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10537_vpn_configuration/VpnConfigurationInfo.h:9:10: fatal error: Lwm2mObjectBase.h: No such file or directory
    9 | #include "Lwm2mObjectBase.h"
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:331: CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10537_vpn_configuration/VpnConfiguration.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp: In member function ‘virtual lwm2m::firmware::DeltaResult lwm2m::firmware::BSDiffAlgorithm::validateDelta(const std::vector<unsigned char>&, uint64_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:385:54: warning: unused parameter ‘expectedSourceSiz’ [-Wunused-parameter]
  385 |                                             uint64_t expectedSourceSize) {
      |                                             ~~~~~~~~~^~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp: In member function ‘virtual lwm2m::firmware::DeltaResult lwm2m::firmware::VCDiffAlgorithm::createDelta(const std::vector<unsigned char>&, const std::vector<unsigned char>&, std::vector<unsigned char>&, lwm2m::firmware::DeltaProgressCallback)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:484:14: warning: variable ‘fromSource’ set but not used [-Wunused-but-set-variable]
  484 |         bool fromSource = false;
      |              ^~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:479:12: warning: variable ‘lastCopyAddr’ set but not used [-Wunused-but-set-variable]
  479 |     size_t lastCopyAddr = 0;
      |            ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp: In member function ‘virtual lwm2m::firmware::DeltaResult lwm2m::firmware::VCDiffAlgorithm::applyDelta(const std::vector<unsigned char>&, const std::vector<unsigned char>&, std::vector<unsigned char>&, lwm2m::firmware::DeltaProgressCallback)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:598:13: warning: unused variable ‘hdrIndicator’ [-Wunused-variable]
  598 |     uint8_t hdrIndicator = *ptr++;
      |             ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:601:13: warning: unused variable ‘winIndicator’ [-Wunused-variable]
  601 |     uint8_t winIndicator = *ptr++;
      |             ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:602:14: warning: unused variable ‘sourceLen’ [-Wunused-variable]
  602 |     uint64_t sourceLen = Impl::readVarInt(ptr);
      |              ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:603:14: warning: unused variable ‘sourcePos’ [-Wunused-variable]
  603 |     uint64_t sourcePos = Impl::readVarInt(ptr);
      |              ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:606:14: warning: unused variable ‘deltaLen’ [-Wunused-variable]
  606 |     uint64_t deltaLen = Impl::readVarInt(ptr);
      |              ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:608:13: warning: unused variable ‘deltaIndicator’ [-Wunused-variable]
  608 |     uint8_t deltaIndicator = *ptr++;
      |             ^~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:611:14: warning: unused variable ‘addrLen’ [-Wunused-variable]
  611 |     uint64_t addrLen = Impl::readVarInt(ptr);
      |              ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp: In member function ‘virtual lwm2m::firmware::DeltaResult lwm2m::firmware::VCDiffAlgorithm::validateDelta(const std::vector<unsigned char>&, uint64_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:660:54: warning: unused parameter ‘expectedSourceSiz’ [-Wunused-parameter]
  660 |                                             uint64_t expectedSourceSize) {
      |                                             ~~~~~~~~~^~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp: In member function ‘virtual lwm2m::firmware::DeltaResult lwm2m::firmware::CourgetteAlgorithm::applyDelta(const std::vector<unsigned char>&, const std::vector<unsigned char>&, std::vector<unsigned char>&, lwm2m::firmware::DeltaProgressCallback)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:1011:22: warning: unused variable ‘format’ [-Wunused-variable]
 1011 |     ExecutableFormat format = static_cast<ExecutableFormat>(delta[8]);
      |                      ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp: In member function ‘virtual lwm2m::firmware::DeltaResult lwm2m::firmware::CourgetteAlgorithm::validateDelta(const std::vector<unsigned char>&, uint64_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/delta_algorithms.cpp:1074:57: warning: unused parameter ‘expectedSourceSize’ [-Wunused-parameter]
 1074 |                                                uint64_t expectedSourceSize) {
      |                                                ~~~~~~~~~^~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:36:10: fatal error: blkid/blkid.h: No such file or directory
   36 | #include <blkid/blkid.h>
      |          ^~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:93: CMakeFiles/friendly_lwm2m.dir/src/platform/linux_platform.cpp.o] Error 1
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:9:
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:554:26: error: ‘thread’ is not a member of std’
  554 |     std::unique_ptr<std::thread> downloadThread_;
      |                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:36:1: note: ‘std::thread’ is defined in header ‘<thread>’; did you forget to ‘#include <thread>’?
   35 | #include <chrono>
  +++ |+#include <thread>
   36 |
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:554:32: error: template argument 1 is invalid
  554 |     std::unique_ptr<std::thread> downloadThread_;
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:554:32: error: template argument 2 is invalid
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:555:26: error: ‘thread’ is not a member of std’
  555 |     std::unique_ptr<std::thread> inferenceThread_;
      |                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:555:26: note: ‘std::thread’ is defined in header ‘<thread>’; did you forget to ‘#include <thread>’?
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:555:32: error: template argument 1 is invalid
  555 |     std::unique_ptr<std::thread> inferenceThread_;
      |                                ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:555:32: error: template argument 2 is invalid
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:298:67: error: default member initializer for ‘lwm2m::objects::EdgeAIInferenceObject::Config::format’ required before the end of its enclosing class
  298 |                                     const Config& config = Config());
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:280:28: note: defined here
  280 |         ModelFormat format = ModelFormat::TFLITE;
      |                            ^~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:298:67: error: default member initializer for ‘lwm2m::objects::EdgeAIInferenceObject::Config::accelerator’ required before the end of its enclosing class
  298 |                                     const Config& config = Config());
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:281:37: note: defined here
  281 |         AcceleratorType accelerator = AcceleratorType::AUTO;
      |                                     ^~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:298:67: error: default member initializer for ‘lwm2m::objects::EdgeAIInferenceObject::Config::numThreads’ required before the end of its enclosing class
  298 |                                     const Config& config = Config());
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:282:24: note: defined here
  282 |         int numThreads = 4;
      |                        ^~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/src/transport/blockwise_transfer.cpp:9:
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::preferredBlockSize’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:298:67: error: default member initializer for ‘lwm2m::objects::EdgeAIInferenceObject::Config::inferenceTimeout’ required before the end of its enclosing class
  298 |                                     const Config& config = Config());
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:121:38: note: defined here
  121 |         BlockSize preferredBlockSize = BlockSize::BLOCK_1024;
      |                                      ^~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:283:35: note: defined here
  283 |         uint32_t inferenceTimeout = 5000;
      |                                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:298:67: error: default member initializer for ‘lwm2m::objects::EdgeAIInferenceObject::Config::batchSize’ required before the end of its enclosing class
  298 |                                     const Config& config = Config());
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:284:23: note: defined here
  284 |         int batchSize = 1;
      |                       ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::bertEnabled’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:298:67: error: default member initializer for ‘lwm2m::objects::EdgeAIInferenceObject::Config::enableProfiling’ required before the end of its enclosing class
  298 |                                     const Config& config = Config());
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:122:26: note: defined here
  122 |         bool bertEnabled = true;
      |                          ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:285:30: note: defined here
  285 |         bool enableProfiling = false;
      |                              ^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::bertBlockSize’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:298:67: error: default member initializer for ‘lwm2m::objects::EdgeAIInferenceObject::Config::gpuMemoryLimit’ required before the end of its enclosing class
  298 |                                     const Config& config = Config());
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:286:33: note: defined here
  286 |         uint64_t gpuMemoryLimit = 0;
      |                                 ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:123:32: note: defined here
  123 |         uint32_t bertBlockSize = 8192;      ///< Default BERT block size
      |                                ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:298:67: error: default member initializer for ‘lwm2m::objects::EdgeAIInferenceObject::Config::allowFP16’ required before the end of its enclosing class
  298 |                                     const Config& config = Config());
      |                                                                   ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/edge_ai_inference_object.h:287:24: note: defined here
  287 |         bool allowFP16 = true;
      |                        ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::maxRetransmits’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:124:33: note: defined here
  124 |         uint32_t maxRetransmits = 5;
      |                                 ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::transferTimeout’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:125:45: note: defined here
  125 |         std::chrono::seconds transferTimeout{600};
      |                                             ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::blockTimeout’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:126:42: note: defined here
  126 |         std::chrono::seconds blockTimeout{30};
      |                                          ^~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::autoRetry’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:127:24: note: defined here
  127 |         bool autoRetry = true;
      |                        ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::verifyChecksum’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:128:29: note: defined here
  128 |         bool verifyChecksum = true;
      |                             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/transport/mqtt_transport.cpp: In member function ‘lwm2m::transport::MqttResult lwm2m::transport::MqttTransport::disconnect(bool)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/transport/mqtt_transport.cpp:123:43: warning: unused parameter ‘sendWill’ [-Wunused-parameter]
  123 | MqttResult MqttTransport::disconnect(bool sendWill) {
      |                                      ~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/transport/mqtt_transport.cpp: In member function ‘lwm2m::transport::MqttResult lwm2m::transport::MqttTransport::process(uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/transport/mqtt_transport.cpp:151:44: warning: unused parameter ‘timeoutMs’ [-Wunused-parameter]
  151 | MqttResult MqttTransport::process(uint32_t timeoutMs) {
      |                                   ~~~~~~~~~^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/starlink/starlink_grpc_client.cpp: In destructor ‘starlink::StarlinkGrpcClient::~StarlinkGrpcClient()’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/starlink/starlink_grpc_client.cpp:64:16: error: invalid use of incomplete type ‘class grpc::CompletionQueue’
   64 |             cq_->Shutdown();
      |                ^~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/src/starlink/starlink_grpc_client.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/src/starlink/starlink_grpc_client.h:29:11: note: forward declaration of ‘class grpc::CompletionQueue’
   29 |     class CompletionQueue;
      |           ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/starlink/starlink_grpc_client.cpp: In member function ‘bool starlink::StarlinkGrpcClient::setWiFiConfig(const starlink::WiFiConfig&, std::string&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/starlink/starlink_grpc_client.cpp:645:58: warning: unused parameter ‘config’ [-Wunused-parameter]
  645 | bool StarlinkGrpcClient::setWiFiConfig(const WiFiConfig& config, std::string& error_msg) {
      |                                        ~~~~~~~~~~~~~~~~~~^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp:9:
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::snapshotStoragePath’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:119:41: note: defined here
  119 |         std::string snapshotStoragePath = "/var/lib/lwm2m/snapshots";
      |                                         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::maxSnapshots’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:120:29: note: defined here
  120 |         size_t maxSnapshots = 5;
      |                             ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::autoSnapshot’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:121:27: note: defined here
  121 |         bool autoSnapshot = true;          ///< Auto-create snapshot before updates
      |                           ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::autoRollbackOnFailure’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:122:36: note: defined here
  122 |         bool autoRollbackOnFailure = true; ///< Auto-rollback on boot failure
      |                                    ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::maxBootAttempts’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:123:29: note: defined here
  123 |         int maxBootAttempts = 3;           ///< Max boot attempts before rollback
      |                             ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::verifyAfterRollback’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:124:34: note: defined here
  124 |         bool verifyAfterRollback = true;   ///< Verify partition after rollback
      |                                  ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::bootVerificationTimeout’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:125:53: note: defined here
  125 |         std::chrono::seconds bootVerificationTimeout{120};  ///< Time to verify boot
      |                                                     ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp: In member function ‘bool lwm2m::objects::EdgeAIInferenceObject::downloadModel(const std::string&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:570:47: error: base operand of ‘->’ is not a pointer
  570 |         if (downloadThread_ && downloadThread_->joinable()) {
      |                                               ^~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:572:28: error: base operand of ‘->’ is not a pointer
  572 |             downloadThread_->join();
      |                            ^~
In file included from /usr/include/c++/13/memory:78,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/src/starlink/starlink_grpc_client.h:19:
/usr/include/c++/13/bits/unique_ptr.h: In instantiation of ‘void std::default_delete<_Tp>::operator()(_Tp*) const [with _Tp = grpc::CompletionQueue]’:
/usr/include/c++/13/bits/unique_ptr.h:404:17:   required from ‘std::unique_ptr<_Tp, _Dp>::~unique_ptr() [with _Tp = grpc::CompletionQueue; _Dp = std::default_delete<grpc::CompletionQueue>]’
/mnt/d/Dev/Friendly-LwM2M-Client/src/starlink/starlink_grpc_client.cpp:49:25:   required from here
/usr/include/c++/13/bits/unique_ptr.h:97:23: error: invalid application of ‘sizeof’ to incomplete type ‘grpc::CompletionQueue’
   97 |         static_assert(sizeof(_Tp)>0,
      |                       ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:577:56: error: cannot convert ‘std::__detail::__unique_ptr_t<std::thread>’ to ‘int’ in assignment
  577 |         downloadThread_ = std::make_unique<std::thread>([this, uri]() {
      |                           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~
      |                                                        |
      |                                                        std::__detail::__unique_ptr_t<std::thread>
  578 |             // Simulate download progress
      |             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  579 |             for (int i = 0; i <= 100 && !shouldStop_; i += 10) {
      |             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  580 |                 if (modelState_ != ModelState::DOWNLOADING) break;
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  581 |                 downloadProgress_ = i;
      |                 ~~~~~~~~~~~~~~~~~~~~~~
  582 |                 if (downloadProgressCallback_) {
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  583 |                     downloadProgressCallback_(i, i < 100 ? "Downloading..." : "Complete");
      |                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  584 |                 }
      |                 ~
  585 |                 std::this_thread::sleep_for(std::chrono::milliseconds(100));
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  586 |             }
      |             ~
  587 |
      |
  588 |             if (!shouldStop_ && modelState_ == ModelState::DOWNLOADING) {
      |             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  589 |                 // In real implementation, save downloaded model and load it
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  590 |                 setModelState(ModelState::VALIDATING);
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  591 |                 std::this_thread::sleep_for(std::chrono::milliseconds(50));
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  592 |                 setModelState(ModelState::IDLE);
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  593 |             }
      |             ~
  594 |         });
      |         ~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp: In member function ‘bool lwm2m::objects::EdgeAIInferenceObject::runInferenceAsync(const std::vector<unsigned char>&, lwm2m::objects::InferenceCallback)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:858:49: error: base operand of ‘->’ is not a pointer
  858 |         if (inferenceThread_ && inferenceThread_->joinable()) {
      |                                                 ^~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:859:29: error: base operand of ‘->’ is not a pointer
  859 |             inferenceThread_->join();
      |                             ^~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:863:57: error: cannot convert ‘std::__detail::__unique_ptr_t<std::thread>’ to ‘int’ in assignment
  863 |         inferenceThread_ = std::make_unique<std::thread>([this, inputData, callback]() {
      |                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                         |
      |                                                         std::__detail::__unique_ptr_t<std::thread>
  864 |             if (!shouldStop_) {
      |             ~~~~~~~~~~~~~~~~~~~
  865 |                 auto result = runInference(inputData);
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  866 |                 if (callback && !shouldStop_) {
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  867 |                     callback(result);
      |                     ~~~~~~~~~~~~~~~~~
  868 |                 }
      |                 ~
  869 |             }
      |             ~
  870 |         });
      |         ~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp: In member function ‘void lwm2m::objects::EdgeAIInferenceObject::stopAllThreads()’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:1251:47: error: base operand of ‘->’ is not a pointer
 1251 |         if (downloadThread_ && downloadThread_->joinable()) {
      |                                               ^~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:1252:28: error: base operand of ‘->’ is not a pointer
 1252 |             downloadThread_->join();
      |                            ^~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:1253:29: error: request for member ‘reset’ in ‘((lwm2m::objects::EdgeAIInferenceObject*)this)->lwm2m::objects::EdgeAIInferenceObject::downloadThread_’, which is of non-class type ‘int’
 1253 |             downloadThread_.reset();
      |                             ^~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:1257:49: error: base operand of ‘->’ is not a pointer
 1257 |         if (inferenceThread_ && inferenceThread_->joinable()) {
      |                                                 ^~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:1258:29: error: base operand of ‘->’ is not a pointer
 1258 |             inferenceThread_->join();
      |                             ^~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/edge_ai_inference_object.cpp:1259:30: error: request for member ‘reset’ in ‘((lwm2m::objects::EdgeAIInferenceObject*)this)->lwm2m::objects::EdgeAIInferenceObject::inferenceThread_’, which is of non-class type ‘int’
 1259 |             inferenceThread_.reset();
      |                              ^~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:20,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:9:
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::snapshotStoragePath’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:119:41: note: defined here
  119 |         std::string snapshotStoragePath = "/var/lib/lwm2m/snapshots";
      |                                         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::maxSnapshots’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:120:29: note: defined here
  120 |         size_t maxSnapshots = 5;
      |                             ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::autoSnapshot’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:121:27: note: defined here
  121 |         bool autoSnapshot = true;          ///< Auto-create snapshot before updates
      |                           ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::autoRollbackOnFailure’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:122:36: note: defined here
  122 |         bool autoRollbackOnFailure = true; ///< Auto-rollback on boot failure
      |                                    ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::maxBootAttempts’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:123:29: note: defined here
  123 |         int maxBootAttempts = 3;           ///< Max boot attempts before rollback
      |                             ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::verifyAfterRollback’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:124:34: note: defined here
  124 |         bool verifyAfterRollback = true;   ///< Verify partition after rollback
      |                                  ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:134:60: error: default member initializer for ‘lwm2m::firmware::RollbackManager::Config::bootVerificationTimeout’ required before the end of its enclosing class
  134 |     explicit RollbackManager(const Config& config = Config());
      |                                                            ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/firmware/rollback_manager.h:125:53: note: defined here
  125 |         std::chrono::seconds bootVerificationTimeout{120};  ///< Time to verify boot
      |                                                     ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:21:
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::preferredBlockSize’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:121:38: note: defined here
  121 |         BlockSize preferredBlockSize = BlockSize::BLOCK_1024;
      |                                      ^~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::bertEnabled’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:122:26: note: defined here
  122 |         bool bertEnabled = true;
      |                          ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::bertBlockSize’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:123:32: note: defined here
  123 |         uint32_t bertBlockSize = 8192;      ///< Default BERT block size
      |                                ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::maxRetransmits’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:124:33: note: defined here
  124 |         uint32_t maxRetransmits = 5;
      |                                 ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::transferTimeout’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:125:45: note: defined here
  125 |         std::chrono::seconds transferTimeout{600};
      |                                             ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::blockTimeout’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:126:42: note: defined here
  126 |         std::chrono::seconds blockTimeout{30};
      |                                          ^~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::autoRetry’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:127:24: note: defined here
  127 |         bool autoRetry = true;
      |                        ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:136:62: error: default member initializer for ‘lwm2m::transport::BlockwiseTransfer::Config::verifyChecksum’ required before the end of its enclosing class
  136 |     explicit BlockwiseTransfer(const Config& config = Config());
      |                                                              ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:128:29: note: defined here
  128 |         bool verifyChecksum = true;
      |                             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:204:74: error: default member initializer for ‘lwm2m::objects::AdvancedFirmwareUpdateObject::Config::downloadPath’ required before the end of its enclosing class
  204 |                                            const Config& config = Config());
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:184:34: note: defined here
  184 |         std::string downloadPath = "/tmp/firmware";
      |                                  ^~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:204:74: error: default member initializer for ‘lwm2m::objects::AdvancedFirmwareUpdateObject::Config::backupPath’ required before the end of its enclosing class
  204 |                                            const Config& config = Config());
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:185:32: note: defined here
  185 |         std::string backupPath = "/var/lib/lwm2m/backup";
      |                                ^~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:204:74: error: default member initializer for ‘lwm2m::objects::AdvancedFirmwareUpdateObject::Config::maxPackageSize’ required before the end of its enclosing class
  204 |                                            const Config& config = Config());
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:186:31: note: defined here
  186 |         size_t maxPackageSize = 256 * 1024 * 1024;  // 256MB
      |                               ^~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:204:74: error: default member initializer for ‘lwm2m::objects::AdvancedFirmwareUpdateObject::Config::defaultBlockSize’ required before the end of its enclosing class
  204 |                                            const Config& config = Config());
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:187:35: note: defined here
  187 |         uint32_t defaultBlockSize = 1024;
      |                                   ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:204:74: error: default member initializer for ‘lwm2m::objects::AdvancedFirmwareUpdateObject::Config::autoReboot’ required before the end of its enclosing class
  204 |                                            const Config& config = Config());
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:188:25: note: defined here
  188 |         bool autoReboot = true;
      |                         ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:204:74: error: default member initializer for ‘lwm2m::objects::AdvancedFirmwareUpdateObject::Config::rebootDelay’ required before the end of its enclosing class
  204 |                                            const Config& config = Config());
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:189:25: note: defined here
  189 |         int rebootDelay = 5;
      |                         ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:204:74: error: default member initializer for ‘lwm2m::objects::AdvancedFirmwareUpdateObject::Config::deltaEnabled’ required before the end of its enclosing class
  204 |                                            const Config& config = Config());
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:190:27: note: defined here
  190 |         bool deltaEnabled = true;
      |                           ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:204:74: error: default member initializer for ‘lwm2m::objects::AdvancedFirmwareUpdateObject::Config::defaultDeltaAlgorithm’ required before the end of its enclosing class
  204 |                                            const Config& config = Config());
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:191:56: note: defined here
  191 |         firmware::DeltaAlgorithm defaultDeltaAlgorithm = firmware::DeltaAlgorithm::BSDIFF;
      |                                                        ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:204:74: error: default member initializer for ‘lwm2m::objects::AdvancedFirmwareUpdateObject::Config::rollbackEnabled’ required before the end of its enclosing class
  204 |                                            const Config& config = Config());
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:192:30: note: defined here
  192 |         bool rollbackEnabled = true;
      |                              ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:204:74: error: default member initializer for ‘lwm2m::objects::AdvancedFirmwareUpdateObject::Config::maxRollbackSnapshots’ required before the end of its enclosing class
  204 |                                            const Config& config = Config());
      |                                                                          ^
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:193:34: note: defined here
  193 |         int maxRollbackSnapshots = 3;
      |                                  ^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp: In member function ‘std::vector<unsigned char> lwm2m::firmware::RollbackManager::calculateStateChecksum()’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp:818:16: warning: ‘int SHA256_Init(SHA256_CTX*)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  818 |     SHA256_Init(&ctx);
      |     ~~~~~~~~~~~^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp:19:
/usr/include/openssl/sha.h:73:27: note: declared here
   73 | OSSL_DEPRECATEDIN_3_0 int SHA256_Init(SHA256_CTX *c);
      |                           ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp:822:22: warning: ‘int SHA256_Update(SHA256_CTX*, const void*, size_t)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  822 |         SHA256_Update(&ctx, comp.checksum.data(), comp.checksum.size());
      |         ~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/include/openssl/sha.h:74:27: note: declared here
   74 | OSSL_DEPRECATEDIN_3_0 int SHA256_Update(SHA256_CTX *c,
      |                           ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp:823:22: warning: ‘int SHA256_Update(SHA256_CTX*, const void*, size_t)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  823 |         SHA256_Update(&ctx, comp.version.data(), comp.version.size());
      |         ~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/include/openssl/sha.h:74:27: note: declared here
   74 | OSSL_DEPRECATEDIN_3_0 int SHA256_Update(SHA256_CTX *c,
      |                           ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp:827:17: warning: ‘int SHA256_Final(unsigned char*, SHA256_CTX*)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  827 |     SHA256_Final(hash.data(), &ctx);
      |     ~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~
/usr/include/openssl/sha.h:76:27: note: declared here
   76 | OSSL_DEPRECATEDIN_3_0 int SHA256_Final(unsigned char *md, SHA256_CTX *c);
      |                           ^~~~~~~~~~~~
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:219: CMakeFiles/friendly_lwm2m.dir/src/starlink/starlink_grpc_client.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/openwrt_platform.cpp: In function ‘std::vector<unsigned char> lwm2m::platform::openwrt::calculateFileSHA256(const std::string&, uint64_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/openwrt_platform.cpp:163:16: warning: ‘int SHA256_Init(SHA256_CTX*)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  163 |     SHA256_Init(&ctx);
      |     ~~~~~~~~~~~^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/src/platform/openwrt_platform.cpp:37:
/usr/include/openssl/sha.h:73:27: note: declared here
   73 | OSSL_DEPRECATEDIN_3_0 int SHA256_Init(SHA256_CTX *c);
      |                           ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/openwrt_platform.cpp:182:22: warning: ‘int SHA256_Update(SHA256_CTX*, const void*, size_t)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  182 |         SHA256_Update(&ctx, buffer.data(), bytesRead);
      |         ~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/include/openssl/sha.h:74:27: note: declared here
   74 | OSSL_DEPRECATEDIN_3_0 int SHA256_Update(SHA256_CTX *c,
      |                           ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/openwrt_platform.cpp:187:17: warning: ‘int SHA256_Final(unsigned char*, SHA256_CTX*)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  187 |     SHA256_Final(hash.data(), &ctx);
      |     ~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~
/usr/include/openssl/sha.h:76:27: note: declared here
   76 | OSSL_DEPRECATEDIN_3_0 int SHA256_Final(unsigned char *md, SHA256_CTX *c);
      |                           ^~~~~~~~~~~~
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:135: CMakeFiles/friendly_lwm2m.dir/src/transport/blockwise_transfer.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp: In member function ‘bool lwm2m::objects::AdvancedFirmwareUpdateObject::initialize()’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:112:51: error: ‘BlockSize’ in ‘class lwm2m::transport::BlockwiseTransfer’ does not name a type
  112 |         static_cast<transport::BlockwiseTransfer::BlockSize>(blockSize_));
      |                                                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp: In member function ‘bool lwm2m::objects::AdvancedFirmwareUpdateObject::execute(lwm2m::objects::FirmwareResourceId, const std::string&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:360:64: warning: unused parameter ‘arguments’ [-Wunused-parameter]
  360 |                                             const std::string& arguments) {
      |                                             ~~~~~~~~~~~~~~~~~~~^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp: In member function ‘void lwm2m::objects::AdvancedFirmwareUpdateObject::downloadThread(const std::string&, bool)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:407:58: warning: unused parameter ‘isDelta’ [-Wunused-parameter]
  407 |                                                     bool isDelta) {
      |                                                     ~~~~~^~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:21:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp: In lambda function:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:467:23: warning: unused parameter ‘ultotal’ [-Wunused-parameter]
  467 |            curl_off_t ultotal, curl_off_t ulnow) -> int {
      |            ~~~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:467:43: warning: unused parameter ‘ulnow’ [-Wunused-parameter]
  467 |            curl_off_t ultotal, curl_off_t ulnow) -> int {
      |                                ~~~~~~~~~~~^~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp: In member function ‘bool lwm2m::objects::AdvancedFirmwareUpdateObject::receivePackageData(const std::vector<unsigned char>&, uint64_t, bool)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:501:61: warning: unused parameter ‘isDelta’ [-Wunused-parameter]
  501 |     const std::vector<uint8_t>& data, uint64_t offset, bool isDelta) {
      |                                                        ~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp: In member function ‘bool lwm2m::objects::AdvancedFirmwareUpdateObject::applyDeltaUpdate(const std::vector<unsigned char>&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:791:9: error: no match for ‘operator!’ (operand type is ‘lwm2m::platform::PlatformResult’)
  791 |     if (!partitionManager->setSlotBootable(inactiveSlot, true)) {
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:791:9: note: candidate: ‘operator!(bool)’ (built-in)
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:791:9: note:   no known conversion for argument 1 from ‘lwm2m::platform::PlatformResult’ to ‘bool’
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:793:33: error: ‘UPDATE_FAILED’ is not a member of ‘lwm2m::objects::FirmwareState’
  793 |         setState(FirmwareState::UPDATE_FAILED);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:797:9: error: no match for ‘operator!’ (operand type is ‘lwm2m::platform::PlatformResult’)
  797 |     if (!partitionManager->switchSlot(inactiveSlot)) {
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:797:9: note: candidate: ‘operator!(bool)’ (built-in)
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:797:9: note:   no known conversion for argument 1 from ‘lwm2m::platform::PlatformResult’ to ‘bool’
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:799:33: error: ‘UPDATE_FAILED’ is not a member of ‘lwm2m::objects::FirmwareState’
  799 |         setState(FirmwareState::UPDATE_FAILED);
      |                                 ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp: In member function ‘void lwm2m::objects::AdvancedFirmwareUpdateObject::setBlockSize(uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:994:55: error: ‘BlockSize’ in ‘class lwm2m::transport::BlockwiseTransfer’ does not name a type
  994 |             static_cast<transport::BlockwiseTransfer::BlockSize>(size));
      |                                                       ^~~~~~~~~
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:205: CMakeFiles/friendly_lwm2m.dir/src/objects/edge_ai_inference_object.cpp.o] Error 1
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:121: CMakeFiles/friendly_lwm2m.dir/src/firmware/rollback_manager.cpp.o] Error 1
make[2]: *** [CMakeFiles/friendly_lwm2m.dir/build.make:163: CMakeFiles/friendly_lwm2m.dir/src/objects/advanced_firmware_update_object.cpp.o] Error 1
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/openwrt_platform.cpp: At global scope:
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/openwrt_platform.cpp:151:29: warning: ‘std::vector<unsigned char> lwm2m::platform::openwrt::calculateSHA256(const std::vector<unsigned char>&)’ defined but not used [-Wunused-function]
  151 | static std::vector<uint8_t> calculateSHA256(const std::vector<uint8_t>& data) {
      |                             ^~~~~~~~~~~~~~~
make[1]: *** [CMakeFiles/Makefile2:101: CMakeFiles/friendly_lwm2m.dir/all] Error 2
make: *** [Makefile:166: all] Error 2