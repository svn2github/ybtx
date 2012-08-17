#pragma once
#include "ScriptDef.h"
#include "CScriptAllocator.h"

struct lua_State;

namespace sqr
{
	class CScript;
	
	class CScriptSerialPacker;

	class CScriptSerialPackerMgr
	{
	public:
		CScriptSerialPackerMgr();
		~CScriptSerialPackerMgr();

		void RegPackFun(CScript& Script, const std::string& strFunName, POST_PACKFUNCTION pPostPackFun);
	
	private:
		typedef set<CScriptSerialPacker*, less<CScriptSerialPacker*>, 
			CScriptAllocator<CScriptSerialPacker*> >	SetPack_t;
		SetPack_t		m_setPacker;
	};
}

