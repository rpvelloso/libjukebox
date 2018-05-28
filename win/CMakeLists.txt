add_library(libjukebox-win
        Mixer/WindowsMixer.cpp
        Mixer/WindowsMixer.h
        Sound/DirectSoundBuffer.cpp
        Sound/DirectSoundBuffer.h)
target_link_libraries(libjukebox-win libjukebox ${DSOUND_LIB} ${USER32_LIB} ${WINMM_LIB})