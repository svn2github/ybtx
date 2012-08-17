#include "stdafx.h"
#include "CBuf.h"
#include "ScriptCommon.h"

#include "ExpHelper.h"

namespace sqr
{
	static int __CBuf_GetLen( lua_State* L )
	{
		lua_pushstring( L, "__len" );
		lua_rawget( L, 1 );

		return 1;
	}

	static void* __makeTable2CBuf( lua_State*L, int nStkIdx, int len )
	{
		void* pBuf = lua_newuserdata( L, len );
		lua_setfield( L, nStkIdx, "__buf" );

		lua_pushinteger( L, len );
		lua_setfield( L, nStkIdx, "__len" );

		lua_pushcfunction( L, &__CBuf_GetLen );
		lua_setfield( L, nStkIdx, "GetLen"  );

		lua_getglobal( L, "__mt_readonly" );
		Ast( !lua_isnil( L, -1 ) );
		lua_setmetatable( L, nStkIdx );

		return pBuf;
	}

	int __createCBuf( lua_State* L )
	{
		int nBp = lua_gettop( L );
		int len = (int32)lua_tointeger( L, nBp );
		Ast( nBp>0 );
		Ast( len>0 );
		lua_newtable( L );

		__makeTable2CBuf( L, nBp+1, len );

		lua_pushvalue( L, nBp+1 );
		lua_replace( L, 1 );
		return 1;
	}
}

