#pragma once

#include "objects/Instance.h"
#include "services/Service.h"
#include "services/Workspace.h"

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

struct Game : Instance {
	std::string Name = "Game";
	const char* ClassName() const { return "Game"; }

	Game() {
		ParentingLocked = true;
	}

	static int l_GetService(lua_State* L) {
		Instance* self = CheckInstance(L, 1);
		const char* name = luaL_checkstring(L, 2);

		Instance* service = self->FindFirstChildOfClass(name);
		if (!service) {
			luaL_error(L, "Service '%s' does not exist", name);
			return 0;
		}

		PushInstance(L, service);
		return 1;
	}

	static int l_FindService(lua_State* L) {
		Instance* self = CheckInstance(L, 1);
		const char* name = luaL_checkstring(L, 2);

		Instance* service = self->FindFirstChildOfClass(name);
		if (!service) {
			lua_pushnil(L);
			return 1;
		}

		PushInstance(L, service);
		return 1;
	}

	bool LuaGet(lua_State* L, const char* key) override {
		if (std::strcmp(key, "GetService") == 0) {
			lua_pushcfunction(L, l_GetService, "GetService");
			return true;
		}

		if (std::strcmp(key, "FindService") == 0) {
			lua_pushcfunction(L, l_FindService, "FindService");
			return true;
		}

		return Instance::LuaGet(L, key);
	}
};