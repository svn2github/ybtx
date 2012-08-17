#pragma once

struct lua_State;

namespace sqr
{
	
	int BeforeResume(lua_State* pState);
	int AfterResume(lua_State* pState);

}
