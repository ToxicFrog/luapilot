#include <lua.h>
#include <luaconf.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_comms.h"
#include "lpi_error.h"

void lpi_write_header(lua_State * L, PI_CHANNEL * chan, int t, lua_Number n)
{
    LPI_CALL(L, PI_Write, chan, LPI_HEADER_FORMAT, t, n);
}

void lpi_write_data(lua_State * L, PI_CHANNEL * chan, const char * buf, size_t size)
{
    if (size == 0) return;
    LPI_CALL(L, PI_Write, chan, "%*c", size, buf);
}

static int lpi_functionWriter(lua_State * L, const void * buf, size_t size, void * unused)
{
    lua_pushlstring(L, buf, size);
    return 0;
}

void lpi_write_object(lua_State *, int, PI_CHANNEL *);

void lpi_write_table(lua_State * L, int index, PI_CHANNEL * chan)
{
    lpi_write_header(L, chan, LUA_TTABLE, LPI_TABLE_START);

    lua_pushnil(L);
    while (lua_next(L, index))
    {
        lpi_write_object(L, lua_gettop(L)-1, chan);  /* write key */
        lpi_write_object(L, lua_gettop(L), chan);    /* write value */
        lua_pop(L, 1);
    }

    lpi_write_header(L, chan, LUA_TTABLE, LPI_TABLE_END);
}

void lpi_write_object(lua_State * L, int index, PI_CHANNEL * chan)
{
    int n;


    if (luaL_getmetafield(L, index, "__recv"))
    {
        /* transmit __recv */
        lpi_write_header(L, chan, LUA_TUSERDATA, (lua_Number)0);
        lpi_write_object(L, lua_gettop(L), chan);
        lua_pop(L, 1);
    }
        
    
    if (luaL_getmetafield(L, index, "__send"))
    {
        lua_pushvalue(L, index);
        lua_call(L, 1, 1);
        lpi_write_object(L, lua_gettop(L), chan);
        lua_pop(L, 1);
        return;
    }
            
    
    switch(lua_type(L, index))
    {
        case LUA_TNIL:
            lpi_write_header(L, chan, LUA_TNIL, (lua_Number)0);
            break;
        case LUA_TNUMBER:
            lpi_write_header(L, chan, LUA_TNUMBER, lua_tonumber(L, index));
            break;
        case LUA_TBOOLEAN:
            lpi_write_header(L, chan, LUA_TBOOLEAN, (lua_Number)lua_toboolean(L, index));
            break;
        case LUA_TSTRING:
            lpi_write_header(L, chan, LUA_TSTRING, (lua_Number)lua_objlen(L, index));
            lpi_write_data(L, chan, lua_tostring(L, index), lua_objlen(L, index));
            break;
        case LUA_TFUNCTION:
            lua_pushvalue(L, index);
            n = lua_gettop(L);
            lua_dump(L, lpi_functionWriter, NULL);
            n = lua_gettop(L) - n;
            lua_concat(L, n);
            lpi_write_header(L, chan, LUA_TFUNCTION, (lua_Number)lua_objlen(L, -1));
            lpi_write_data(L, chan, lua_tostring(L, -1), lua_objlen(L, -1));
            lua_pop(L, 2);
            break;
        case LUA_TTABLE:
            lpi_write_table(L, index, chan);
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
