#include "stdafx.h"
#include "CScriptUnPackSerial.h"
#include "CScript.h"
#include "ScriptCommon.h"
#include "CValueTable.h"
#include "CValueTable.h"
#include "ScriptKey.h"
#include "CClass.h"

void CScriptUnPackSerial::UnPackData(void* pData, const char* vecBuf, 
									 size_t stDataSize, uint32& uBufPos)
{
	memcpy(pData, vecBuf + uBufPos, stDataSize);
	uBufPos += (uint32)stDataSize;
}

bool CScriptUnPackSerial::UnPackBoolean(lua_State* pState, const char* vecBuf, 
								   uint32& uTypePos, uint32& uBufPos)
{
	bool bBoolean;
	UnPackData(&bBoolean, vecBuf, sizeof(bool), uBufPos);

	lua_pushboolean(pState, bBoolean);

	uTypePos += 1;

	return bBoolean;
}

double CScriptUnPackSerial::UnPackNumber(lua_State* pState, const char* vecBuf, 
								  uint32& uTypePos, uint32& uBufPos)
{
	double dNum;
	UnPackData(&dNum, vecBuf, sizeof(double), uBufPos);

	lua_pushnumber(pState, dNum);

	uTypePos += 1;

	return dNum;
}

const char* CScriptUnPackSerial::UnPackString(lua_State* pState, const char* strType, const char* vecBuf, 
								  uint32& uTypePos, uint32& uBufPos)
{
	//string 格式为 第一个4字节为字符串buffer的长度
	uint32 uLen = 0;
	UnPackData(&uLen, vecBuf, sizeof(uint32), uBufPos);

	const char* str = &vecBuf[uBufPos];

	lua_pushlstring(pState, str, (int32)uLen - 1);

	uTypePos += 1;
	uBufPos += uLen;

	return str;
}

void CScriptUnPackSerial::UnPackNil(lua_State* pState, uint32& uTypePos)
{
	lua_pushnil(pState);
	uTypePos += 1;
}

//light userdata，由于里面可能是function传到外面在传回来的，所以要在ref表里面找一下
void* CScriptUnPackSerial::UnPackFunctionContext(lua_State* pState, const char* vecBuf, 
									uint32& uTypePos, uint32& uBufPos)
{
	void* pContext;

	UnPackData(&pContext, vecBuf, sizeof(void*), uBufPos);

	if (!pContext)
	{
		lua_pushnil(pState);
	}
	else
	{
		lua_pushlightuserdata(pState, GetSerialRefTblKey());
		lua_gettable(pState, LUA_GLOBALSINDEX);
		if (lua_isnil(pState, -1))
		{
			lua_pop(pState, 1);
			lua_pushlightuserdata(pState, pContext);
		}
		else
		{
			size_t stRefFun;
			memcpy(&stRefFun, &pContext, sizeof(size_t));
			int iRefFun = (int)stRefFun;
			lua_rawgeti(pState, -1, iRefFun);
			luaL_unref(pState, -2, iRefFun);
			lua_remove(pState, -2);
		}
	}

	uTypePos += 1;

	return pContext;
}


void CScriptUnPackSerial::UnPackTable(lua_State* pState, const char* strType, const char* vecBuf, 
								 uint32& uTypePos, uint32& uBufPos)
{
	//table 格式 t(s[n]ds[n]t(ddds[n]))
	//table 里面的数据为key-value对，又有可能出现table套table的情况，tmd的真难分析
		
	lua_newtable(pState);

	//因为table可能递归unpack，所以这里我们要不停的检查
	luaL_checkstack(pState, 2, "too many arguments in upack table");

	uint32 uLeftBracket = uTypePos + 1;

	uint32 uPos = uLeftBracket + 1;

	char ch = strType[uPos];

	while (ch != '}')
	{
		//处理key
		if (ch == 's')
		{
			UnPackString(pState, strType, vecBuf, uPos, uBufPos);
		}
		else if (ch == 'd')
		{
			UnPackNumber(pState, vecBuf, uPos, uBufPos);
		}
		else
		{
			GenErr("table里面的key只能是string或者number");
		}

		//处理value
		ch = strType[uPos];
		switch(ch)
		{
		case 's':
			{
				UnPackString(pState, strType, vecBuf, uPos, uBufPos);
				break;
			}
		case 'd':
			{
				UnPackNumber(pState, vecBuf, uPos, uBufPos);
				break;
			}
		case 'b':
			{
				UnPackBoolean(pState, vecBuf, uPos, uBufPos);
				break;
			}
		case 't':
			{
				UnPackTable(pState, strType, vecBuf, uPos, uBufPos);
				break;
			}
		case 'v':
			{
				UnPackValueTable(pState, strType, vecBuf, uPos, uBufPos);
				break;
			}
		default:
			GenErr("table中的value只允许为string，number，boolean或者table");
			break;
		}
					
		lua_settable(pState, -3);

		ch = strType[uPos];
	}

	uTypePos = uPos + 1;
}

void CScriptUnPackSerial::UnPackValueTable(lua_State* pState, const char* strType, 
										   const char* vecBuf, uint32& uTypePos, 
										   uint32& uBufPos)
{
	luaL_checkstack(pState, 3, "UnPackValueTable check stack error");

	uint32 uLen = 0;
	UnPackData(&uLen, vecBuf, sizeof(uint32), uBufPos);

	CValueTable* pValueTable = new CValueTable;

	pValueTable->UnSerialize(&vecBuf[uBufPos], uLen);

	CBaseObject* pObject = static_cast<CBaseObject*>(pValueTable);

	void** ppObject = (void**)lua_newuserdata(pState, sizeof(void*));
	*ppObject = pObject;

	int32 absIdx = lua_gettop(pState);

	static const char* szClassName = typeid(CValueTable).name();	
	CClass* pInfo = _S( pState ).GetClassInfoWithType(szClassName);

	lua_rawgeti(pState, LUA_REGISTRYINDEX, pInfo->GetClassRef());//userdata, class

	lua_setmetatable( pState, absIdx );			//userdata

	pObject->SetObjectFlag( eBOSF_VisableToScript | eBOSF_CreatedByScript );

	lua_pushvalue(pState, -1); //userdata, userdata

	MapObjectToUserdata(pState, pObject); //userdata

	lua_settop(pState, absIdx);

	uTypePos += 1;
	uBufPos += uLen;
}
