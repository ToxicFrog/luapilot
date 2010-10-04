#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_util.h"
#include "lpi_process.h"
#include "lpi_names.h"
#include "lpi_error.h"

static int lpi_process_method_handle;

static int lpi_process_tostring(lua_State * L)
{
  PI_PROCESS * obj = *(PI_PROCESS **)luaL_checkudata(L, 1, "PI_PROCESS *");
  const char * name = LPI_CALL(L, PI_GetName, obj);
  lua_pushfstring(L, "%s '%s': %p", "PI_PROCESS", name, (void *)obj);
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
  luaL_checkudata(L, 1, "PI_PROCESS *");
  luaL_checkany(L, 2);
  luaL_checkany(L, 3);

  lua_getfenv(L, 1);
  lua_insert(L, -3);
  lua_settable(L, -3);
  return 0;
}

static int lpi_process_index(lua_State * L)
{
  luaL_checkudata(L, 1, "PI_PROCESS *");
  luaL_checkany(L, 2);

  lua_getfenv(L, 1);
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
    luaL_checkudata(L, 2, "PI_PROCESS *");

    lua_getfenv(L, 2);                      /* dtb self env */
    lua_getfield(L, 3, "__process");        /* dtb self env fn */
    lua_insert(L, 2);                       /* dtb fn self env */
    
    int nargs = lua_objlen(L, 4)+1;

    l_unpack(L, 4);                        /* dtb fn self env ... */
    lua_remove(L, 4);                      /* dtb fn self ... */
    
    int rv;
    if ((rv = lua_pcall(L, nargs, 1, 1)) != 0)
    {
        PI_Abort(0, lua_tostring(L, -1), __FILE__, __LINE__);
        return rv;
    }
    
    rv = lua_tointeger(L, -1);
    return rv;
}

PI_PROCESS ** lpi_process_push(lua_State * L)
{
    PI_PROCESS ** obj = lua_newuserdata(L, sizeof(PI_PROCESS *));
    luaL_newmetatable(L, "PI_PROCESS *");
    lua_setmetatable(L, -2);
    return obj;
}

static void lpi_process_environfy(lua_State * L)
{
    /* attach __index to the environment so that method lookups work */
    lua_newtable(L);                                /* env emt */
    lua_rawgeti(L, LUA_REGISTRYINDEX, lpi_process_method_handle);
                                                    /* env emt methods */
    lua_setfield(L, -2, "__index");                 /* env emt */
    lua_setmetatable(L, -2);                        /* env */
}

int lpi_process(lua_State * L)
{
    /* remove 'self' from stack */
    lua_remove(L, 1);
    
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
    PI_PROCESS ** obj = lpi_process_push(L);        /* fn env ud */
    
    /* attach __index to the environment so that method lookups work */
    lua_insert(L, 1);                               /* ud fn env */
    lpi_process_environfy(L);

    /* store the process function in the environment */
    lua_insert(L, 2);                               /* ud env fn */
    lua_setfield(L, -2, "__process");               /* ud env */

    /* assign the environment table */
    lua_setfenv(L, 1);

    /* store the wrapper itself in the registry so that the thunk can get it later */
    lua_pushvalue(L, 1);
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* create the process */
    *obj = LPI_CALL(L, PI_CreateProcess, lpi_processThunk, ref, L);

    return 1;
}

static const char * lpi_process_methods[] = {
    "getName", "getName",
    "setName", "setName",
    NULL
};

void lpi_process_init(lua_State * L)
{
    /* create metatable */
    luaL_newmetatable(L, "PI_PROCESS *");
    luaL_register(L, NULL, lpi_process_mt);
    lua_pop(L, 1);
    
    lpi_process_method_handle = lpi_methods(L, "process", lpi_process_methods, lpi_process);

    PI_PROCESS ** Pmain = lpi_process_push(L);
    lua_newtable(L);
    lpi_process_environfy(L);
    lua_setfenv(L, -2);
    
    lua_setfield(L, -2, "main");
    *Pmain = PI_MAIN;

    return;
}
