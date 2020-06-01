cmake_minimum_required(VERSION 3.10)
project(libjukebox)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_subdirectory("jukebox")
if (WIN32)
    add_subdirectory("win")
else()
    add_subdirectory("linux")
endif()
add_subdirectory("jukebox_test")
