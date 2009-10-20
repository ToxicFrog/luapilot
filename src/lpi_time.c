#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

int lpi_startTime(lua_State * L)
{
    PI_StartTime();
    return 0;
}

int lpi_endTime(lua_State * L)
{
    lua_pushnumber(L, PI_EndTime());
    return 1;
}
