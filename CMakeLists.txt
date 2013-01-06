cmake_minimum_required(VERSION 2.8)

project(sawteeth)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Os -Wno-multichar -fno-exceptions")

include_directories(lib/headers)

set(STLIB_SOURCES
	${CMAKE_SOURCE_DIR}/lib/source/ins_part_channel.cpp
	${CMAKE_SOURCE_DIR}/lib/source/insply.cpp
	${CMAKE_SOURCE_DIR}/lib/source/lfo.cpp
	${CMAKE_SOURCE_DIR}/lib/source/player.cpp 
	${CMAKE_SOURCE_DIR}/lib/source/song.cpp 
	${CMAKE_SOURCE_DIR}/lib/source/txt.cpp 
	${CMAKE_SOURCE_DIR}/lib/source/wave.cpp
)

add_subdirectory(Source)
add_subdirectory(lib)
