#define DEF_ERROR_TEXT

#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>
#include <pilot_error.h>

void lpi_error(lua_State * L, int n)
{
    luaL_error(L, "LuaPilot: error %d raised by pilot library: %s", n, ErrorText[n]);
}
