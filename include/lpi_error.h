#include <lua.h>
#include <pilot_error.h>

void lpi_error(lua_State *, int);

#define LPI_CALL(L, fun, ...) fun(__VA_ARGS__);  \
    if (PI_Errno != PI_NO_ERROR) \
        lpi_error(L, PI_Errno)
