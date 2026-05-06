# Build options
option(WPP_BUILD_WITH_EXCEPTIONS "Enable support of Exceptions" OFF)
option(WPP_BUILD_WITH_RTTI "Enable support of RTTI" OFF)
option(WPP_BUILD_FOR_64_BIT "Build for 64-bit system or 32-bit" ON)

# Position-independent code is code that can execute
# properly regardless of where in memory it is located.
# This is essential for shared libraries.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Link-time optimization
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")

# Main comiler options
add_compile_options(
    -Waggregate-return
    -Wall
    -Wcast-align
    -Wextra
    -Wfloat-equal
    -Wpointer-arith
    -Wshadow
    -Wswitch-default
    -Wwrite-strings
    # Unused parameters are common in this ifdef-littered code-base, but of no danger
    -Wno-unused-parameter
    # Too many false positives
    -Wno-uninitialized
    # NOTE: -pedantic is NOT used because WPP logging macros use GNU extensions (##__VA_ARGS__)
    # Turn (most) warnings into errors
    -Werror
    # Disabled because of existing, non-trivially fixable code
    -Wno-error=cast-align
)

# Compiler-specific options for WPP logging macro compatibility
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        # Allow usage ##__VA_ARGS__ in macros
        -Wno-gnu-zero-variadic-macro-arguments
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(
        # Allow GNU extension in variadic macros (needed for WPP logging)
        -Wno-variadic-macros
    )
endif()

# Build for system type
if (WPP_BUILD_FOR_64_BIT)
    message(STATUS "Building for 64 bit system")
    add_compile_options(-m64)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m64")
else()
    message(STATUS "Building for 32 bit system")
    add_compile_options(-m32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
endif()

# Exceptions comiler options (C++ only)
if (WPP_BUILD_WITH_EXCEPTIONS)
    message(STATUS "Exceptions are supported")
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fexceptions>)
else()
    message(STATUS "Exceptions are not supported")
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-asynchronous-unwind-tables>)
endif()

# RTTI comiler options (C++ only)
if (WPP_BUILD_WITH_RTTI)
    message(STATUS "RTTI is supported")
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-frtti>)
else()
    message(STATUS "RTTI is not supported")
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
endif()