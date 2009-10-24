#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

int lpi_channel(lua_State * L);
int lpi_channelHasData(lua_State * L);
int lpi_copyChannels(lua_State * L);
void lpi_channel_init(lua_State * L);
