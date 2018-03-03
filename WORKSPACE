# win32

new_local_repository(
	name = "win32_libs",
	path = "/c/Program Files (x86)/Windows Kits/10/Lib/10.0.16299.0/um/x64",
	build_file_content = """
cc_library(
	name = "dsound",
	srcs = [
		"dsound.lib",
	],
	visibility = ["//visibility:public"],
)

cc_library(
	name = "user32",
	srcs = [
		"User32.lib",
	],
	visibility = ["//visibility:public"],
)

cc_library(
	name = "winmm",
	srcs = [
		"winmm.lib",
	],
	visibility = ["//visibility:public"],
)
""",
)

# linux

new_local_repository(
	name = "linux_libs",
	path = "/usr/lib/x86_64-linux-gnu",
	build_file_content = """
cc_library(
	name = "asound",
	srcs = ["libasound.so.2"],
	deps = [":pthread"],
	visibility = ["//visibility:public"],
)

cc_library(
	name = "pthread",
	srcs = ["libpthread.so"],
	visibility = ["//visibility:public"],
)
""",
)

