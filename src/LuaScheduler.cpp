#include "LuaScheduler.h"

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <raylib.h>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include "objects/BaseScript.h"

extern LuaScheduler gLuaScheduler;

LuaThread* GetCurrentLuaThread(lua_State* L) {
    auto it = gLuaScheduler.threadMap.find(L);
    if (it == gLuaScheduler.threadMap.end()) {
        return nullptr;
    }

    return it->second;
}

LuaScheduler* GetScheduler(lua_State* L) {
    return &gLuaScheduler;
}

// UNUSED

LuaThread* CreateThread(LuaScheduler& sched, int funcIndex) {
    lua_State* L = sched.L;
	
	if (!lua_isfunction(L, funcIndex)) {
    	const char* typeName = lua_typename(L, lua_type(L, funcIndex));
		printf("RUNTIME: Failed to create thread, see info below\n");
		printf(" Message: Expected 'function', got '%s'\n", typeName);
    	return nullptr;
	}

    lua_State* thread = lua_newthread(L);
	luaL_sandboxthread(thread);

	int ref = lua_ref(L, -1);
	lua_pop(L, 1);

    lua_pushvalue(L, funcIndex);
    lua_xmove(L, thread, 1);

    auto* t = new LuaThread{};
    t->thread = thread;
    t->threadRef = ref;
    t->scheduler = &sched;

	sched.threads.push_back(t);
	sched.threadMap[thread] = t;

    return t;
}

void StartScript(LuaScheduler& sched, BaseScript* script) {
	lua_State* L = sched.L;

	lua_State* thread = lua_newthread(L);
	int ref = lua_ref(L, -1);
	lua_pop(L, 1);

	luaL_sandboxthread(thread);

	lua_CompileOptions opts{};

	size_t bytecodeSize = 0;
	char* bytecode = luau_compile(script->Source.data(), script->Source.size(), &opts, &bytecodeSize);
	if (!bytecode || bytecodeSize == 0) {
		printf("RUNTIME: Failed to compile bytecode for script %s\n", script->Name.data());
		if (bytecode) free(bytecode);
		return;
	}

	const std::string chunkName = "@" + script->Name;
	if (luau_load(thread, chunkName.c_str(), bytecode, bytecodeSize, 0) != 0) {
		printf("RUNTIME: Failed to load bytecode for script %s\n", script->Name.data());
		printf("  Message: %s\n", lua_tostring(thread, -1));
		lua_pop(thread, 1);
		free(bytecode);
		return;
	}

	free(bytecode);

	auto* t = new LuaThread{};
    t->thread = thread;
	t->threadRef = ref;
    t->scheduler = &sched;

	sched.threads.push_back(t);
	sched.threadMap[thread] = t;
}

static int l_task_wait(lua_State* L) {
    double duration = luaL_optnumber(L, 1, 0.0);

    LuaThread* self = GetCurrentLuaThread(L);
	if (!self) {
		luaL_error(L, "task.wait must be called from a coroutine");
        return 0;
    }

	if (!lua_isyieldable(self->thread)) {
		luaL_error(L, "coroutine is not yieldable");
        return 0;
	}

    self->yieldTime = GetTime();
    self->wakeTime = self->yieldTime + duration;
	self->expectsReturn = true;
    self->waiting = true;

    return lua_yield(L, 0);
}

// TODO: reimplement task.spawn
/*
static int l_task_spawn(lua_State* L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);

    LuaScheduler* sched = GetScheduler(L);
	if (!sched) {
		luaL_error(L, "task.spawn must be called from a coroutine");
		return 0;
	}

    lua_pushvalue(L, 1);

    int funcIndex = lua_gettop(L);
    StartScript(*sched, funcIndex);

    lua_pop(L, 1);
    return 0;
}
*/

void RegisterTaskAPI(lua_State* L) {
	lua_newtable(L);
	lua_pushcfunction(L, l_task_wait, "task.wait"); lua_setfield(L, -2, "wait");
	//lua_pushcfunction(L, l_task_spawn, "task.spawn"); lua_setfield(L, -2, "spawn");
	lua_setglobal(L, "task");

	// backwards compatibility
	lua_pushcfunction(L, l_task_wait, "task.wait"); lua_setglobal(L, "wait");
}

void LuaScheduler::Step() {
    double now = GetTime();

	lua_gc(L, LUA_GCSTEP, 200);

	for (auto it = threads.begin(); it != threads.end(); ) {
		LuaThread* t = *it;

		if (t->waiting) {
			if (now < t->wakeTime) {
				it++;
				continue;
			}

			t->waiting = false;
		}
	
		int nargs = 0;
		if (t->expectsReturn) {
			t->expectsReturn = false;

			double elapsed = now - t->yieldTime;
			lua_pushnumber(t->thread, elapsed);
			nargs = 1;
		}

		int status = lua_resume(t->thread, nullptr, nargs); // SEGFAULT OCCURS HERE

		if (status == LUA_YIELD) {
			++it;
		} else if (status == LUA_OK) {
			lua_unref(L, t->threadRef);

			threadMap.erase(t->thread);
			delete t;
			it = threads.erase(it);
		} else {
			const char* err = lua_tostring(t->thread, -1);
			printf("RUNTIME: Lua thread has encountered an error, see info below\n");
			printf("  Status : %i\n", status);
			printf("  Message: '%s'\n", err);

			lua_unref(L, t->threadRef);
			lua_pop(t->thread, 1);
			
			threadMap.erase(t->thread);
			delete t;
			it = threads.erase(it);
		}
	}
}
