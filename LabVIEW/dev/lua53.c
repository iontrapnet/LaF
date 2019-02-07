#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int luaopen_eris(lua_State*);

static const luaL_Reg loadedlibs[] = {
  {"_G", luaopen_base},
  {"package", luaopen_package},
  {"coroutine", luaopen_coroutine},
  {"table", luaopen_table},
  {"io", luaopen_io},
  {"os", luaopen_os},
  {"string", luaopen_string},
  {"math", luaopen_math},
  {"utf8", luaopen_utf8},
  {"debug", luaopen_debug},
  {"bit32", luaopen_bit32},
  {"eris", luaopen_eris},
  {NULL, NULL}
};

LUALIB_API void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib;
  for (lib = loadedlibs; lib->func; lib++) {
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1);
  }
}

#undef lua_insert
LUA_API void lua_insert(lua_State *L, int i) {
    lua_rotate(L, i, 1);
}

#undef lua_pcall
LUA_API int lua_pcall(lua_State *L, int n, int r, int f) {
    return lua_pcallk(L, n, r, f, 0, NULL);
}

#undef lua_tointeger
LUA_API lua_Integer lua_tointeger(lua_State *L, int i) {
    return lua_tointegerx(L, i, NULL);
}

#undef lua_tonumber
LUA_API lua_Number lua_tonumber(lua_State *L, int i) {
    return lua_tonumberx(L, i, NULL);
}

#undef lua_yield
LUA_API int lua_yield(lua_State *L, int n) {
    return lua_yieldk(L, n, 0, NULL);
}