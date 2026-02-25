#pragma once

#include <cstring>
#include <raylib.h>
#include <vector>

#include "datatypes/LuaColor3.h"
#include "datatypes/LuaVector2.h"
#include "datatypes/LuaVector3.h"

#include "Service.h"
#include "objects/Instance.h"

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

struct DebugTextCmd {
    LuaVector2 pos;
    std::string text;
    LuaColor3 color;
    int fontSize;
};

struct DebugLine2DCmd {
    LuaVector2 a;
    LuaVector2 b;
    LuaColor3 color;
};

struct DebugLine3DCmd {
    LuaVector3 a;
    LuaVector3 b;
    LuaColor3 color;
};

struct DebugVisualService : Service {
	std::vector<DebugTextCmd> textCmds;
    std::vector<DebugLine2DCmd> line2DCmds;
    std::vector<DebugLine3DCmd> line3DCmds;

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
    }

	static int l_DrawText(lua_State* L) {
	    auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

		const char* str = luaL_checkstring(L, 2);
	    float x = (float)luaL_checknumber(L, 3);
	    float y = (float)luaL_checknumber(L, 4);

	    LuaColor3 color = CreateColor3(0, 0, 0);
	    int fontSize = 20;

		if (!lua_isnoneornil(L, 5)) {
	        fontSize = (int)luaL_checkinteger(L, 5);
	    }

	    if (!lua_isnoneornil(L, 6)) {
	        color = *CheckColor3(L, 6);
	    }

	    serv->textCmds.push_back({
	        .pos = { x, y },
	        .text = str,
	        .color = color,
	        .fontSize = fontSize
	    });

	    return 0;
	}

	static int l_DrawLine2D(lua_State* L) {
	    auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

	    LuaVector2 a = *CheckVector2(L, 2);
	    LuaVector2 b = *CheckVector2(L, 3);
	    LuaColor3 color = lua_isnoneornil(L, 4) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 4);

	    serv->line2DCmds.push_back({ a, b, color });
	    return 0;
	}

	static int l_DrawLine3D(lua_State* L) {
	    auto* serv = *(DebugVisualService**)luaL_checkudata(L, 1, "Instance");

	    LuaVector3 a = *CheckVector3(L, 2);
	    LuaVector3 b = *CheckVector3(L, 3);
	    LuaColor3 color = lua_isnoneornil(L, 4) ? CreateColor3(0, 0, 0) : *CheckColor3(L, 4);

	    serv->line3DCmds.push_back({ a, b, color });
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

		return Instance::LuaGet(L, key);
	}
};

static void RenderDebugVisuals2D(DebugVisualService* dbg) {
	for (auto& t : dbg->textCmds) {
        DrawText(t.text.c_str(), (int)t.pos.x, (int)t.pos.y, t.fontSize, RaylibColorFromLuauColor3(t.color));
    }

    for (auto& l : dbg->line2DCmds) {
        DrawLineV(RaylibVector2FromLuaVector2(l.a), RaylibVector2FromLuaVector2(l.b), RaylibColorFromLuauColor3(l.color));
    }
}

static void RenderDebugVisuals3D(DebugVisualService* dbg) {
	for (auto& l : dbg->line3DCmds) {
        DrawLine3D(RaylibVector3FromLuaVector3(l.a), RaylibVector3FromLuaVector3(l.b), RaylibColorFromLuauColor3(l.color));
    }
}