package(default_visibility = ["//visibility:public"])

cc_library(
	name = "sound",
	srcs = ["Sound.cpp"],
	hdrs = ["Sound.h"],
	deps = [":sound_impl"],
)

cc_library(
	name = "sound_impl",
	srcs = ["SoundImpl.cpp"],
	hdrs = ["SoundImpl.h"],
	deps = [
		"//jukebox/FileFormats:sound_file",
	],
)

cc_library(
	name = "factory",
	hdrs = ["Factory.h"],
	deps = [
		":sound",
		"//jukebox/FileFormats:sound_file",
	] + select({
		"@bazel_tools//src/conditions:windows": ["//win/Sound:direct_sound"],
		"//conditions:default": ["//linux/Sound:alsa_sound"],
	}),
)
