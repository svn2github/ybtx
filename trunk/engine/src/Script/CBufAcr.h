#pragma once

struct lua_State;

namespace sqr
{
	int __createCBufAcr( lua_State* L );
	void __makeTable2CBufAcr( lua_State* L, int nStkIdx, void* p );
}

