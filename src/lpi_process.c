#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_util.h"
#include "lpi_process.h"

static int lpi_process_tostring(lua_State * L)
{
  luaL_checkudata(L, 1, "PI_PROCESS *");
  lua_pushfstring(L, "%s: %p", "PI_PROCESS", lua_topointer(L, 1));
  return 1;
}

static int lpi_process_type(lua_State * L)
{
  luaL_checkudata(L, 1, "PI_PROCESS *");
  lua_pushstring(L, "PI_PROCESS");
  return 1;
}

static int lpi_process_newindex(lua_State * L)
{
  lpi_Process * P = luaL_checkudata(L, 1, "PI_PROCESS *");
  luaL_checkany(L, 2);
  luaL_checkany(L, 3);

  lua_rawgeti(L, LUA_REGISTRYINDEX, P->env);
  lua_insert(L, 2);
  lua_settable(L, 2);

  return 0;
}

static int lpi_process_index(lua_State * L)
{
  lpi_Process * P = luaL_checkudata(L, 1, "PI_PROCESS *");
  luaL_checkany(L, 2);

  lua_rawgeti(L, LUA_REGISTRYINDEX, P->env);
  lua_insert(L, -2);
  lua_gettable(L, -2);
  return 1;
}

luaL_Reg lpi_process_mt[] = {
  { "__tostring",    lpi_process_tostring },
  { "__type",        lpi_process_type },
  { "__newindex",    lpi_process_newindex },
  { "__index",       lpi_process_index },
  { NULL, NULL }
};

static int lpi_processThunk(int n, void * L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "debug");
    lua_getfield(L, 1, "traceback");
    lua_remove(L, 1);
    
    lua_rawgeti(L, LUA_REGISTRYINDEX, n);   /* dtb self */

    lpi_Process * P = luaL_checkudata(L, 2, "PI_PROCESS *");
    
    lua_rawgeti(L, LUA_REGISTRYINDEX, P->fn);  /* dtb self fn */
    lua_insert(L, -2);                          /* dtb fn self */

    lua_rawgeti(L, LUA_REGISTRYINDEX, P->env); /* dtb fn self env  */

    int nargs = lua_objlen(L, 4)+1;

    l_unpack(L, 4);                        /* dtb fn self env ... */
    lua_remove(L, 4);                      /* dtb fn self ... */
    
    int rv;
    if ((rv = lua_pcall(L, nargs, 1, 1)) != 0)
    {
        //        fprintf(stderr, "In process %s: %s\n", PI_GetName(NULL), lua_tostring(L, -1));
        PI_Abort(0, lua_tostring(L, -1), __FILE__, __LINE__);
        return rv;
    }
    
    rv = lua_tointeger(L, -1);
    return rv;
}

lpi_Process * lpi_process_new(lua_State * L)
{
  lpi_Process * P = lua_newuserdata(L, sizeof(lpi_Process));
  luaL_newmetatable(L, "PI_PROCESS *");
  lua_setmetatable(L, -2);

  return P;
}

int lpi_process(lua_State * L)
{
    /* verify first argument */
    luaL_checktype(L, 1, LUA_TFUNCTION);
    
    /* pack all other arguments into the environment table */
    lua_newtable(L);
    while (lua_gettop(L) > 2)
    {
        lua_pushvalue(L, 2);
        lua_rawseti(L, -2, lua_objlen(L, -2)+1);
        lua_remove(L, 2);
    }
    
    /* create the PI_PROCESS wrapper */
    lpi_Process * P = lpi_process_new(L);
    lua_insert(L, 1);

    /* store the environment and process function in it */
    P->env = luaL_ref(L, LUA_REGISTRYINDEX);
    P->fn = luaL_ref(L, LUA_REGISTRYINDEX);

    /* store the wrapper itself in the registry so that the thunk can get it later */
    lua_pushvalue(L, 1);
    int n = luaL_ref(L, LUA_REGISTRYINDEX);

    /* create the PI_PROCESS */
    P->proc = PI_CreateProcess(lpi_processThunk, n, L);

    return 1;
}

void lpi_process_init(lua_State * L)
{
  luaL_newmetatable(L, "PI_PROCESS *");
  luaL_register(L, NULL, lpi_process_mt);
  lua_pop(L, 1);
  return;
}
