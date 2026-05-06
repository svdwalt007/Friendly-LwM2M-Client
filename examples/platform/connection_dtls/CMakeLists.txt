# Use TinyDTLS-specific connection if TinyDTLS is enabled, otherwise use multi-provider connection
if(DTLS_PROVIDER_TINYDTLS)
    # Old TinyDTLS-only connection (requires tinydtls.h)
    set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/Connection.cpp PARENT_SCOPE)
else()
    # New multi-provider connection (works with any DTLS provider)
    set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/Connection_new.cpp PARENT_SCOPE)
endif()
# Update INCLUDES variable from parent scope.
set(INCLUDES ${INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/ PARENT_SCOPE)