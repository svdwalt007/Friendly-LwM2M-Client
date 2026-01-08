# MIKROBUS Object (ID 10520) - Walt Technologies

set(MIKROBUS_SOURCES
    Mikrobus.cpp
)

set(MIKROBUS_HEADERS
    Mikrobus.h
    MikrobusInfo.h
    MikrobusConfig.h
)

# Add sources to parent
set(OBJECT_SOURCES ${OBJECT_SOURCES} ${MIKROBUS_SOURCES} PARENT_SCOPE)
set(OBJECT_HEADERS ${OBJECT_HEADERS} ${MIKROBUS_HEADERS} PARENT_SCOPE)
