#include "callf.h"
ptr_t _lua_newstate,_lua_close,_lua_newthread,_lua_atpanic,_lua_gettop,_lua_settop,_lua_pushvalue,_lua_remove,_lua_insert,_lua_replace,_lua_checkstack,_lua_xmove,_lua_isnumber,_lua_isstring,_lua_iscfunction,_lua_isuserdata,_lua_type,_lua_typename,_lua_equal,_lua_rawequal,_lua_lessthan,_lua_tonumber,_lua_tointeger,_lua_toboolean,_lua_tolstring,_lua_objlen,_lua_tocfunction,_lua_touserdata,_lua_tothread,_lua_topointer,_lua_pushnil,_lua_pushnumber,_lua_pushinteger,_lua_pushlstring,_lua_pushstring,_lua_pushcclosure,_lua_pushboolean,_lua_pushfstring,_lua_pushlightuserdata,_lua_pushthread,_lua_gettable,_lua_getfield,_lua_rawget,_lua_rawgeti,_lua_createtable,_lua_newuserdata,_lua_getmetatable,_lua_getfenv,_lua_settable,_lua_setfield,_lua_rawset,_lua_rawseti,_lua_setmetatable,_lua_setfenv,_lua_call,_lua_pcall,_lua_cpcall,_lua_load,_lua_dump,_lua_yield,_lua_resume,_lua_status,_lua_gc,_lua_error,_lua_next,_lua_concat,_lua_sethook,_luaL_callmeta,_luaL_error,_luaL_loadbuffer,_luaL_loadfile,_luaL_newstate,_luaL_openlibs,_luaL_traceback;
API void lua51_init() {
ptr_t lib;
#define CALLF(...) dllc_callf(__VA_ARGS__)
dllc_init("127.0.0.1",8096);
CALLF(1,"s=i","lua51.dll",&lib);
CALLF(2,"is=i",lib,"lua_newstate",&_lua_newstate);
CALLF(2,"is=i",lib,"lua_close",&_lua_close);
CALLF(2,"is=i",lib,"lua_newthread",&_lua_newthread);
CALLF(2,"is=i",lib,"lua_atpanic",&_lua_atpanic);
CALLF(2,"is=i",lib,"lua_gettop",&_lua_gettop);
CALLF(2,"is=i",lib,"lua_settop",&_lua_settop);
CALLF(2,"is=i",lib,"lua_pushvalue",&_lua_pushvalue);
CALLF(2,"is=i",lib,"lua_remove",&_lua_remove);
CALLF(2,"is=i",lib,"lua_insert",&_lua_insert);
CALLF(2,"is=i",lib,"lua_replace",&_lua_replace);
CALLF(2,"is=i",lib,"lua_checkstack",&_lua_checkstack);
CALLF(2,"is=i",lib,"lua_xmove",&_lua_xmove);
CALLF(2,"is=i",lib,"lua_isnumber",&_lua_isnumber);
CALLF(2,"is=i",lib,"lua_isstring",&_lua_isstring);
CALLF(2,"is=i",lib,"lua_iscfunction",&_lua_iscfunction);
CALLF(2,"is=i",lib,"lua_isuserdata",&_lua_isuserdata);
CALLF(2,"is=i",lib,"lua_type",&_lua_type);
CALLF(2,"is=i",lib,"lua_typename",&_lua_typename);
CALLF(2,"is=i",lib,"lua_equal",&_lua_equal);
CALLF(2,"is=i",lib,"lua_rawequal",&_lua_rawequal);
CALLF(2,"is=i",lib,"lua_lessthan",&_lua_lessthan);
CALLF(2,"is=i",lib,"lua_tonumber",&_lua_tonumber);
CALLF(2,"is=i",lib,"lua_tointeger",&_lua_tointeger);
CALLF(2,"is=i",lib,"lua_toboolean",&_lua_toboolean);
CALLF(2,"is=i",lib,"lua_tolstring",&_lua_tolstring);
CALLF(2,"is=i",lib,"lua_objlen",&_lua_objlen);
CALLF(2,"is=i",lib,"lua_tocfunction",&_lua_tocfunction);
CALLF(2,"is=i",lib,"lua_touserdata",&_lua_touserdata);
CALLF(2,"is=i",lib,"lua_tothread",&_lua_tothread);
CALLF(2,"is=i",lib,"lua_topointer",&_lua_topointer);
CALLF(2,"is=i",lib,"lua_pushnil",&_lua_pushnil);
CALLF(2,"is=i",lib,"lua_pushnumber",&_lua_pushnumber);
CALLF(2,"is=i",lib,"lua_pushinteger",&_lua_pushinteger);
CALLF(2,"is=i",lib,"lua_pushlstring",&_lua_pushlstring);
CALLF(2,"is=i",lib,"lua_pushstring",&_lua_pushstring);
CALLF(2,"is=i",lib,"lua_pushcclosure",&_lua_pushcclosure);
CALLF(2,"is=i",lib,"lua_pushboolean",&_lua_pushboolean);
CALLF(2,"is=i",lib,"lua_pushfstring",&_lua_pushfstring);
CALLF(2,"is=i",lib,"lua_pushlightuserdata",&_lua_pushlightuserdata);
CALLF(2,"is=i",lib,"lua_pushthread",&_lua_pushthread);
CALLF(2,"is=i",lib,"lua_gettable",&_lua_gettable);
CALLF(2,"is=i",lib,"lua_getfield",&_lua_getfield);
CALLF(2,"is=i",lib,"lua_rawget",&_lua_rawget);
CALLF(2,"is=i",lib,"lua_rawgeti",&_lua_rawgeti);
CALLF(2,"is=i",lib,"lua_createtable",&_lua_createtable);
CALLF(2,"is=i",lib,"lua_newuserdata",&_lua_newuserdata);
CALLF(2,"is=i",lib,"lua_getmetatable",&_lua_getmetatable);
CALLF(2,"is=i",lib,"lua_getfenv",&_lua_getfenv);
CALLF(2,"is=i",lib,"lua_settable",&_lua_settable);
CALLF(2,"is=i",lib,"lua_setfield",&_lua_setfield);
CALLF(2,"is=i",lib,"lua_rawset",&_lua_rawset);
CALLF(2,"is=i",lib,"lua_rawseti",&_lua_rawseti);
CALLF(2,"is=i",lib,"lua_setmetatable",&_lua_setmetatable);
CALLF(2,"is=i",lib,"lua_setfenv",&_lua_setfenv);
CALLF(2,"is=i",lib,"lua_call",&_lua_call);
CALLF(2,"is=i",lib,"lua_pcall",&_lua_pcall);
CALLF(2,"is=i",lib,"lua_cpcall",&_lua_cpcall);
CALLF(2,"is=i",lib,"lua_load",&_lua_load);
CALLF(2,"is=i",lib,"lua_dump",&_lua_dump);
CALLF(2,"is=i",lib,"lua_yield",&_lua_yield);
CALLF(2,"is=i",lib,"lua_resume",&_lua_resume);
CALLF(2,"is=i",lib,"lua_status",&_lua_status);
CALLF(2,"is=i",lib,"lua_gc",&_lua_gc);
CALLF(2,"is=i",lib,"lua_error",&_lua_error);
CALLF(2,"is=i",lib,"lua_next",&_lua_next);
CALLF(2,"is=i",lib,"lua_concat",&_lua_concat);
CALLF(2,"is=i",lib,"lua_sethook",&_lua_sethook);
CALLF(2,"is=i",lib,"luaL_callmeta",&_luaL_callmeta);
CALLF(2,"is=i",lib,"luaL_error",&_luaL_error);
CALLF(2,"is=i",lib,"luaL_loadbuffer",&_luaL_loadbuffer);
CALLF(2,"is=i",lib,"luaL_loadfile",&_luaL_loadfile);
CALLF(2,"is=i",lib,"luaL_newstate",&_luaL_newstate);
CALLF(2,"is=i",lib,"luaL_openlibs",&_luaL_openlibs);
CALLF(2,"is=i",lib,"luaL_traceback",&_luaL_traceback);
}
API int32 lua_newstate(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_newstate,"ii=i",x1,x2,&x0);
return x0;
}
API void lua_close(int32 x1) {
CALLF(_lua_close,"i=",x1);
}
API int32 lua_newthread(int32 x1) {
int32 x0;
CALLF(_lua_newthread,"i=i",x1,&x0);
return x0;
}
API int32 lua_atpanic(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_atpanic,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_gettop(int32 x1) {
int32 x0;
CALLF(_lua_gettop,"i=i",x1,&x0);
return x0;
}
API void lua_settop(int32 x1,int32 x2) {
CALLF(_lua_settop,"ii=",x1,x2);
}
API void lua_pushvalue(int32 x1,int32 x2) {
CALLF(_lua_pushvalue,"ii=",x1,x2);
}
API void lua_remove(int32 x1,int32 x2) {
CALLF(_lua_remove,"ii=",x1,x2);
}
API void lua_insert(int32 x1,int32 x2) {
CALLF(_lua_insert,"ii=",x1,x2);
}
API void lua_replace(int32 x1,int32 x2) {
CALLF(_lua_replace,"ii=",x1,x2);
}
API int32 lua_checkstack(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_checkstack,"ii=i",x1,x2,&x0);
return x0;
}
API void lua_xmove(int32 x1,int32 x2,int32 x3) {
CALLF(_lua_xmove,"iii=",x1,x2,x3);
}
API int32 lua_isnumber(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_isnumber,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_isstring(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_isstring,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_iscfunction(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_iscfunction,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_isuserdata(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_isuserdata,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_type(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_type,"ii=i",x1,x2,&x0);
return x0;
}
API char* lua_typename(int32 x1,int32 x2) {
char* x0;
CALLF(_lua_typename,"ii=s",x1,x2,&x0);
return x0;
}
API int32 lua_equal(int32 x1,int32 x2,int32 x3) {
int32 x0;
CALLF(_lua_equal,"iii=i",x1,x2,x3,&x0);
return x0;
}
API int32 lua_rawequal(int32 x1,int32 x2,int32 x3) {
int32 x0;
CALLF(_lua_rawequal,"iii=i",x1,x2,x3,&x0);
return x0;
}
API int32 lua_lessthan(int32 x1,int32 x2,int32 x3) {
int32 x0;
CALLF(_lua_lessthan,"iii=i",x1,x2,x3,&x0);
return x0;
}
API real64 lua_tonumber(int32 x1,int32 x2) {
real64 x0;
CALLF(_lua_tonumber,"ii=d",x1,x2,&x0);
return x0;
}
API int32 lua_tointeger(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_tointeger,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_toboolean(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_toboolean,"ii=i",x1,x2,&x0);
return x0;
}
API char* lua_tolstring(int32 x1,int32 x2,int32* x3) {
char* x0;
CALLF(_lua_tolstring,"iiI=s",x1,x2,x3,&x0);
return x0;
}
API int32 lua_objlen(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_objlen,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_tocfunction(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_tocfunction,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_touserdata(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_touserdata,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_tothread(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_tothread,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_topointer(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_topointer,"ii=i",x1,x2,&x0);
return x0;
}
API void lua_pushnil(int32 x1) {
CALLF(_lua_pushnil,"i=",x1);
}
API void lua_pushnumber(int32 x1,real64 x2) {
CALLF(_lua_pushnumber,"id=",x1,x2);
}
API void lua_pushinteger(int32 x1,int32 x2) {
CALLF(_lua_pushinteger,"ii=",x1,x2);
}
API void lua_pushlstring(int32 x1,char* x2,int32 x3) {
CALLF(_lua_pushlstring,"isi=",x1,x2,x3);
}
API void lua_pushstring(int32 x1,char* x2) {
CALLF(_lua_pushstring,"is=",x1,x2);
}
API void lua_pushcclosure(int32 x1,int32 x2,int32 x3) {
CALLF(_lua_pushcclosure,"iii=",x1,x2,x3);
}
API void lua_pushboolean(int32 x1,int32 x2) {
CALLF(_lua_pushboolean,"ii=",x1,x2);
}
API char* lua_pushfstring(int32 x1,char* x2) {
char* x0;
CALLF(_lua_pushfstring,"is.=s",x1,x2,&x0);
return x0;
}
API void lua_pushlightuserdata(int32 x1,int32 x2) {
CALLF(_lua_pushlightuserdata,"ii=",x1,x2);
}
API int32 lua_pushthread(int32 x1) {
int32 x0;
CALLF(_lua_pushthread,"i=i",x1,&x0);
return x0;
}
API void lua_gettable(int32 x1,int32 x2) {
CALLF(_lua_gettable,"ii=",x1,x2);
}
API void lua_getfield(int32 x1,int32 x2,char* x3) {
CALLF(_lua_getfield,"iis=",x1,x2,x3);
}
API void lua_rawget(int32 x1,int32 x2) {
CALLF(_lua_rawget,"ii=",x1,x2);
}
API void lua_rawgeti(int32 x1,int32 x2,int32 x3) {
CALLF(_lua_rawgeti,"iii=",x1,x2,x3);
}
API void lua_createtable(int32 x1,int32 x2,int32 x3) {
CALLF(_lua_createtable,"iii=",x1,x2,x3);
}
API int32 lua_newuserdata(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_newuserdata,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_getmetatable(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_getmetatable,"ii=i",x1,x2,&x0);
return x0;
}
API void lua_getfenv(int32 x1,int32 x2) {
CALLF(_lua_getfenv,"ii=",x1,x2);
}
API void lua_settable(int32 x1,int32 x2) {
CALLF(_lua_settable,"ii=",x1,x2);
}
API void lua_setfield(int32 x1,int32 x2,char* x3) {
CALLF(_lua_setfield,"iis=",x1,x2,x3);
}
API void lua_rawset(int32 x1,int32 x2) {
CALLF(_lua_rawset,"ii=",x1,x2);
}
API void lua_rawseti(int32 x1,int32 x2,int32 x3) {
CALLF(_lua_rawseti,"iii=",x1,x2,x3);
}
API int32 lua_setmetatable(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_setmetatable,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_setfenv(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_setfenv,"ii=i",x1,x2,&x0);
return x0;
}
API void lua_call(int32 x1,int32 x2,int32 x3) {
CALLF(_lua_call,"iii=",x1,x2,x3);
}
API int32 lua_pcall(int32 x1,int32 x2,int32 x3,int32 x4) {
int32 x0;
CALLF(_lua_pcall,"iiii=i",x1,x2,x3,x4,&x0);
return x0;
}
API int32 lua_cpcall(int32 x1,int32 x2,int32 x3) {
int32 x0;
CALLF(_lua_cpcall,"iii=i",x1,x2,x3,&x0);
return x0;
}
API int32 lua_load(int32 x1,int32 x2,int32 x3,char* x4) {
int32 x0;
CALLF(_lua_load,"iiis=i",x1,x2,x3,x4,&x0);
return x0;
}
API int32 lua_dump(int32 x1,int32 x2,int32 x3) {
int32 x0;
CALLF(_lua_dump,"iii=i",x1,x2,x3,&x0);
return x0;
}
API int32 lua_yield(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_yield,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_resume(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_resume,"ii=i",x1,x2,&x0);
return x0;
}
API int32 lua_status(int32 x1) {
int32 x0;
CALLF(_lua_status,"i=i",x1,&x0);
return x0;
}
API int32 lua_gc(int32 x1,int32 x2,int32 x3) {
int32 x0;
CALLF(_lua_gc,"iii=i",x1,x2,x3,&x0);
return x0;
}
API int32 lua_error(int32 x1) {
int32 x0;
CALLF(_lua_error,"i=i",x1,&x0);
return x0;
}
API int32 lua_next(int32 x1,int32 x2) {
int32 x0;
CALLF(_lua_next,"ii=i",x1,x2,&x0);
return x0;
}
API void lua_concat(int32 x1,int32 x2) {
CALLF(_lua_concat,"ii=",x1,x2);
}
API int32 lua_sethook(int32 x1,int32 x2,int32 x3,int32 x4) {
int32 x0;
CALLF(_lua_sethook,"iiii=i",x1,x2,x3,x4,&x0);
return x0;
}
API int32 luaL_callmeta(int32 x1,int32 x2,char* x3) {
int32 x0;
CALLF(_luaL_callmeta,"iis=i",x1,x2,x3,&x0);
return x0;
}
API int32 luaL_error(int32 x1,char* x2) {
int32 x0;
CALLF(_luaL_error,"is.=i",x1,x2,&x0);
return x0;
}
API int32 luaL_loadbuffer(int32 x1,char* x2,int32 x3,char* x4) {
int32 x0;
CALLF(_luaL_loadbuffer,"isis=i",x1,x2,x3,x4,&x0);
return x0;
}
API int32 luaL_loadfile(int32 x1,char* x2) {
int32 x0;
CALLF(_luaL_loadfile,"is=i",x1,x2,&x0);
return x0;
}
API int32 luaL_newstate() {
int32 x0;
CALLF(_luaL_newstate,"=i",&x0);
return x0;
}
API void luaL_openlibs(int32 x1) {
CALLF(_luaL_openlibs,"i=",x1);
}
API void luaL_traceback(int32 x1,int32 x2,char* x3,int32 x4) {
CALLF(_luaL_traceback,"iisi=",x1,x2,x3,x4);
}