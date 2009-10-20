/**
 * Entry point for Lua pilot bindings
 * Includes library loader and top level pilot functions
 **/
#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_util.h"
#include "lpi_process.h"
#include "lpi_channel.h"
#include "lpi_log.h"
#include "lpi_time.h"
#include "lpi_names.h"
#include "lpi_abort.h"
#include "lpi_write.h"
#include "lpi_read.h"

/**
 * lpi_configure: PI_Configure
 * number = pilot.configure(string...)
 *
 * Constructs a fake argc and argv from the arguments passed to it.
 * Arguments not convertible to strings will raise an error.
 * Returns the worldsize as reported by PI_Configure, and sets
 * pilot.worldsize accordingly.
 **/
int lpi_configure(lua_State * L)
{
    int argc = lua_gettop(L)+1;
    char ** argv = malloc(argc * sizeof(char *));
    
    argv[0] = "(lua)";
    for (int i = 1; i < argc; ++i)
    {
        argv[i] = lua_tostring(L, i);
        if (!argv[i])
            luaL_typerror(L, i, "string");
    }
    
    int N = PI_Configure(&argc, &argv);
    lua_getglobal(L, "pilot");
    lua_pushinteger(L, N);
    lua_setfield(L, -2, "worldsize");
    lua_pop(L, 1);
    lua_pushinteger(L, N);
    
    free(argv);
    return 1;
}

/**
 * lpi_startAll: PI_StartAll
 * pilot.startAll()
 *
 * Starts all created processes.
 **/
int lpi_startAll(lua_State * L)
{
    PI_StartAll();
    return 0;
}

/**
 * lpi_stopMain: PI_StopMain
 * pilot.stopMain(number returncode)
 *
 * Stops the main process. Blocks until all other processes have also stopped.
 * returncode is reported to Pilot as the exit code of this process.
 **/
int lpi_stopMain(lua_State * L)
{
    int n = luaL_checkinteger(L, 1);
    PI_StopMain(n);
    return 0;
}

const luaL_Reg lr_pilot[] = {
    { "configure",      lpi_configure   },
    { "startAll",       lpi_startAll    },
    { "stopMain",       lpi_stopMain    },
    { "process",        lpi_process     },
    { "log",            lpi_log         },
    { "isLogging",      lpi_isLogging   },
    { "startTime",      lpi_startTime   },
    { "endTime",        lpi_endTime     },
    { "setName",        lpi_setName     },
    { "getName",        lpi_getName     },
    { "abort",          lpi_abort,      },
    { "channel",        lpi_channel,    },
    { "channelHasData", lpi_channelHasData },
    { "read",           lpi_read        },
    { "write",          lpi_write       },
    { NULL,             NULL            }
};

/**
 * Library entry point.
 * Loads all interface functions into global table pilot,
 * initializes the PI_PROCESS and PI_CHANNEL [FIXME: PI_BUNDLE] metatables,
 * and creates a process wrapper for PI_MAIN, as pilot.main.
 *
 * Returns the global pilot table.
 **/
int luaopen_pilot(lua_State * L)
{
    luaL_register(L, "pilot", lr_pilot);
    lpi_process_init(L);
    lpi_channel_init(L);
    
    lpi_Process * Pmain = lpi_process_new(L);
    lua_newtable(L);
    Pmain->env = luaL_ref(L, LUA_REGISTRYINDEX);
    Pmain->proc = PI_MAIN;
    lua_setfield(L, -2, "main");

    return 1;
}                                                                 

