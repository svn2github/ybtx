#include "stdafx.h"
#include "ScriptLog.h"
#include "ScriptCommon.h"
#include "ErrLogHelper.h"
#include "CScriptData.h"
#include "ScriptDebugHelper.h"
#include "ScriptKey.h"
#include "ExpHelper.h"
#include "CLogOwner.h"
#include "CError.h"

namespace sqr
{
	//这个函数只用于apcall和coroutine内部
	int LogScriptExp( lua_State* pState )
	{
		CScriptData* pThis= CScript::Inst()->GetData();

		if( pThis->m_pDebuger )
			return 0;

		string strError = lua_tostring(pState, -1);

		string strStack;
		GetStackInfo(pState, strStack);

		CError error(strError, strStack);
		LogExp(error, NULL, false);
	
		return 0;
	}

	static CLogOwner* GetConn(lua_State* pState)
	{
		if(lua_istable(pState, -1))
		{
			lua_pushlightuserdata(pState, GetCppObjPtrKey()); //obj, key
			lua_rawget(pState, -2);
			void* pConn = NULL;
			if(lua_islightuserdata(pState, -1))
			{	
				pConn = lua_touserdata(pState, -1);
			}
			else if(lua_isuserdata(pState, -1))
			{
				void** pObject = (void**)lua_touserdata(pState, -1);
				pConn = *pObject;
			}

			if(pConn)
			{
				return dynamic_cast<CLogOwner*>((CBaseObject*)pConn);
			}
		}

		return NULL;
	}

	static int _LogErr(lua_State* pState, const char* szCategory)
	{
		//必须有msg信息
		Ast(lua_isstring(pState, 1));
	
		string strError = lua_tostring(pState, 1);

		string strStack;
		GetStackInfo(pState, strStack);
		
		CError error(strError, strStack);
	
		if(szCategory)
			error.SetCategory(szCategory);

		int32 iTop = lua_gettop(pState);
		for(int32 i = 2; i <= iTop; i++)
		{
			if(lua_isstring(pState, i))
			{
				error.AppendMsg(lua_tostring(pState, i));
			}
		}

		CLogOwner* pConn = GetConn(pState);
		uint32 uErrCode =GetErrIndexCode(error);
		LogExp(error, pConn, false);

		lua_pushnumber(pState, uErrCode);

		return 1;
	}

	int CfgLogScriptErr( lua_State* pState )
	{
		return _LogErr(pState, CFG_ERR_MSG);
	}

	int LogScriptErr( lua_State* pState )
	{
		return _LogErr(pState, NULL);
	}

	int LogCategoryScriptErr( lua_State* pState )
	{
		Ast(lua_isstring(pState, 1));
		const char* szCategory = lua_tostring(pState, 1);
		lua_remove(pState, 1);
		return _LogErr(pState, szCategory);
	}
}
