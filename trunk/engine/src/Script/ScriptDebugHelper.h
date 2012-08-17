#pragma once

struct lua_State;

namespace sqr
{

	string GetFileNameFromPathName(const char* szPathName);

	bool DebugShowLine( const char* szFilePathName, int line, bool bIsCurLine );

	uint32 GetRunStackDepth(lua_State* pState);

	bool PrintFrame( lua_State *pState, uint32 uLevel, ostream& out = std::cout);

	int GetCurLine( lua_State *pState, uint32 uLevel );

	void Print_lua( lua_State *pL, const list<string>& listField );

	int DebugBreak(lua_State* pState);

	int SaveDebugBreaks();

	int LoadDebugBreaks();

	void GetStackInfo(lua_State* pState, string& sStackInfo);
}

//ÏÔÊ¾½Å±¾µÄ¶ÑÕ»
void sbt();	

