#include <lua.h>
#include <lauxlib.h>
#include <pilot.h>

typedef struct {
  PI_PROCESS * proc;
  int fn;
  int env;
} lpi_Process;

int lpi_process(lua_State * L);
lpi_Process * lpi_process_new(lua_State * L);
void lpi_process_init(lua_State * L);

