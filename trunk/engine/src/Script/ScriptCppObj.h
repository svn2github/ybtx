#pragma once

#include "CScript.h"

struct lua_State;

namespace sqr
{
	int32 CreateDynamicObjFromLua(lua_State* pState);
	
	int32 CreateStaticObjFromLua(lua_State* pState);

	int32 CreateIsolatedObjFromLua(lua_State* pState);

	int32  DestroyCppObjFromLua(lua_State* pState);

	int32 InitCppObjFromLua(lua_State* pState);
}
