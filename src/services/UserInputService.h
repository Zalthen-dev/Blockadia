#pragma once

#include <raylib.h>

#include "Service.h"

#include "datatypes/LuaVector3.h"
#include "lua.h"
#include "lualib.h"
#include "luacode.h"

struct UserInputService : Service {
	float MouseDeltaSensitivity;

	UserInputService() {
		Name = "UserInputService";

		MouseDeltaSensitivity = 1.0f;
	}

	const char* ClassName() const override {
		return "UserInputService";
	}

	static int l_GetDeviceRotation(lua_State* L) {
		PushVector3(L, 0, 0, 0);
		return 1;
	}

	bool LuaGet(lua_State* L, const char* key) override {
		if (std::strcmp(key, "GamepadEnabled") == 0) {
			lua_pushboolean(L, IsGamepadAvailable(1));
			return true;
		}

		if (std::strcmp(key, "GyroscopeEnabled") == 0) {
			// idk how to detect if a gyroscope is available
			lua_pushboolean(L, false);
			return true;
		}

		if (std::strcmp(key, "KeyboardEnabled") == 0) {
			// idk how to detect if a keyboard is available
			lua_pushboolean(L, true);
			return true;
		}

		if (std::strcmp(key, "MouseEnabled") == 0) {
			// idk how to detect if a mouse is available
			lua_pushboolean(L, true);
			return true;
		}

		if (std::strcmp(key, "TouchEnabled") == 0) {
			// idk how to detect if a touch screen is available
			lua_pushboolean(L, false);
			return true;
		}

		if (std::strcmp(key, "MouseDeltaSensitivity")) {
			lua_pushnumber(L, static_cast<double>(MouseDeltaSensitivity));
			return true;
		}

		return Service::LuaGet(L, key);
	}
};
