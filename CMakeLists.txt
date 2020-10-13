
cmake_minimum_required(VERSION 3.6)

project(animator)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-function")

add_subdirectory("Mono1/")
include_directories("Mono1/src" "Mono1/third_party")

# Animation tool

set(osx_bundle_icons osx/icons.icns)
set_source_files_properties(
    ${osx_bundle_icons} PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources"
)

file(GLOB_RECURSE source_files "src/*.cpp")
# add_executable(animator MACOSX_BUNDLE ${osx_bundle_icons} ${source_files})
add_executable(animator ${source_files})
add_dependencies(animator mono)
target_link_libraries(animator mono)
