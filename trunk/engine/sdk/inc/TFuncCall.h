#pragma once 
#include "CScriptProc.h"

struct lua_State;

namespace sqr
{
	class CScript;

	template <typename Func>
	class TFuncCall
		: public CBaseFuncCall<CFunction>
	{
		typedef CBaseFuncCall<CFunction>	ParentType;
		friend class	CScript;
	
		static int32	Call( lua_State* pState );

	public:
		TFuncCall( CScript& Script, const char* szFormat, Func pFunc, bool bClass, const char* szFunName );

	protected:
		Func			m_pFunc;
	};

}

