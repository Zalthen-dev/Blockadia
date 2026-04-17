#pragma once

#include "Service.h"

#include "lua.h"
#include <cstring>

struct ServerScriptService : Service {
	bool LoadStringEnabled;

	ServerScriptService() {
		Name = "ServerScriptService";
	}

	const char* ClassName() const override {
		return "ServerScriptService";
	}

	bool LuaGet(lua_State* L, const char* key) override {
		if (std::strcmp(key, "LoadStringEnabled") == 0) {
			lua_pushboolean(L, LoadStringEnabled);
			return true;
		}

		return Service::LuaGet(L, key);
	}
};
