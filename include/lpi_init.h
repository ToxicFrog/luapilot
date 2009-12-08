#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

int luaopen_pilot(lua_State *);
int luaopen_luapilot(lua_State *);
int lpi_init(lua_State *, int *, char ***);
