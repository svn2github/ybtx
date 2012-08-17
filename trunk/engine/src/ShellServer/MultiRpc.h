#pragma once

#include "ScriptRpc.h"

namespace sqr
{


	template<typename RecvType,typename SendType>
	class TRegisterMultiRpc
	{
	public:
		typedef TPackerTable<RecvType> CMultiPackerTable;

		typedef TUnPacker<SendType> CMultiUnPacker;
		typedef TUnPackerTable<SendType> CMultiUnPackerTable;

		static int32  Register(CScript& Script, const char * szName)
		{
			//	CScript& Script = *(CBaseScriptApp::Inst()->GetScript());
			REGIST_ABSTRACT_B_CLASS( Script, CMultiPackerTable);
			REGIST_CLASSFUNCTION(Script,CMultiPackerTable,Init, "s:t");
			REGIST_CLASSFUNCTION(Script,CMultiPackerTable,RegistPackFunc, "tsIs");
			REGIST_STATICFUNCTION( Script, CMultiPackerTable, Inst, ":(CPackerTable)" );

			REGIST_ABSTRACT_B_CLASS( Script, CMultiUnPacker);
			REGIST_CLASSFUNCTION(Script,CMultiUnPacker,UnPack, (  ("("+string(szName)+"):i").c_str()   )    );

			REGIST_ABSTRACT_B_CLASS( Script, CMultiUnPackerTable);
			REGIST_CLASSFUNCTION(Script,CMultiUnPackerTable,Init, "s:t");
			REGIST_CLASSFUNCTION(Script,CMultiUnPackerTable,RegistUnPackFunc, "tsIs");
			REGIST_CLASSFUNCTION( Script, CMultiUnPackerTable, GetUnPacker, "s:(CUnPacker)" );
			REGIST_STATICFUNCTION( Script, CMultiUnPackerTable, Inst, ":(CUnPackerTable)" );
			return 1;
		}
	};

}

#define REGIST_MULTI_RPC( ScriptClass, SendPipeName, RecvPipeName ) \
	TRegisterMultiRpc< SendPipeName,RecvPipeName >::Register(ScriptClass, #RecvPipeName);



