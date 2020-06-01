cc_binary(
	name = "demo",
	srcs = ["main.cpp"],
	deps = [
		"//jukebox",
	],
	data = [
		"//jukebox_test/data:sound",
	],
)

cc_binary(
	name = "soundfont",
	srcs = ["soundfontDemo.cpp"],
	deps = [
		"//jukebox",
		"//jukebox/FileFormats:midi", # for MIDI configurator
	],
	data = [
		"//jukebox_test/data:sound",
		"//soundfont",
	],
	defines = select({
		# goes back 6: jukebox_test/demo + 4 more paths
		"@bazel_tools//src/conditions:windows": ["SOUNDFONT_PATH=../../../../../../soundfont/"],
		"//conditions:default": [],
	}),
)
