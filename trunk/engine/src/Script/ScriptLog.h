#pragma once

struct lua_State;

namespace sqr
{

	int LogScriptExp( lua_State* pState );
	int CfgLogScriptErr( lua_State* pState );
	int LogScriptErr( lua_State* pState );
	int LogCategoryScriptErr( lua_State* pState );
}
