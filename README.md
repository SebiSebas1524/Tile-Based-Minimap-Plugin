<img width="1869" height="643" alt="299873078-34e2f9bb-eb1d-40d0-84bb-2bf0b4742d57" src="https://github.com/user-attachments/assets/06a9f6cc-773b-4162-8f80-c17698fe1d3e" />

This project is made for my Year 3 at Breda University of Applied Science

# Tile-Based-Minimap-Plugin

Develop a public Godot Engine addon (via GDExtension in C++)for
creating tile-based minimaps with: ​​

- Dynamic tile loading for large maps ​​
- Basic blip markers (player + waypoint) ​
- Simple tools to create tiles

# Compiling the plugin yourself

Requirements:
- a version of Python and pip
- SCons (`pip install scons`) & SCons being declared in your PATH [("how-to" here)](https://stackoverflow.com/a/63925889)
- Godot 4.4+

Commands for cloning the repository (with the `godot-cpp` submodule included) and building the plugin with **SCons**:
```
git clone --recursive https://github.com/SebiSebas1524/Tile-Based-Minimap-Plugin
cd Tile-Based-Minimap-Plugin/
sconstruct
```

The build is currently being outputted to `demo/addons/`, but you can easily change this in the `SConstruct` configuration file (`folder = your_path_here`)

# **This project is still work in progress**
