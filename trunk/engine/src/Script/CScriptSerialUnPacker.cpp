#include "stdafx.h"
#include "CScriptSerialUnPacker.h"
#include "CScriptUnPackSerial.h"
#include "ScriptCommon.h"
#include "CScriptData.h"
#include "ErrLogHelper.h"
#include "CValueTable.h"

CScriptSerialUnPackerMgr::CScriptSerialUnPackerMgr()
{

}

CScriptSerialUnPackerMgr::~CScriptSerialUnPackerMgr()
{

}

void CScriptSerialUnPackerMgr::
UnPackSerialFun(void* pScript, const char* strType, size_t stTypeSize, const char* vecBuf, size_t stBufSize)
{
	CScript& Script = *(static_cast<CScript*>(pScript));

	Script.AstInSameThread();

	CScriptData* pScriptData=Script.GetData();
	lua_State* pState = pScriptData->m_pState;

	int32 iTop = lua_gettop(pState);

	int32 nErrFunIndex=0;
	lua_pushcfunction(pState,&CScriptData::ErrorHandler );
	nErrFunIndex=lua_gettop(pState);

	uint32 uTypePos = 0;
	uint32 uBufPos = 0;

	uint32 uSize = (uint32)stTypeSize;

	//得到function name
	const char* szFunName = CScriptUnPackSerial::UnPackString(pState, strType, vecBuf, uTypePos, uBufPos);

	lua_pop(pState, 1);

	lua_getglobal(pState, szFunName);
	if (lua_isnil(pState, -1))
	{
		ostringstream oss;
		oss << szFunName << "没有存在，或者不是function" << endl;
		GenErr("CScriptSerialUnPackerMgr::UnPackSerialFun Error", oss.str().c_str());
	}

	int32 iParam = lua_gettop(pState);

	while (uTypePos < uSize)
	{
		char ch = strType[uTypePos];
		switch(ch)
		{
		case 's':
			CScriptUnPackSerial::UnPackString(pState, strType, vecBuf, uTypePos, uBufPos);
			break;
		case 'd':
			CScriptUnPackSerial::UnPackNumber(pState, vecBuf, uTypePos, uBufPos);
			break;
		case 'b':
			CScriptUnPackSerial::UnPackBoolean(pState, vecBuf, uTypePos, uBufPos);
			break;
		case 'n':
			CScriptUnPackSerial::UnPackNil(pState, uTypePos);
			break;
		case 'f':
			CScriptUnPackSerial::UnPackFunctionContext(pState, vecBuf, uTypePos, uBufPos);
			break;
		case 't':
			CScriptUnPackSerial::UnPackTable(pState, strType, vecBuf, uTypePos, uBufPos);
			break;
		case 'v':
			CScriptUnPackSerial::UnPackValueTable(pState, strType, vecBuf, uTypePos, uBufPos);
			break;
		default:
			GenErr("不正确的参数类型");
		}

		//unpack serial可能会因为参数过多导致堆栈溢出，这里每次unpack的时候都检查
		int32 ExtraSlot = 1;
		luaL_checkstack(pState, ExtraSlot, "too many arguments in upack serial");
	}

	int32 ParamNum = lua_gettop(pState) - iParam; //得到参数数量

	pScriptData->m_stSerialUnpackCounter+=10;

	int nErrCode = lua_pcall(pState, ParamNum, 0, nErrFunIndex);
	if(nErrCode == LUA_ERRRUN || nErrCode == LUA_ERRMEM
		|| nErrCode == LUA_ERRERR)
	{
		string strError = lua_tostring(pState, -1);
		
		CError error(strError, pScriptData->m_sStackInfo.c_str());
		LogExp(error, NULL, false);
		OnScriptError(pState, nErrCode);
	}

	lua_settop(pState, iTop);
}

void CScriptSerialUnPackerMgr::RegUnPackFun(CScript& Script, ON_UNPACKFUNCTION* pOnUnPackFun)
{
	*pOnUnPackFun = &CScriptSerialUnPackerMgr::UnPackSerialFun;

	/*lua_State* pState = Script.GetData()->m_pState;

	int32 iTop = lua_gettop(pState);

	lua_pushstring(pState, strFunName.c_str());
	lua_gettable(pState, -2);
	Ast(!lua_isnil(pState, -1));

	lua_settop(pState, iTop);*/
}
