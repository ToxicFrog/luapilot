#include <lua.h>
#include <luaconf.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_comms.h"

void lpi_write_header(PI_CHANNEL * chan, int t, lua_Number n)
{
  PI_Write(chan, LPI_HEADER_FORMAT, t, n);
}

void lpi_write_data(PI_CHANNEL * chan, const char * buf, size_t size)
{
  PI_Write(chan, "%*c", size, buf);
}

void lpi_write_object(lua_State * L, int index, PI_CHANNEL * chan)
{
  switch(lua_type(L, index))
  {
    case LUA_TNIL:
      lpi_write_header(chan, LUA_TNIL, 0.0);
      break;
    case LUA_TNUMBER:
      lpi_write_header(chan, LUA_TNUMBER, lua_tonumber(L, index));
      break;
    case LUA_TBOOLEAN:
      lpi_write_header(chan, LUA_TBOOLEAN, (lua_Number)lua_toboolean(L, index));
      break;
    case LUA_TSTRING:
      lpi_write_header(chan, LUA_TSTRING, (lua_Number)lua_objlen(L, index));
      lpi_write_data(chan, lua_tostring(L, index), lua_objlen(L, index));
      break;
    default:
      luaL_typerror(L, index, "nil, number, boolean, or string");
      break;
  }
}

int lpi_write(lua_State * L)
{
  PI_CHANNEL * chan = *(PI_CHANNEL **)luaL_checkudata(L, 1, "PI_CHANNEL *");

  for (int i = 2; i <= lua_gettop(L); ++i)
  {
    lpi_write_object(L, i, chan);
  }

  return 0;
}
