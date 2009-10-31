#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_process.h"
#include "lpi_read.h"
#include "lpi_write.h"
#include "lpi_names.h"

/**
 * C/space constructor for Lua channels
 **/
static void lpi_channel_push(lua_State * L, PI_CHANNEL * channel)
{
    PI_CHANNEL ** obj = lua_newuserdata(L, sizeof(PI_CHANNEL *));
    luaL_newmetatable(L, "PI_CHANNEL *");
    lua_setmetatable(L, -2);
    *obj = channel;
    return;
}

/**
 * Lua interface to constructor
 **/
int lpi_channel(lua_State * L)
{
    PI_PROCESS * from = *(PI_PROCESS **)luaL_checkudata(L, 1, "PI_PROCESS *");
    PI_PROCESS * to = *(PI_PROCESS **)luaL_checkudata(L, 2, "PI_PROCESS *");

    PI_CHANNEL * channel = PI_CreateChannel(from, to);
    lpi_channel_push(L, channel);
    return 1;
}


/**
 * pilot.copyChannels
 **/
int lpi_copyChannels(lua_State * L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    int reverse = lua_toboolean(L, 2);

    int size = lua_objlen(L, 1);
    PI_CHANNEL ** channels = malloc(size * sizeof(PI_CHANNEL *));
    
    for (int i = 0; i < size; ++i)
    {
        lua_rawgeti(L, 1, i+1);
        channels[i] = *(PI_CHANNEL **)luaL_checkudata(L, -1, "PI_CHANNEL *");
        lua_pop(L, 1);
    }

    PI_CHANNEL ** copies = PI_CopyChannels(reverse? PI_REVERSE:PI_SAME, channels, size);

    lua_newtable(L);
    for (int i = 1; i <= size; ++i)
    {
        lpi_channel_push(L, copies[i-1]);
        lua_rawseti(L, -2, i);
    }

    free(copies);
    free(channels);
    return 1;
}

/******************************************************************************
 * Methods
 *
 * Only channelHasData is here
 * getName and setName are in lpi_names.c
 * read and write are in lpi_read.c and lpi_write.c, as they are
 * complicated enough to get their own files.
 ******************************************************************************/

/**
 * pilot.channelHasData
 * channel:hasData
 **/
int lpi_channelHasData(lua_State * L)
{
    PI_CHANNEL * obj = *(PI_CHANNEL **)luaL_checkudata(L, 1, "PI_CHANNEL *");
    lua_pushboolean(L, PI_ChannelHasData(obj));
    return 1;
}


/******************************************************************************
 * Metamethods
 ******************************************************************************/

/**
 * __tostring
 *
 * returns "PI_CHANNEL: <address>"
 **/
static int lpi_channel_tostring(lua_State * L)
{
    PI_CHANNEL * obj = *(PI_CHANNEL **)luaL_checkudata(L, 1, "PI_CHANNEL *");
    //  lua_pushfstring(L, "%s '%s': %p", "PI_CHANNEL", PI_GetName(*obj), (void *)obj);
    lua_pushfstring(L, "%s: %p", "PI_CHANNEL", (void *)obj);
    return 1;
}

/**
 * __type
 *
 * returns "PI_CHANNEL"
 **/
static int lpi_channel_type(lua_State * L)
{
    luaL_checkudata(L, 1, "PI_CHANNEL *");
    lua_pushstring(L, "PI_CHANNEL");
    return 1;
}

/**
 * Table of metamethods
 **/
luaL_Reg lpi_channel_mt[] = {
    { "__tostring", lpi_channel_tostring },
    { "__type", lpi_channel_type },
    { NULL, NULL }
};

/**
 * Table of methods
 **/
luaL_Reg lpi_channel_methods[] = {
    { "hasData", lpi_channelHasData },
    { "read", lpi_read },
    { "write", lpi_write },
    { "setName", lpi_setName },
    { "getName", lpi_getName },
    { NULL, NULL }
};

/**
 * Initialization
 **/
void lpi_channel_init(lua_State * L)
{
    /* initialize metamethods */
    luaL_newmetatable(L, "PI_CHANNEL *");
    luaL_register(L, NULL, lpi_channel_mt);

    /* create the methods table and point __index at it */
    lua_newtable(L);
    luaL_register(L, NULL, lpi_channel_methods);
    lua_setfield(L, -2, "__index");

    /* cleanup */
    lua_pop(L, 1);
    return;
}
