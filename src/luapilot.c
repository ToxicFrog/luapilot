#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

extern int luaL_openlibs(lua_State *);
extern int luaopen_pilot(lua_State *);

int main(int argc, char ** argv)
{                                                                                            
    lua_State * L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_pilot(L);

    int N = PI_Configure(&argc, &argv);
    
    lua_pushinteger(L, N);
    lua_setfield(L, -2, "worldsize");
    lua_pushnil(L);
    lua_setfield(L, -2, "configure");
    lua_pop(L, 1);
    
    if (argc < 2) return 1;
    
    luaL_loadfile(L, argv[1]);                                                                                  
    for (int i = 2; i < argc; ++i)
    {
        lua_pushstring(L, argv[i]);
    }
    lua_call(L, argc-2, 0);
    
    return 0;
}
