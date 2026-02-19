#include "LuaScheduler.h"

#include <cstdio>
#include <raylib.h>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

LuaThread* GetCurrentLuaThread(lua_State* L) {
    extern LuaScheduler* gLuaScheduler;

    auto it = gLuaScheduler->threadMap.find(L);
    if (it == gLuaScheduler->threadMap.end()) {
        return nullptr;
    }

    return it->second;
}

LuaScheduler* GetScheduler(lua_State* L) {
    extern LuaScheduler* gLuaScheduler;
    return gLuaScheduler;
}

LuaThread* CreateThread(LuaScheduler& sched, int funcIndex) {
    lua_State* L = sched.L;
	
	if (!lua_isfunction(L, funcIndex)) {
    	const char* typeName = lua_typename(L, lua_type(L, funcIndex));
		printf("RUNTIME: Failed to create thread, see info below\n");
		printf(" Message: Expected 'function', got '%s'\n", typeName);
    	return nullptr;
	}

    lua_State* thread = lua_newthread(L);
    
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

void StartScript(LuaScheduler& sched, int funcIndex) {
	if (!lua_isfunction(sched.L, funcIndex)) {
    	const char* typeName = lua_typename(sched.L, lua_type(sched.L, funcIndex));
		printf("RUNTIME: Failed to create thread, see info below\n");
		printf(" Message: Expected 'function', got '%s'\n", typeName);
    	return;
	}

	LuaThread* t = CreateThread(sched, funcIndex);

	/*
	int status = lua_resume(t->thread, nullptr, 0);

	if (status != LUA_OK && status != LUA_YIELD) {
		const char* err = lua_tostring(t->thread, -1);
		
		printf("RUNTIME: Lua thread has encountered an error, see info below\n");
		printf(" Status : %i\n", status);
		printf(" Message: '%s'\n", err);

		lua_pop(t->thread, 1);
	}
	*/
}

static int l_task_wait(lua_State* L) {
    double duration = luaL_optnumber(L, 1, 0.0);

    LuaThread* self = GetCurrentLuaThread(L);
	if (!self) {
		luaL_error(L, "task.wait must be called from a coroutine");
        return 0;
    }

    self->yieldTime = GetTime();
    self->wakeTime = self->yieldTime + duration;
    self->waiting = true;

    return lua_yield(L, 0);
}

static int l_task_spawn(lua_State* L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);

    LuaScheduler* sched = GetScheduler(L);
	if (!sched) {
		luaL_error(L, "task.spawn must be called from a coroutine");
		return 0;
	}

    // push function onto main state stack
    lua_pushvalue(L, 1);

    int funcIndex = lua_gettop(L);
    StartScript(*sched, funcIndex);

    lua_pop(L, 1);
    return 0;
}

void RegisterTaskAPI(lua_State* L) {
	lua_newtable(L);
	lua_pushcfunction(L, l_task_wait, "task.wait"); lua_setfield(L, -2, "wait");
	lua_pushcfunction(L, l_task_spawn, "task.spawn"); lua_setfield(L, -2, "spawn");
	lua_setglobal(L, "task");
}

void LuaScheduler::Step() {
    double now = GetTime();

	for (auto it = threads.begin(); it != threads.end(); ) {
		LuaThread* t = *it;
	
		int nargs = 0;
	
		if (t->waiting) {
			if (now < t->wakeTime) {
				++it;
				continue;
			}
		
			t->waiting = false;
			double elapsed = now - t->yieldTime;
			lua_pushnumber(t->thread, elapsed);
			nargs = 1;
		}
	
		int status = lua_resume(t->thread, nullptr, nargs);
	
		if (status == LUA_YIELD) {
			++it;
			continue;
		} else if (status == LUA_OK) {
			lua_unref(L, t->threadRef);
			threadMap.erase(t->thread);
			
			delete t;
			it = threads.erase(it);
			continue;
		} else {
			const char* err = lua_tostring(t->thread, -1);
			printf("RUNTIME: Lua thread has encountered an error, see info below\n");
			printf(" Status : %i\n", status);
			printf(" Message: '%s'\n", err);
			lua_pop(t->thread, 1);
			
			lua_unref(L, t->threadRef);
			threadMap.erase(t->thread);
			
			delete t;
			it = threads.erase(it);
			continue;
		}
	}
}
