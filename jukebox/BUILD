package(default_visibility = ["//visibility:public"])

# a convenient target that includes the whole sound library
# you can also use the individual targets in the client side, instead, if you need faster compilation

cc_library(
    name = "jukebox",
    deps = [
		"//jukebox/FileFormats:sound_file",
		"//jukebox/FileFormats:wave_file",
		"//jukebox/FileFormats:vorbis_file",
		"//jukebox/Mixer:mixer",
		"//jukebox/Sound:factory",
		"//jukebox/Sound:sound",
    ],
)
