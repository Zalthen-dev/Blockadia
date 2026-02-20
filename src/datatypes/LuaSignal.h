#pragma once

#include <cstring>
#include <stdio.h>
#include <vector>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#define LUA_SIGNAL "LuaSignal"
#define LUA_CONNECTION "LuaConnection"

struct LuaConnection {
    lua_State* L;
    int funcRef; // reference to Lua function
    bool disconnected = false;

    ~LuaConnection() {
        if (!disconnected) {
            lua_unref(L, funcRef);
        }
    }

    void Disconnect() {
        if (!disconnected) {
            lua_unref(L, funcRef);
            disconnected = true;
        }
    }

    void Fire(int nargs = 0) {
        if (disconnected) return;
        lua_rawgeti(L, LUA_REGISTRYINDEX, funcRef);

		if (!lua_isfunction(L, -1)) {
        	printf("RUNTIME: Registry reference isn't a function\n");
        	lua_pop(L, 1);
        	return;
    	}

        if (nargs > 0) 
			lua_insert(L, -1 - nargs);

        if (lua_pcall(L, nargs, 0, 0) != LUA_OK) {
            const char* err = lua_tostring(L, -1);

			printf("RUNTIME: Lua error in Signal:Fire\n");
			printf("  Message: %s\n", err);

            lua_pop(L, 1);
        }
    }
};

struct LuaSignal {
    std::vector<LuaConnection*> connections;

    LuaConnection* Connect(lua_State* L, int funcIndex) {
        lua_pushvalue(L, funcIndex);
        int ref = lua_ref(L, funcIndex);
		lua_pop(L, 1);

        auto* conn = new LuaConnection{};
		conn->L = L;
		conn->funcRef = ref;

        connections.push_back(conn);
        return conn;
    }

    void Fire(int nargs = 0) {
        // copy to avoid invalidation if connections disconnect during fire
        auto copy = connections;
        for (auto* conn : copy) {
            conn->Fire(nargs);
        }
    }
};

static LuaConnection* CheckConnection(lua_State* L, int idx) {
	return *(LuaConnection**)luaL_checkudata(L, idx, LUA_CONNECTION);
}

static LuaSignal* CheckSignal(lua_State* L, int idx) {
	return *(LuaSignal**)luaL_checkudata(L, idx, LUA_SIGNAL);
}

static void PushSignal(lua_State* L, LuaSignal* sig) {
    LuaSignal** udata = (LuaSignal**)lua_newuserdata(L, sizeof(LuaSignal*));
    *udata = sig;

    luaL_getmetatable(L, LUA_SIGNAL);
    lua_setmetatable(L, -2);
}

static int l_Signal_Connect(lua_State* L) {
    LuaSignal* sig = CheckSignal(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    LuaConnection* conn = sig->Connect(L, 2);

    // push a userdata representing the connection
    LuaConnection** udata = (LuaConnection**)lua_newuserdata(L, sizeof(LuaConnection*));
    *udata = conn;

    luaL_getmetatable(L, LUA_CONNECTION);
    lua_setmetatable(L, -2);

    return 1;
}

static int l_Signal_index(lua_State* L) {
	LuaSignal* signal = CheckSignal(L, 1);
	const char* key = luaL_checkstring(L, 2);

	if (std::strcmp(key, "Connect") == 0) {
		lua_pushcfunction(L, l_Signal_Connect, "Connect");
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

static int l_Connection_Disconnect(lua_State* L) {
    LuaConnection* conn = CheckConnection(L, 1);
    conn->Disconnect();
    return 0;
}

static void RegisterSignal(lua_State* L) {
	luaL_newmetatable(L, LUA_SIGNAL);
	//lua_pushcfunction(L, l_Signal_Connect, "connect"); lua_setfield(L, -2, "Connect");
	lua_pushcfunction(L, l_Signal_index, "index"); lua_setfield(L, -2, "__index");
	lua_pop(L,1);

	luaL_newmetatable(L, LUA_CONNECTION);
	lua_pushcfunction(L, l_Connection_Disconnect, "disconnect"); lua_setfield(L, -2, "Disconnect");
	lua_pop(L,1);
}