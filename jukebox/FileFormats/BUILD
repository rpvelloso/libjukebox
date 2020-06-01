package(default_visibility = ["//visibility:public"])

cc_library(
	name = "file_loader",
	hdrs = ["FileLoader.h"],
	deps = [
		":sound_file_impl",
	],
)

cc_library(
	name = "flac",
	srcs = [
		"FLACFileImpl.cpp",
		"//jukebox/Decoders:FLACDecoderImpl.cpp",
		"//jukebox/Decoders:FLACDecoderImpl.h",
	],
	hdrs = [
		"FLACFileImpl.h",
	],
	deps = [
		":file_loader",
		":sound_file",
		":sound_file_impl",
		"//jukebox/Decoders:decoder",
		"//jukebox/Decoders/dr_flac",
	],
)

cc_library(
	name = "midi",
	srcs = [
		"MIDIFileImpl.cpp",
		"//jukebox/Decoders:MIDIDecoderImpl.cpp",
		"//jukebox/Decoders:MIDIDecoderImpl.h",
		"//jukebox/Decoders:MIDIConfigurator.h",
		"midi/MidiFile.h",
	],
	hdrs = [
		"MIDIFileImpl.h",
	],
	deps = [
		":file_loader",
		":sound_file",
		":sound_file_impl",
		"//jukebox/Decoders:decoder",
		"//jukebox/Decoders/fluidsynth",
	],
)

cc_library(
	name = "mp3",
	srcs = [
		"MP3FileImpl.cpp",
		"//jukebox/Decoders:MP3DecoderImpl.cpp",
		"//jukebox/Decoders:MP3DecoderImpl.h",
	],
	hdrs = [
		"MP3FileImpl.h",
	],
	deps = [
		":file_loader",
		":sound_file",
		":sound_file_impl",
		"//jukebox/Decoders:decoder",
		"//jukebox/Decoders/dr_mp3",
	],
)

cc_library(
	name = "mod",
	srcs = [
		"ModFileImpl.cpp",
		"//jukebox/Decoders:ModDecoderImpl.cpp",
		"//jukebox/Decoders:ModDecoderImpl.h",
	],
	hdrs = [
		"ModFileImpl.h",
	],
	deps = [
		":file_loader",
		":sound_file",
		":sound_file_impl",
		"//jukebox/Decoders:decoder",
		"//jukebox/Decoders/micromod",
	],
)

cc_library(
	name = "sound_file",
	srcs = ["SoundFile.cpp"],
	hdrs = ["SoundFile.h"],
	deps = [
		":sound_file_impl",
	],
)

cc_library(
	name = "sound_file_impl",
	srcs = [
		"SoundFileImpl.cpp",
		"//jukebox/Decoders:DecoderImpl.cpp",
		"//jukebox/Decoders:DecoderImpl.h",
	],
	hdrs = ["SoundFileImpl.h"],
)

cc_library(
	name = "vorbis",
	srcs = [
		"VorbisFileImpl.cpp",
		"//jukebox/Decoders:VorbisDecoderImpl.cpp",
		"//jukebox/Decoders:VorbisDecoderImpl.h",
	],
	hdrs = [
		"VorbisFileImpl.h",
	],
	deps = [
		":file_loader",
		":sound_file",
		":sound_file_impl",
		"//jukebox/Decoders:decoder",
		"//jukebox/Decoders/stb_vorbis",
	],
)

cc_library(
	name = "wave",
	srcs = [
		"WaveFileImpl.cpp",
		"//jukebox/Decoders:WaveDecoderImpl.cpp",
		"//jukebox/Decoders:WaveDecoderImpl.h",
	],
	hdrs = [
		"WaveFileImpl.h",
	],
	deps = [
		":file_loader",
		":sound_file",
		":sound_file_impl",
		"//jukebox/Decoders:decoder",
		"//jukebox/Decoders/dr_wav",
	],
)
