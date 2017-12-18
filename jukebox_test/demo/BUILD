cc_library(
	name = "main",
	srcs = ["main.cpp"],
	deps = [
		"//jukebox/FileFormats:sound_file",
		"//jukebox/Mixer:mixer",
		"//jukebox/Sound:sound",
	],
)

cc_binary(
	name = "main_win32",
	data = [
		"//jukebox_test/data:sound",
	],
	deps = [
		":main",
		"//jukebox/FileFormats:wave_file",  # because the factory implementation is there :-(
		"//win/Mixer:mixer",
		"//win/Sound:direct_sound",
	],
)