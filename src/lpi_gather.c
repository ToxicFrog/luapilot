#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_comms.h"
#include "lpi_error.h"

void lpi_gather_to_table(lua_State * L, PI_BUNDLE * bundle)
{
    size_t size = LPI_CALL(L, PI_GetBundleSize, bundle);
    int * types = malloc(sizeof(int) * size);
    lua_Number * magnitudes = malloc(sizeof(lua_Number) * size);

    LPI_CALL(L, PI_Gather, bundle, LPI_HEADER_FORMAT, types, magnitudes);

    for (int i = 0; i < size; ++i)
    {
        switch(types[i])
        {
            case LUA_TNIL:
                lua_pushnil(L);
                break;
            case LUA_TBOOLEAN:
                lua_pushboolean(L, (int)magnitudes[i]);
                break;
            case LUA_TNUMBER:
                lua_pushnumber(L, magnitudes[i]);
                break;
            default:
                luaL_error(L, "unable to gather type %s - only nil/boolean/number are permitted", lua_typename(L, types[i]));
                break;
        }
        lua_rawseti(L, -2, i+1);
    }
}
            
 
int lpi_gather(lua_State * L)
{
    PI_BUNDLE * bundle = *(PI_BUNDLE **)luaL_checkudata(L, 1, "PI_BUNDLE *");
    int n = luaL_optint(L, 2, 1);

    for (int i = 0; i < n; ++i)
    {
        lua_newtable(L);
        lpi_gather_to_table(L, bundle);
    }

    return n;
}
