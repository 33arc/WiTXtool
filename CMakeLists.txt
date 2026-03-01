cmake_minimum_required(VERSION 3.10) # 4.0.0 is very futuristic; 3.10 is standard
project(WiTXtool C)

set(CMAKE_C_STANDARD 99)

# Tell the compiler where to find your .h files
include_directories(include)

# Add zlib subdirectory (assuming you have the zlib source in external/zlib)
add_subdirectory(external/zlib)

# Define the executable and its source files
add_executable(WiTXtool
    src/main.c
    src/crypto.c
    src/compress.c
)

# Link Windows-specific crypto libraries
if(WIN32)
    # FIX: Changed 'myapp' to 'WiTXtool'
    target_link_libraries(WiTXtool PRIVATE advapi32)
endif()

# Link to the zlib target defined in your submodule
# FIX: Changed 'myapp' to 'WiTXtool'
# NOTE: Depending on the zlib version, the target might be 'zlibstatic' or just 'zlib'
target_link_libraries(WiTXtool PRIVATE zlibstatic)
