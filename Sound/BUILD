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
	deps = ["//FileFormats:sound_file"],
)
