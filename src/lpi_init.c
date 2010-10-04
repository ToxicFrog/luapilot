/**
 * Entry point for Lua pilot bindings
 * Includes library loader and top level pilot functions
 **/
#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

#include "lpi_error.h"
#include "lpi_util.h"
#include "lpi_process.h"
#include "lpi_channel.h"
#include "lpi_log.h"
#include "lpi_time.h"
#include "lpi_names.h"
#include "lpi_abort.h"
#include "lpi_write.h"
#include "lpi_read.h"
#include "lpi_bundle.h"
#include "lpi_broadcast.h"
#include "lpi_gather.h"

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
    const char ** argv = malloc(argc * sizeof(char *));
    
    argv[0] = "(lua)";
    for (int i = 1; i < argc; ++i)
    {
        argv[i] = lua_tostring(L, i);
        if (!argv[i])
            luaL_typerror(L, i, "string");
    }
    
    int N = LPI_CALL(L, PI_Configure, &argc, &argv);
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
    LPI_CALL(L, PI_StartAll);
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
    LPI_CALL(L, PI_StopMain, n);
    return 0;
}

/**
 * All of the Pilot functions, apart from the constructors for processes,
 * channels, and bundles. Those are installed as __call-able tables by
 * their respective init functions.
**/
const luaL_Reg lr_pilot[] = {
    { "configure",      lpi_configure   },
    { "startAll",       lpi_startAll    },
    { "stopMain",       lpi_stopMain    },
    { "log",            lpi_log         },
    { "isLogging",      lpi_isLogging   },
    { "startTime",      lpi_startTime   },
    { "endTime",        lpi_endTime     },
    { "setName",        lpi_setName     },
    { "getName",        lpi_getName     },
    { "abort",          lpi_abort,      },
    { "copyChannels",   lpi_copyChannels },
    { "channelHasData", lpi_channelHasData },
    { "read",           lpi_read        },
    { "write",          lpi_write       },
    { "select",         lpi_select      },
    { "trySelect",      lpi_trySelect   },
    { "getBundleSize",  lpi_getBundleSize },
    { "getBundleChannel", lpi_getBundleChannel },
    { "broadcast",      lpi_broadcast },
    { "gather",         lpi_gather },
    { NULL,             NULL            }
};

/**
 * Library entry point from Lua
 * Loads all interface functions into global table pilot,
 * initializes the PI_PROCESS and PI_CHANNEL [FIXME: PI_BUNDLE] metatables,
 * and creates a process wrapper for PI_MAIN, as pilot.main.
 *
 * Returns the global pilot table.
 **/
int luaopen_pilot(lua_State * L)
{
    /* tell Pilot to return and set PI_Errno on error, rather than aborting */
    PI_OnErrorReturn = 1;

    luaL_register(L, "pilot", lr_pilot);
    lpi_process_init(L);
    lpi_channel_init(L);
    lpi_bundle_init(L);

    return 1;
}                                                                 

int luaopen_luapilot(lua_State * L)
{
    return luaopen_pilot(L);
}

/**
 * Library entry point from C
 * Calls the Lua initializer, then configures Pilot
**/
int lpi_init(lua_State * L, int * argc, char *** argv)
{
    luaopen_pilot(L);
    int N = LPI_CALL(L, PI_Configure, argc, argv);
    lua_pushinteger(L, N);
    lua_setfield(L, -2, "worldsize");
    lua_pop(L, 1);
    return N;    
}
