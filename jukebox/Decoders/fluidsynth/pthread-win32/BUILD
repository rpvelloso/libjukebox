package(default_visibility = ["//jukebox:__subpackages__"])

cc_library(
	name = "pthread-win32",
	srcs = ["pthread.c"],
	hdrs = ["pthread.h"],
	textual_hdrs = glob([
		"*.c",
		"*.h",
    ]),
	defines = [
		"HAVE_STRUCT_TIMESPEC",
		"PTW32_STATIC_LIB",
		"PTW32_BUILD_INLINED",
	],
	includes = ["."],
)