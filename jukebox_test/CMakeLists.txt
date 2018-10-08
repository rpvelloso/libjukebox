add_executable(jukeboxtest
        demo/main.cpp)

target_link_libraries(jukeboxtest libjukebox libjukebox-win ${OS_LIBS})
