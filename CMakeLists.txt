cmake_minimum_required(VERSION 4.0.0)
project(MyProject C)

# Add zlib subdirectory (submodule)
add_subdirectory(external/zlib)

# Add your executable
add_executable(myapp main.c)

# Link your executable to zlib
# zlibstatic is the target defined by zlib's CMakeLists.txt
target_link_libraries(myapp PRIVATE zlibstatic)
