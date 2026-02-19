#pragma once

#include <vector>
#include <unordered_map>

#include "lua.h"

struct LuaScheduler; // forward def
struct LuaThread {
	lua_State* thread;
	int threadRef;

	double wakeTime{0.0};
	double yieldTime{0.0};
	bool waiting = false;

	LuaScheduler* scheduler;
};

struct LuaScheduler {
	lua_State* L;
	std::vector<LuaThread*> threads;

	std::unordered_map<lua_State*, LuaThread*> threadMap;

	void Step();
};

// lua handling

LuaThread* CreateThread(lua_State* L);
void StartScript(LuaScheduler& sched, int funcIndex);

// lua functions

static int l_task_wait(lua_State* L);
static int l_task_spawn(lua_State* L);

void RegisterTaskAPI(lua_State* L);