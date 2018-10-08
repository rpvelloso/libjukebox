package(default_visibility = ["//visibility:public"])

cc_library(
	name = "mixer",
	srcs = ["Mixer.cpp"],
	hdrs = ["Mixer.h"],
	deps = [
		":factory",
		":mixer_impl",
	],
)

cc_library(
	name = "mixer_impl",
	srcs = ["MixerImpl.h"],
)

cc_library(
	name = "factory",
	hdrs = ["Factory.h"],
	deps = [
		":mixer_impl",
	] + select({
		"@bazel_tools//src/conditions:windows": ["//win/Mixer:mixer"],
		"//conditions:default": ["//linux/Mixer:alsa_mixer"],
	}),
)
