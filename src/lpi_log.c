#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

int lpi_log(lua_State * L)
{
    PI_Log(luaL_checkstring(L, 1));
    return 1;
}

int lpi_isLogging(lua_State * L)
{
    lua_pushboolean(L, PI_IsLogging());
    return 1;
}

