package(default_visibility = ["//jukebox:__subpackages__"])

cc_library(
	name = "mixer",
	srcs = ["WindowsMixer.cpp"],
	hdrs = ["WindowsMixer.h"],
	deps = [
		"//jukebox/Mixer:mixer_impl",
		"@win32_libs//:winmm",
	],
)
