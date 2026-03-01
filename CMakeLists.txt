cmake_minimum_required(VERSION 3.10)
project(WiTXtool C)

set(CMAKE_C_STANDARD 99)

# --- ZLIB CONFIGURATION ---
# Disable Zlib's extra "fluff" to speed up compilation
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Build zlib examples" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "Skip zlib install rules" FORCE)

# Since we want a single portable .exe, we usually want the static version
# This helps the compiler find 'zlibstatic' correctly
add_subdirectory(external/zlib)

# --- PROJECT CONFIGURATION ---
include_directories(include)
# Also include zlib's folder so your code can find "zlib.h"
include_directories(external/zlib) 
include_directories(${CMAKE_CURRENT_BINARY_DIR}/external/zlib)

add_executable(WiTXtool
    src/main.c
    src/crypto.c
    src/compress.c
)

# --- LINKING ---
if(WIN32)
    target_link_libraries(WiTXtool PRIVATE advapi32)
endif()

# Link against the static zlib target
target_link_libraries(WiTXtool PRIVATE zlibstatic)
