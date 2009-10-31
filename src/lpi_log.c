#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_error.h"

int lpi_log(lua_State * L)
{
    LPI_CALL(L, PI_Log, luaL_checkstring(L, 1));
    return 1;
}

int lpi_isLogging(lua_State * L)
{
    int il = LPI_CALL(L, PI_IsLogging);
    lua_pushboolean(L, il);
    return 1;
}

