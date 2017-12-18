package(default_visibility = ["//visibility:public"])

cc_library(
	name = "direct_sound",
	srcs = ["DirectSoundBuffer.cpp"],
	hdrs = ["DirectSoundBuffer.h"],
	deps = [
		"//FileFormats:sound_file",
		"//Sound:sound",
		"//Sound:sound_impl",
		"@win32_libs//:dsound",
		"@win32_libs//:winmm",
	],
)
