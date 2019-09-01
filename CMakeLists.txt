cmake_minimum_required(VERSION 3.10)
project(libjukebox)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)

include_directories(.)

add_subdirectory("jukebox")
add_subdirectory("win")
add_subdirectory("jukebox_test")
