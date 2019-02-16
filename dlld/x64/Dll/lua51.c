#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lua_State void
#define lua_Alloc void*
#define lua_CFunction void*
#define lua_Integer int
#define lua_Number double
#define lua_Reader void*
#define lua_Writer void*
#define lua_Hook void*
#define lua_Debug void*

#define LUA_GLOBALSINDEX	(-10002)
#define LUA_TNONE		(-1)
#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8
#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILRET 4
#define LUA_MASKCALL	(1 << LUA_HOOKCALL)
#define LUA_MASKRET	(1 << LUA_HOOKRET)
#define LUA_MASKLINE	(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT	(1 << LUA_HOOKCOUNT)

#include "lua51.h"

#define lua_pushcfunction(L,f)	lua_pushcclosure(L, (f), 0)
#define lua_setglobal(L,s)	lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_getglobal(L,s)	lua_getfield(L, LUA_GLOBALSINDEX, (s))
#define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)
#define lua_pop(L,n)		lua_settop(L, -(n)-1)
#define luaL_typename(L,i)	lua_typename(L, lua_type(L,(i)))

#if !defined(LUA_OK)
#define LUA_OK 0
#endif

#if !defined(lua_writestringerror)
#define lua_writestringerror(s,p) \
        (fprintf(stderr, (s), (p)), fflush(stderr))
#endif

static lua_State *globalL = NULL;

static void lstop (lua_State *L, lua_Debug *ar) {
  (void)ar;  /* unused arg. */
  lua_sethook(L, NULL, 0, 0);  /* reset hook */
  luaL_error(L, "interrupted!");
}

static void laction (int i) {
  signal(i, SIG_DFL); /* if another SIGINT happens, terminate process */
  lua_sethook(globalL, lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}

static void l_message (const char *pname, const char *msg) {
  if (pname) lua_writestringerror("%s: ", pname);
  lua_writestringerror("%s\n", msg);
}

static int report (lua_State *L, int status) {
  if (status != LUA_OK) {
    const char *msg = lua_tostring(L, -1);
    l_message("lua", msg);
    lua_pop(L, 1);  /* remove message */
  }
  return status;
}

static int msghandler (lua_State *L) {
  const char *msg = lua_tostring(L, 1);
  if (msg == NULL) {  /* is error object not a string? */
    if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
        lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
      return 1;  /* that is the message */
    else
      msg = lua_pushfstring(L, "(error object is a %s value)",
                               luaL_typename(L, 1));
  }
  luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
  return 1;  /* return the traceback */
}

static int docall (lua_State *L, int narg, int nres) {
  int status;
  int base = lua_gettop(L) - narg;  /* function index */
  lua_pushcfunction(L, msghandler);  /* push message handler */
  lua_insert(L, base);  /* put it under function and args */
  globalL = L;  /* to be available to 'laction' */
  signal(SIGINT, laction);  /* set C-signal handler */
  status = lua_pcall(L, narg, nres, base);
  signal(SIGINT, SIG_DFL); /* reset C-signal handler */
  lua_remove(L, base);  /* remove message handler from the stack */
  return status;
}

static void createargtable (lua_State *L, char **argv, int argc, int script) {
  int i, narg;
  if (script == argc) script = 0;  /* no script name? */
  narg = argc - (script + 1);  /* number of positive indices */
  lua_createtable(L, narg, script + 1);
  for (i = 0; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i - script);
  }
  lua_setglobal(L, "arg");
}

static int dochunk (lua_State *L, int status) {
  if (status == LUA_OK) status = docall(L, 0, 0);
  return report(L, status);
}

static int dofile (lua_State *L, const char *name) {
  return dochunk(L, luaL_loadfile(L, name));
}

static int dostring (lua_State *L, const char *s, const char *name) {
  return dochunk(L, luaL_loadbuffer(L, s, strlen(s), name));
}

static int dolibrary (lua_State *L, const char *name) {
  int status;
  lua_getglobal(L, "require");
  lua_pushstring(L, name);
  status = docall(L, 1, 1);  /* call 'require(name)' */
  if (status == LUA_OK)
    lua_setglobal(L, name);  /* global[name] = require return */
  return report(L, status);
}

/*static char buf[1024];
static int l_input(lua_State *L) {	
	fgets(buf,1024,stdin);
	lua_pushstring(L, buf);
	return 1;
}
static int l_output(lua_State *L) {
	fprintf(stdout,"%s",lua_tolstring(L,1,0));
	lua_settop(L,0);
	return 0;
}*/

void lua51_init();
int main(int argc, const char** argv) {
    lua_State *L;
	const char* input = "function input()print(string.char(255))return io.read()end";
	
	lua51_init();
	L = luaL_newstate();
    if (L == NULL) return -1;
    luaL_openlibs(L);
	/*lua_pushcfunction(L, l_input);
    lua_setglobal(L, "input");
	lua_pushcfunction(L, l_output);
    lua_setglobal(L, "output");*/
	luaL_loadbuffer(L, input, strlen(input), 0);
	lua_pcall(L, 0, 0, 0);
    createargtable(L, argv, argc, argc > 1 ? 1 : 0);
    if (argc > 1) {
        if (argv[1][0] == '=')
            dostring(L, argv[1] + 1, "main");
        else
            dofile(L, argv[1]);
    } else {
        dolibrary(L, "ilua");
    }
    lua_close(L);
    return 0;
}
