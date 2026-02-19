#pragma once

#include "Instance.h"
#include "lua.h"
#include "lualib.h"

#include "datatypes/LuaUDim.h"
#include "datatypes/LuaVector2.h"
#include "datatypes/LuaColor3.h"

struct GuiObject : Instance {
    LuaUDim2 Size;
    LuaUDim2 Position;

    LuaVector2 AnchorPoint{0, 0};

    Vector3 BackgroundColor{1, 1, 1};
    float BackgroundTransparency{0.0f};

    bool Visible{true};

    const char* ClassName() const override {
        return "GuiObject";
    }

    GuiObject() {
        Size = { {0,0}, {0,0} };
        Position = { {0,0}, {0,0} };
    }

	bool LuaGet(lua_State* L, const char* key) override {
	    if (!strcmp(key, "Size")) {
	        PushUDim2(L, Size.x, Size.y);
	        return true;
	    }

	    if (!strcmp(key, "Position")) {
	        PushUDim2(L, Position.x, Position.y);
	        return true;
	    }

	    if (!strcmp(key, "AnchorPoint")) {
	        PushVector2(L, AnchorPoint.x, AnchorPoint.y);
	        return true;
	    }

	    if (!strcmp(key, "BackgroundColor3")) {
	        lua_pushvector(L,
	            BackgroundColor.x,
	            BackgroundColor.y,
	            BackgroundColor.z
	        );
	        return true;
	    }

	    if (!strcmp(key, "BackgroundTransparency")) {
	        lua_pushnumber(L, BackgroundTransparency);
	        return true;
	    }

	    if (!strcmp(key, "Visible")) {
	        lua_pushboolean(L, Visible);
	        return true;
	    }

	    return Instance::LuaGet(L, key);
	}

	bool LuaSet(lua_State* L, const char* key, int idx) override {
	    if (!strcmp(key, "Size")) {
	        Size = *CheckUDim2(L, idx);
	        return true;
	    }

	    if (!strcmp(key, "Position")) {
	        Position = *CheckUDim2(L, idx);
	        return true;
	    }

	    if (!strcmp(key, "AnchorPoint")) {
	        auto* v = CheckVector2(L, idx);
	        AnchorPoint = { v->x, v->y };
	        return true;
	    }

	    if (!strcmp(key, "BackgroundColor3")) {
			LuaColor3* clr = CheckColor3(L, idx);
			BackgroundColor = {clr->r, clr->g, clr->b};

	        return true;
	    }

	    if (!strcmp(key, "BackgroundTransparency")) {
	        BackgroundTransparency = std::clamp((float)luaL_checknumber(L, idx), 0.0f, 1.0f);
	        return true;
	    }

	    if (!strcmp(key, "Visible")) {
	        Visible = lua_toboolean(L, idx);
	        return true;
	    }

	    return Instance::LuaSet(L, key, idx);
	}
};
