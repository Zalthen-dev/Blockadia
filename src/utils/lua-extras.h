#define lua_setcfunctionfield(L, idx, f, d, k) lua_pushcfunction(L, f, d); lua_setfield(L, idx, k)
#define lua_setcfunctionglobal(L, f, d, k) lua_pushcfunction(L, f, d); lua_setglobal(L, k)

#define lua_setinstfield(L, idx, inst, k) PushInstance(L, inst); lua_setfield(L, idx, k)
#define lua_setinstglobal(L, inst, k) PushInstance(L, inst); lua_setglobal(L, k)

#define lua_checkfloat(L, numArg) (float)luaL_checknumber(L, numArg)
#define lua_checkint(L, numArg) (int)luaL_checknumber(L, numArg)