# Install script for directory: /home/sean/work/Friendly-LwM2M-Client/wpp/registry/objects

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/m_0_lwm2m_security/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/m_1_lwm2m_server/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/m_3_device/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/o_10512_starlink_terminal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/o_10519_hardware_watchdog/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/o_10520_mikrobus/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/o_12_wlan_connectivity/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/o_13_bearer_selection/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/o_2_lwm2m_access_control/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/o_3339_audio_clip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/o_4_connectivity_monitoring/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/o_5_firmware_update/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/o_6_location/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10513_router_management/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10514_ethernet_interface/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10515_gpio_control/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10516_usb_management/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10517_storage_management/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10518_system_monitor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10521_firewall_config/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10522_poe_management/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10525_wan_failover_policy/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10526_multiwan_health_check/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10527_wifi_client_management/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10528_wifi_channel_optimization/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10529_matter_bridge/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10530_matter_device/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10531_thread_network/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10532_zigbee_coordinator/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10533_zigbee_device/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10534_zigbee_group/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10535_lan_configuration/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10536_routing_table/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/w_10537_vpn_configuration/cmake_install.cmake")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/sean/work/Friendly-LwM2M-Client/build/examples/wpp/registry/objects/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
