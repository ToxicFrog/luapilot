#include <lua.h>

void lpi_bundle_init(lua_State *);
int lpi_bundle(lua_State *);
int lpi_select(lua_State *);
int lpi_trySelect(lua_State *);
int lpi_getBundleSize(lua_State *);
int lpi_getBundleChannel(lua_State *);
