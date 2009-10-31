#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_error.h"

int lpi_startTime(lua_State * L)
{
    LPI_CALL(L, PI_StartTime);
    return 0;
}

int lpi_endTime(lua_State * L)
{
    double et = LPI_CALL(L, PI_EndTime);
    lua_pushnumber(L, (lua_Number)et);
    return 1;
}
