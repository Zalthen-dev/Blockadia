#pragma once

#include "Instance.h"

#include "datatypes/LuaUDim.h"
#include <cstring>

struct UICorner : public Cloneable<UICorner, Instance> {
	std::string Name = "UICorner";
	LuaUDim CornerRadius;

	const char* ClassName() const override {
		return "UICorner";
	}

	bool LuaGet(lua_State *L, const char *key) override {
		if (std::strcmp(key, "CornerRadius") == 0) {
			PushUDim(L, CornerRadius.scale, CornerRadius.offset);
			return true;
		}

		return Instance::LuaGet(L, key);
	}

	bool LuaSet(lua_State *L, const char *key, int index) override {
		if (std::strcmp(key, "CornerRadius") == 0) {
			CornerRadius = *CheckUDim(L, index);
			return true;
		}

		return Instance::LuaSet(L, key, index);
	}
};