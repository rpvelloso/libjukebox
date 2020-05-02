package(default_visibility = ["//jukebox:__subpackages__"])

cc_library(
	name = "fluidsynth",
	srcs = ["fs.c"],
	hdrs = ["fs.h"],
	defines = select({
		"@bazel_tools//src/conditions:windows": ["HAVE_WINDOWS_H"],
		"//conditions:default": [],
	})
)