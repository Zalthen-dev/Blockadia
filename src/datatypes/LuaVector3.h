#pragma once

#include <cmath>
#include <cstring>
#include <raylib.h>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include "utils/ExtraMath.h"

#define LUA_VECTOR3 "LuaVector3"

struct LuaVector3 {
    float x, y, z;
};

static LuaVector3* CheckVector3(lua_State* L, int idx) {
    return (LuaVector3*)luaL_checkudata(L, idx, LUA_VECTOR3);
}

static int PushVector3(lua_State* L, float x, float y, float z) {
	auto* v = (LuaVector3*)lua_newuserdata(L, sizeof(LuaVector3));
	v->x = x;
	v->y = y;
	v->z = z;
	luaL_getmetatable(L, LUA_VECTOR3);
	lua_setmetatable(L, -2);
	return 1;
}

static void PushVector3Const(lua_State* L, float x, float y, float z) {
    PushVector3(L, x, y, z);
}

static int l_Vector3_new(lua_State* L) {
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    float z = luaL_checknumber(L, 3);

    auto* v = (LuaVector3*)lua_newuserdata(L, sizeof(LuaVector3));
    v->x = x;
    v->y = y;
    v->z = z;

    luaL_getmetatable(L, LUA_VECTOR3);
    lua_setmetatable(L, -2);

    return 1;
}

static int l_Vector3_Abs(lua_State* L) {
    auto* v = CheckVector3(L, 1);
    return PushVector3(L, fabsf(v->x), fabsf(v->y), fabsf(v->z));
}

static int l_Vector3_Ceil(lua_State* L) {
    auto* v = CheckVector3(L, 1);
    return PushVector3(L, ceilf(v->x), ceilf(v->y), ceilf(v->z));
}

static int l_Vector3_Floor(lua_State* L) {
    auto* v = CheckVector3(L, 1);
    return PushVector3(L, floorf(v->x), floorf(v->y), floorf(v->z));
}

static int l_Vector3_Sign(lua_State* L) {
    auto* v = CheckVector3(L, 1);
    return PushVector3(L, signf(v->x), signf(v->y), signf(v->z));
}

static int l_Vector3_Dot(lua_State* L) {
    auto* a = CheckVector3(L, 1);
    auto* b = CheckVector3(L, 2);

    lua_pushnumber(L, a->x*b->x + a->y*b->y + a->z*b->z);
    return 1;
}

static int l_Vector3_Cross(lua_State* L) {
    auto* a = CheckVector3(L, 1);
    auto* b = CheckVector3(L, 2);

    return PushVector3(L, a->y*b->z - a->z*b->y, a->z*b->x - a->x*b->z, a->x*b->y - a->y*b->x);
}

static int l_Vector3_Angle(lua_State* L) {
    auto* a = CheckVector3(L, 1);
    auto* b = CheckVector3(L, 2);

    float dot = a->x*b->x + a->y*b->y + a->z*b->z;
    float magA = sqrtf(a->x*a->x + a->y*a->y + a->z*a->z);
    float magB = sqrtf(b->x*b->x + b->y*b->y + b->z*b->z);

    if (magA < 1e-6f || magB < 1e-6f) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    float c = dot / (magA * magB);
    c = fmaxf(-1.0f, fminf(1.0f, c));

    lua_pushnumber(L, acosf(c));
    return 1;
}

static int l_Vector3_FuzzyEq(lua_State* L) {
    auto* a = CheckVector3(L, 1);
    auto* b = CheckVector3(L, 2);
    float eps = (float)luaL_optnumber(L, 3, 1e-5f);

    bool eq = fabsf(a->x - b->x) <= eps && fabsf(a->y - b->y) <= eps && fabsf(a->z - b->z) <= eps;

    lua_pushboolean(L, eq);
    return 1;
}

static int l_Vector3_Lerp(lua_State* L) {
    auto* a = CheckVector3(L, 1);
    auto* b = CheckVector3(L, 2);
    float t = (float)luaL_checknumber(L, 3);

    return PushVector3(L, a->x + (b->x - a->x) * t, a->y + (b->y - a->y) * t, a->z + (b->z - a->z) * t);
}

static int l_Vector3_Max(lua_State* L) {
    auto* a = CheckVector3(L, 1);
    auto* b = CheckVector3(L, 2);

    return PushVector3(L, fmaxf(a->x, b->x), fmaxf(a->y, b->y), fmaxf(a->z, b->z));
}

static int l_Vector3_Min(lua_State* L) {
    auto* a = CheckVector3(L, 1);
    auto* b = CheckVector3(L, 2);

    return PushVector3(L, fminf(a->x, b->x), fminf(a->y, b->y), fminf(a->z, b->z));
}


static int l_Vector3_index(lua_State* L) {
    auto* v = (LuaVector3*)luaL_checkudata(L, 1, LUA_VECTOR3);
    const char* key = luaL_checkstring(L, 2);

	if (!strcmp(key, "Abs"))      { lua_pushcfunction(L, l_Vector3_Abs, "Abs"); return 1; }
    if (!strcmp(key, "Ceil"))     { lua_pushcfunction(L, l_Vector3_Ceil, "Ceil"); return 1; }
    if (!strcmp(key, "Floor"))    { lua_pushcfunction(L, l_Vector3_Floor, "Floor"); return 1; }
    if (!strcmp(key, "Sign"))     { lua_pushcfunction(L, l_Vector3_Sign, "Sign"); return 1; }
    if (!strcmp(key, "Cross"))    { lua_pushcfunction(L, l_Vector3_Cross, "Cross"); return 1; }
    if (!strcmp(key, "Angle"))    { lua_pushcfunction(L, l_Vector3_Angle, ""); return 1; }
    if (!strcmp(key, "Dot"))      { lua_pushcfunction(L, l_Vector3_Dot, "Dot"); return 1; }
    if (!strcmp(key, "FuzzyEq"))  { lua_pushcfunction(L, l_Vector3_FuzzyEq, "FuzzyEq"); return 1; }
    if (!strcmp(key, "Lerp"))     { lua_pushcfunction(L, l_Vector3_Lerp, "Lerp"); return 1; }
    if (!strcmp(key, "Max"))      { lua_pushcfunction(L, l_Vector3_Max, "Max"); return 1; }
    if (!strcmp(key, "Min"))      { lua_pushcfunction(L, l_Vector3_Min, "Min"); return 1; }


    if (std::strcmp(key, "X") == 0) { lua_pushnumber(L, v->x); return 1; }
    if (std::strcmp(key, "Y") == 0) { lua_pushnumber(L, v->y); return 1; }
    if (std::strcmp(key, "Z") == 0) { lua_pushnumber(L, v->z); return 1; }

	if (std::strcmp(key, "Magnitude") == 0) {
		lua_pushnumber(L, sqrt(v->x*v->x + v->y*v->y + v->z*v->z));
		return 1;
	}

	if (std::strcmp(key, "Unit") == 0) {
	    float mag = sqrtf(v->x*v->x + v->y*v->y + v->z*v->z);

	    // Create new Vector3 userdata
	    auto* u = (LuaVector3*)lua_newuserdata(L, sizeof(LuaVector3));

	    if (mag > 0.0f) {
	        u->x = v->x / mag;
	        u->y = v->y / mag;
	        u->z = v->z / mag;
	    } else {
	        u->x = 0.0f;
	        u->y = 0.0f;
	        u->z = 0.0f;
	    }

	    luaL_getmetatable(L, LUA_VECTOR3);
	    lua_setmetatable(L, -2);

	    return 1;
	}

    lua_pushnil(L);
    return 1;
}

static int l_Vector3_newindex(lua_State* L) {
	luaL_error(L, "Vector3 is immutable");
    return 0;
}

static int l_Vector3_eq(lua_State* L) {
    auto* a = (LuaVector3*)luaL_checkudata(L, 1, LUA_VECTOR3);
    auto* b = (LuaVector3*)luaL_checkudata(L, 2, LUA_VECTOR3);

    lua_pushboolean(L, a->x == b->x && a->y == b->y && a->z == b->z);
    return 1;
}

static int l_Vector3_tostring(lua_State* L) {
    auto* v = (LuaVector3*)luaL_checkudata(L, 1, LUA_VECTOR3);
    lua_pushfstring(L, "Vector3(%f, %f, %f)", v->x, v->y, v->z);
    return 1;
}

static int l_Vector3_add(lua_State* L) {
    auto* a = CheckVector3(L, 1);
    auto* b = CheckVector3(L, 2);

    return PushVector3(L, a->x + b->x, a->y + b->y, a->z + b->z);
}

static int l_Vector3_sub(lua_State* L) {
    auto* a = CheckVector3(L, 1);
    auto* b = CheckVector3(L, 2);

    return PushVector3(L, a->x - b->x, a->y - b->y, a->z - b->z);
}

static int l_Vector3_mul(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        float s = (float)lua_tonumber(L, 1);
        auto* v = CheckVector3(L, 2);
        return PushVector3(L, v->x*s, v->y*s, v->z*s);
    }

    if (lua_isnumber(L, 2)) {
        auto* v = CheckVector3(L, 1);
        float s = (float)lua_tonumber(L, 2);
        return PushVector3(L, v->x*s, v->y*s, v->z*s);
    }

	luaL_error(L, "Vector3 * Vector3 is not supported");
    return 0; 
}

static int l_Vector3_div(lua_State* L) {
    auto* v = CheckVector3(L, 1);
    float s = (float)luaL_checknumber(L, 2);

    if (s == 0.0f) {
		luaL_error(L, "Cannot divide Vector3 by zero");
		return 0;
	}

	return PushVector3(L, v->x / s, v->y / s, v->z / s);
}

static int l_Vector3_unm(lua_State* L) {
    auto* v = CheckVector3(L, 1);
    return PushVector3(L, -v->x, -v->y, -v->z);
}

inline void RegisterVector3(lua_State* L) {
    luaL_newmetatable(L, "LuaVector3");

    lua_pushcfunction(L, l_Vector3_index, "index"); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_Vector3_newindex, "newindex"); lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, l_Vector3_eq, "eq"); lua_setfield(L, -2, "__eq");
	lua_pushcfunction(L, l_Vector3_tostring, "tostring"); lua_setfield(L, -2, "__tostring");
	lua_pushcfunction(L, l_Vector3_add, "add"); lua_setfield(L, -2, "__add");
	lua_pushcfunction(L, l_Vector3_sub, "sub"); lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, l_Vector3_mul, "mul"); lua_setfield(L, -2, "__mul");
	lua_pushcfunction(L, l_Vector3_div, "div"); lua_setfield(L, -2, "__div");

    lua_pop(L, 1);

    lua_newtable(L);
	lua_pushcfunction(L, l_Vector3_newindex, "newindex"); lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, l_Vector3_new, "new"); lua_setfield(L, -2, "new");
	PushVector3Const(L, 0, 0, 0); lua_setfield(L, -2, "zero");
	PushVector3Const(L, 1, 1, 1); lua_setfield(L, -2, "one");
	PushVector3Const(L, 1, 0, 0); lua_setfield(L, -2, "xAxis");
	PushVector3Const(L, 0, 1, 0); lua_setfield(L, -2, "yAxis");
	PushVector3Const(L, 0, 0, 1); lua_setfield(L, -2, "zAxis");

    lua_setglobal(L, "Vector3");
}
