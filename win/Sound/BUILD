package(default_visibility = ["//visibility:public"])

cc_library(
	name = "direct_sound",
	srcs = ["DirectSoundBuffer.cpp"],
	hdrs = ["DirectSoundBuffer.h"],
	deps = [
		"//jukebox/FileFormats:sound_file",
		"//jukebox/Sound:sound",
		"//jukebox/Sound:sound_impl",
		"@win32_libs//:dsound",
		"@win32_libs//:user32",
		"@win32_libs//:winmm",
	],
)
