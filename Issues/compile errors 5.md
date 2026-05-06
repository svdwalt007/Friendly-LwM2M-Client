 cmake ../examples -DCMAKE_BUILD_TYPE=MinSizeRel
CMake Warning (dev) at CMakeLists.txt:2 (project):
  cmake_minimum_required() should be called prior to this top-level project()
  call.  Please see the cmake-commands(7) manual for usage documentation of
  both commands.
This warning is for project developers.  Use -Wno-dev to suppress it.

-- The CXX compiler identification is GNU 13.3.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Building for 64 bit system
-- Exceptions are not supported
-- RTTI is not supported
CMake Warning (dev) at /mnt/d/Dev/Friendly-LwM2M-Client/wpp/configs/wpp_config.cmake:142 (set):
  Cannot set "WPP_DEFINITIONS": current scope has no parent.
Call Stack (most recent call first):
  CMakeLists.txt:10 (include)
This warning is for project developers.  Use -Wno-dev to suppress it.

-- Found CURL: /usr/lib/x86_64-linux-gnu/libcurl.so (found version "8.5.0")
-- Found OpenSSL: /usr/lib/x86_64-linux-gnu/libcrypto.so (found version "3.0.13")
-- WppExample: Endiannes not set, defaulting to little endian
CMake Error at /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10513_router_management/CMakeLists.txt:4 (target_sources):
  Cannot specify sources for target "Wpp" which is not built by this project.


CMake Error at /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10514_ethernet_interface/CMakeLists.txt:4 (target_sources):
  Cannot specify sources for target "Wpp" which is not built by this project.


CMake Error at /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10515_gpio_control/CMakeLists.txt:4 (target_sources):
  Cannot specify sources for target "Wpp" which is not built by this project.


CMake Error at /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10516_usb_management/CMakeLists.txt:4 (target_sources):
  Cannot specify sources for target "Wpp" which is not built by this project.


CMake Error at /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10517_storage_management/CMakeLists.txt:4 (target_sources):
  Cannot specify sources for target "Wpp" which is not built by this project.


CMake Error at /mnt/d/Dev/Friendly-LwM2M-Client/wpp/registry/objects/w_10518_system_monitor/CMakeLists.txt:4 (target_sources):
  Cannot specify sources for target "Wpp" which is not built by this project.


CMake Error at CMakeLists.txt:66 (message):
  Failed to build libcoap: libcoap_build.sh: line 2: $'\r': command not found

: invalid optionsh: line 4: set: -

  set: usage: set [-abefhkmnptuvxBCEHPT] [-o option-name] [--] [-] [arg ...]

  libcoap_build.sh: line 5: $'\r': command not found

  libcoap_build.sh: line 9: $'\r': command not found

  libcoap_build.sh: line 10: cd: $'libcoap\r\r': No such file or directory

  libcoap_build.sh: line 11: $'\r': command not found

  libcoap_build.sh: line 38: syntax error: unexpected end of file



CMake Error in CMakeLists.txt:
  No cmake_minimum_required command is present.  A line of code such as

    cmake_minimum_required(VERSION 4.3)

  should be added at the top of the file.  The version specified may be lower
  if you wish to support older CMake versions for this project.  For more
  information run "cmake --help-policy CMP0000".


-- Configuring incomplete, errors occurred!