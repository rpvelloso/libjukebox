package(default_visibility = ["//jukebox:__subpackages__"])

cc_library(
	name = "alsa_sound",
	srcs = ["AlsaHandle.cpp"],
	hdrs = ["AlsaHandle.h"],
	deps = [
		"//jukebox/Decoders:decoder",
		"//jukebox/Decoders/Decorators",
		"//jukebox/FileFormats:sound_file",
		"//jukebox/Sound:sound_impl",
		"@linux_libs//:asound",
	],
)
