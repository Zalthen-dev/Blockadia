#pragma once

#include "objects/Instance.h"

#include <raylib.h>

#include "lua.h"
#include "lualib.h"

struct ScreenGui : Instance {
	std::string Name = "ScreenGui";

	float displayorder = 1;
	bool Enabled = true;
	bool IgnoreGuiInset = false;

	const char* ClassName() const override {
        return "ScreenGui";
    }

	bool LuaGet(lua_State* L, const char* key) override {
		if (std::strcmp(key, "Enabled") == 0) {
			lua_pushboolean(L, Enabled);
			return true;
		}

		if (std::strcmp(key, "IgnoreGuiInset") == 0) {
			lua_pushboolean(L, IgnoreGuiInset);
			return true;
		}

		if (std::strcmp(key, "AbsolutePosition") == 0) {
			lua_pushvector(L, 0, 0, 0);
			return true;
		}

		if (std::strcmp(key, "AbsoluteSize") == 0) {
			lua_pushvector(L, GetScreenWidth(), GetScreenHeight(), 0);
			return true;
		}

		return Instance::LuaGet(L, key);
	}

	bool LuaSet(lua_State* L, const char* key, int valueIndex) override {
		if (std::strcmp(key, "Enabled") == 0) {
			Enabled = luaL_checkboolean(L, valueIndex);
			return true;
		}

		if (std::strcmp(key, "IgnoreGuiInset") == 0) {
			IgnoreGuiInset = luaL_checkboolean(L, valueIndex);
			return true;
		}

		return Instance::LuaSet(L, key, valueIndex);
	}
};