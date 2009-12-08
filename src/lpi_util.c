#include <lua.h>
#include <lauxlib.h>

#include "lpi_process.h"

void l_unpack(lua_State * L, int index)
{
    int n = lua_objlen(L, index);
    for (int i = 1; i <= n; ++i)
    {
        lua_rawgeti(L, index, i);
    }
}

int l_checkutype(lua_State * L, int index, const char * type)
{
    lua_getmetatable(L, index);
    luaL_newmetatable(L, type);
    int rv = lua_equal(L, -1, -2);
    lua_pop(L, 2);
    return rv;
}

void * lpi_pipointer(lua_State * L, int index)
{
    if (!lua_touserdata(L, index)
        || !(l_checkutype(L, index, "PI_PROCESS *")
        || l_checkutype(L, index, "PI_CHANNEL *")
        || l_checkutype(L, index, "PI_BUNDLE *")))
    {
        luaL_error(L, "luapilot: object at stack index %d is not a PI_OBJECT* but rather a %s", index, luaL_typename(L, index));
        return NULL;
    }

    return *((void **)lua_touserdata(L, index));
}

int lpi_methods(lua_State * L, const char * name, const char ** methods, lua_CFunction constructor)
{
    lua_newtable(L); /* pilot methods */
    for (int i = 0; methods[i] != NULL; i += 2)
    {
        lua_getfield(L, -2, methods[i+1]);    /* pilot methods method */
        lua_setfield(L, -2, methods[i]);  /* pilot methods */
    }
    
    lua_newtable(L);                        /* pilot methods mt */
    lua_pushcfunction(L, constructor);      /* pilot methods mt cons */
    lua_setfield(L, -2, "__call");          /* pilot methods mt */
    lua_setmetatable(L, -2);                /* pilot methods */
    
    lua_pushvalue(L, -1);
    int n = luaL_ref(L, LUA_REGISTRYINDEX);
    
    lua_setfield(L, -2, name);              /* pilot */
    
    return n;
}
