package(default_visibility = ["//visibility:public"])

cc_library(
	name = "alsa_sound",
	srcs = ["AlsaHandle.cpp"],
	hdrs = ["AlsaHandle.h"],
	deps = [
		"//jukebox/FileFormats:sound_file",
		"//jukebox/Sound:sound",
		"//jukebox/Sound:sound_impl",
		"@linux_libs//:asound",
	],
)
