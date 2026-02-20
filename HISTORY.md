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
