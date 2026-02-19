#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

struct Instance;
static Instance* CheckInstance(lua_State* L, int index) {
    return *(Instance**)luaL_checkudata(L, index, "Instance");
}

static void PushInstance(lua_State* L, Instance* inst) {
    Instance** udata = (Instance**)lua_newuserdata(L, sizeof(Instance*));
    *udata = inst;

    luaL_getmetatable(L, "Instance");
    lua_setmetatable(L, -2);
}

struct Instance {
    std::string Name = "Instance";
    Instance* Parent = nullptr;
    std::vector<Instance*> Children;

	bool ParentingLocked = false;

    virtual ~Instance() = default;

    virtual const char* ClassName() const = 0;

	Instance* FindFirstChildOfClass(const char* className) {
		for (Instance* child : Children) {
			if (std::strcmp(child->ClassName(), className) == 0)
				return child;
		}
		
		return nullptr;
	}

	static int l_GetChildren(lua_State* L) {
    	Instance* obj = CheckInstance(L, 1);

    	lua_newtable(L);
    	int i = 1;

    	for (Instance* child : obj->Children) {
        	PushInstance(L, child);
        	lua_rawseti(L, -2, i++);
    	}

    	return 1;
	}

	static int l_GetDescendants(lua_State* L) {
	    Instance* obj = CheckInstance(L, 1);

	    std::vector<Instance*> stack;
	    std::vector<Instance*> descendants;

		for (Instance* child : obj->Children)
			stack.push_back(child);

		while (!stack.empty()) {
			Instance* current = stack.back();
			stack.pop_back();

			descendants.push_back(current);

			for (Instance* child : current->Children) {
				stack.push_back(child);
			}
		}

		lua_newtable(L);
		int i = 1;
		for (Instance* inst : descendants) {
		    PushInstance(L, inst);
		    lua_rawseti(L, -2, i++);
		}

		return 1;
	}

	static int l_FindFirstChild(lua_State* L) {
		Instance* obj = CheckInstance(L, 1);
		std::string str = luaL_checkstring(L, 1);

		for (Instance* child : obj->Children) {
			if (child->Name == str) {
				PushInstance(L, child);
				return 1;
			}
		}

		lua_pushnil(L);
		return 1;
	}

    virtual bool LuaGet(lua_State* L, const char* key) {

		// methods
		if (std::strcmp(key, "GetChildren") == 0) {
			lua_pushcfunction(L, l_GetChildren, "GetChildren");
			return true;
		}

		if (std::strcmp(key, "GetDescendants") == 0) {
			lua_pushcfunction(L, l_GetDescendants, "GetDescendants");
			return true;
		}

		if (std::strcmp(key, "Destroy") == 0) {
			lua_pushcfunction(L, l_Destroy, "Destroy");
			return true;
		}

		// properties
        if (std::strcmp(key, "Name") == 0) {
            lua_pushstring(L, Name.c_str());
            return true;
        }

        if (std::strcmp(key, "Parent") == 0){
            if (Parent)
                PushInstance(L, Parent);
            else
                lua_pushnil(L);
            return true;
        }

        if (std::strcmp(key, "ClassName") == 0) {
            lua_pushstring(L, ClassName());
            return true;
        }

        return false;
    }

    virtual bool LuaSet(lua_State* L, const char* key, int valueIndex) {
        if (strcmp(key, "Name") == 0) {
            Name = luaL_checkstring(L, valueIndex);
            return true;
        }

        if (strcmp(key, "Parent") == 0) {
			if (ParentingLocked) {
				luaL_error(L, "attempted to set parent of object whose parent is locked");
				return false;
			} else {
				Instance* newParent = CheckInstance(L, valueIndex);
            	SetParent(newParent);
			}
            
            return true;
        }

        return false;
    }

    void SetParent(Instance* newParent) {
		if (newParent == Parent) return;

	    // Remove from old parent
	    if (Parent) {
	        auto& siblings = Parent->Children;
	        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
	    }

	    Parent = newParent;

	    // Add to new parent
	    if (Parent)
	        Parent->Children.push_back(this);
	}

    virtual void Destroy() {
        SetParent(nullptr);
        delete this;
    }

	static int l_Destroy(lua_State* L) {
		Instance* obj = CheckInstance(L, 1);

		if (obj->ParentingLocked) return 0;
		obj->Destroy();

		return 0;
	}

	static int Index(lua_State* L) {
		Instance* obj = CheckInstance(L, 1);
		const char* key = luaL_checkstring(L, 2);

		if (obj->LuaGet(L, key))
			return 1;
		
		lua_pushnil(L);
		return 1;
	}

	static int NewIndex(lua_State* L) {
		Instance* obj = CheckInstance(L, 1);
		const char* key = luaL_checkstring(L, 2);

		if (!obj->LuaSet(L, key, 3))
			luaL_error(L, "invalid type for property '%s'", key);

		return 0;
	}

	static void SetupAPI(lua_State* L) {
    	luaL_newmetatable(L, "Instance");
		
    	lua_pushcfunction(L, Index, "__index"); lua_setfield(L, -2, "__index");
    	lua_pushcfunction(L, NewIndex, "__newindex"); lua_setfield(L, -2, "__newindex");
		//lua_pushcfunction(L, m_ToString, "__tostring"); lua_setfield(L, -2, "__tostring");

		//lua_pushcfunction(L, l_GetChildren, "GetChildren"); lua_setfield(L, -2, "GetChildren");
		//lua_pushcfunction(L, l_FindFirstChild, "FindFirstChild"); lua_setfield(L, -2, "FindFirstChild");
		//lua_pushcfunction(L, l_Destroy, "Destroy"); lua_setfield(L, -2, "Destroy");

    	lua_pop(L, 1);
	}
};
