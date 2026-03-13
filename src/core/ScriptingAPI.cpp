#include "ScriptingAPI.h"

#include "raylib.h"

#include <string>
#include <cstring>

#include "lua.h"
#include "lualib.h"

#include "LuaScheduler.h"

#include "datatypes/LuaVector2.h"
#include "datatypes/LuaVector3.h"
#include "datatypes/LuaUDim.h"
#include "datatypes/LuaColor3.h"
#include "datatypes/LuaSignal.h"
#include "datatypes/LuaRandom.h"

#include "objects/Frame.h"
#include "objects/Instance.h"
#include "objects/Folder.h"
#include "objects/Part.h"
#include "objects/ScreenGui.h"
#include "objects/Script.h"
#include "objects/LocalScript.h"

#include "Game.h"
#include "objects/Sound.h"
#include "objects/TextLabel.h"
#include "services/Workspace.h"

extern Game* gGame;
extern Workspace* gWorkspace;

#define CreateAndPushObject(type) type* obj = new type{}; obj->Name = obj->ClassName(); PushInstance(L, obj)

static int l_Instance_new(lua_State* L) {
	const char* key = luaL_checkstring(L, 1);

	if (std::strcmp(key, "Part") == 0) {
		CreateAndPushObject(Part);
		return 1;
	}

	if (std::strcmp(key, "Sound") == 0) {
		CreateAndPushObject(ObjectSound);
		return 1;
	}

	if (std::strcmp(key, "Folder") == 0) {
		CreateAndPushObject(Folder);
		return 1;
	}

	if (std::strcmp(key, "ScreenGui") == 0) {
		CreateAndPushObject(ScreenGui);
		return 1;
	}

	if (std::strcmp(key, "Frame") == 0) {
		CreateAndPushObject(Frame);
		return 1;
	}

	if (std::strcmp(key, "TextLabel") == 0) {
		CreateAndPushObject(TextLabel);
		return 1;
	}

	if (std::strcmp(key, "Script") == 0) {
		CreateAndPushObject(Script);
		return 1;
	}
	if (std::strcmp(key, "LocalScript") == 0) {
		CreateAndPushObject(LocalScript);
		return 1;
	}

	luaL_errorL(L, "Could not create instance of type '%s'", key);
	return 0;
}

static int l_Time(lua_State* L) {
	lua_pushnumber(L, GetTime());
	return 1;
}

/*
void RestrictScriptingAPI(lua_State* L) {
	auto copyField = [&](const char* name) {
		lua_getfield(L, -2, name);
		if (!lua_isnil(L, -1)) {
			lua_setfield(L, -2, name);
		} else {
			lua_pop(L, 1);
		}
	};

	lua_getglobal(L, "os");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
	} else {
		copyField("clock");
		copyField("time");
		copyField("diffTime");
		copyField("date");

		lua_setglobal(L, "os");
		lua_pop(L, 1);
	}

	lua_getglobal(L, "io");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
	} else {
		lua_setglobal(L, "io");
		lua_pop(L, 1);
	}
}
*/

void SetScriptingAPI(lua_State* L) {
	RegisterTaskAPI(L);

	Instance::SetupAPI(L);
	RegisterVector3(L);
	RegisterVector2(L);
	RegisterUDim(L);
	RegisterUDim2(L);
	RegisterColor3(L);
	RegisterSignal(L);
	RegisterRandom(L);

	PushInstance(L, gGame); lua_setglobal(L, "game");
	PushInstance(L, gGame); lua_setglobal(L, "Game");

	PushInstance(L, gWorkspace); lua_setglobal(L, "workspace");

    lua_newtable(L);

    lua_pushcfunction(L, l_Instance_new, "Instance.new"); lua_setfield(L, -2, "new");
    

    lua_setglobal(L, "Instance");

	//RestrictScriptingAPI(L);

	lua_pushcfunction(L, l_Time, "time"); lua_setglobal(L, "time");

	// get rid of IO library (even if i think its not imported)
	lua_pushnil(L); lua_setglobal(L, "io");
}