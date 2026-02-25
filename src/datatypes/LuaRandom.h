#pragma once

#include <cstring>
#include <random>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include "datatypes/LuaVector3.h"
#include "raylib.h"
#include "raymath.h"

#define LUA_RANDOM "LuaRandom"

struct LuaRandom {
	std::mt19937 rng;
	double seed;
};

static LuaRandom* CheckRandom(lua_State* L, int idx) {
	return (LuaRandom*)luaL_checkudata(L, idx, LUA_RANDOM);
}

static void PushRandom(lua_State* L, double seed) {
    LuaRandom* r = (LuaRandom*)lua_newuserdata(L, sizeof(LuaRandom));
	r->seed = seed;

	if (seed == 0) {
		r->rng.seed(0);
    } else {
		r->rng.seed(static_cast<uint32_t>(seed));
    }

    luaL_getmetatable(L, LUA_RANDOM);
    lua_setmetatable(L, -2);
}

static int l_Random_NextNumber(lua_State* L) {
	LuaRandom* r = CheckRandom(L, 1);
	double min = luaL_checknumber(L, 2);
	double max = luaL_checknumber(L, 2);

	std::uniform_real_distribution<double> dist(min, max);
	double val = dist(r->rng);

	lua_pushnumber(L, val);
	return 1;
}

static int l_Random_NextInteger(lua_State* L) {
	LuaRandom* r = CheckRandom(L, 1);
	double min = luaL_checknumber(L, 2);
	double max = luaL_checknumber(L, 2);

	std::uniform_int_distribution<int> dist(min, max);
	int val = dist(r->rng);

	lua_pushnumber(L, static_cast<double>(val));
	return 1;
}

static int l_Random_NextUnitVector(lua_State* L) {
    LuaRandom* r = CheckRandom(L, 1);

    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

    float u = dist01(r->rng);
    float v = dist01(r->rng);

    float theta = 2.0f * PI * u;
    float z = 2.0f * v - 1.0f;
    float r_xy = sqrtf(1.0f - z * z);

    float x = r_xy * cosf(theta);
    float y = r_xy * sinf(theta);

    PushVector3(L, x, y, z);
    return 1;
}

static int l_Random_index(lua_State* L) {
	LuaRandom* r = CheckRandom(L, 1);
	const char* key = luaL_checkstring(L, 2);

	if (std::strcmp(key, "NextNumber") == 0) {
		lua_pushcfunction(L, l_Random_NextNumber, "Random:NextNumber");
		return 1;
	}

	if (std::strcmp(key, "NextInteger") == 0) {
		lua_pushcfunction(L, l_Random_NextInteger, "Random:NextInteger");
		return 1;
	}

	if (std::strcmp(key, "NextUnitVector") == 0) {
		lua_pushcfunction(L, l_Random_NextUnitVector, "Random:NextUnitVector");
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

static int l_Random_new(lua_State* L) {
	double seed = luaL_optnumber(L, 1, 0);
	PushRandom(L, seed);
	return 1;
}

static void RegisterRandom(lua_State* L) {
	luaL_newmetatable(L, LUA_RANDOM);
	lua_pushcfunction(L, l_Random_index, "Random.__index"); lua_setfield(L, -2, "__index");
	lua_pop(L, 1);

	lua_newtable(L);
	lua_pushcfunction(L, l_Random_new, "Random.new"); lua_setfield(L, -2, "new");
	lua_setglobal(L, "Random");
}