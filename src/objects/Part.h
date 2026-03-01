#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <raylib.h>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include "objects/Instance.h"
#include "datatypes/LuaVector3.h"
#include "datatypes/LuaColor3.h"

struct Part : Instance {
	Vector3 Position{0, 0.5, 0};
	Vector3 Rotation{0, 0, 0};
	Vector3 Size{2, 1, 4};
	Vector3 Velocity{0, 0, 0};
	Color color{163, 162, 165, 255};
	// where is transparency property?
	// its baked into the color property since its a raylib color
	
	std::string Shape = "Block";
	bool Anchored = false;

	const char* ClassName() const override {
        return "Part";
    }

	bool LuaGet(lua_State* L, const char* key) override {
		if (std::strcmp(key, "Position") == 0) {
			PushVector3(L, Position.x, Position.y, Position.z);
			return true;
		}

		if (std::strcmp(key, "Rotation") == 0) {
			PushVector3(L, Rotation.x, Rotation.y, Rotation.z);
			return true;
		}

		if (std::strcmp(key, "Size") == 0) {
			PushVector3(L, Size.x, Size.y, Size.z);
			return true;
		}

		if (std::strcmp(key, "Velocity") == 0) {
			PushVector3(L, Velocity.x, Velocity.y, Velocity.z);
			return true;
		}

		if (std::strcmp(key, "Color") == 0) {
			PushColor3(L, (float)color.r/255, (float)color.g/255, (float)color.b/255);
			return true;
		}

		if (std::strcmp(key, "Transparency") == 0) {
			lua_pushnumber(L, 1.f - (float)color.a/255);
			return true;
		}

		if (std::strcmp(key, "Shape") == 0) {
			lua_pushstring(L, Shape.data());
			return true;
		}
		
		if (std::strcmp(key, "Anchored") == 0) {
			lua_pushboolean(L, Anchored);
			return true;
		}

		return Instance::LuaGet(L, key);
	}
	
	bool LuaSet(lua_State* L, const char* key, int valueIndex) override {
		if (std::strcmp(key, "Position") == 0) {
			Position = RaylibVector3FromLuaVector3(*CheckVector3(L, valueIndex));
			return true;
		}

		if (std::strcmp(key, "Rotation") == 0) {
			Rotation = RaylibVector3FromLuaVector3(*CheckVector3(L, valueIndex));
			return true;
		}

		if (std::strcmp(key, "Size") == 0) {
			Size = RaylibVector3FromLuaVector3(*CheckVector3(L, valueIndex));
			return true;
		}

		if (std::strcmp(key, "Velocity") == 0) {
			Velocity = RaylibVector3FromLuaVector3(*CheckVector3(L, valueIndex));
			return true;
		}

		if (std::strcmp(key, "Color") == 0) {
			LuaColor3* clr = CheckColor3(L, valueIndex);

			color.r = (unsigned char)(clr->r*255);
			color.g = (unsigned char)(clr->g*255);
			color.b = (unsigned char)(clr->b*255);

			return true;
		}

		if (std::strcmp(key, "Transparency") == 0) {
			float transparency = static_cast<float>(luaL_checknumber(L, valueIndex));
			color.a = (unsigned char)((1.f - transparency)*255);
			return true;
		}

		if (std::strcmp(key, "Shape") == 0) {
			Shape = luaL_checkstring(L, valueIndex);
			return true;
		}

		if (std::strcmp(key, "Anchored") == 0) {
			Anchored = luaL_checkboolean(L, valueIndex);
			return true;
		}

		return Instance::LuaSet(L, key, valueIndex);
	}
};