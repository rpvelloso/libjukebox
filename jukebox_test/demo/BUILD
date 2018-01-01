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
	
# I am getting undefined reference to jukebox::factory::makeMixerImpl()
# even after adding "//linux/Mixer:alsa_mixer" dependency
# gcc is probably messing up with the linking settings.
# workaround: manually include AlsaMixer.cpp as a source to get rid of this issue
# note: :main_win32 does not have this problem and it links properly
# note: adding AlsaMixer.cpp to //:linux_deps also does not help
	srcs = ["//linux/Mixer:AlsaMixer.cpp"],
)
