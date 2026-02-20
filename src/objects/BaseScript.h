#pragma once

#include <cstring>
#include <string>

#include "Instance.h"

struct BaseScript : Instance {
	std::string Name = "BaseScript";
	std::string Source = "print('Hello World')";

	const char* ClassName() const override {
		return "BaseScript";
	}

	bool LuaGet(lua_State* L, const char* key) override {
		if (std::strcmp(key, "Source") == 0) {
			lua_pushstring(L, Source.c_str());
			return true;
		}

		return Instance::LuaGet(L, key);
	}

	bool LuaSet(lua_State* L, const char* key, int idx) override {
		if (std::strcmp(key, "Source") == 0) {
			Source = luaL_checkstring(L, idx);
			return true;
		}

		return Instance::LuaSet(L, key, idx);
	}
};