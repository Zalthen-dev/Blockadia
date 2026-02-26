# Blockadia [ DEMO ]
<img src="repo/BlockadiaCard.png"/>

The Game Engine built with freedom and ease-of-use in mind <br>
Build games big and small with Luau, easy to learn and capable <br>

## API

Blockadia has a relatively small API right now but expect additions soon! If you know how to code, you can help out by creating a pull request!

Even with this API you can do a lot of things! Here's a quick overview of the API
- Services
	- `Workspace`
	- `Lighting`
	- `StarterGui`
	- `RunService`
	- `DebugVisualService`
- Libraries
	- `task`
	- `Color3`
	- `Random`
	- `Signal`
	- `UDim` & `UDim2`
	- `Vector2` & `Vector3`
- Objects (creatable from a script)
	- `Part`
	- `Sound` (untested)
	- `Folder`
	- `ScreenGui`
	- `Frame`
	- `TextGui`
	- `Script`
	- `LocalScript`

# How to Use

## Documentation

Documentation has not yet been started, but you can help out with documentation!

If you want to help with documentation, most methods and libraries are used in `main.luau` with short examples!

## Building from the Source

1. Clone repository
```bash
git clone https://www.github.com/Zalthen-dev/Blockadia.git
cd Blockadia
```

2. Clone dependencies
```bash
cd vendor
git clone https://github.com/raysan5/raylib.git
git clone https://github.com/luau-lang/luau.git
```

3. Build
```bash
cd ..
cmake -S . -B .
cmake --build .
```

The executable file will be created in the repository directory