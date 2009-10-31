#include <lua.h>
#include <luaconf.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_comms.h"
#include "lpi_error.h"

void lpi_broadcast_header(lua_State * L, PI_BUNDLE * bundle, int t, lua_Number n)
{
    LPI_CALL(L, PI_Broadcast, bundle, LPI_HEADER_FORMAT, t, n);
}

void lpi_broadcast_data(lua_State * L, PI_BUNDLE * bundle, const char * buf, size_t size)
{
    LPI_CALL(L, PI_Broadcast, bundle, "%*c", size, buf);
}

static int lpi_functionWriter(lua_State * L, const void * buf, size_t size, void * unused)
{
    lua_pushlstring(L, buf, size);
    return 0;
}

void lpi_broadcast_object(lua_State *, int, PI_BUNDLE *);

void lpi_broadcast_table(lua_State * L, int index, PI_BUNDLE * bundle)
{
    lpi_broadcast_header(L, bundle, LUA_TTABLE, LPI_TABLE_START);

    lua_pushnil(L);
    while (lua_next(L, index))
    {
        lpi_broadcast_object(L, lua_gettop(L)-1, bundle);  /* write key */
        lpi_broadcast_object(L, lua_gettop(L), bundle);    /* write value */
        lua_pop(L, 1);
    }

    lpi_broadcast_header(L, bundle, LUA_TTABLE, LPI_TABLE_END);
}

void lpi_broadcast_object(lua_State * L, int index, PI_BUNDLE * bundle)
{
    int n;


    if (luaL_getmetafield(L, index, "__recv"))
    {
        /* transmit __recv */
        lpi_broadcast_header(L, bundle, LUA_TUSERDATA, (lua_Number)0);
        lpi_broadcast_object(L, lua_gettop(L), bundle);
        lua_pop(L, 1);
    }
        
    
    if (luaL_getmetafield(L, index, "__send"))
    {
        lua_pushvalue(L, index);
        lua_call(L, 1, 1);
        lpi_broadcast_object(L, lua_gettop(L), bundle);
        lua_pop(L, 1);
        return;
    }
            
    
    switch(lua_type(L, index))
    {
        case LUA_TNIL:
            lpi_broadcast_header(L, bundle, LUA_TNIL, (lua_Number)0);
            break;
        case LUA_TNUMBER:
            lpi_broadcast_header(L, bundle, LUA_TNUMBER, lua_tonumber(L, index));
            break;
        case LUA_TBOOLEAN:
            lpi_broadcast_header(L, bundle, LUA_TBOOLEAN, (lua_Number)lua_toboolean(L, index));
            break;
        case LUA_TSTRING:
            lpi_broadcast_header(L, bundle, LUA_TSTRING, (lua_Number)lua_objlen(L, index));
            lpi_broadcast_data(L, bundle, lua_tostring(L, index), lua_objlen(L, index));
            break;
        case LUA_TFUNCTION:
            lua_pushvalue(L, index);
            n = lua_gettop(L);
            lua_dump(L, lpi_functionWriter, NULL);
            n = lua_gettop(L) - n;
            lua_concat(L, n);
            lpi_broadcast_header(L, bundle, LUA_TFUNCTION, (lua_Number)lua_objlen(L, -1));
            lpi_broadcast_data(L, bundle, lua_tostring(L, -1), lua_objlen(L, -1));
            lua_pop(L, 2);
            break;
        case LUA_TTABLE:
            lpi_broadcast_table(L, index, bundle);
            break;
        default:
            luaL_typerror(L, index, "nil, number, boolean, or string");
            break;
    }
}

int lpi_broadcast(lua_State * L)
{
    PI_BUNDLE * bundle = *(PI_BUNDLE **)luaL_checkudata(L, 1, "PI_BUNDLE *");

    for (int i = 2; i <= lua_gettop(L); ++i)
    {
        lpi_broadcast_object(L, i, bundle);
    }

    return 0;
}
