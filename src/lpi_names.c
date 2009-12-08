#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_error.h"
#include "lpi_util.h"

/**
 * lpi_setName: PI_SetName
 * pilot.setName(PI_OBJECT obj, [string name])
 *
 * Sets the name of a process, channel, or bundle. If name is omitted, sets
 * the name to ''.
 **/
int lpi_setName(lua_State * L)
{
  void * obj = lpi_pipointer(L, 1);
  if (!obj) luaL_typerror(L, 1, "PI_PROCESS, PI_BUNDLE or PI_CHANNEL");

  const char * name = luaL_optstring(L, 2, NULL);

  LPI_CALL(L, PI_SetName, obj, name);

  return 0;
}

/**
 * lpi_getName: PI_GetName
 * pilot.getName([PI_OBJECT obj])
 *
 * Returns the name of a process, channel, or bundle. If obj is omitted,
 * returns the name of the current process.
 **/
int lpi_getName(lua_State * L)
{
    void * obj;
    if (lua_topointer(L, 1) == NULL)
        obj = NULL;
    else
        obj = lpi_pipointer(L, 1);

    const char * name = LPI_CALL(L, PI_GetName, obj);
    lua_pushstring(L, name);

    return 1;
}
