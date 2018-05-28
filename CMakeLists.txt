cmake_minimum_required(VERSION 3.10)
project(libjukebox)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

FIND_LIBRARY(DSOUND_LIB dsound)
FIND_LIBRARY(USER32_LIB user32)
FIND_LIBRARY(WINMM_LIB winmm)
set(OS_LIBS ${DSOUND_LIB} ${USER32_LIB} ${WINMM_LIB})

add_subdirectory("jukebox")
add_subdirectory("win")
add_subdirectory("jukebox_test")
