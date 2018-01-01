package(default_visibility = ["//visibility:public"])

cc_library(
	name = "alsa_mixer",
	srcs = ["AlsaMixer.cpp"],
	hdrs = ["AlsaMixer.h"],
	deps = [
		"//jukebox/Mixer:mixer_impl",
		"@linux_libs//:asound",
	],
)
