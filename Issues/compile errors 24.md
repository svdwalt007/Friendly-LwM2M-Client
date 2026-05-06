[ 97%] Building CXX object CMakeFiles/WppExample.dir/platform/connection_dtls/Connection.cpp.o
/mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.cpp: In function ‘void fwUpdaterInit(wpp::WppClient&)’:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.cpp:142:30: error: too few arguments to function ‘bool initOpenWrtFirmwareUpdate(wpp::WppClient&, OpenWrtFwManager&)’
  142 |     initOpenWrtFirmwareUpdate(client);
      |     ~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.h:7,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/objects.cpp:1:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwInit.h:109:6: note: declared here
  109 | bool initOpenWrtFirmwareUpdate(WppClient &client, OpenWrtFwManager &manager);
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~
make[2]: *** [CMakeFiles/WppExample.dir/build.make:870: CMakeFiles/WppExample.dir/objects.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
make[1]: *** [CMakeFiles/Makefile2:1125: CMakeFiles/WppExample.dir/all] Error 2
make: *** [Makefile:136: all] Error 