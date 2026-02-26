#pragma once

#include "raylib.h"
#include "raymath.h"

#include "GuiObject.h"

#include "datatypes/LuaColor3.h"

#include "lua.h"
#include "lualib.h"
#include <cstdio>

struct TextLabel : GuiObject {
	std::string Name = "TextLabel";
	std::string Text = "TextLabel";
	LuaColor3 TextColor{0, 0, 0};
	float TextTransparency = 0.0f;
	float TextSize = 20;

	const char* ClassName() const override {
        return "TextLabel";
    }

	bool LuaGet(lua_State* L, const char* key) override {
		if (std::strcmp(key, "Text") == 0) {
			lua_pushstring(L, Text.data());
			return true;
		}

		if (std::strcmp(key, "TextColor") == 0) {
			PushColor3(L, TextColor.r, TextColor.g, TextColor.b);
			return true;
		}

		if (std::strcmp(key, "TextTransparency") == 0) {
			lua_pushnumber(L, static_cast<double>(TextTransparency));
			return true;
		}

		if (std::strcmp(key, "TextSize") == 0) {
			lua_pushnumber(L, static_cast<double>(TextSize));
			return true;
		}

		return GuiObject::LuaGet(L, key);
	}

	bool LuaSet(lua_State* L, const char* key, int valueIndex) override {
		if (std::strcmp(key, "Text") == 0) {
			Text = luaL_checkstring(L, valueIndex);
			return true;
		}

		if (std::strcmp(key, "TextColor") == 0) {
			TextColor = *CheckColor3(L, valueIndex);
			return true;
		}

		if (std::strcmp(key, "TextTransparency") == 0) {
			TextTransparency = static_cast<float>(luaL_checknumber(L, valueIndex));
			return true;
		}

		if (std::strcmp(key, "TextSize") == 0) {
			TextSize = static_cast<float>(luaL_checknumber(L, valueIndex));
			return true;
		}

		return GuiObject::LuaSet(L, key, valueIndex);
	}

	void Draw(Rectangle rect, Color color) override {
		rect.x += rect.width*0.5f;
		rect.y += rect.height*0.5f;

		Vector2 position = {rect.x, rect.y};
		Vector2 origin = {rect.width*0.5f, rect.height*0.5f};

		DrawRectanglePro(rect, origin, Rotation, color);

		Font font = GetFontDefault();
	    float spacing = 1.0f;

	    Vector2 textSize = MeasureTextEx(
	        font,
	        Text.c_str(),
	        (float)TextSize,
	        spacing
	    );

	    Vector2 localPos = {(rect.width - textSize.x) * 0.5f, (rect.height - textSize.y) * 0.5f };

	    Vector2 center = {
	        rect.x + rect.width * 0.5f,
	        rect.y + rect.height * 0.5f
	    };

	    Vector2 textPos = {
	        rect.x + localPos.x,
	        rect.y + localPos.y
	    };

		Vector2 textOrigin = {textSize.x / 2, textSize.y/2};

	    Color textCol = RaylibColorFromLuauColor3(TextColor);
	    textCol.a = (unsigned char)(255 * (1.0f - TextTransparency));

	    DrawTextPro(font, Text.c_str(), textPos, origin, Rotation, (float)TextSize, spacing, textCol);
	}
};