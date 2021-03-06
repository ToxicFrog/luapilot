#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

/**
 * lpi_abort: PI_Abort
 * pilot.abort(message)
 *
 * Aborts the entire program with the given message, reporting the line and
 * file where called (if possible). Uses MPI error code 0.
 * Does not return.
 **/
int lpi_abort(lua_State * L)
{
    const char * message = luaL_checkstring(L, 1);
    lua_Debug ar;
    int line = 0;
    const char * src = "<<unknown>>";

    /* get file and line information using the lua debug interface */
    if (lua_getstack(L, 1, &ar))
    {
        lua_getinfo(L, "Sl", &ar);
        if (ar.currentline > 0)
        {
            /* activation record got, extract the info from it */
            line = ar.currentline;
            src = ar.short_src;
        }
    }

    PI_Abort(0, message, src, line);
    return 0;
}
