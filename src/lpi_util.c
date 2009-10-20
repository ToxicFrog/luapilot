#include <lua.h>
#include <lauxlib.h>

#include "lpi_process.h"

void l_unpack(lua_State * L, int index)
{
    int n = lua_objlen(L, index);
    for (int i = 1; i <= n; ++i)
    {
        lua_rawgeti(L, index, i);
    }
}

int l_checkutype(lua_State * L, int index, const char * type)
{
  lua_getmetatable(L, index);
  luaL_newmetatable(L, type);
  int rv = lua_equal(L, -1, -2);
  lua_pop(L, 2);
  return rv;
}

void * lpi_pipointer(lua_State * L, int index)
{
  void * obj = NULL;

  if (!lua_touserdata(L, index))
    return NULL;
  if (l_checkutype(L, index, "PI_PROCESS *"))
    obj = ((lpi_Process *)lua_touserdata(L, index))->proc;
  else if (l_checkutype(L, index, "PI_CHANNEL *") || l_checkutype(L, index, "PI_BUNDLE *"))
    obj = *((void **)lua_touserdata(L, index));

  return obj;
}
