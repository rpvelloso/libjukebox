package(default_visibility = ["//visibility:public"])

cc_library(
	name = "mixer",
	srcs = ["WindowsMixer.cpp"],
	hdrs = ["WindowsMixer.h"],
	deps = [
		"//jukebox/Mixer:mixer_impl",
		"@win32_libs//:winmm",
	],
)
