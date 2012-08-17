
#define LUAPROFILERNAME "profiler"

extern "C"
{
#include "../lua/lua.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"
}

int luaopen_profiler(lua_State *L);
int profiler_init(lua_State *L);
int profiler_stop(lua_State *L);


