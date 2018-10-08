cc_binary(
	name = "demo",
	srcs = ["main.cpp"],
	deps = [
		"//jukebox/FileFormats:sound_file",
		"//jukebox/FileFormats:wave_file",
		"//jukebox/Mixer:mixer",
		"//jukebox/Sound:factory",
		"//jukebox/Sound:sound",
	],
	data = [
		"//jukebox_test/data:sound",
	],
)
