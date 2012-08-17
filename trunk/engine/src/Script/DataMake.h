#pragma once 

#include "CBaseType.h"
#include "CScript.h"

namespace sqr
{		

	class CScriptDataMaker
	{				//Factory， 生成CBaseType及其子类， 提供给CScriptProc
	public:
		enum EState{ eParam, eResult, eError, eEnd };

		CScriptDataMaker( CScript* pScript, const char* szFormat );
		CBaseType* MakeNextData( size_t& posCur );

		bool IsEnd() 				{ return m_eState==eEnd; 	}
		bool IsErr() 				{ return m_eState==eError; 	}
		bool IsParam() 				{ return m_eState==eParam; 	}
		bool IsResult() 			{ return m_eState==eResult; }	
	protected:
		CBaseType* MakeObject( CBaseType::EFlag eFlag );
		EState		m_eState;
		const char* m_szFormat;
		const char* m_pCur;
		CScript* m_pScript;
	};

	
}
