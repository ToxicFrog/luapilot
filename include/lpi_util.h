#include <lua.h>

void l_unpack(lua_State * L, int index);
int l_checkutype(lua_State * L, int index, const char * type);
int lpi_ispilotobject(lua_State * L, int index);
int lpi_methods(lua_State * L, char * name, const char ** methods, lua_CFunction constructor);
