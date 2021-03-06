#include <stdlib.h>
#include <strings.h>
#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_broadcast.h"
#include "lpi_error.h"
#include "lpi_util.h"

static void lpi_bundle_push(lua_State * L, PI_BUNDLE * bundle)
{
  PI_BUNDLE ** obj = lua_newuserdata(L, sizeof(PI_BUNDLE *));
  luaL_newmetatable(L, "PI_BUNDLE *");
  lua_setmetatable(L, -2);
  *obj = bundle;
  return;
}

int lpi_bundle(lua_State * L)
{
    lua_remove(L, 1); /* remove 'self' */
    
    int type;
    const char * type_str = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    int size = lua_objlen(L, 2);

    if (!strcasecmp(type_str, "select"))
        type = PI_SELECT;
    else if (!strcasecmp(type_str, "gather"))
        type = PI_GATHER;
    else if (!strcasecmp(type_str, "broadcast"))
        type = PI_BROADCAST;
    else
        luaL_error(L, "Invalid first argument to pilot.bundle: needs to be one of 'select', 'gather', or 'broadcast'");

    PI_CHANNEL ** channels = malloc(size * sizeof(PI_CHANNEL *));
    
    for (int i = 0; i < size; ++i)
    {
        lua_rawgeti(L, 2, i+1);
        channels[i] = *(PI_CHANNEL **)luaL_checkudata(L, -1, "PI_CHANNEL *");
        lua_pop(L, 1);
    }


    PI_BUNDLE * bundle = LPI_CALL(L, PI_CreateBundle, type, channels, size);
    lpi_bundle_push(L, bundle);

    lua_newtable(L);
    for (int i = 1; i <= size; ++i)
    {
        lua_rawgeti(L, 2, i);
        lua_rawseti(L, -2, i);
    }
    lua_setfenv(L, -2);

    free(channels);
    return 1;
}

/**
 * getChannel
 **/
int lpi_getBundleChannel(lua_State * L)
{
    PI_BUNDLE * obj = *(PI_BUNDLE **)luaL_checkudata(L, 1, "PI_BUNDLE *");
    int n = luaL_checkinteger(L, 2);
    PI_CHANNEL * chan = LPI_CALL(L, PI_GetBundleChannel, obj, n-1);

    if (!chan) /* invalid index. It's not fatal in Pilot, so return nil */
    {
        lua_pushnil(L);
        
    } else { /* rather than pushing a new fuserdata, grab it from the env */
        lua_getfenv(L, 1);
        lua_rawgeti(L, -1, n);
    }
    
    return 1;
}

/**
 * select
 **/
int lpi_select(lua_State * L)
{
    PI_BUNDLE * obj = *(PI_BUNDLE **)luaL_checkudata(L, 1, "PI_BUNDLE *");
    lua_settop(L, 1);

    int n = LPI_CALL(L, PI_Select, obj);
    lua_pushinteger(L, n+1);
    lpi_getBundleChannel(L);
    lua_pushvalue(L, 2);
    return 2;
}

/**
 * trySelect
 **/
int lpi_trySelect(lua_State * L)
{
    PI_BUNDLE * obj = *(PI_BUNDLE **)luaL_checkudata(L, 1, "PI_BUNDLE *");
    lua_settop(L, 1);

    int n = LPI_CALL(L, PI_TrySelect, obj);
    lua_pushinteger(L, n+1);
    lpi_getBundleChannel(L);
    lua_pushvalue(L, 2);
    return 2;
}

/**
* getSize
**/
int lpi_getBundleSize(lua_State * L)
{
    PI_BUNDLE * obj = *(PI_BUNDLE **)luaL_checkudata(L, 1, "PI_BUNDLE *");

    int n = LPI_CALL(L, PI_GetBundleSize, obj);
    lua_pushinteger(L, n);
    return 1;
}


/******************************************************************************
 * Metamethods
 ******************************************************************************/

/**
 * __tostring
 * 
 * returns "PI_BUNDLE 'name': 0x1234"
**/
static int lpi_bundle_tostring(lua_State * L)
{
    PI_BUNDLE * obj = *(PI_BUNDLE **)luaL_checkudata(L, 1, "PI_BUNDLE *");
    const char * name = LPI_CALL(L, PI_GetName, obj);
    lua_pushfstring(L, "%s '%s': %p", "PI_BUNDLE", name, (void *)obj);
    return 1;
}

/**
 * __index
 *
 * Allows you to go bundle[n] to get the n'th contained channel, by forwarding
 * to bundle:getChannel.
 * Otherwise tries to look it up in the table of methods.
**/
static int lpi_bundle_index(lua_State * L)
{
    luaL_checkudata(L, 1, "PI_BUNDLE *");
    
    if (lua_type(L, 2) == LUA_TNUMBER)
    {
        return lpi_getBundleChannel(L);
    }

    lua_gettable(L, lua_upvalueindex(1));
    return 1;
}

/**
 * __type
 *
 * returns "PI_BUNDLE"
**/
static int lpi_bundle_type(lua_State * L)
{
    luaL_checkudata(L, 1, "PI_BUNDLE *");
    lua_pushstring(L, "PI_BUNDLE");
    return 1;
}

luaL_Reg lpi_bundle_mt[] = {
    { "__tostring",  lpi_bundle_tostring },
    { "__type",      lpi_bundle_type },
    { "__len",       lpi_getBundleSize },
/*  { "__index",     lpi_bundle_index }, needs special handling - see lpi_bundle_init */
    { NULL, NULL }
};

static const char * lpi_bundle_methods[] = {
    "getName", "getName",
    "setName", "setName",
    "getSize", "getBundleSize",
    "getChannel", "getBundleChannel",
    "select", "select",
    "trySelect", "trySelect",
    "broadcast", "broadcast",
    "gather", "gather",
    NULL
};

void lpi_bundle_init(lua_State * L)
{
    int method_ref = lpi_methods(L, "bundle", lpi_bundle_methods, lpi_bundle);
    
    luaL_newmetatable(L, "PI_BUNDLE *");            /* mt */
    luaL_register(L, NULL, lpi_bundle_mt);          /* mt */
    lua_rawgeti(L, LUA_REGISTRYINDEX, method_ref);  /* mt methods */
    lua_pushcclosure(L, lpi_bundle_index, 1);       /* mt __index */
    lua_setfield(L, -2, "__index");                 /* mt */
    lua_pop(L, 1);                                  /* */
    return;
}
