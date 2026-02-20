#pragma once

#include <vector>
#include <unordered_map>

#include "objects/BaseScript.h"

#include "lua.h"

struct LuaScheduler; // forward def
struct LuaThread {
	lua_State* thread;
	int threadRef;

	double wakeTime{0.0};
	double yieldTime{0.0};
	bool expectsReturn = false;
	bool waiting = false;

	LuaScheduler* scheduler;
};

struct LuaScheduler {
	lua_State* L;
	std::vector<LuaThread*> threads;

	std::unordered_map<lua_State*, LuaThread*> threadMap;

	void Step();
};

extern LuaScheduler gLuaScheduler;

// lua handling

LuaThread* CreateThread(lua_State* L);
void StartScript(LuaScheduler& sched, BaseScript* script);

// lua functions

static int l_task_wait(lua_State* L);
static int l_task_spawn(lua_State* L);

void RegisterTaskAPI(lua_State* L);