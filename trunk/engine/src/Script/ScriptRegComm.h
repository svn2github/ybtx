#pragma once

struct lua_State;

namespace sqr
{
	//运行建立类机制的脚本
	void RegDefClass(lua_State* pState);

	//运行创建DebugPrint函数的脚本
	void RegDebugPrint(lua_State* pState);

	//运行本引擎提供的lua扩展过功能
	void RegExtend(lua_State* pState);

}
