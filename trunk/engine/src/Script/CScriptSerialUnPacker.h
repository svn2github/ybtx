#pragma once 

#include "ScriptDef.h"

namespace sqr
{
	class CScript;

	class CScriptSerialUnPackerMgr
	{
	public:
		CScriptSerialUnPackerMgr();
		~CScriptSerialUnPackerMgr();

		void RegUnPackFun(CScript& Script, ON_UNPACKFUNCTION* pOnUnPackFun);
		
		static void UnPackSerialFun(void* pScript, const char* strType, size_t stTypeSize, 
			const char* vecBuf, size_t stBufSize);
	
	};

}

