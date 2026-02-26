#pragma once

#include <algorithm>
#include <cstring>
#include <raylib.h>
#include <vector>

#include "datatypes/LuaColor3.h"
#include "datatypes/LuaUDim.h"
#include "datatypes/LuaVector2.h"
#include "datatypes/LuaVector3.h"

#include "Service.h"
#include "objects/Instance.h"

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

struct DebugTextCmd {
    LuaUDim2 pos;
    std::string text;
    LuaColor3 color;
    int fontSize;
};

struct DebugLine2DCmd {
    LuaUDim2 from;
    LuaUDim2 to;
    LuaColor3 color;
};

struct DebugLine3DCmd {
    LuaVector3 from;
    LuaVector3 to;
    LuaColor3 color;
};

struct DebugSquareCmd {
	LuaUDim2 position;
	LuaUDim2 size;
	LuaColor3 color;
	double transparency;
	double outlineThickness;
	bool isOutline;
};

struct DebugVisualService : Service {
	std::vector<DebugTextCmd> textCmds;
    std::vector<DebugLine2DCmd> line2DCmds;
    std::vector<DebugLine3DCmd> line3DCmds;
	std::vector<DebugSquareCmd> squareCmds;

    DebugVisualService() {
        Name = "DebugVisualService";
    }

    const char* ClassName() const override {
        return "DebugVisualService";
    }

	void Clear() {
        textCmds.clear();
        line2DCmds.clear();
        line3DCmds.clear();
		squareCmds.clear();
    }

	static int l_DrawText(lua_State* L) {
		auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

		const char* str = luaL_checkstring(L, 2);
		LuaUDim2 udim2 = *CheckUDim2(L, 3);

		LuaColor3 color = CreateColor3(0, 0, 0);
		int fontSize = 20;

		if (!lua_isnoneornil(L, 4)) color = *CheckColor3(L, 4);
		if (!lua_isnoneornil(L, 5)) fontSize = (int)luaL_checkinteger(L, 5);

		serv->textCmds.push_back({udim2, str, color, fontSize});
		return 0;
	}

	static int l_DrawLine2D(lua_State* L) {
	    auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

	    LuaUDim2 a = *CheckUDim2(L, 2);
	    LuaUDim2 b = *CheckUDim2(L, 3);
	    LuaColor3 color = lua_isnoneornil(L, 4) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 4);

	    serv->line2DCmds.push_back({a, b, color});
	    return 0;
	}

	static int l_DrawLine3D(lua_State* L) {
	    auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

	    LuaVector3 a = *CheckVector3(L, 2);
	    LuaVector3 b = *CheckVector3(L, 3);
	    LuaColor3 color = lua_isnoneornil(L, 4) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 4);

	    serv->line3DCmds.push_back({a, b, color});
	    return 0;
	}

	static int l_DrawRectangle(lua_State* L) {
		auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

		LuaUDim2 pos = *CheckUDim2(L, 2);
		LuaUDim2 size = *CheckUDim2(L, 3);
		LuaColor3 color = lua_isnoneornil(L, 4) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 4);
		double transparency = lua_isnoneornil(L, 5) ? 0.0 : luaL_checknumber(L, 5);

		serv->squareCmds.push_back({pos, size, color, transparency, 1, false});
		return 0;
	}

	static int l_DrawRectangleOutline(lua_State* L) {
		auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

		LuaUDim2 pos = *CheckUDim2(L, 2);
		LuaUDim2 size = *CheckUDim2(L, 3);
		LuaColor3 color = lua_isnoneornil(L, 4) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 4);
		double transparency = lua_isnoneornil(L, 5) ? 0.0 : luaL_checknumber(L, 5);
		double outlineThickness = lua_isnoneornil(L, 6) ? 1.0 : std::max(luaL_checknumber(L, 6), 1.0);

		serv->squareCmds.push_back({pos, size, color, transparency, outlineThickness, true});
		return 0;
	}

	bool LuaGet(lua_State* L, const char* key) override {
		if (std::strcmp(key, "DrawText") == 0) {
			lua_pushcfunction(L, l_DrawText, "DebugVisualService:DrawText");
			return true;
		}

		if (std::strcmp(key, "DrawLine2D") == 0) {
			lua_pushcfunction(L, l_DrawLine2D, "DebugVisualService:DrawLine2D");
			return true;
		}

		if (std::strcmp(key, "DrawLine3D") == 0) {
			lua_pushcfunction(L, l_DrawLine3D, "DebugVisualService:DrawLine3D");
			return true;
		}

		if (std::strcmp(key, "DrawRectangle") == 0) {
			lua_pushcfunction(L, l_DrawRectangle, "DebugVisualService:DrawRectangle");
			return true;
		}

		if (std::strcmp(key, "DrawRectangleOutline") == 0) {
			lua_pushcfunction(L, l_DrawRectangleOutline, "DebugVisualService:DrawRectangleOutline");
			return true;
		}

		return Instance::LuaGet(L, key);
	}
};

static void RenderDebugVisuals2D(DebugVisualService* dbg) {
	Vector2 screenBounds = {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

	for (auto& t : dbg->textCmds) {
		Vector2 pos = {
			(t.pos.x.scale * screenBounds.x) + t.pos.x.offset,
			(t.pos.y.scale * screenBounds.y) + t.pos.y.offset
		};

        DrawText(t.text.c_str(), (int)pos.x, (int)pos.y, t.fontSize, RaylibColorFromLuauColor3(t.color));
    }

    for (auto& l : dbg->line2DCmds) {
		Vector2 from = {
			(l.from.x.scale * screenBounds.x) + l.from.x.offset,
			(l.from.y.scale * screenBounds.y) + l.from.y.offset
		};

		Vector2 to = {
			(l.to.x.scale * screenBounds.x) + l.to.x.offset,
			(l.to.y.scale * screenBounds.y) + l.to.y.offset
		};

		DrawLineV(from, to, RaylibColorFromLuauColor3(l.color));
	}

	for (auto& l : dbg->squareCmds) {
		Color clr = RaylibColorFromLuauColor3(l.color);
		clr.a = (unsigned char)((1.0 - l.transparency) * 255.0);

		Vector2 size = {
			(l.size.x.scale * screenBounds.x) + l.size.x.offset,
			(l.size.y.scale * screenBounds.y) + l.size.y.offset
		};
		
		Vector2 pos = {
			(l.position.x.scale * screenBounds.x) + l.position.x.offset,
			(l.position.y.scale * screenBounds.y) + l.position.y.offset
		};

		if (l.isOutline) {
			float outlineThickness = static_cast<float>(l.outlineThickness);
			Vector2 topBottomLineSizes = {size.x, outlineThickness};
			Vector2 leftRightLineSizes = {outlineThickness, size.y - outlineThickness*2};

			Vector2 bottomLinePos = {pos.x, pos.y + (size.y - outlineThickness)};
			Vector2 leftLinePos = {pos.x, pos.y + outlineThickness};
			Vector2 rightLinePos = {pos.x + (size.x - outlineThickness), pos.y + outlineThickness};

			// Top & bottom lines
			DrawRectangleV(pos, topBottomLineSizes, clr);
			DrawRectangleV(bottomLinePos, topBottomLineSizes, clr);

			// Left & right lines
			DrawRectangleV(leftLinePos, leftRightLineSizes, clr);
			DrawRectangleV(rightLinePos, leftRightLineSizes, clr);
		} else {
			DrawRectangleV(pos, size, clr);
		}
	}
}

static void RenderDebugVisuals3D(DebugVisualService* dbg) {
	for (auto& l : dbg->line3DCmds) {
        DrawLine3D(RaylibVector3FromLuaVector3(l.from), RaylibVector3FromLuaVector3(l.to), RaylibColorFromLuauColor3(l.color));
    }
}