#pragma once

#include "lua.h"
#include "lualib.h"
#include "objects/Instance.h"
#include "Service.h"
#include "raylib.h"
#include <algorithm>
#include <cstring>

struct DebrisData {
	Instance* instance;
	double destroyTime;
};

struct Debris : Service {
	std::vector<DebrisData> items;

	Debris() {
		Name = "Debris";
	}

	void AddItem(Instance* instance, double lifetime) {
		items.push_back((DebrisData){
			instance,
			GetTime() + lifetime
		});
	}

	void UpdateDebris() {
		double time = GetTime();
		for( DebrisData& item : items) {
			if (item.destroyTime <= time) {
				if (item.instance) {

					if (!item.instance->ParentingLocked)
						item.instance->Destroy();
				}
			}
		}

		items.erase(std::remove_if(
			items.begin(),
			items.end(),
			[&](DebrisData& d) {
				return d.destroyTime <= GetTime();
			}
		), items.end());
	}

	const char* ClassName() const override {
		return "Debris";
	}

	static int l_AddItem(lua_State* L) {
		auto* serv = *(Debris**)luaL_checkudata(L, 1, "Instance");
		Instance* inst = CheckInstance(L, 2);

		double lifetime = luaL_optnumber(L, 3, 0);
		lifetime = (lifetime < 0 ? 0 : lifetime);

		serv->AddItem(inst, lifetime);

		return 0;
	}

	bool LuaGet(lua_State* L, const char* key) override {
		if (std::strcmp(key, "AddItem") == 0 || std::strcmp(key, "addItem") == 0) {
			lua_pushcfunction(L, l_AddItem, "Debris:AddItem");
			return true;
		}

		return Service::LuaGet(L, key);
	}
};
