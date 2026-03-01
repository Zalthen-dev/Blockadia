#pragma once

#include <algorithm>
#include <cstring>
#include <raylib.h>
#include <type_traits>
#include <variant>
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
#include "raymath.h"

struct DebugBaseCmd {
	static std::string GetType() { return Type; }
private:
	static std::string Type;
};

struct DebugTextCmd : DebugBaseCmd {
	LuaUDim2 pos;
	std::string text;
	LuaColor3 color;
	int fontSize;
private:
	std::string Type = "Text";
};

struct DebugLine2DCmd : DebugBaseCmd {
	LuaUDim2 from;
	LuaUDim2 to;
	LuaColor3 color;
	float arrowSize;
	bool isArrow;
private:
	std::string Type = "Line2D";
};

struct DebugLine3DCmd : DebugBaseCmd {
	LuaVector3 from;
	LuaVector3 to;
	LuaColor3 color;
	float arrowSize;
	int arrowSegments;
	bool isArrow;
private:
	std::string Type = "Line3D";
};

struct DebugRectangleCmd : DebugBaseCmd {
	LuaUDim2 position;
	LuaUDim2 size;
	LuaColor3 color;
	double transparency;
	double outlineThickness;
	bool isOutline;
private:
	std::string Type = "Rectangle";
};

using DebugCmd = std::variant<
	DebugTextCmd,
	DebugLine2DCmd,
	DebugLine3DCmd,
	DebugRectangleCmd
>;

struct DebugVisualService : Service {
	std::vector<DebugCmd> cmds;

	DebugVisualService() {
		Name = "DebugVisualService";
	}

    const char* ClassName() const override {
        return "DebugVisualService";
    }

	void Clear() {
		cmds.clear();
    }

	static int l_DrawText(lua_State* L) {
		auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

		DebugTextCmd c;
		c.text = luaL_checkstring(L, 2);
		c.pos = *CheckUDim2(L, 3);
		c.color = lua_isnoneornil(L, 4) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 4);
		c.fontSize = lua_isnoneornil(L, 5) ? 20 : (int)luaL_checkinteger(L, 5);

		serv->cmds.push_back(c);
		return 0;
	}

	static int l_DrawLine2D(lua_State* L) {
		auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

		DebugLine2DCmd c;
		c.from = *CheckUDim2(L, 2);
		c.to = *CheckUDim2(L, 3);
		c.color = lua_isnoneornil(L, 4) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 4);
		c.isArrow = false;

		serv->cmds.push_back(c);
		return 0;
	}

	static int l_DrawArrow2D(lua_State* L) {
		auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

		DebugLine2DCmd c;
		c.isArrow = true;
		c.from = *CheckUDim2(L, 2);
		c.to = *CheckUDim2(L, 3);
		c.arrowSize = lua_isnoneornil(L, 4) ? 10 : luaL_checknumber(L, 4);
		c.color = lua_isnoneornil(L, 5) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 5);

		serv->cmds.push_back(c);
		return 0;
	}

	static int l_DrawLine3D(lua_State* L) {
		auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

		DebugLine3DCmd c;
		c.isArrow = false;
		c.from = *CheckVector3(L, 2);
		c.to = *CheckVector3(L, 3);
		c.color = lua_isnoneornil(L, 4) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 4);

		serv->cmds.push_back(c);
		return 0;
	}

	static int l_DrawArrow3D(lua_State* L) {
		auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

		DebugLine3DCmd c;
		c.isArrow = true;
		c.from = *CheckVector3(L, 2);
		c.to = *CheckVector3(L, 3);
		c.arrowSize = lua_isnoneornil(L, 4) ? 1 : luaL_checknumber(L, 4);
		c.arrowSegments = lua_isnoneornil(L, 5) ? 4 : luaL_checkinteger(L, 5);
		c.color = lua_isnoneornil(L, 6) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 6);

		serv->cmds.push_back(c);
		return 0;
	}

	static int l_DrawRectangle(lua_State* L) {
		auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

		DebugRectangleCmd c;
		c.position = *CheckUDim2(L, 2);
		c.size = *CheckUDim2(L, 3);
		c.color = lua_isnoneornil(L, 4) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 4);
		c.transparency = lua_isnoneornil(L, 5) ? 0.0 : luaL_checknumber(L, 5);
		c.outlineThickness = 1;
		c.isOutline = false;

		serv->cmds.push_back(c);
		return 0;
	}

	static int l_DrawRectangleOutline(lua_State* L) {
		auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

		DebugRectangleCmd c;
		c.position = *CheckUDim2(L, 2);
		c.size = *CheckUDim2(L, 3);
		c.color = lua_isnoneornil(L, 4) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 4);
		c.transparency = lua_isnoneornil(L, 5) ? 0.0 : luaL_checknumber(L, 5);
		c.outlineThickness = lua_isnoneornil(L, 6) ? 1.0 : std::max(luaL_checknumber(L, 6), 1.0);
		c.isOutline = true;

		serv->cmds.push_back(c);
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

		if (std::strcmp(key, "DrawArrow2D") == 0) {
			lua_pushcfunction(L, l_DrawArrow2D, "DebugVisualService:DrawArrow2D");
			return true;
		}

		if (std::strcmp(key, "DrawLine3D") == 0) {
			lua_pushcfunction(L, l_DrawLine3D, "DebugVisualService:DrawLine3D");
			return true;
		}

		if (std::strcmp(key, "DrawArrow3D") == 0) {
			lua_pushcfunction(L, l_DrawArrow3D, "DebugVisualService:DrawArrow3D");
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

static void DebugDrawText2D(DebugTextCmd cmd, Vector2 screenBounds) {
	Vector2 pos = {
		(cmd.pos.x.scale * screenBounds.x) + cmd.pos.x.offset,
		(cmd.pos.y.scale * screenBounds.y) + cmd.pos.y.offset
	};

	DrawText(cmd.text.c_str(), (int)pos.x, (int)pos.y, cmd.fontSize, RaylibColorFromLuauColor3(cmd.color));
}

static void DebugDrawLine2D(DebugLine2DCmd cmd, Vector2 screenBounds) {
	Vector2 from = {
		(cmd.from.x.scale * screenBounds.x) + cmd.from.x.offset,
		(cmd.from.y.scale * screenBounds.y) + cmd.from.y.offset
	};

	Vector2 to = {
		(cmd.to.x.scale * screenBounds.x) + cmd.to.x.offset,
		(cmd.to.y.scale * screenBounds.y) + cmd.to.y.offset
	};

	DrawLineV(from, to, RaylibColorFromLuauColor3(cmd.color));

	if (cmd.isArrow && cmd.arrowSize > 1) {
		Vector2 direction = Vector2Normalize(from - to);

		Vector2 direction1 = Vector2Rotate(direction, -30 * DEG2RAD);
		Vector2 direction2 = Vector2Rotate(direction, 30 * DEG2RAD);

		Vector2 point1 = to + (direction1 * cmd.arrowSize);
		Vector2 point2 = to + (direction2 * cmd.arrowSize);

		DrawLineV(to, point1, RaylibColorFromLuauColor3(cmd.color));
		DrawLineV(point1, point2, RaylibColorFromLuauColor3(cmd.color));
		DrawLineV(point2, to, RaylibColorFromLuauColor3(cmd.color));
	}
}

static void DebugDrawRectangle(DebugRectangleCmd cmd, Vector2 screenBounds) {
	Color clr = RaylibColorFromLuauColor3(cmd.color);
	clr.a = (unsigned char)((1.0 - cmd.transparency) * 255.0);

	Vector2 size = {
		(cmd.size.x.scale * screenBounds.x) + cmd.size.x.offset,
		(cmd.size.y.scale * screenBounds.y) + cmd.size.y.offset
	};

	Vector2 pos = {
		(cmd.position.x.scale * screenBounds.x) + cmd.position.x.offset,
		(cmd.position.y.scale * screenBounds.y) + cmd.position.y.offset
	};

	if (cmd.isOutline) {
		float outlineThickness = static_cast<float>(cmd.outlineThickness);
		
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

static void DebugDrawLine3D(DebugLine3DCmd cmd) {
	Vector3 from = RaylibVector3FromLuaVector3(cmd.from);
	Vector3 to = RaylibVector3FromLuaVector3(cmd.to);
	Color color = RaylibColorFromLuauColor3(cmd.color);

	DrawLine3D(from,  to, color);

	if (cmd.isArrow && cmd.arrowSize > 0 && cmd.arrowSegments > 3) {
		Vector3 direction = Vector3Normalize(from - to);
		Vector3 coneToPos = to + (direction * cmd.arrowSize);

		DrawCylinderWiresEx(to, coneToPos, 0, cmd.arrowSize, cmd.arrowSegments, color);
	}
}

static void RenderDebugVisuals2D(DebugVisualService* dbg) {
	Vector2 screenBounds = {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

	for (auto& baseCmd : dbg->cmds) {
		std::visit([&](auto& c) {
			using T = std::decay_t<decltype(c)>;

			if constexpr (std::is_same_v<T, DebugTextCmd>) {
				DebugDrawText2D(c, screenBounds);
			} else if constexpr (std::is_same_v<T, DebugLine2DCmd>) {
				DebugDrawLine2D(c, screenBounds);
			} else if constexpr (std::is_same_v<T, DebugRectangleCmd>) {
				DebugDrawRectangle(c, screenBounds);
			}
		}, baseCmd);
	}
}

static void RenderDebugVisuals3D(DebugVisualService* dbg) {
	for (auto& baseCmd : dbg->cmds) {
		std::visit([&](auto& c) {
			using T = std::decay_t<decltype(c)>;

			if constexpr (std::is_same_v<T, DebugLine3DCmd>) {
				DebugDrawLine3D(c);
			}
		}, baseCmd);
	}
}