package(default_visibility = ["//jukebox:__subpackages__"])

cc_library(
	name = "direct_sound",
	srcs = ["DirectSoundBuffer.cpp"],
	hdrs = ["DirectSoundBuffer.h"],
	deps = [
		"//jukebox/FileFormats:sound_file",
		"//jukebox/Decoders:decoder",
		"//jukebox/Sound:sound_impl",
		"@win32_libs//:dsound",
		"@win32_libs//:dxguid",
		"@win32_libs//:user32",
		"@win32_libs//:winmm",
	],
)
