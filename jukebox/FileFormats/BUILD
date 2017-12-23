package(default_visibility = ["//visibility:public"])

cc_library(
	name = "sound_file",
	srcs = ["SoundFile.cpp"],
	hdrs = ["SoundFile.h"],
	deps = [":sound_file_impl"],
)

cc_library(
	name = "sound_file_impl",
	srcs = ["SoundFileImpl.h"],
)

cc_library(
	name = "wave_file",
	srcs = ["WaveFile.cpp"],
	hdrs = ["WaveFile.h"],
	deps = [
		":sound_file",
		":sound_file_impl",
	],
)

cc_library(
	name = "vorbis_file",
	srcs = ["VorbisFile.cpp"],
	hdrs = ["VorbisFile.h"],
	deps = [
		":sound_file",
		":sound_file_impl",
		"//jukebox/FileFormats/stb_vorbis",
	],
)
