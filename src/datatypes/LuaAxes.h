#pragma once

#include <raylib.h>
#include "raymath.h"

#include <cstring>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#define LUA_AXES "LuaAxes"

struct LuaAxes {
	bool x, y, z;
	bool top, bottom, left, right, back, front;
};

static LuaAxes* CheckAxes(lua_State* L, int idx) {
	return static_cast<LuaAxes*>(luaL_checkudata(L, idx, LUA_AXES));
}

static void PushAxes(lua_State* L, bool x, bool y, bool z, bool top, bool bottom, bool left, bool right, bool back, bool front) {
	LuaAxes* c = (LuaAxes*)lua_newuserdata(L, sizeof(LuaAxes));

	luaL_getmetatable(L, LUA_AXES);
	lua_setmetatable(L, -2);
}

static void PushAxes(lua_State* L, bool x, bool y, bool z) {
	PushAxes(L, x, y, z, false, false, false, false, false, false);
}

static void PushAxes(lua_State* L, bool top, bool bottom, bool left, bool right, bool back, bool front) {
	PushAxes(L, false, false, false, top, bottom, left, right, back, front);
}

static void PushAxes(lua_State* L) {
	PushAxes(L, false, false, false, false, false, false, false, false, false);
}

static int l_Axes_index(lua_State* L) {
	LuaAxes* axes = CheckAxes(L, 1);
	const char* key = luaL_checkstring(L, 2);
	
	#define CheckKeyBoolean(k, v) if (std::strcmp(key, k) == 0) { lua_pushboolean(L, v); return 1; }

	CheckKeyBoolean("X", axes->x);
	CheckKeyBoolean("Y", axes->y);
	CheckKeyBoolean("Z", axes->z);
	CheckKeyBoolean("Top",    axes->top);
	CheckKeyBoolean("Bottom", axes->bottom);
	CheckKeyBoolean("Left",   axes->left);
	CheckKeyBoolean("Right",  axes->right);
	CheckKeyBoolean("Back",   axes->back);
	CheckKeyBoolean("Front",  axes->front);

	#undef CheckKeyBoolean

	lua_pushnil(L);
	return 1;
}

static int l_Axes_new(lua_State* L) {
	// TODO: when Enums are added, make this support Enum.Axes and Enum.NormalId

	PushAxes(L);
	return 1;
}

static void RegisterAxes(lua_State* L) {
	luaL_newmetatable(L, LUA_AXES);
		lua_pushcfunction(L, l_Axes_index, "Axes.__index"); lua_setfield(L, -2, "__index");
	lua_pop(L,1);

	lua_newtable(L);
		lua_pushcfunction(L, l_Axes_new, "Axes.new"); lua_setfield(L, -2, "new");
	lua_setglobal(L, "Axes");
}