lua_State *lua_newstate(lua_Alloc f, void *ud);
void lua_close(lua_State *L);
lua_State *lua_newthread(lua_State *L);
lua_CFunction lua_atpanic(lua_State *L, lua_CFunction panicf);
int lua_gettop(lua_State *L);
void lua_settop(lua_State *L, int idx);
void lua_pushvalue(lua_State *L, int idx);
void lua_remove(lua_State *L, int idx);
void lua_insert(lua_State *L, int idx);
void lua_replace(lua_State *L, int idx);
int lua_checkstack(lua_State *L, int sz);
void lua_xmove(lua_State *from, lua_State *to, int n);
int lua_isnumber(lua_State *L, int idx);
int lua_isstring(lua_State *L, int idx);
int lua_iscfunction(lua_State *L, int idx);
int lua_isuserdata(lua_State *L, int idx);
int lua_type(lua_State *L, int idx);
const char *lua_typename(lua_State *L, int tp);
int lua_equal(lua_State *L, int idx1, int idx2);
int lua_rawequal(lua_State *L, int idx1, int idx2);
int lua_lessthan(lua_State *L, int idx1, int idx2);
lua_Number lua_tonumber(lua_State *L, int idx);
lua_Integer lua_tointeger(lua_State *L, int idx);
int lua_toboolean(lua_State *L, int idx);
const char *lua_tolstring(lua_State *L, int idx, size_t *len);
size_t lua_objlen(lua_State *L, int idx);
lua_CFunction lua_tocfunction(lua_State *L, int idx);
void	 *lua_touserdata(lua_State *L, int idx);
lua_State *lua_tothread(lua_State *L, int idx);
const void *lua_topointer(lua_State *L, int idx);
void lua_pushnil(lua_State *L);
void lua_pushnumber(lua_State *L, lua_Number n);
void lua_pushinteger(lua_State *L, lua_Integer n);
void lua_pushlstring(lua_State *L, const char *s, size_t l);
void lua_pushstring(lua_State *L, const char *s);
void lua_pushcclosure(lua_State *L, lua_CFunction fn, int n);
void lua_pushboolean(lua_State *L, int b);
const char *lua_pushfstring(lua_State *L, const char *fmt, ...);
void lua_pushlightuserdata(lua_State *L, void *p);
int lua_pushthread(lua_State *L);
void lua_gettable(lua_State *L, int idx);
void lua_getfield(lua_State *L, int idx, const char *k);
void lua_rawget(lua_State *L, int idx);
void lua_rawgeti(lua_State *L, int idx, int n);
void lua_createtable(lua_State *L, int narr, int nrec);
void *lua_newuserdata(lua_State *L, size_t sz);
int lua_getmetatable(lua_State *L, int objindex);
void lua_getfenv(lua_State *L, int idx);
void lua_settable(lua_State *L, int idx);
void lua_setfield(lua_State *L, int idx, const char *k);
void lua_rawset(lua_State *L, int idx);
void lua_rawseti(lua_State *L, int idx, int n);
int lua_setmetatable(lua_State *L, int objindex);
int lua_setfenv(lua_State *L, int idx);
void lua_call(lua_State *L, int nargs, int nresults);
int lua_pcall(lua_State *L, int nargs, int nresults, int errfunc);
int lua_cpcall(lua_State *L, lua_CFunction func, void *ud);
int lua_load(lua_State *L, lua_Reader reader, void *dt, const char *chunkname);
int lua_dump(lua_State *L, lua_Writer writer, void *data);
int lua_yield(lua_State *L, int nresults);
int lua_resume(lua_State *L, int narg);
int lua_status(lua_State *L);
int lua_gc(lua_State *L, int what, int data);
int lua_error(lua_State *L);
int lua_next(lua_State *L, int idx);
void lua_concat(lua_State *L, int n);
int lua_sethook(lua_State *L, lua_Hook func, int mask, int count);
int luaL_callmeta(lua_State *L, int obj, const char *e);
int luaL_error(lua_State *L, const char *fmt, ...);
int luaL_loadbuffer(lua_State *L, const char *buff, size_t sz,const char *name);
int luaL_loadfile(lua_State *L, const char *filename);
lua_State *luaL_newstate();
void luaL_openlibs(lua_State *L);
void luaL_traceback(lua_State *L, lua_State *L1, const char *msg,int level);
