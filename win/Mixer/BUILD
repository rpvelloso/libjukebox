package(default_visibility = ["//visibility:public"])

cc_library(
	name = "windows_mixer",
	srcs = ["WindowsMixer.cpp"],
	hdrs = ["WindowsMixer.h"],
	deps = [
		"//Mixer:mixer",
		"//Mixer:mixer_impl",
		"@win32_libs//:winmm",
	],
)
