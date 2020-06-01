package(default_visibility = ["//visibility:public"])

cc_library(
	name = "decoder",
	srcs = ["Decoder.cpp"],
	hdrs = ["Decoder.h"],
	deps = [
		"//jukebox/FileFormats:sound_file",
		"//jukebox/FileFormats:sound_file_impl",
	],
)

# jukebox/FileFormats/BUILD uses the specific decoder implementations
exports_files(glob(["*Impl.cpp", "*Impl.h", "MIDI*"]))