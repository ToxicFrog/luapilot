#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_process.h"
#include "lpi_read.h"
#include "lpi_write.h"
#include "lpi_names.h"
#include "lpi_error.h"
#include "lpi_util.h"

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
    lua_remove(L, 1); /* remove 'self' since this is called as a metamethod */
    PI_PROCESS * from = *(PI_PROCESS **)luaL_checkudata(L, 1, "PI_PROCESS *");
    PI_PROCESS * to = *(PI_PROCESS **)luaL_checkudata(L, 2, "PI_PROCESS *");

    PI_CHANNEL * channel = LPI_CALL(L, PI_CreateChannel, from, to);
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

    PI_CHANNEL ** copies = LPI_CALL(L,
        PI_CopyChannels,
        reverse? PI_REVERSE:PI_SAME,
        channels,
        size);

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
    int hasdata = LPI_CALL(L, PI_ChannelHasData, obj);
    lua_pushboolean(L, hasdata);
    return 1;
}


/******************************************************************************
 * Metamethods
 ******************************************************************************/

/**
 * __tostring
 *
 * returns "PI_CHANNEL '<name>': <address>"
 **/
static int lpi_channel_tostring(lua_State * L)
{
    PI_CHANNEL * obj = *(PI_CHANNEL **)luaL_checkudata(L, 1, "PI_CHANNEL *");
    const char * name = LPI_CALL(L, PI_GetName, obj);
    lua_pushfstring(L, "%s '%s': %p", "PI_CHANNEL", name, (void *)obj);
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

static const char * lpi_channel_methods[] = {
    "hasData", "channelHasData",
    "read", "read",
    "write", "write",
    "setName", "setName",
    "getName", "getName",
    NULL
};

/**
 * Initialization
 **/
void lpi_channel_init(lua_State * L)
{
    int method_ref = lpi_methods(L, "channel", lpi_channel_methods, lpi_channel);
    
    /* create metatable */
    luaL_newmetatable(L, "PI_CHANNEL *");
    luaL_register(L, NULL, lpi_channel_mt);
    lua_rawgeti(L, LUA_REGISTRYINDEX, method_ref);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    return;
}
