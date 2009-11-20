#include <stdlib.h>
#include <lua.h>
#include <luaconf.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_comms.h"
#include "lpi_error.h"

void lpi_read_header(lua_State * L, PI_CHANNEL * chan, int * t, lua_Number * n)
{
    LPI_CALL(L, PI_Read, chan, LPI_HEADER_FORMAT, t, n);
}

char * lpi_read_data(lua_State * L, PI_CHANNEL * chan, size_t size)
{
    char * buf = malloc(size);
    LPI_CALL(L, PI_Read, chan, "%*c", size, buf);
    return buf;
}

/* forward declaration so read_table doesn't explode */
void lpi_read_object(lua_State *, PI_CHANNEL *);

void lpi_read_table(lua_State * L, PI_CHANNEL * chan)
{
    while (1)
    {
        /* read key */
        lpi_read_object(L, chan);
        
        /* end of table? */
        if (lua_type(L, -1) == LUA_TNIL)
        {
            lua_pop(L, 1);
            break;
        }

        /* read value */
        lpi_read_object(L, chan);

        /* set */
        lua_settable(L, -3);
    }
}
    

void lpi_read_object(lua_State * L, PI_CHANNEL * chan)
{
    int t; lua_Number n; char * buf;

    lpi_read_header(L, chan, &t, &n);

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
            if (n == 0) lua_pushliteral(L, "");
            else
            {
                buf = lpi_read_data(L, chan, (size_t)n);
                lua_pushlstring(L, buf, (size_t)n);
                free(buf);
            }
            break;
        case LUA_TFUNCTION:
            buf = lpi_read_data(L, chan, (size_t)n);
            if(luaL_loadbuffer(L, buf, (size_t)n, "<function from PI_Read>"))
            {
                luaL_error(L, "error loading transmitted function: %s", lua_tostring(L, -1));
            }
            free(buf);
            break;
        case LUA_TTABLE:
            if (n == LPI_TABLE_START)
            {
                lua_newtable(L);
                lpi_read_table(L, chan);
            } else if (n == LPI_TABLE_END)
            {
                lua_pushnil(L);
            }

            break;
        case LUA_TUSERDATA:
            lpi_read_object(L, chan); /* read __recv */
            lpi_read_object(L, chan); /* read arg */
            lua_call(L, 1, 1);        /* call */
            break;
        default:
            luaL_error(L, "Unknown type on the wire: %s", lua_typename(L, t));
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
