cmake_minimum_required(VERSION 3.10)
project(libjukebox)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

FIND_LIBRARY(DSOUND_LIB dsound)
FIND_LIBRARY(DXGUID_LIB NAMES dxguid PATHS "C:/mingw-dists/mingw-w64/x86_64-7.3.0-posix-seh-rt_v5-rev0/mingw64/x86_64-w64-mingw32/lib")
FIND_LIBRARY(WINMM_LIB winmm)
set(OS_LIBS ${DSOUND_LIB} ${DXGUID_LIB} ${WINMM_LIB})

add_subdirectory("jukebox")
add_subdirectory("win")
add_subdirectory("jukebox_test")
