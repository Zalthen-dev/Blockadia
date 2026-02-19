> [!WARNING]
>
> Blockadia is broken at the moment, and I don't have any idea on how to fix it
> 
> It has to do with the LuaScheduler, specifically it segfaults on the first `lua_resume` call wherever it may be
>
> If you have some knowledge on using Luau with C++, please help out!

# Blockadia

My 500th unfinished game engine project I'm probably not working on, but hey you can execute one single Luau script

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