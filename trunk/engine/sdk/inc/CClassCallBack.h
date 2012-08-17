#pragma once

#include "CScriptProc.h"
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition.hpp>

#include "TFuncCallCheck.h"
#include "TFuncCallHelper.h"

#include "CScriptAllocator.h"

struct lua_State;


namespace sqr
{

	class Script;

	//------------------------------------------------------------------
	class CBaseObject;
	class CClassCallBack : public CBaseFuncCall<CBaseCallBack>
	{
		typedef CBaseFuncCall<CBaseCallBack> ParentType;
	public:
		CClassCallBack(CScript& Script, const char* szClassName, const char* szFunName, 
					   const char* szFormat, bool bPureCallBackFunc);

		~CClassCallBack();
		
		template<typename Func>
		void CheckCallBackFunc(Func pFunc);
	
		bool CallClassCallBack(CBaseObject* pObject, char* pDataBuf);
	
		static void* GetCallBackData(CBaseObject* pContext, const char* szFunName);
	
	private:
		typedef basic_string<char, std::char_traits<char>, CScriptAllocator<char> > String_t;

		String_t m_sClassName;
		String_t m_sFunName;
		String_t m_szFormat;
		bool	m_bPureCallBackFunc;
	};





}

