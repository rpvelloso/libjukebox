include_directories(${LUA_INCLUDE_DIR} ${SOL_INCLUDE_DIR})

add_executable(jukeboxdemo
        demo/main.cpp)
add_executable(jukeboxdemo_loop
        demo/loop.cpp)
add_executable(jukeboxdemo_play
        demo/play.cpp)
add_executable(jukeboxdemo_soundShell
        demo/soundShell.cpp)

target_link_libraries(jukeboxdemo libjukebox libjukebox-win)
target_link_libraries(jukeboxdemo_loop libjukebox libjukebox-win)
target_link_libraries(jukeboxdemo_play libjukebox libjukebox-win)
target_link_libraries(jukeboxdemo_soundShell libjukebox libjukebox-win ${LUA_LIB})
