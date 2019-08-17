if (WIN32)
    add_compile_definitions(HAVE_WINDOWS_H)
endif()

add_library(fluidsynth
        fs.c
        fs.h)