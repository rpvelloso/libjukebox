package(default_visibility = ["//visibility:public"])

cc_library(
	name = "sound",
	srcs = [
		"FileWriterSoundImpl.cpp",
		"FileWriterSoundImpl.h",
		"Factory.cpp",
		"Sound.cpp",
		"Decorators/FadeOnStopSoundImpl.cpp",
		"Decorators/FadeOnStopSoundImpl.h",
	],
	hdrs = [
		"Factory.h",
		"Sound.h",
	],
	deps = [
		":sound_impl",
		"//jukebox/Decoders:decoder",
		"//jukebox/Decoders/Decorators",
		"//jukebox/FileFormats:flac",
		"//jukebox/FileFormats:midi",
		"//jukebox/FileFormats:mod",
		"//jukebox/FileFormats:mp3",
		"//jukebox/FileFormats:sound_file",
		"//jukebox/FileFormats:vorbis",
		"//jukebox/FileFormats:wave",
	] + select({
		"@bazel_tools//src/conditions:windows": ["//win/Sound:direct_sound"],
		"//conditions:default": ["//linux/Sound:alsa_sound"],
	}),
)

cc_library(
	name = "sound_impl",
	srcs = ["SoundImpl.cpp"],
	hdrs = ["SoundImpl.h"],
	deps = [
		"//jukebox/Decoders:decoder",
	],
)
