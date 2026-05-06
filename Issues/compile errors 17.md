make -j$(nproc)
[  1%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUpdater.cpp.o
[  3%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwDownloader.cpp.o
[  3%] Building CXX object OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwInit.cpp.o
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/WppRegistry.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwUpdater.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/Object.h:14:10: fatal error: ObjSubject.h: No such file or directory
   14 | #include "ObjSubject.h"
      |          ^~~~~~~~~~~~~~
compilation terminated.
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/WppRegistry.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwInit.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwInit.cpp:7:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/Object.h:14:10: fatal error: ObjSubject.h: No such file or directory
   14 | #include "ObjSubject.h"
      |          ^~~~~~~~~~~~~~
compilation terminated.
In file included from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/ObjectImpl.h:11,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/WppRegistry.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/client/WppClient.h:16,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.h:12,
                 from /mnt/d/Dev/Friendly-LwM2M-Client/examples/OpenWrtFirmwareUpdater/OpenWrtFwDownloader.cpp:8:
/mnt/d/Dev/Friendly-LwM2M-Client/examples/../wpp/registry/data_model/object/Object.h:14:10: fatal error: ObjSubject.h: No such file or directory
   14 | #include "ObjSubject.h"
      |          ^~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:79: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwUpdater.cpp.o] Error 1
make[2]: *** Waiting for unfinished jobs....
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:93: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwDownloader.cpp.o] Error 1
make[2]: *** [OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/build.make:107: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/OpenWrtFwInit.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:1189: OpenWrtFirmwareUpdater/CMakeFiles/OpenWrtFirmwareUpdater.dir/all] Error 2
make: *** [Makefile:136: all] Error 2