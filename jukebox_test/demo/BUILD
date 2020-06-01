cc_binary(
	name = "demo",
	srcs = ["main.cpp"],
	deps = [
		"//jukebox",
		"//jukebox/FileFormats:midi", # for MIDI configurator
	],
	data = [
		"//jukebox_test/data:sound",
		"//soundfont",
	],
)
