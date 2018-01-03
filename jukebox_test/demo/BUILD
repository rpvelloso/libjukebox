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
		"//:win_deps",
	],
)

cc_binary(
	name = "main_linux",
	data = [
		"//jukebox_test/data:sound",
	],
	deps = [
		":main",
		"//:linux_deps",
	],
)
