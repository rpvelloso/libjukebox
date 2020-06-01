package(default_visibility = ["//jukebox:__subpackages__"])

cc_library(
	name = "alsa_mixer",
	srcs = ["AlsaMixer.cpp"],
	hdrs = ["AlsaMixer.h"],
	deps = [
		"//jukebox/Mixer:mixer_impl",
		"@linux_libs//:asound",
	],
)
