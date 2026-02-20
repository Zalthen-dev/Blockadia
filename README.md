> [!IMPORTANT]
>
> As of 2/19/2026, the Segmentation Fault issue involving the LuaScheduler has been fixed!
>
> Yeah I'm kinda shocked messing around with the declaration stuff got it to work

# Blockadia

My 500th unfinished game engine project I'm probably not working on, but hey you can execute one single Luau script

## API

Blockadia has a pretty limited API right now but you can still do plenty of things!

A lot of would-be common methods aren't supported, but expect them to be supported in the future!

## Build instructions

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