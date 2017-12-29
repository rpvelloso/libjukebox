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

cc_binary(
	name = "main_linux",
	data = [
		"//jukebox_test/data:sound",
	],
	deps = [
		":main",
		"//jukebox/FileFormats:wave_file",  # because the factory implementation is there :-(
		"//linux/Mixer:alsa_mixer",
		"//linux/Sound:alsa_sound",
	],
	
	# I am getting undefined reference to jukebox::factory::makeMixerImpl()
	# even after adding "//linux/Mixer:alsa_mixer" dependency
	# gcc is probably messing up with the linking settings.
	# I have to manually include AlsaMixer.cpp as a source to get rid of this issue
	# note that the windows target does not have this problem and it links properly
	srcs = ["//linux/Mixer:AlsaMixer.cpp"],
)
