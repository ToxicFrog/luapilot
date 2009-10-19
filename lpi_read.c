#include <stdlib.h>
#include <lua.h>
#include <luaconf.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_comms.h"

void lpi_read_header(PI_CHANNEL * chan, int * t, lua_Number * n)
{
  PI_Read(chan, LPI_HEADER_FORMAT, t, n);
}

char * lpi_read_data(PI_CHANNEL * chan, size_t size)
{
  char * buf = malloc(size);
  PI_Read(chan, "%*c", size, buf);
  return buf;
}

void lpi_read_object(lua_State * L, PI_CHANNEL * chan)
{
  int t; lua_Number n; char * buf;

  lpi_read_header(chan, &t, &n);

  switch(t)
    {
    case LUA_TNIL:
      lua_pushnil(L);
      break;
    case LUA_TBOOLEAN:
      lua_pushboolean(L, (int)n);
      break;
    case LUA_TNUMBER:
      lua_pushnumber(L, n);
      break;
    case LUA_TSTRING:
      buf = lpi_read_data(chan, (size_t)n);
      lua_pushlstring(L, buf, (size_t)n);
      free(buf);
      break;
    default:
      luaL_error("Unknown type on the wire: %s", lua_typename(L, t));
      break;
    }

  return;
}

int lpi_read(lua_State * L)
{
  PI_CHANNEL * chan = *(PI_CHANNEL **)luaL_checkudata(L, 1, "PI_CHANNEL *");
  int n = luaL_optint(L, 2, 1);

  for (int i = 0; i < n; ++i)
  {
    lpi_read_object(L, chan);
  }

  return n;
}
