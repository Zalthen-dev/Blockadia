#pragma once

#include <cstring>

#include "Service.h"
#include "lua.h"

#include "utils/VecMath.h"

struct Lighting : Service {
	float Brightness;
	float ClockTime;
	bool GlobalShadows;
	bool Outlines;
	bool PrioritizeLightingQuality;

    Lighting() {
        Name = "Lighting";
		Brightness = 3;
		ClockTime = 14;
		GlobalShadows = true;
		Outlines = false;
		PrioritizeLightingQuality = false;
    }

    const char* ClassName() const override {
        return "Lighting";
    }

	static int l_GetSunDirection(lua_State* L) {
		Lighting* lighting = *(Lighting**)luaL_checkudata(L, 1, "Instance");

		Vector3 sunDir = SunDirFromClock(lighting->ClockTime);
		lua_pushvector(L, sunDir.x, sunDir.y, sunDir.z);

		return 1;
	}

	bool LuaGet(lua_State* L, const char* key) override {
		if (std::strcmp(key, "GetSunDirection") == 0) {
			lua_pushcfunction(L, l_GetSunDirection, "GetSunDirection");
			return true;
		}

		if (std::strcmp(key, "Brightness") == 0) {
			lua_pushnumber(L, static_cast<double>(Brightness));
			return true;
		}

		if (std::strcmp(key, "ClockTime") == 0) {
			lua_pushnumber(L, static_cast<double>(ClockTime));
			return true;
		}

		if (std::strcmp(key, "GlobalShadows") == 0) {
			lua_pushnumber(L, static_cast<double>(GlobalShadows));
			return true;
		}

		if (std::strcmp(key, "Outlines") == 0) {
			lua_pushnumber(L, static_cast<double>(Outlines));
			return true;
		}

		if (std::strcmp(key, "PrioritizeLightingQuality") == 0) {
			lua_pushnumber(L, static_cast<double>(PrioritizeLightingQuality));
			return true;
		}
		
		return Instance::LuaGet(L, key);
	}
	
	bool LuaSet(lua_State* L, const char* key, int valueIndex) override {
		if (std::strcmp(key, "Brightness") == 0) {
			Brightness = static_cast<float>(lua_tonumber(L, valueIndex));
			return true;
		}

		if (std::strcmp(key, "ClockTime") == 0) {
			ClockTime = static_cast<float>(lua_tonumber(L, valueIndex));
			return true;
		}

		if (std::strcmp(key, "GlobalShadows") == 0) {
			GlobalShadows = lua_toboolean(L, valueIndex);
			return true;
		}

		if (std::strcmp(key, "Outlines") == 0) {
			Outlines = lua_toboolean(L, valueIndex);
			return true;
		}

		if (std::strcmp(key, "PrioritizeLightingQuality") == 0) {
			PrioritizeLightingQuality = lua_toboolean(L, valueIndex);
			return true;
		}

		return Instance::LuaSet(L, key, valueIndex);
	}
};
