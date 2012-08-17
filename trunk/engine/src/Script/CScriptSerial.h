#pragma once 

#include "ScriptDef.h"


namespace sqr
{
	class CScript;

	class CScriptSerialData;

	class CScriptSerial
	{
	public:
		CScriptSerial();
		~CScriptSerial();
		
		void RegPackFun(CScript& Script, const std::string& strFunName, POST_PACKFUNCTION);

		void RegUnPackFun(CScript& Script, ON_UNPACKFUNCTION* pOnUnPackFun);
		
	private:
		CScriptSerialData*			m_pData;
	};

}

