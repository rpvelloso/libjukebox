FIND_LIBRARY(ASOUND_LIB asound)
FIND_LIBRARY(PTHREAD_LIB pthread)
set(LINUX_LIBS ${ASOUND_LIB} ${PTHREAD_LIB})

add_library(libjukebox-impl
        Mixer/AlsaMixer.cpp
        Mixer/AlsaMixer.h
        Sound/AlsaHandle.cpp
        Sound/AlsaHandle.h)
target_link_libraries(libjukebox-impl libjukebox ${LINUX_LIBS})