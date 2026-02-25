#pragma once

#include <cassert>
#include <cstdio>
#include <functional>
#include <memory>
#include <vector>
#include <cstring>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#define LUA_SIGNAL "LuaSignal"
#define LUA_CONNECTION "LuaConnection"

// --- LuaSignal object ---
struct LuaSignal {
    struct Listener {
        int callbackRef = LUA_NOREF;
        bool connected = true;
        bool once = false;
    };

    lua_State* Lm;
    std::vector<Listener> listeners;

    explicit LuaSignal(lua_State* mainState) : Lm(mainState) {}
    ~LuaSignal() {
        for (auto& l : listeners) {
            if (l.callbackRef != LUA_NOREF) {
                lua_unref(Lm, l.callbackRef);
            }
        }
    }

    void Fire(lua_State* src, int firstArgIdx, int argc) {
        // Copy listeners to avoid modification during iteration
        auto snapshot = listeners;
        for (auto& l : snapshot) {
            if (!l.connected || l.callbackRef == LUA_NOREF) continue;

            lua_rawgeti(Lm, LUA_REGISTRYINDEX, l.callbackRef);

            for (int i = 0; i < argc; ++i) {
                lua_pushvalue(src, firstArgIdx + i);
                lua_xmove(src, Lm, 1);
            }

            if (lua_pcall(Lm, argc, 0, 0) != LUA_OK) {
                const char* err = lua_tostring(Lm, -1);
                printf("Signal Error: %s\n", err);
                lua_pop(Lm, 1);
            }

            if (l.once) {
                Disconnect(l.callbackRef);
            }
        }
    }

    void Disconnect(int ref) {
        for (auto& l : listeners) {
            if (l.callbackRef == ref) {
                if (l.connected) {
                    l.connected = false;
                    lua_unref(Lm, l.callbackRef);
                    l.callbackRef = LUA_NOREF;
                }
                break;
            }
        }
    }

	bool luaSide{false};
};

struct LuaSignalUD {
    std::shared_ptr<LuaSignal> sig;
};

inline LuaSignalUD* CheckSignal(lua_State* L, int idx) {
    return static_cast<LuaSignalUD*>(luaL_checkudata(L, idx, LUA_SIGNAL));
}

inline void PushSignal(lua_State* L, std::shared_ptr<LuaSignal> sig) {
    void* mem = lua_newuserdata(L, sizeof(LuaSignalUD));
    new (mem) LuaSignalUD{ sig };
    luaL_getmetatable(L, LUA_SIGNAL);
    lua_setmetatable(L, -2);
}

static int l_Signal_gc(lua_State* L) {
    auto* ud = CheckSignal(L, 1);
    if (ud) ud->~LuaSignalUD();
    return 0;
}

static int l_Signal_Connect(lua_State* L) {
    auto* ud = CheckSignal(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    lua_pushvalue(L, 2);
    lua_xmove(L, ud->sig->Lm, 1);
    int ref = lua_ref(ud->sig->Lm, -1);

    ud->sig->listeners.push_back({
        .callbackRef = ref,
        .connected = true,
        .once = false
    });

    void* mem = lua_newuserdata(L, sizeof(int));
    new (mem) int(ref);
    luaL_getmetatable(L, LUA_CONNECTION);
    lua_setmetatable(L, -2);

    return 1;
}

static int l_Signal_Fire(lua_State* L) {
    auto* ud = CheckSignal(L, 1);
    int argc = lua_gettop(L) - 1;
    ud->sig->Fire(L, 2, argc);
    return 0;
}

static int l_Signal_index(lua_State* L) {
    auto* ud = CheckSignal(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (std::strcmp(key, "Connect") == 0) {
        lua_pushcfunction(L, l_Signal_Connect, "Signal:Connect");
        return 1;
    }
    if (std::strcmp(key, "Fire") == 0) {
        lua_pushcfunction(L, l_Signal_Fire, "Signal:Fire");
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

static int l_Signal_new(lua_State* L) {
    auto sig = std::make_shared<LuaSignal>(L);
	sig->luaSide = true;

    void* mem = lua_newuserdata(L, sizeof(LuaSignalUD));
    new (mem) LuaSignalUD{ sig };

    luaL_getmetatable(L, LUA_SIGNAL);
    lua_setmetatable(L, -2);
    return 1;
}

inline void FireSignal(const std::shared_ptr<LuaSignal>& sig, std::function<void(lua_State*)> pushArgs) {
    lua_State* L = sig->Lm;
    int base = lua_gettop(L);

    pushArgs(L);

    int argc = lua_gettop(L) - base;
    sig->Fire(L, base + 1, argc);

    lua_settop(L, base);
}

// --- register ---
static void RegisterSignal(lua_State* L) {
    // Signal metatable
    luaL_newmetatable(L, LUA_SIGNAL);
    lua_pushcfunction(L, l_Signal_index, "__index"); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_Signal_gc, "__gc"); lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);

    // Signal constructor
    lua_newtable(L);
    lua_pushcfunction(L, l_Signal_new, "new"); lua_setfield(L, -2, "new");
    lua_setglobal(L, "Signal");
}