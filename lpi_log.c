#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

int lpi_log(lua_State * L)
{
    lua_getglobal(L, "string");
    lua_getfield(L, -1, "format");
    lua_insert(L, 1);
    lua_pop(L, 1);
    lua_call(L, lua_gettop(L)-1, 1);
    PI_Log(lua_tostring(L, 1));
    return 1;
}

int lpi_isLogging(lua_State * L)
{
    lua_pushboolean(L, PI_IsLogging());
    return 1;
}

