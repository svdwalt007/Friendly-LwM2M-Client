make -j$(nproc)
[  3%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.cpp.o
[  7%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10526_multiwan_health_check/MultiWanHealthCheck.cpp.o
[ 10%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.cpp.o
[ 14%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization.cpp.o
[ 17%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10535_lan_configuration/LanConfiguration.cpp.o
[ 21%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10536_routing_table/RoutingTable.cpp.o
[ 25%] Building CXX object CMakeFiles/friendly_lwm2m.dir/wpp/registry/objects/w_10537_vpn_configuration/VpnConfiguration.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10536_routing_table/RoutingTable.h:14,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10536_routing_table/RoutingTable.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.h:13:10: fatal error: liblwm2m.h: No such file or directory
   13 | #include "liblwm2m.h"
      |          ^~~~~~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.h:13,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10527_wifi_client_management/WifiClientManagement.cpp:6:
/mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/data_model/instance/Instance.h:13:10: fatal error: liblwm2m.h: No such file or directory
   13 | #include "liblwm2m.h"make -j$(nproc)
[  4%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/platform/openwrt_platform.cpp.o
[ 14%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/platform/linux_platform.cpp.o
[ 14%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/firmware/delta_algorithms.cpp.o
[ 19%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/firmware/rollback_manager.cpp.o
[ 23%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/transport/blockwise_transfer.cpp.o
[ 28%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/transport/mqtt_transport.cpp.o
[ 38%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/objects/advanced_firmware_update_object.cpp.o
[ 38%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/objects/mqtt_server_object.cpp.o
[ 57%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/objects/edge_ai_inference_object.cpp.o
[ 57%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/objects/lwm2m_cose_object.cpp.o
[ 57%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/starlink/starlink_grpc_client.cpp.o
[ 57%] Building CXX object CMakeFiles/friendly_lwm2m.dir/src/starlink/starlink_data_collector.cpp.o
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
/mnt/d/Dev/Friendly-LwM2M-Client/src/transport/mqtt_transport.cpp: In member function ‘lwm2m::transport::MqttResult lwm2m::transport::MqttTransport::disconnect(bool)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/transport/mqtt_transport.cpp:123:43: warning: unused parameter ‘sendWill’ [-Wunused-parameter]
  123 | MqttResult MqttTransport::disconnect(bool sendWill) {
      |                                      ~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/transport/mqtt_transport.cpp: In member function ‘lwm2m::transport::MqttResult lwm2m::transport::MqttTransport::process(uint32_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/transport/mqtt_transport.cpp:151:44: warning: unused parameter ‘timeoutMs’ [-Wunused-parameter]
  151 | MqttResult MqttTransport::process(uint32_t timeoutMs) {
      |                                   ~~~~~~~~~^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp: In member function ‘std::vector<unsigned char> lwm2m::firmware::RollbackManager::calculateStateChecksum()’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp:822:16: warning: ‘int SHA256_Init(SHA256_CTX*)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  822 |     SHA256_Init(&ctx);
      |     ~~~~~~~~~~~^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp:19:
/usr/include/openssl/sha.h:73:27: note: declared here
   73 | OSSL_DEPRECATEDIN_3_0 int SHA256_Init(SHA256_CTX *c);
      |                           ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp:826:22: warning: ‘int SHA256_Update(SHA256_CTX*, const void*, size_t)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  826 |         SHA256_Update(&ctx, comp.checksum.data(), comp.checksum.size());
      |         ~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/include/openssl/sha.h:74:27: note: declared here
   74 | OSSL_DEPRECATEDIN_3_0 int SHA256_Update(SHA256_CTX *c,
      |                           ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp:827:22: warning: ‘int SHA256_Update(SHA256_CTX*, const void*, size_t)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  827 |         SHA256_Update(&ctx, comp.version.data(), comp.version.size());
      |         ~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/include/openssl/sha.h:74:27: note: declared here
   74 | OSSL_DEPRECATEDIN_3_0 int SHA256_Update(SHA256_CTX *c,
      |                           ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/firmware/rollback_manager.cpp:831:17: warning: ‘int SHA256_Final(unsigned char*, SHA256_CTX*)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  831 |     SHA256_Final(hash.data(), &ctx);
      |     ~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~
/usr/include/openssl/sha.h:76:27: note: declared here
   76 | OSSL_DEPRECATEDIN_3_0 int SHA256_Final(unsigned char *md, SHA256_CTX *c);
      |                           ^~~~~~~~~~~~
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
/mnt/d/Dev/Friendly-LwM2M-Client/src/starlink/starlink_grpc_client.cpp: In member function ‘bool starlink::StarlinkGrpcClient::setWiFiConfig(const starlink::WiFiConfig&, std::string&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/starlink/starlink_grpc_client.cpp:655:58: warning: unused parameter ‘config’ [-Wunused-parameter]
  655 | bool StarlinkGrpcClient::setWiFiConfig(const WiFiConfig& config, std::string& error_msg) {
      |                                        ~~~~~~~~~~~~~~~~~~^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp: In function ‘std::vector<unsigned char> lwm2m::platform::linux_generic::calculateFileSHA256(const std::string&, uint64_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:108:16: warning: ‘int SHA256_Init(SHA256_CTX*)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  108 |     SHA256_Init(&ctx);
      |     ~~~~~~~~~~~^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:39:
/usr/include/openssl/sha.h:73:27: note: declared here
   73 | OSSL_DEPRECATEDIN_3_0 int SHA256_Init(SHA256_CTX *c);
      |                           ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:128:26: warning: ‘int SHA256_Update(SHA256_CTX*, const void*, size_t)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  128 |             SHA256_Update(&ctx, buffer.data(), bytesRead);
      |             ~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/include/openssl/sha.h:74:27: note: declared here
   74 | OSSL_DEPRECATEDIN_3_0 int SHA256_Update(SHA256_CTX *c,
      |                           ^~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:133:17: warning: ‘int SHA256_Final(unsigned char*, SHA256_CTX*)’ is deprecated: Since OpenSSL 3.0 [-Wdeprecated-declarations]
  133 |     SHA256_Final(hash.data(), &ctx);
      |     ~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~
/usr/include/openssl/sha.h:76:27: note: declared here
   76 | OSSL_DEPRECATEDIN_3_0 int SHA256_Final(unsigned char *md, SHA256_CTX *c);
      |                           ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp: In member function ‘virtual lwm2m::platform::PlatformResult lwm2m::platform::linux_generic::LinuxFlashManager::lock(const std::string&, uint64_t, uint64_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:498:44: warning: unused parameter ‘device’ [-Wunused-parameter]
  498 |     PlatformResult lock(const std::string& device, uint64_t offset, uint64_t size) override {
      |                         ~~~~~~~~~~~~~~~~~~~^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:498:61: warning: unused parameter ‘offset’ [-Wunused-parameter]
  498 |     PlatformResult lock(const std::string& device, uint64_t offset, uint64_t size) override {
      |                                                    ~~~~~~~~~^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:498:78: warning: unused parameter ‘size’ [-Wunused-parameter]
  498 |     PlatformResult lock(const std::string& device, uint64_t offset, uint64_t size) override {
      |                                                                     ~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp: In member function ‘virtual lwm2m::platform::PlatformResult lwm2m::platform::linux_generic::LinuxFlashManager::unlock(const std::string&, uint64_t, uint64_t)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:503:46: warning: unused parameter ‘device’ [-Wunused-parameter]
  503 |     PlatformResult unlock(const std::string& device, uint64_t offset, uint64_t size) override {
      |                           ~~~~~~~~~~~~~~~~~~~^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:503:63: warning: unused parameter ‘offset’ [-Wunused-parameter]
  503 |     PlatformResult unlock(const std::string& device, uint64_t offset, uint64_t size) override {
      |                                                      ~~~~~~~~~^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:503:80: warning: unused parameter ‘size’ [-Wunused-parameter]
  503 |     PlatformResult unlock(const std::string& device, uint64_t offset, uint64_t size) override {
      |                                                                       ~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp: In member function ‘virtual lwm2m::platform::PlatformResult lwm2m::platform::linux_generic::LinuxBootloaderControl::setBootAttempts(lwm2m::platform::PartitionSlot, int)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:609:50: warning: unused parameter ‘slot’ [-Wunused-parameter]
  609 |     PlatformResult setBootAttempts(PartitionSlot slot, int attempts) override {
      |                                    ~~~~~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp: In member function ‘virtual int lwm2m::platform::linux_generic::LinuxBootloaderControl::getBootAttempts(lwm2m::platform::PartitionSlot)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:613:39: warning: unused parameter ‘slot’ [-Wunused-parameter]
  613 |     int getBootAttempts(PartitionSlot slot) override {
      |                         ~~~~~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/openwrt_platform.cpp: At global scope:
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/openwrt_platform.cpp:151:29: warning: ‘std::vector<unsigned char> lwm2m::platform::openwrt::calculateSHA256(const std::vector<unsigned char>&)’ defined but not used [-Wunused-function]
  151 | static std::vector<uint8_t> calculateSHA256(const std::vector<uint8_t>& data) {
      |                             ^~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp: At global scope:
/mnt/d/Dev/Friendly-LwM2M-Client/src/platform/linux_platform.cpp:96:13: warning: ‘bool lwm2m::platform::linux_generic::writeFile(const std::string&, const std::string&)’ defined but not used [-Wunused-function]
   96 | static bool writeFile(const std::string& path, const std::string& content) {
      |             ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp: In member function ‘bool lwm2m::objects::AdvancedFirmwareUpdateObject::execute(lwm2m::objects::FirmwareResourceId, const std::string&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:364:64: warning: unused parameter ‘arguments’ [-Wunused-parameter]
  364 |                                             const std::string& arguments) {
      |                                             ~~~~~~~~~~~~~~~~~~~^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp: In member function ‘void lwm2m::objects::AdvancedFirmwareUpdateObject::downloadThread(const std::string&, bool)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:411:58: warning: unused parameter ‘isDelta’ [-Wunused-parameter]
  411 |                                                     bool isDelta) {
      |                                                     ~~~~~^~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:21:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp: In lambda function:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:471:23: warning: unused parameter ‘ultotal’ [-Wunused-parameter]
  471 |            curl_off_t ultotal, curl_off_t ulnow) -> int {
      |            ~~~~~~~~~~~^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:471:43: warning: unused parameter ‘ulnow’ [-Wunused-parameter]
  471 |            curl_off_t ultotal, curl_off_t ulnow) -> int {
      |                                ~~~~~~~~~~~^~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp: In member function ‘bool lwm2m::objects::AdvancedFirmwareUpdateObject::receivePackageData(const std::vector<unsigned char>&, uint64_t, bool)’:
/mnt/d/Dev/Friendly-LwM2M-Client/src/objects/advanced_firmware_update_object.cpp:505:61: warning: unused parameter ‘isDelta’ [-Wunused-parameter]
  505 |     const std::vector<uint8_t>& data, uint64_t offset, bool isDelta) {
      |                                                        ~~~~~^~~~~~~
[ 61%] Linking CXX static library libfriendly_lwm2m.a
[ 61%] Built target friendly_lwm2m
[ 76%] Building CXX object CMakeFiles/test_delta_algorithms.dir/test/test_delta_algorithms.cpp.o
[ 76%] Building CXX object CMakeFiles/test_blockwise_transfer.dir/test/test_blockwise_transfer.cpp.o
[ 76%] Building CXX object CMakeFiles/test_rollback_manager.dir/test/test_rollback_manager.cpp.o
[ 80%] Building CXX object CMakeFiles/test_firmware_update_object.dir/test/test_firmware_update_object.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_rollback_manager.cpp: In member function ‘virtual lwm2m::platform::PlatformResult lwm2m::firmware::test::MockPartitionManager::verifyPartition(lwm2m::platform::PartitionSlot, const std::vector<unsigned char>&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_rollback_manager.cpp:98:70: warning: unused parameter ‘slot’ [-Wunused-parameter]
   98 |     platform::PlatformResult verifyPartition(platform::PartitionSlot slot,
      |                                              ~~~~~~~~~~~~~~~~~~~~~~~~^~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_rollback_manager.cpp:99:75: warning: unused parameter ‘expectedChecksum’ [-Wunused-parameter]
   99 |                                               const std::vector<uint8_t>& expectedChecksum) override {
      |                                               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~
In file included from /usr/include/gtest/gtest.h:72,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:9:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_DefaultConfiguration_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:53:70: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   53 |     EXPECT_EQ(transfer_->getPreferredBlockSize(), BlockwiseTransfer::BlockSize::BLOCK_1024);
      |                                                                      ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_SetBlockSize_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:58:57: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   58 |     transfer_->setPreferredBlockSize(BlockwiseTransfer::BlockSize::BLOCK_512);
      |                                                         ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:59:70: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   59 |     EXPECT_EQ(transfer_->getPreferredBlockSize(), BlockwiseTransfer::BlockSize::BLOCK_512);
      |                                                                      ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:61:57: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   61 |     transfer_->setPreferredBlockSize(BlockwiseTransfer::BlockSize::BLOCK_256);
      |                                                         ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:62:70: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   62 |     EXPECT_EQ(transfer_->getPreferredBlockSize(), BlockwiseTransfer::BlockSize::BLOCK_256);
      |                                                                      ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_BlockSizeValues_Test:TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:88:71: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   88 |     EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_16), 16u);
      |                                                                       ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:89:71: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   89 |     EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_32), 32u);
      |                                                                       ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:90:71: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   90 |     EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_64), 64u);
      |                                                                       ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:91:71: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   91 |     EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_128), 128u);
      |                                                                       ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:92:71: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   92 |     EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_256), 256u);
      |                                                                       ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:93:71: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   93 |     EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_512), 512u);
      |                                                                       ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:94:71: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   94 |     EXPECT_EQ(BlockwiseTransfer::getBlockSizeBytes(BlockwiseTransfer::BlockSize::BLOCK_1024), 1024u);
      |                                                                       ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_SZXValues_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:98:51: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   98 |     EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_16), 0);
      |                                                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:99:51: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
   99 |     EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_32), 1);
      |                                                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:100:51: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
  100 |     EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_64), 2);
      |                                                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:101:51: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
  101 |     EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_128), 3);
      |                                                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:102:51: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
  102 |     EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_256), 4);
      |                                                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:103:51: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
  103 |     EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_512), 5);
      |                                                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:104:51: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
  104 |     EXPECT_EQ(static_cast<int>(BlockwiseTransfer::BlockSize::BLOCK_1024), 6);
      |                                                   ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_StartTransfer_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:113:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  113 |     EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
      |                                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_StartTransferEmptyData_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:119:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  119 |     EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_INVALID_DATA);
      |                                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_StartTransferEmptyResource_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:124:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  124 |     EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_INVALID_RESOURCE);
      |                                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_ETagGeneration_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:134:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  134 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:137:28: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getETag’
  137 |     auto etag = transfer_->getETag(transferId);
      |                            ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_UniqueETagsForDifferentData_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:147:27: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  147 |     auto id1 = transfer_->getActiveTransferId();
      |                           ^~~~~~~~~~~~~~~~~~~
      |                           getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:148:29: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getETag’
  148 |     auto etag1 = transfer_->getETag(id1);
      |                             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:153:27: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  153 |     auto id2 = transfer_->getActiveTransferId();
      |                           ^~~~~~~~~~~~~~~~~~~
      |                           getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:154:29: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getETag’
  154 |     auto etag2 = transfer_->getETag(id2);
      |                             ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_ReceiveBlocks_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:165:57: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
  165 |     transfer_->setPreferredBlockSize(BlockwiseTransfer::BlockSize::BLOCK_1024);
      |                                                         ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:170:30: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘beginReceive’
  170 |     auto result = transfer_->beginReceive(resourcePath, totalSize);
      |                              ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:171:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  171 |     EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
      |                                          ^~~~~~
In file included from /usr/include/gtest/gtest.h:72,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp:9:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp: In member function ‘virtual void lwm2m::objects::test::FirmwareUpdateObjectTest_InitialState_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp:65:29: error: ‘class lwm2m::objects::AdvancedFirmwareUpdateObject’ has no member named ‘getResult’; did you mean ‘setResult’?
   65 |     EXPECT_EQ(firmwareObj_->getResult(), UpdateResult::DEFAULT);
      |                             ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:173:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  173 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:181:46: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  181 |         EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
      |                                              ^~~~~~
In file included from /usr/include/gtest/gtest-printers.h:122,
                 from /usr/include/gtest/gtest-matchers.h:49,
                 from /usr/include/gtest/internal/gtest-death-test-internal.h:47,
                 from /usr/include/gtest/gtest-death-test.h:43,
                 from /usr/include/gtest/gtest.h:65:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:185:28: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘isTransferComplete’
  185 |     EXPECT_TRUE(transfer_->isTransferComplete(transferId));
      |                            ^~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_OutOfOrderBlockRejected_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:189:57: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
  189 |     transfer_->setPreferredBlockSize(BlockwiseTransfer::BlockSize::BLOCK_1024);
      |                                                         ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:191:16: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘beginReceive’
  191 |     transfer_->beginReceive("/resource", 2048);
      |                ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:192:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  192 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:201:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  201 |     EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_BLOCK_OUT_OF_ORDER);
      |                                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_ResumeTransfer_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:210:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  210 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:211:28: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getETag’
  211 |     auto etag = transfer_->getETag(transferId);
      |                            ^~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:218:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  218 |     EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
      |                                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_ResumeWithWrongETag_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:223:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  223 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:229:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  229 |     EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_ETAG_MISMATCH);
      |                                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_TransferStatistics_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:237:16: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘beginReceive’
  237 |     transfer_->beginReceive("/resource", 4096);
      |                ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:238:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  238 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_ThroughputCalculation_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:254:16: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘beginReceive’
  254 |     transfer_->beginReceive("/resource", 10240);
      |                ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:255:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  255 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_BERTBlockSize_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:279:26: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getBERTBlockSize’; did you mean ‘setBERTBlockSize’?
  279 |     EXPECT_EQ(transfer_->getBERTBlockSize(), 8192u);
      |                          ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_BERTForLargeTransfer_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:290:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  290 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_BERTNotUsedForSmallTransfer_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:302:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  302 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_ChecksumCalculation_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:313:16: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘beginReceive’
  313 |     transfer_->beginReceive("/resource", 2048);
      |                ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:314:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  314 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:322:32: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getChecksum’
  322 |     auto checksum = transfer_->getChecksum(transferId);
      |                                ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_VerifyChecksumSuccess_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:328:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  328 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:331:40: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getChecksum’
  331 |     auto expectedChecksum = transfer_->getChecksum(transferId);
      |                                        ^~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:334:30: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘verifyChecksum’
  334 |     auto result = transfer_->verifyChecksum(transferId, expectedChecksum);
      |                              ^~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:335:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  335 |     EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
      |                                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_VerifyChecksumFailure_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:340:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  340 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:345:30: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘verifyChecksum’
  345 |     auto result = transfer_->verifyChecksum(transferId, wrongChecksum);
      |                              ^~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:346:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  346 |     EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_CHECKSUM_MISMATCH);
      |                                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_CancelTransfer_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:355:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  355 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:358:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  358 |     EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);
      |                                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:361:28: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  361 |     EXPECT_TRUE(transfer_->getActiveTransferId().empty());
      |                            ^~~~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_CancelNonExistentTransfer_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:366:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  366 |     EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_TRANSFER_NOT_FOUND);
      |                                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_ProgressCallback_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:377:36: error: cannot convert ‘lwm2m::transport::test::BlockwiseTransferTest_ProgressCallback_Test::TestBody()::<lambda(uint64_t, uint64_t)>’ to ‘lwm2m::transport::BlockProgressCallback’ {aka ‘std::function<void(const lwm2m::transport::TransferStats&)>’}
  377 |     transfer_->setProgressCallback([&](uint64_t current, uint64_t total) {
      |                                    ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                    |
      |                                    lwm2m::transport::test::BlockwiseTransferTest_ProgressCallback_Test::TestBody()::<lambda(uint64_t, uint64_t)>
  378 |         callbackCount++;
      |         ~~~~~~~~~~~~~~~~
  379 |         EXPECT_GE(current, lastCurrent);
      |         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  380 |         EXPECT_LE(current, total);
      |         ~~~~~~~~~~~~~~~~~~~~~~~~~~
  381 |         lastCurrent = current;
      |         ~~~~~~~~~~~~~~~~~~~~~~
  382 |     });
      |     ~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/include/transport/blockwise_transfer.h:349:52: note:   initializing argument 1 of ‘void lwm2m::transport::BlockwiseTransfer::setProgressCallback(lwm2m::transport::BlockProgressCallback)’
  349 |     void setProgressCallback(BlockProgressCallback callback);
      |                              ~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:384:16: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘beginReceive’
  384 |     transfer_->beginReceive("/resource", 4096);
      |                ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:385:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  385 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
In file included from /usr/include/gtest/gtest-printers.h:122,
                 from /usr/include/gtest/gtest-matchers.h:49,
                 from /usr/include/gtest/internal/gtest-death-test-internal.h:47,
                 from /usr/include/gtest/gtest-death-test.h:43,
                 from /usr/include/gtest/gtest.h:65:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp: In member function ‘virtual void lwm2m::objects::test::FirmwareUpdateObjectTest_RollbackEnabled_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp:232:31: error: ‘class lwm2m::objects::AdvancedFirmwareUpdateObject’ has no member named ‘isRollbackEnabled’; did you mean ‘rollbackEnabled_’?
  232 |     EXPECT_TRUE(firmwareObj_->isRollbackEnabled());
      |                               ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp:232:31: error: ‘bool lwm2m::objects::AdvancedFirmwareUpdateObject::rollbackEnabled_’ is private within this context
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_HandleDuplicateBlock_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:400:16: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘beginReceive’
  400 |     transfer_->beginReceive("/resource", 2048);
      |                ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:401:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  401 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:410:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  410 |     EXPECT_EQ(result, BlockwiseTransfer::Result::SUCCESS);  // Should ignore duplicate
      |                                          ^~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp: In member function ‘virtual void lwm2m::transport::test::BlockwiseTransferTest_BlockSizeMismatch_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:417:57: error: ‘lwm2m::transport::BlockwiseTransfer::BlockSize’ has not been declared
  417 |     transfer_->setPreferredBlockSize(BlockwiseTransfer::BlockSize::BLOCK_1024);
      |                                                         ^~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:418:16: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘beginReceive’
  418 |     transfer_->beginReceive("/resource", 2048);
      |                ^~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:419:34: error: ‘class lwm2m::transport::BlockwiseTransfer’ has no member named ‘getActiveTransferId’; did you mean ‘getActiveTransfers’?
  419 |     auto transferId = transfer_->getActiveTransferId();
      |                                  ^~~~~~~~~~~~~~~~~~~
      |                                  getActiveTransfers
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_blockwise_transfer.cpp:424:42: error: ‘lwm2m::transport::BlockwiseTransfer::Result’ has not been declared
  424 |     EXPECT_EQ(result, BlockwiseTransfer::Result::ERROR_BLOCK_SIZE_MISMATCH);
      |                                          ^~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp:10:
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:522:10: note: declared private here
  522 |     bool rollbackEnabled_ = true;
      |          ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp:235:32: error: ‘class lwm2m::objects::AdvancedFirmwareUpdateObject’ has no member named ‘isRollbackEnabled’; did you mean ‘rollbackEnabled_’?
  235 |     EXPECT_FALSE(firmwareObj_->isRollbackEnabled());
      |                                ^~~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp:235:32: error: ‘bool lwm2m::objects::AdvancedFirmwareUpdateObject::rollbackEnabled_’ is private within this context
/mnt/d/Dev/Friendly-LwM2M-Client/include/objects/advanced_firmware_update_object.h:522:10: note: declared private here
  522 |     bool rollbackEnabled_ = true;
      |          ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp: In member function ‘virtual void lwm2m::objects::test::UpdateResultTest_ResultCodes_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp:378:46: error: ‘NOT_ENOUGH_FLASH’ is not a member of ‘lwm2m::objects::UpdateResult’; did you mean ‘NOT_ENOUGH_STORAGE’?
  378 |     EXPECT_EQ(static_cast<int>(UpdateResult::NOT_ENOUGH_FLASH), 2);
      |                                              ^~~~~~~~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp:379:46: error: ‘OUT_OF_RAM’ is not a member of ‘lwm2m::objects::UpdateResult’; did you mean ‘OUT_OF_MEMORY’?
  379 |     EXPECT_EQ(static_cast<int>(UpdateResult::OUT_OF_RAM), 3);
      |                                              ^~~~~~~~~~
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp: In member function ‘virtual void lwm2m::objects::test::UpdateResultTest_DeltaSpecificResults_Test::TestBody()’:
/mnt/d/Dev/Friendly-LwM2M-Client/test/test_firmware_update_object.cpp:389:46: error: ‘DELTA_MISMATCH’ is not a member of ‘lwm2m::objects::UpdateResult’; did you mean ‘DELTA_SOURCE_MISMATCH’?
  389 |     EXPECT_EQ(static_cast<int>(UpdateResult::DELTA_MISMATCH), 10);
      |                                              ^~~~~~~~~~~~~~
make[2]: *** [CMakeFiles/test_blockwise_transfer.dir/build.make:79: CMakeFiles/test_blockwise_transfer.dir/test/test_blockwise_transfer.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:165: CMakeFiles/test_blockwise_transfer.dir/all] Error 2
make[1]: *** Waiting for unfinished jobs....
make[2]: *** [CMakeFiles/test_firmware_update_object.dir/build.make:79: CMakeFiles/test_firmware_update_object.dir/test/test_firmware_update_object.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:229: CMakeFiles/test_firmware_update_object.dir/all] Error 2
[ 85%] Linking CXX executable test_rollback_manager
[ 85%] Built target test_rollback_manager
[ 90%] Linking CXX executable test_delta_algorithms
[ 90%] Built target test_delta_algorithms
make: *** [Makefile:166: all] Error 2