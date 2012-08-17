#include "stdafx.h"
#include "CClassCallBack.h"
#include "CScript.h"
#include "CClass.h"
#include "CDataType.h"
#include "ScriptCallBaseData.h"
#include "CScriptData.h"
#include "ScriptCommon.h"
#include "CBaseObject.h"
#include "ErrLogHelper.h"
#include "CCallBackProfiler.h"

CClassCallBack::CClassCallBack(CScript& Script, const char* szClassName, const char* szFunName, 
							   const char* szFormat, bool bPureCallBackFunc)
: ParentType(Script, szFormat, szFunName)
, m_sClassName(szClassName)
, m_sFunName(szFunName)
, m_szFormat(szFormat)
, m_bPureCallBackFunc(bPureCallBackFunc)
{	
	CClass* pClass = Script.GetClassInfo( szClassName );
	if( ! pClass )
	{
		ostringstream strm;
		strm<<"\""<<szClassName<<"\" not exists.";
		GenErr(strm.str());
	}

	if (m_pData->m_bRetArgList)
	{
		GenErr("可变参数不允许用于callback的返回值");
	}

	pClass->RegCallBackFunc(szFunName, this);

}

CClassCallBack::~CClassCallBack()
{
}


void* CClassCallBack::GetCallBackData(CBaseObject* pObject, const char* szFunName)
{
	//如果不是由脚本库创建的，那么是不可能有callback回调的
	if(!pObject->CreatedByScript())
		return NULL;

	CScript* pScript = CScript::Inst();	

	if(!pScript)
		return NULL;

	//pScript->AstInSameThread();

	const char* szClassName = typeid(*pObject).name();
	if (!szClassName)
	{
		char buffer[100];
		sprintf(buffer, "传给%s的c++对象没有正确的rtti信息", szFunName);
		GenErr(buffer);
	}

	CClass* pClass = pScript->GetClassInfoWithType(szClassName);

	if(!pClass)
	{
		char buffer[100];
		sprintf(buffer, "传给%s的c++对象的rtti类名%s没有注册", szFunName, szClassName);
		GenErr(buffer);
	}

	if(!pClass->IsCallBack())
		return NULL;
	
	void* pCall = pClass->GetCallBackFunc(szFunName);
	
	return pCall;
}


bool CClassCallBack::CallClassCallBack(CBaseObject* pObject, char* pDataBuf)
{
	CScript::Inst()->CallBackProfileBegin(m_sFunName.c_str());

	CScriptData* pData=CScript::Inst()->GetData();
	lua_State* pState;

	if(pData->m_pRunningState)
	{	 
		//当前正在执行的coroutine不是NULL,说明当前callback是被某个lua调用的c函数触发的，那么应该使用这个当前lua的coroutine
		pState=pData->m_pRunningState;
	}
	else
	{
		//当前执行coroutine是NULL，说明目前堆栈上没有任何lua函数。
		pState=m_pDefaultState;
	}

	//_S(pState).AstInSameThread();

	int32 nTop = lua_gettop(pState);

	int32 nErrFunIndex=0;
	lua_pushcfunction(pState,&CScriptData::ErrorHandler );
	nErrFunIndex=lua_gettop(pState);

	int32 nErrCode = 0;

	GetGlobalObjTable(pState);
	
	EBaseObjectType eType = pObject->GetBaseObjectType();

	if (eType == eBOT_Share)
	{
		lua_pushnumber(pState, pObject->GetBaseObjectID());
	}
	else
	{
		lua_pushlightuserdata(pState, pObject);
	}

	lua_gettable(pState, -2);

	if(lua_isnil(pState, -1))
	{
		lua_settop(pState, nTop);
		return false;
	}

	lua_pushlstring(pState, m_sFunName.c_str(), m_sFunName.size());
	lua_gettable(pState, -2);		//err func, __objtable, obj, func	
	if(lua_isnil(pState, -1))
	{
		if (!m_bPureCallBackFunc)
		{
			lua_settop(pState, nTop);
			return false;
		}
		else
		{
			ostringstream strm;
			strm<<"类: "<< m_sClassName.c_str()<<" 的CallBack函数: "<< m_sFunName.c_str()<<" 必须在lua中重载后才能使用！"<<endl;
			GenErr(strm.str());
		}
	}

	int32 nArg = (int32)m_pData->m_vecParam.size();
	int StackSize = nArg + m_pData->m_nReturnNum + 1;
	//callback的参数过多，可能导致lua堆栈溢出，这里检查一下
	luaL_checkstack(pState, StackSize, "too many arguments in callback");

	lua_insert( pState, -2 );	//err, objtable, func, obj
	
	int32 nArgBegin = lua_gettop(pState);
	PushParam2LuaStack( pState, pDataBuf );

	int32 nArgEnd = lua_gettop(pState);	

	++pData->m_stCallbackCounter;
	nErrCode = lua_pcall( pState, nArgEnd - nArgBegin + 1, m_pData->m_nReturnNum, nErrFunIndex );

	if(nErrCode == LUA_ERRRUN || nErrCode == LUA_ERRMEM
		|| nErrCode == LUA_ERRERR)
	{
		const char* szError = lua_tostring(pState, -1);

		lua_settop(pState, nTop);	

		ostringstream str;
		str << "在" << m_sClassName.c_str() << "::" << m_sFunName.c_str() 
			<< " 里面拦截到错误" << endl;

		CError error(szError);
		error.AppendInfo(str.str().c_str());
		error.AppendStackInfo(pData->m_sStackInfo.c_str());

		LogExp(error, NULL, false);

		OnScriptError(pState, nErrCode);
		return false;
	}


	if( m_pData->m_pResult )
		m_pData->m_pResult->GetFromLuaStack( pState, pDataBuf, -1 );

	//调整lua堆栈，为调用前大小
	lua_settop(pState, nTop);

	CScript::Inst()->CallBackProfileEnd(m_sFunName.c_str());

	return true;
}
