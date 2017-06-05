
cmake_minimum_required(VERSION 2.6)

project(animator)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wno-long-long -pedantic -Wno-unused-parameter -Wno-unused-variable -Wno-format-pedantic")

add_subdirectory("Mono1/")
include_directories("Mono1/MonoEngine" "Mono1/External Libs")

# Animation tool
file(GLOB_RECURSE source_files "src/*.cpp")
add_executable(animator ${source_files})
add_dependencies(animator mono)
target_link_libraries(animator mono)
