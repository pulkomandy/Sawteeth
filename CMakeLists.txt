cmake_minimum_required(VERSION 2.8)

project(sawteeth)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Os -Wno-multichar -fno-exceptions")

include_directories(lib/headers)

add_subdirectory(Source)
add_subdirectory(lib)
