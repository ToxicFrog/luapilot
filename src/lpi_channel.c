#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_process.h"
#include "lpi_read.h"
#include "lpi_write.h"
#include "lpi_names.h"

static void lpi_channel_push(lua_State * L, PI_CHANNEL * channel)
{
  PI_CHANNEL ** obj = lua_newuserdata(L, sizeof(PI_CHANNEL *));
  luaL_newmetatable(L, "PI_CHANNEL *");
  lua_setmetatable(L, -2);
  *obj = channel;
  return;
}

int lpi_channel(lua_State * L)
{
  lpi_Process * from = luaL_checkudata(L, 1, "PI_PROCESS *");
  lpi_Process * to = luaL_checkudata(L, 2, "PI_PROCESS *");

  PI_CHANNEL * channel = PI_CreateChannel(from->proc, to->proc);
  lpi_channel_push(L, channel);
  return 1;
}

int lpi_channelHasData(lua_State * L)
{
  PI_CHANNEL * obj = *(PI_CHANNEL **)luaL_checkudata(L, 1, "PI_CHANNEL *");
  lua_pushboolean(L, PI_ChannelHasData(obj));
  return 1;
}

static int lpi_channel_tostring(lua_State * L)
{
  PI_CHANNEL * obj = *(PI_CHANNEL **)luaL_checkudata(L, 1, "PI_CHANNEL *");
  //  lua_pushfstring(L, "%s '%s': %p", "PI_CHANNEL", PI_GetName(*obj), (void *)obj);
  lua_pushfstring(L, "%s: %p", "PI_CHANNEL", (void *)obj);
  return 1;
}

static int lpi_channel_type(lua_State * L)
{
  luaL_checkudata(L, 1, "PI_CHANNEL *");
  lua_pushstring(L, "PI_CHANNEL");
  return 1;
}

luaL_Reg lpi_channel_mt[] = {
  { "__tostring", lpi_channel_tostring },
  { "__type", lpi_channel_type },
  { NULL, NULL }
};

luaL_Reg lpi_channel_methods[] = {
    { "hasData", lpi_channelHasData },
    { "read", lpi_read },
    { "write", lpi_write },
    { "setName", lpi_setName },
    { "getName", lpi_getName },
    { NULL, NULL }
};

void lpi_channel_init(lua_State * L)
{
  luaL_newmetatable(L, "PI_CHANNEL *");
  luaL_register(L, NULL, lpi_channel_mt);
  lua_newtable(L);
  luaL_register(L, NULL, lpi_channel_methods);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
  return;
}
