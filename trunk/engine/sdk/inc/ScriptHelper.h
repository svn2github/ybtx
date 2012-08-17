#pragma once

struct lua_State;

namespace sqr
{
	int32 LuaGetTop(lua_State* pState);
	void LuaSetTop(lua_State* pState, int32 top);
	void LuaPop(lua_State* pState, int32 pop);

	void LuaPushNumber(lua_State* pState, double value);
	void LuaPushString(lua_State* pState, const char* str);
	void LuaPushBoolean(lua_State* pState, int value);
	void LuaPushLightUserData(lua_State* pState, void* pData);

	int32 LuaToBoolean(lua_State* pState, int32 index);
	double LuaToNumber(lua_State* pState, int32 index);
	const char* LuaToString(lua_State* pState, int32 index);
	void* LuaToUserData(lua_State* pState, int32 index);

	int32 LuaUpValueIndex(int32 index);
	void LuaL_Error(lua_State* pState, const char* str);

	int32 LuaIsNil(lua_State* pState, int32 index);
	int32 LuaIsNumber(lua_State* pState, int32 index);
	int32 LuaIsString(lua_State* pState, int32 index);
	int32 LuaIsTable(lua_State* pState, int32 index);
	int32 LuaIsUserData(lua_State* pState, int32 index);
}

