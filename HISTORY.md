# 2/25/2026

Added new object, `TextLabel`, you can set text and text color

Extended `DebugVisualService` with two new functions, and modified some functions
- New functions
	- `DebugVisualService:DrawRectangle(size: UDim2, pos: UDim2, color: Color3?, transparency: number?)`
		- `color` defaults to `Color3.new(0, 0, 0)`
		- `transparency` defaults to `0`
	- `DebugVisualService:DrawRectangleOutline(size: UDim2, pos: UDim2, color: Color3?, transparency: number?, thickness: number?)`
		- `color` defaults to `Color3.new(0, 0, 0)`
		- `transparency` defaults to `0`
		- `thickness` defaults to `1`
- Modified functions (the optional parameter defaults are still the same as before)
	- `DebugVisualService:DrawText(text: string, pos: UDim2, color: Color3? fontSize: number?)`
	- `DebugVisualService:DrawLine2D(from: UDim2, to: UDim2, color: Color3?)`

# 2/24/2026

Signals are now fixed, heck yeah

<!--

Hello, yeah ChatGPT came up with the solution, and also correctly creating signals
on the C++ side, had to fiddle around with LuaScheduler initialization but it works!

-->

Parts now have their color show

Tried adding textures, they aren't tiled correctly (TODO: use a different texture)

Frames can now be rotated

Added `LuaRandom`, which internally uses the Mersenne Twister PRNG for random numbers
- Access from Luau using `Random`
- Current methods below, may be expanded
	- `Random:NextNumber(min: number, max: number)`
	- `Random:NextInteger(min: number, max: number)`
	- `Random:NextUnitVector()`

Added `DebugVisualService`, incomplete as of right now but allows for drawing simple visuals in an immediate-mode manner
- Access with `game:GetService("DebugVisualService")`
- Note that the methods below MUST be called every frame for the visuals to be consistent, using `RunService.PreRender` is the best way to ensure this as it always fires before anything is drawn
- Current methods below, may be expanded
	- `DebugVisualService:DrawText(text: string, x: number, y: number, fontSize: number?, color: Color3?)`
		- `fontSize` defaults to `20`
		- `color` defaults to `Color3.new(0, 0, 0)`
	- `DebugVisualService:DrawLine2D(from: Vector2, to: Vector2, color: Color3?)`
		- `color` defaults to `Color3.new(0, 0, 0)`
	- `DebugVisualService:DrawLine3D(from: Vector3, to: Vector3, color: Color3?)`
		- `color` defaults to `Color3.new(0, 0, 0)`

# 2/19/2026

Added History document (you're reading from it!) to keep track of changes I guess

The segmentation fault that occured in the `LuaScheduler.cpp` has been fixed, and new instance types added

<!--

Hello, I literally asked ChatGPT and Gemini both repeatedly and didn't get an answer that worked

I think the issue was from some insane crazy mysterious C++ dark magic with external variables that broke it, or just me being dumb

Whatever it's fixed, and I'll just work on other things

-->

* Objects
	* BaseScript
	* Script
	* LocalScript

`task.spawn` is removed, but will be added back later

Some work done on fixing Signals, but they're still broken

# 2/18/2026

Repository created, comes with a bunch of stuff right away:
* Objects
	* Parts
	* ScreenGuis
	* Frames
	* Folders
* Services
	* Game
	* Workspace
	* Lighting
	* RunService
	* StarterGui
* Libraries
	* Instance library
	* Task library (just `wait` and `spawn`)
	* Instance methods
