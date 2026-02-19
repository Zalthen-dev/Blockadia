#pragma once

#include "Service.h"

#include "datatypes/LuaSignal.h"

struct RunService : Service {
	LuaSignal RenderStepped; // PreRender
    LuaSignal Stepped;
    LuaSignal Heartbeat; // PostSimulation

    RunService() {
        Name = "RunService";
    }

    const char* ClassName() const override {
        return "RunService";
    }

	bool LuaGet(lua_State* L, const char* key) override {
		if (std::strcmp(key, "RenderStepped") == 0 || std::strcmp(key, "PreRender") == 0) {
			PushSignal(L, &RenderStepped);
			return true;
		}

		if (std::strcmp(key, "Stepped") == 0 || std::strcmp(key, "PreSimulation") == 0) {
			PushSignal(L, &Stepped);
			return true;
		}

		if (std::strcmp(key, "Heartbeat") == 0 || std::strcmp(key, "PostSimulation") == 0) {
			PushSignal(L, &Heartbeat);
			return true;
		}
		
		return Instance::LuaGet(L, key);
	}
};
