#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

int lpi_process(lua_State * L);
void lpi_process_init(lua_State * L);
PI_PROCESS ** lpi_process_push(lua_State * L);
