#pragma once

#include <cmath>
#include <cstring>
#include <raylib.h>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include "utils/ExtraMath.h"

#define LUA_VECTOR2 "LuaVector2"

struct LuaVector2 {
    float x, y;
};

static LuaVector2* CheckVector2(lua_State* L, int idx) {
    return (LuaVector2*)luaL_checkudata(L, idx, LUA_VECTOR2);
}

static int PushVector2(lua_State* L, float x, float y) {
	auto* v = (LuaVector2*)lua_newuserdata(L, sizeof(LuaVector2));
	v->x = x;
	v->y = y;
	luaL_getmetatable(L, LUA_VECTOR2);
	lua_setmetatable(L, -2);
	return 1;
}

static void PushVector2Const(lua_State* L, float x, float y) {
    PushVector2(L, x, y);
}

static int l_Vector2_new(lua_State* L) {
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);

    auto* v = (LuaVector2*)lua_newuserdata(L, sizeof(LuaVector2));
    v->x = x;
    v->y = y;

    luaL_getmetatable(L, LUA_VECTOR2);
    lua_setmetatable(L, -2);

    return 1;
}

static int l_Vector2_Abs(lua_State* L) {
    auto* v = CheckVector2(L, 1);
    return PushVector2(L, fabsf(v->x), fabsf(v->y));
}

static int l_Vector2_Ceil(lua_State* L) {
    auto* v = CheckVector2(L, 1);
    return PushVector2(L, ceilf(v->x), ceilf(v->y));
}

static int l_Vector2_Floor(lua_State* L) {
    auto* v = CheckVector2(L, 1);
    return PushVector2(L, floorf(v->x), floorf(v->y));
}

static int l_Vector2_Sign(lua_State* L) {
    auto* v = CheckVector2(L, 1);
    return PushVector2(L, signf(v->x), signf(v->y));
}

static int l_Vector2_Dot(lua_State* L) {
    auto* a = CheckVector2(L, 1);
    auto* b = CheckVector2(L, 2);

    lua_pushnumber(L, a->x*b->x + a->y*b->y);
    return 1;
}

static int l_Vector2_Cross(lua_State* L) {
    auto* a = CheckVector2(L, 1);
    auto* b = CheckVector2(L, 2);

	lua_pushnumber(L, (a->x*b->y - a->y*b->x));
    return 1; 
}

static int l_Vector2_Angle(lua_State* L) {
    auto* a = CheckVector2(L, 1);
    auto* b = CheckVector2(L, 2);

    float dot = a->x*b->x + a->y*b->y;
    float mag = sqrtf((a->x*a->x + a->y*a->y) * (b->x*b->x + b->y*b->y));

    if (mag <= 1e-6f) {
        lua_pushnumber(L, 0.0);
    } else {
        lua_pushnumber(L, acosf(dot / mag));
    }
    return 1;
}


static int l_Vector2_FuzzyEq(lua_State* L) {
    auto* a = CheckVector2(L, 1);
    auto* b = CheckVector2(L, 2);
    float eps = (float)luaL_optnumber(L, 3, 1e-5f);

    bool eq = fabsf(a->x - b->x) <= eps && fabsf(a->y - b->y) <= eps;

    lua_pushboolean(L, eq);
    return 1;
}

static int l_Vector2_Lerp(lua_State* L) {
    auto* a = CheckVector2(L, 1);
    auto* b = CheckVector2(L, 2);
    float t = (float)luaL_checknumber(L, 3);

    return PushVector2(L, a->x + (b->x - a->x) * t, a->y + (b->y - a->y) * t);
}

static int l_Vector2_Max(lua_State* L) {
    auto* a = CheckVector2(L, 1);
    auto* b = CheckVector2(L, 2);

    return PushVector2(L, fmaxf(a->x, b->x), fmaxf(a->y, b->y));
}

static int l_Vector2_Min(lua_State* L) {
    auto* a = CheckVector2(L, 1);
    auto* b = CheckVector2(L, 2);

    return PushVector2(L, fminf(a->x, b->x), fminf(a->y, b->y));
}

static int l_Vector2_index(lua_State* L) {
    auto* v = (LuaVector2*)luaL_checkudata(L, 1, LUA_VECTOR2);
    const char* key = luaL_checkstring(L, 2);

	if (!strcmp(key, "Abs"))      { lua_pushcfunction(L, l_Vector2_Abs, "Abs"); return 1; }
    if (!strcmp(key, "Ceil"))     { lua_pushcfunction(L, l_Vector2_Ceil, "Ceil"); return 1; }
    if (!strcmp(key, "Floor"))    { lua_pushcfunction(L, l_Vector2_Floor, "Floor"); return 1; }
    if (!strcmp(key, "Sign"))     { lua_pushcfunction(L, l_Vector2_Sign, "Sign"); return 1; }
    if (!strcmp(key, "Cross"))    { lua_pushcfunction(L, l_Vector2_Cross, "Cross"); return 1; }
    if (!strcmp(key, "Angle"))    { lua_pushcfunction(L, l_Vector2_Angle, ""); return 1; }
    if (!strcmp(key, "Dot"))      { lua_pushcfunction(L, l_Vector2_Dot, "Dot"); return 1; }
    if (!strcmp(key, "FuzzyEq"))  { lua_pushcfunction(L, l_Vector2_FuzzyEq, "FuzzyEq"); return 1; }
    if (!strcmp(key, "Lerp"))     { lua_pushcfunction(L, l_Vector2_Lerp, "Lerp"); return 1; }
    if (!strcmp(key, "Max"))      { lua_pushcfunction(L, l_Vector2_Max, "Max"); return 1; }
    if (!strcmp(key, "Min"))      { lua_pushcfunction(L, l_Vector2_Min, "Min"); return 1; }


    if (std::strcmp(key, "X") == 0) { lua_pushnumber(L, v->x); return 1; }
    if (std::strcmp(key, "Y") == 0) { lua_pushnumber(L, v->y); return 1; }

	if (std::strcmp(key, "Magnitude") == 0) {
		lua_pushnumber(L, sqrt(v->x*v->x + v->y*v->y));
		return 1;
	}

	if (std::strcmp(key, "Unit") == 0) {
	    float mag = sqrtf(v->x*v->x + v->y*v->y);

	    // Create new Vector2 userdata
	    auto* u = (LuaVector2*)lua_newuserdata(L, sizeof(LuaVector2));

	    if (mag > 0.0f) {
	        u->x = v->x / mag;
	        u->y = v->y / mag;
	    } else {
	        u->x = 0.0f;
	        u->y = 0.0f;
	    }

	    luaL_getmetatable(L, LUA_VECTOR2);
	    lua_setmetatable(L, -2);

	    return 1;
	}

    lua_pushnil(L);
    return 1;
}

static int l_Vector2_newindex(lua_State* L) {
	luaL_error(L, "Vector2 is immutable");
    return 0;
}

static int l_Vector2_eq(lua_State* L) {
    auto* a = (LuaVector2*)luaL_checkudata(L, 1, LUA_VECTOR2);
    auto* b = (LuaVector2*)luaL_checkudata(L, 2, LUA_VECTOR2);

    lua_pushboolean(L, a->x == b->x && a->y == b->y);
    return 1;
}

static int l_Vector2_tostring(lua_State* L) {
    auto* v = (LuaVector2*)luaL_checkudata(L, 1, LUA_VECTOR2);
    lua_pushfstring(L, "Vector2(%f, %f)", v->x, v->y);
    return 1;
}

static int l_Vector2_add(lua_State* L) {
    auto* a = CheckVector2(L, 1);
    auto* b = CheckVector2(L, 2);

    return PushVector2(L, a->x + b->x, a->y + b->y);
}

static int l_Vector2_sub(lua_State* L) {
    auto* a = CheckVector2(L, 1);
    auto* b = CheckVector2(L, 2);

    return PushVector2(L, a->x - b->x, a->y - b->y);
}

static int l_Vector2_mul(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        float s = (float)lua_tonumber(L, 1);
        auto* v = CheckVector2(L, 2);
        return PushVector2(L, v->x*s, v->y*s);
    }

    if (lua_isnumber(L, 2)) {
        auto* v = CheckVector2(L, 1);
        float s = (float)lua_tonumber(L, 2);
        return PushVector2(L, v->x*s, v->y*s);
    }

	luaL_error(L, "Vector2 * Vector2 is not supported");
    return 0; 
}

static int l_Vector2_div(lua_State* L) {
    auto* v = CheckVector2(L, 1);
    float s = (float)luaL_checknumber(L, 2);

    if (s == 0.0f) {
		luaL_error(L, "Cannot divide Vector2 by zero");
		return 0;
	}

	return PushVector2(L, v->x / s, v->y / s);
}

static int l_Vector2_unm(lua_State* L) {
    auto* v = CheckVector2(L, 1);
    return PushVector2(L, -v->x, -v->y);
}

inline void RegisterVector2(lua_State* L) {
    luaL_newmetatable(L, "LuaVector2");

    lua_pushcfunction(L, l_Vector2_index, "index"); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_Vector2_newindex, "newindex"); lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, l_Vector2_eq, "eq"); lua_setfield(L, -2, "__eq");
	lua_pushcfunction(L, l_Vector2_tostring, "tostring"); lua_setfield(L, -2, "__tostring");
	lua_pushcfunction(L, l_Vector2_add, "add"); lua_setfield(L, -2, "__add");
	lua_pushcfunction(L, l_Vector2_sub, "sub"); lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, l_Vector2_mul, "mul"); lua_setfield(L, -2, "__mul");
	lua_pushcfunction(L, l_Vector2_div, "div"); lua_setfield(L, -2, "__div");
	lua_pushcfunction(L, l_Vector2_unm, "unm"); lua_setfield(L, -2, "__unm");

    lua_pop(L, 1);

    lua_newtable(L);
	lua_pushcfunction(L, l_Vector2_newindex, "newindex"); lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, l_Vector2_new, "new"); lua_setfield(L, -2, "new");
	PushVector2Const(L, 0, 0); lua_setfield(L, -2, "zero");
	PushVector2Const(L, 1, 1); lua_setfield(L, -2, "one");
	PushVector2Const(L, 1, 0); lua_setfield(L, -2, "xAxis");
	PushVector2Const(L, 0, 1); lua_setfield(L, -2, "yAxis");

    lua_setglobal(L, "Vector2");
}
