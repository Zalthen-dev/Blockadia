#pragma once

#include <cstring>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include "utils/ExtraMath.h"

#define LUA_COLOR3 "LuaColor3"

struct LuaColor3 {
    float r;
    float g;
    float b;
};

static LuaColor3* CheckColor3(lua_State* L, int idx) {
    return (LuaColor3*)luaL_checkudata(L, idx, LUA_COLOR3);
}

static void PushColor3(lua_State* L, float r, float g, float b) {
    LuaColor3* c = (LuaColor3*)lua_newuserdata(L, sizeof(LuaColor3));
    c->r = r; 
	c->g = g; 
	c->b = b;

    luaL_getmetatable(L, LUA_COLOR3);
    lua_setmetatable(L, -2);
}

static int l_Color3_Lerp(lua_State* L) {
	auto* a = CheckColor3(L, 1);
    auto* b = CheckColor3(L, 2);
	float t = luaL_checknumber(L, 3);

	PushColor3(L, 
		lerpf(a->r, b->r, t),
		lerpf(a->g, b->g, t), 
		lerpf(a->b, b->b, t)
	);
	
	return 1;
}

static int l_Color3_index(lua_State* L) {
    auto* c = CheckColor3(L, 1);
    const char* key = luaL_checkstring(L, 2);

	if (std::strcmp(key, "Lerp") == 0) {
		lua_pushcfunction(L, l_Color3_Lerp, "Lerp");
		return 1;
	}

    if (std::strcmp(key, "R") == 0) { lua_pushnumber(L, c->r); return 1; }
    if (std::strcmp(key, "G") == 0) { lua_pushnumber(L, c->g); return 1; }
    if (std::strcmp(key, "B") == 0) { lua_pushnumber(L, c->b); return 1; }

    return 0;
}

static int l_Color3_add(lua_State* L) {
    auto* a = CheckColor3(L, 1);
    auto* b = CheckColor3(L, 2);
    PushColor3(L, a->r+b->r, a->g+b->g, a->b+b->b);
    return 1;
}

static int l_Color3_sub(lua_State* L) {
    auto* a = CheckColor3(L, 1);
    auto* b = CheckColor3(L, 2);
    PushColor3(L, a->r-b->r, a->g-b->g, a->b-b->b);
    return 1;
}

static int l_Color3_new(lua_State* L) {
    float r = luaL_checknumber(L,1);
    float g = luaL_checknumber(L,2);
    float b = luaL_checknumber(L,3);
    PushColor3(L,r,g,b);
    return 1;
}

static int l_Color3_fromRGB(lua_State* L) {
	float r = luaL_checknumber(L,1);
    float g = luaL_checknumber(L,2);
    float b = luaL_checknumber(L,3);
    PushColor3(L,r/255,g/255,b/255);
    return 1;
}

static void RegisterColor3(lua_State* L) {
    luaL_newmetatable(L, LUA_COLOR3);

    lua_pushcfunction(L, l_Color3_index, "index"); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_Color3_add, "add");     lua_setfield(L, -2, "__add");
    lua_pushcfunction(L, l_Color3_sub, "sub");     lua_setfield(L, -2, "__sub");

    lua_pop(L,1);

	lua_newtable(L); // Color3

    lua_pushcfunction(L, l_Color3_new, "new"); lua_setfield(L, -2, "new");
    lua_pushcfunction(L, l_Color3_fromRGB, "fromRGB"); lua_setfield(L, -2, "fromRGB");

    lua_setglobal(L, "Color3");
}
