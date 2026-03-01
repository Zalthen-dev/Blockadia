#pragma once

#include <raylib.h>
#include "datatypes/LuaVector3.h"
#include "raymath.h"

#include <cstring>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#define LUA_CFRAME "LuaCFrame"

struct LuaCFrame {
	Matrix m;
};

static LuaCFrame* CheckCFrame(lua_State* L, int idx) {
	return static_cast<LuaCFrame*>(luaL_checkudata(L, idx, LUA_CFRAME));
}

static void PushCFrame(lua_State* L, const Matrix& m) {
	LuaCFrame* c = (LuaCFrame*)lua_newuserdata(L, sizeof(LuaCFrame));
    c->m = m;

    luaL_getmetatable(L, LUA_CFRAME);
    lua_setmetatable(L, -2);
}

static Matrix CFrameIdentity() {
	return MatrixIdentity();
}

static Matrix CFrameFromPosition(Vector3 pos) {
	return MatrixTranslate(pos.x, pos.y, pos.z);
}

inline Matrix CFrameFromBasis(Vector3 pos, Vector3 right, Vector3 up, Vector3 back) {
	Matrix m = MatrixIdentity();
	
	m.m0 = right.x; m.m1 = right.y; m.m2 = right.z;
	m.m4 = up.x;    m.m5 = up.y;    m.m6 = up.z;
	m.m8 = back.x;  m.m9 = back.y;  m.m10 = back.z;

	m.m12 = pos.x;
	m.m13 = pos.y;
	m.m14 = pos.z;

	return m;
}

inline Vector3 CFrameGetPosition(const LuaCFrame& cf) {
    return { cf.m.m12, cf.m.m13, cf.m.m14 };
}

inline Vector3 CFrameGetRight(const LuaCFrame& c) {
    return { c.m.m0, c.m.m1, c.m.m2 };
}

inline Vector3 CFrameGetUp(const LuaCFrame& c) {
    return { c.m.m4, c.m.m5, c.m.m6 };
}

inline Vector3 CFrameGetBack(const LuaCFrame& c) {
    return { c.m.m8, c.m.m9, c.m.m10 };
}

inline Vector3 CFrameGetLook(const LuaCFrame& c) {
    Vector3 b = CFrameGetBack(c);
    return { -b.x, -b.y, -b.z };
}

static Matrix CFrameFromEuler(Vector3 pos, Vector3 rotDeg) {
	Quaternion q = QuaternionFromEuler(
		DEG2RAD * rotDeg.x,
		DEG2RAD * rotDeg.y,
		DEG2RAD * rotDeg.z
	);

	Matrix r = QuaternionToMatrix(q);
	Matrix t = MatrixTranslate(pos.x, pos.y, pos.z);

	return MatrixMultiply(t, r);
}

static LuaCFrame CFrameMultiply(const LuaCFrame& a, const LuaCFrame& b) {
	return LuaCFrame{ MatrixMultiply(a.m, b.m) };
}

inline Matrix CFrameLookAt(Vector3 pos, Vector3 target, Vector3 up = {0,1,0}) {
	Vector3 look = Vector3Normalize(Vector3Subtract(target, pos));
	if (Vector3Length(look) < 0.0001f) {
		return MatrixTranslate(pos.x, pos.y, pos.z);
	}

	Vector3 right = Vector3Normalize(Vector3CrossProduct(up, look));
	Vector3 trueUp = Vector3CrossProduct(look, right);

	Vector3 back = Vector3Negate(look);
	return CFrameFromBasis(pos, right, trueUp, back);
}

static int l_CFrame_index(lua_State* L) {
	LuaCFrame* c = CheckCFrame(L, 1);
    const char* key = luaL_checkstring(L, 2);

	if (std::strcmp(key, "Position") == 0) {
		Vector3 pos = CFrameGetPosition(*c);
		PushVector3(L, pos.x, pos.y, pos.z);
		return 1;
	} else if (std::strcmp(key, "Rotation") == 0) {
		Matrix rot = c->m;
		rot.m12 = rot.m13 = rot.m14 = 0.0f;
		PushCFrame(L, rot);
		return 1;
	} else if (std::strcmp(key, "X") == 0) {
		Vector3 pos = CFrameGetPosition(*c);
		lua_pushnumber(L, pos.x);
		return 1;
	} else if (std::strcmp(key, "Y") == 0) {
		Vector3 pos = CFrameGetPosition(*c);
		lua_pushnumber(L, pos.y);
		return 1;
	} else if (std::strcmp(key, "Z") == 0) {
		Vector3 pos = CFrameGetPosition(*c);
		lua_pushnumber(L, pos.z);
		return 1;
	} else if (std::strcmp(key, "LookVector") == 0) {
		Vector3 v = CFrameGetLook(*c);
		PushVector3(L, v.x, v.y, v.z);
		return 1;
	} else if (std::strcmp(key, "RightVector") == 0 || std::strcmp(key, "XVector") == 0) {
		Vector3 v = CFrameGetRight(*c);
		PushVector3(L, v.x, v.y, v.z);
		return 1;
	} else if (std::strcmp(key, "UpVector") == 0 || std::strcmp(key, "YVector") == 0) {
		Vector3 v = CFrameGetUp(*c);
		PushVector3(L, v.x, v.y, v.z);
		return 1;
	} else if (std::strcmp(key, "ZVector") == 0) {
		Vector3 v = CFrameGetBack(*c);
		PushVector3(L, v.x, v.y, v.z);
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

static int l_CFrame_mul(lua_State* L);

static int l_CFrame_new(lua_State* L) {
    int argc = lua_gettop(L);

	if (argc == 0) {
		PushCFrame(L, MatrixIdentity());
		return 1;
	}

	if (argc == 1 && luaL_checkudata(L, 1, LUA_VECTOR3)) {
		Vector3 p = RaylibVector3FromLuaVector3(*CheckVector3(L, 1));
		PushCFrame(L, MatrixTranslate(p.x, p.y, p.z));
		return 1;
	}

	if (argc == 2 &&
		luaL_checkudata(L, 1, LUA_VECTOR3) &&
		luaL_checkudata(L, 2, LUA_VECTOR3)) {
			
		Vector3 pos = RaylibVector3FromLuaVector3(*CheckVector3(L, 1));
		Vector3 target = RaylibVector3FromLuaVector3(*CheckVector3(L, 2));
		PushCFrame(L, CFrameLookAt(pos, target));
		return 1;
	}

	if (argc == 3 &&
		lua_isnumber(L,1) &&
		lua_isnumber(L,2) &&
		lua_isnumber(L,3)) {

		float x = (float)lua_tonumber(L, 1);
		float y = (float)lua_tonumber(L, 2);
		float z = (float)lua_tonumber(L, 3);

		PushCFrame(L, MatrixTranslate(x, y, z));
		return 1;
    }

	luaL_error(L, "Invalid arguments to CFrame.new");
    return 0;
}

static int l_CFrame_fromEulerAnglesXYZ(lua_State* L) {
    float x = (float)luaL_checknumber(L, 1);
    float y = (float)luaL_checknumber(L, 2);
    float z = (float)luaL_checknumber(L, 3);

    PushCFrame(L, CFrameFromEuler(Vector3Zero(), {x, y, z}));
    return 1;
}

static int l_CFrame_lookAt(lua_State* L) {

	return 1;
}

static void RegisterCFrame(lua_State* L) {
	luaL_newmetatable(L, LUA_CFRAME);

	lua_pop(L,1);

	lua_newtable(L); // CFrame

	lua_pushcfunction(L, l_CFrame_new, "CFrame.new"); lua_setfield(L, -2, "new");
	lua_pushcfunction(L, l_CFrame_lookAt, "CFrame.lookAt"); lua_setfield(L, -2, "lookAt");
	lua_pushcfunction(L, l_CFrame_fromEulerAnglesXYZ, "CFrame.fromEulerAnglesXYZ"); lua_setfield(L, -2, "fromEulerAnglesXYZ");

	lua_setglobal(L, "CFrame");
}