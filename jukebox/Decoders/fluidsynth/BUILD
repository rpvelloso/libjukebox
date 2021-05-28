package(default_visibility = ["//jukebox:__subpackages__"])

cc_library(
	name = "fluidsynth",
	srcs = [
		"fs.c",
	],
	hdrs = [
		"fs.h",
		"soundfont.h",
	],
	defines = select({
		"@bazel_tools//src/conditions:windows": [
			"HAVE_WINDOWS_H",
			"WIN32",
		],
		"//conditions:default": ["HAVE_UNISTD_H"],
	}),
	deps = select({
		"@bazel_tools//src/conditions:windows": [
			"//jukebox/Decoders/fluidsynth/pthread-win32",
		],
		"//conditions:default": [],
	}),
	includes = ["./pthread-win32"],
)
