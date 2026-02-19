#pragma once

#include <cstring>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

// both UDim's are included here cuz why not (cuz chatgpt said so)

#define LUA_UDIM  "LuaUDim"
#define LUA_UDIM2 "LuaUDim2"

struct LuaUDim {
    float scale;
    float offset;
};

struct LuaUDim2 {
    LuaUDim x;
    LuaUDim y;
};

// Helper: UDim

static LuaUDim* CheckUDim(lua_State* L, int idx) {
    return (LuaUDim*)luaL_checkudata(L, idx, LUA_UDIM);
}

static void PushUDim(lua_State* L, float scale, float offset) {
    LuaUDim* u = (LuaUDim*)lua_newuserdata(L, sizeof(LuaUDim));
    u->scale = scale;
    u->offset = offset;

    luaL_getmetatable(L, LUA_UDIM);
    lua_setmetatable(L, -2);
}

// Helper: UDim2

static LuaUDim2* CheckUDim2(lua_State* L, int idx) {
    return (LuaUDim2*)luaL_checkudata(L, idx, LUA_UDIM2);
}

static void PushUDim2(lua_State* L, LuaUDim x, LuaUDim y) {
    LuaUDim2* u = (LuaUDim2*)lua_newuserdata(L, sizeof(LuaUDim2));
    u->x = x;
    u->y = y;

    luaL_getmetatable(L, LUA_UDIM2);
    lua_setmetatable(L, -2);
}

// UDim stuff

static int l_UDim_add(lua_State* L) {
    auto* a = CheckUDim(L, 1);
    auto* b = CheckUDim(L, 2);
    PushUDim(L, a->scale + b->scale, a->offset + b->offset);
    return 1;
}

static int l_UDim_sub(lua_State* L) {
    auto* a = CheckUDim(L, 1);
    auto* b = CheckUDim(L, 2);
    PushUDim(L, a->scale - b->scale, a->offset - b->offset);
    return 1;
}

static int l_UDim_index(lua_State* L) {
    auto* u = CheckUDim(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (std::strcmp(key, "Scale") == 0)  { lua_pushnumber(L, u->scale);  return 1; }
    if (std::strcmp(key, "Offset") == 0) { lua_pushnumber(L, u->offset); return 1; }

    lua_pushnil(L);
    return 1;
}

static int l_UDim_tostring(lua_State* L) {
	auto* u = CheckUDim(L, 1);
	lua_pushfstring(L, "UDim(%f, %f)", u->scale, u->offset);
	return 1;
}

static int l_UDim_new(lua_State* L) {
    float scale  = luaL_checknumber(L, 1);
    float offset = luaL_checknumber(L, 2);
    PushUDim(L, scale, offset);
    return 1;
}

// UDim2 stuff

static int l_UDim2_add(lua_State* L) {
    auto* a = CheckUDim2(L, 1);
    auto* b = CheckUDim2(L, 2);

    PushUDim2(L,
        { a->x.scale + b->x.scale, a->x.offset + b->x.offset },
        { a->y.scale + b->y.scale, a->y.offset + b->y.offset }
    );
    return 1;
}

static int l_UDim2_sub(lua_State* L) {
    auto* a = CheckUDim2(L, 1);
    auto* b = CheckUDim2(L, 2);

    PushUDim2(L,
        { a->x.scale - b->x.scale, a->x.offset - b->x.offset },
        { a->y.scale - b->y.scale, a->y.offset - b->y.offset }
    );
    return 1;
}

static int l_UDim2_Lerp(lua_State* L) {
    auto* a = CheckUDim2(L, 1);
    auto* b = CheckUDim2(L, 2);
    float t = luaL_checknumber(L, 3);

    auto lerp = [&](float x, float y) {
        return x + (y - x) * t;
    };

    PushUDim2(L,
        { lerp(a->x.scale, b->x.scale), lerp(a->x.offset, b->x.offset) },
        { lerp(a->y.scale, b->y.scale), lerp(a->y.offset, b->y.offset) }
    );
    return 1;
}

static int l_UDim2_tostring(lua_State* L) {
	auto* u = CheckUDim2(L, 1);
	lua_pushfstring(L, "UDim2(%f, %f, %f, %f)", u->x.scale, u->x.offset, u->y.scale, u->y.offset);
	return 1;
}

static int l_UDim2_index(lua_State* L) {
    auto* u = CheckUDim2(L, 1);
    const char* key = luaL_checkstring(L, 2);

	if (std::strcmp(key, "Lerp") == 0) {
		lua_pushcfunction(L, l_UDim2_Lerp, "Lerp");
		return 1;
	}

    if (std::strcmp(key, "X") == 0 || std::strcmp(key, "Width") == 0) {
        PushUDim(L, u->x.scale, u->x.offset);
        return 1;
    }

    if (std::strcmp(key, "Y") == 0 || std::strcmp(key, "Height") == 0) {
        PushUDim(L, u->y.scale, u->y.offset);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

static int l_UDim2_new(lua_State* L) {
    PushUDim2(L,
        { (float)luaL_checknumber(L,1), (float)luaL_checknumber(L,2) },
        { (float)luaL_checknumber(L,3), (float)luaL_checknumber(L,4) }
    );
    return 1;
}

static int l_UDim2_fromScale(lua_State* L) {
    float x = luaL_checknumber(L,1);
    float y = luaL_checknumber(L,2);
    PushUDim2(L, {x,0}, {y,0});
    return 1;
}

static int l_UDim2_fromOffset(lua_State* L) {
    float x = luaL_checknumber(L,1);
    float y = luaL_checknumber(L,2);
    PushUDim2(L, {0,x}, {0,y});
    return 1;
}



static void RegisterUDim(lua_State* L) {
    luaL_newmetatable(L, LUA_UDIM);

    lua_pushcfunction(L, l_UDim_index, "index"); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_UDim_add, "add"); lua_setfield(L, -2, "__add");
    lua_pushcfunction(L, l_UDim_sub, "sub"); lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, l_UDim_tostring, "tostring"); lua_setfield(L, -2, "__tostring");

    lua_pop(L, 1);

	lua_newtable(L); // UDim
    lua_pushcfunction(L, l_UDim_new, "new"); lua_setfield(L, -2, "new");
    lua_setglobal(L, "UDim");
}

static void RegisterUDim2(lua_State* L) {
	luaL_newmetatable(L, LUA_UDIM2);

    lua_pushcfunction(L, l_UDim2_index, "index"); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_UDim2_add, "add"); lua_setfield(L, -2, "__add");
    lua_pushcfunction(L, l_UDim2_sub, "sub"); lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, l_UDim2_tostring, "tostring"); lua_setfield(L, -2, "__tostring");

    lua_pop(L, 1);

	lua_newtable(L);

    lua_pushcfunction(L, l_UDim2_new, "new"); lua_setfield(L, -2, "new");
    lua_pushcfunction(L, l_UDim2_fromScale, "fromScale"); lua_setfield(L, -2, "fromScale");
    lua_pushcfunction(L, l_UDim2_fromOffset, "fromOffset"); lua_setfield(L, -2, "fromOffset");

    lua_setglobal(L, "UDim2");
}