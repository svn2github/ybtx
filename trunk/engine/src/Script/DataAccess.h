#pragma once

//这个文件包含了大部分cpp调用lua以及lua调用cpp的代码

#include "ScriptDef.h"
#include "malloc.h"
#include "CScriptProc.h"

extern "C"
{
#include "lua/lua.h"
}

namespace sqr
{	
	class  CScript;
	struct CallBaseData;

	class CScript;


	class CMemberVariable : public CScriptProc
	{
		friend class CScriptData;
		friend class CScript;
		int32	m_nOffset;
		static int32 GetCppMemberVar( lua_State* pState );
		static int32 IsCppMemberVar( lua_State* pState );
	public:
		CMemberVariable( CScript& Script, const char* szFormat, int32 nOff );
		~CMemberVariable();
	};
}
