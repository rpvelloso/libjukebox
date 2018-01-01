package(default_visibility = ["//visibility:public"])

cc_library(
	name = "win_deps",
	deps = [
		"//jukebox/FileFormats:wave_file",  # because the factory implementation is there :-(
		"//win/Mixer:mixer",
		"//win/Sound:direct_sound",
	],
)

cc_library(
	name = "linux_deps",
	deps = [
		"//jukebox/FileFormats:wave_file",  # because the factory implementation is there :-(
		"//linux/Mixer:alsa_mixer",
		"//linux/Sound:alsa_sound",
	],
)
