#!/usr/bin/env python
import os

env = SConscript("godot-cpp/SConstruct")

env.Append(CPPPATH=["include/"])

sources = Glob("include/*.cpp")

folder = "demo/addons/TileBasedMinimapPlugin"

for filename in os.listdir(folder):
	file_path = os.path.join(folder, filename)
	if os.path.isfile(file_path):
		os.remove(file_path)

if env["platform"] == "macos":
	file_name = "libtbm_plugin.{}.{}".format(env["platform"], env["target"])

	library = env.SharedLibrary(
		"{}/{}.framework/{}".format(folder, file_name, file_name),
		source=sources
	)
else:
	library = env.SharedLibrary(
		"{}/libtbm_plugin{}{}"
			.format(folder, env["suffix"], env["SHLIBSUFFIX"]),
		source=sources,
	)

gdextension_copy = env.Command(
	target="{}/tbm_plugin.gdextension".format(folder),
	source="tbm_plugin.gdextension",
	action=Copy("$TARGET", "$SOURCE")
)

env.Depends(gdextension_copy, library)

CacheDir(".scons_cache/")

Default(library)

Default(gdextension_copy)