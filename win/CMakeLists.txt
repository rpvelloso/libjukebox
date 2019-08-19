FIND_LIBRARY(DSOUND_LIB dsound)
# TODO use a variable for mingw path
FIND_LIBRARY(DXGUID_LIB NAMES dxguid PATHS "C:/mingw-dists/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/x86_64-w64-mingw32/lib")
FIND_LIBRARY(WINMM_LIB winmm)
set(WIN_LIBS ${DSOUND_LIB} ${DXGUID_LIB} ${WINMM_LIB})

add_library(libjukebox-win
        Mixer/WindowsMixer.cpp
        Mixer/WindowsMixer.h
        Sound/DirectSoundBuffer.cpp
        Sound/DirectSoundBuffer.h)
target_link_libraries(libjukebox-win libjukebox ${WIN_LIBS})