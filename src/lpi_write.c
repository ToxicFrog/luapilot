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

int lpi_functionWriter(lua_State * L, const void * buf, size_t size, void * unused)
{
    lua_pushlstring(L, buf, size);
    return 0;
}

void lpi_write_object(lua_State *, int, PI_CHANNEL *);

void lpi_write_table(lua_State * L, int index, PI_CHANNEL * chan)
{
    lpi_write_header(chan, LUA_TTABLE, LPI_TABLE_START);

    lua_pushnil(L);
    while (lua_next(L, index))
    {
        lpi_write_object(L, lua_gettop(L)-1, chan);  /* write key */
        lpi_write_object(L, lua_gettop(L), chan);    /* write value */
        lua_pop(L, 1);
    }

    lpi_write_header(chan, LUA_TTABLE, LPI_TABLE_END);
}

void lpi_write_object(lua_State * L, int index, PI_CHANNEL * chan)
{
    int n;

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
	case LUA_TFUNCTION:
	    lua_pushvalue(L, index);
	    n = lua_gettop(L);
	    lua_dump(L, lpi_functionWriter, NULL);
	    n = lua_gettop(L) - n;
	    lua_concat(L, n);
            lpi_write_header(chan, LUA_TFUNCTION, (lua_Number)lua_objlen(L, -1));
            lpi_write_data(chan, lua_tostring(L, -1), lua_objlen(L, -1));
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