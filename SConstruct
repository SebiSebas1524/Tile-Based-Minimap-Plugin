#!/usr/bin/env python

env = SConscript("godot-cpp/SConstruct")

env.Append(CPPPATH=["include/"])

sources = Glob("include/*.cpp")

folder = "build/addons/TileBasedMinimapPlugin"

if env["platform"] == "macos":
	file_name = "libtbmplugin.{}.{}".format(env["platform"], env["target"])

	library = env.SharedLibrary(
		"{}/{}.framework/{}".format(folder, file_name, file_name),
		source=sources
	)
else:
	library = env.SharedLibrary(
		"{}/libtbmplugin{}{}"
			.format(folder, env["suffix"], env["SHLIBSUFFIX"]),
		source=sources,
	)

gdextension_copy = env.Command(
	target="{}/TileBasedMinimapPlugin.gdextension".format(folder),
	source="TileBasedMinimapPlugin.gdextension",
	action=Copy("$TARGET", "$SOURCE")
)

env.Depends(gdextension_copy, library)

CacheDir(".scons_cache/")

Default(library)

Default(gdextension_copy)