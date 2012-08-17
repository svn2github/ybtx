#include "stdafx.h"
#include "CScriptPackSerial.h"
#include "CScript.h"
#include "ScriptCommon.h"
#include <stdlib.h>
#include "CValueTable.h"
#include "ScriptKey.h"

void CScriptPackSerial::PackData(VecBuf_t& vecBuf, const void* pData, 
								 size_t stDataSize, uint32& uPos)
{
	vecBuf.insert(vecBuf.end(), stDataSize, 0);
	memcpy(&vecBuf[uPos], pData, stDataSize);
	uPos += (uint32)stDataSize;
}

//处理字串
void CScriptPackSerial::PackString(const char* str, size_t stLen, String_t& strType, 
								   VecBuf_t& vecBuf, uint32& uPos)
{
	uint32 len = (uint32)stLen + 1;
	
	strType.append("s");
	
	PackData(vecBuf, &len, sizeof(uint32), uPos);
	PackData(vecBuf, str, len, uPos);
}

//处理bool
void CScriptPackSerial::PackBoolean(bool bBoolean, String_t& strType, 
									VecBuf_t& vecBuf, uint32& uPos)
{
	strType.append("b");

	PackData(vecBuf, &bBoolean, sizeof(bool), uPos);
}

//处理double，因为lua里面的number是用double表示的
void CScriptPackSerial::PackDouble(double dNum, String_t& strType, 
								   VecBuf_t& vecBuf, uint32& uPos)
{
	strType.append("d");
	
	PackData(vecBuf, &dNum, sizeof(double), uPos);
}

//处理nil
void CScriptPackSerial::PackNil(String_t& strType)
{
	strType.append("n");
}

//处理light user data
void CScriptPackSerial::PackFunctionContext(void* pContext, String_t& strType, 
									 VecBuf_t& vecBuf, uint32& uPos)
{
	strType.append("f");

	PackData(vecBuf, (void*)&pContext, sizeof(void*), uPos);
}

//对于function，由于这个function是要运回来处理的，所以我们这里进行ref引用，同时把
//引用的值作为light userdata传递出去
void CScriptPackSerial::PackFunction(lua_State* pState, int32 index, String_t& strType, 
									 VecBuf_t& vecBuf, uint32& uPos)
{
	int32 iTop = lua_gettop(pState);
	lua_pushlightuserdata(pState, GetSerialRefTblKey());
	lua_gettable(pState, LUA_GLOBALSINDEX);
	if (lua_isnil(pState, -1))
	{
		lua_pop(pState, 1);
		lua_newtable(pState);
		lua_pushlightuserdata(pState, GetSerialRefTblKey());
		lua_pushvalue(pState, -2);
		lua_settable(pState, LUA_GLOBALSINDEX);

		lua_pushvalue(pState, -1);
		lua_setglobal(pState, "__SerailRefTbl");
	}

	lua_pushvalue(pState, index);
	
	//增加userdata的引用，如果不做，很有可能被回收
	size_t iFunRef = (size_t)luaL_ref(pState, -2);
	void* pContext;
	memcpy(&pContext, &iFunRef, sizeof(size_t));
	lua_settop(pState, iTop);

	PackFunctionContext(pContext, strType, vecBuf, uPos);
}

//处理table
void CScriptPackSerial::PackTable(lua_State* pState, int32 index, String_t& strType, 
								  VecBuf_t& vecBuf, uint32& uPos)
{
	int32 iTop = lua_gettop(pState);

	Ast(lua_istable(pState, index));

	strType.append("t{");

	lua_pushnil(pState);

	while(lua_next(pState, index) != 0)
	{
		//key at stack -2, value at stack -1
		int32 iKeyType = lua_type(pState, -2);
		switch(iKeyType)
		{
		case LUA_TNUMBER:
			{
				PackDouble(lua_tonumber(pState, -2), strType, vecBuf, uPos);
				break;
			}
		case LUA_TSTRING:
			{
				size_t stLen = 0;
				const char* str = lua_tolstring(pState, -2, &stLen);
				PackString(str, stLen, strType, vecBuf, uPos);
				break;
			}
		default:
			{
				luaL_error(pState, "Table的key只能为number或者string");
				break;
			}
		}

		int32 iValueType = lua_type(pState, -1);
		switch(iValueType)
		{
		case LUA_TNUMBER:
			{
				PackDouble(lua_tonumber(pState, -1), strType, vecBuf, uPos);
				break;
			}
		case LUA_TSTRING:
			{
				size_t stLen = 0;
				const char* str = lua_tolstring(pState, -1, &stLen);
				PackString(str, stLen, strType, vecBuf, uPos);
				break;
			}
		case LUA_TBOOLEAN:
			{
				PackBoolean((lua_toboolean(pState, -1) == 1), strType, vecBuf, uPos);
				break;
			}
		case LUA_TTABLE:
			{
				int32 top = lua_gettop(pState);
				PackTable(pState, top, strType, vecBuf, uPos);
				break;
			}
		case LUA_TUSERDATA:
			{
				CScriptPackSerial::PackValueTable(pState, strType, vecBuf, uPos);
				break;
			}
		default:
			{
				luaL_error(pState, "Table的value只能为number,string,boolean或者table");
				break;
			}
		}

		lua_pop(pState, 1);
	}

	strType.append("}");

	lua_settop(pState, iTop);

}

void CScriptPackSerial::PackValueTable(lua_State* pState, String_t& strType, 
										VecBuf_t& vecBuf, uint32& uPos)
{
	strType.append("v");

	void** ppObject = (void**)lua_touserdata(pState, -1);
	void* pObject = *ppObject;

	CValueTable* pValueTable = dynamic_cast<CValueTable*>((CBaseObject*)pObject);

	if (!pValueTable)
	{
		uint32 uLen = 0;
		PackData(vecBuf, &uLen, sizeof(uint32), uPos);
		return;
	}


	uint32 uBufferSize = (uint32)pValueTable->GetSerializeBufferSize();
	PackData(vecBuf, &uBufferSize, sizeof(uint32), uPos);

	size_t stSize = vecBuf.size();
	vecBuf.insert(vecBuf.end(), uBufferSize, 0);
	pValueTable->Serialize(&vecBuf[stSize]);

	uPos += uBufferSize;
}
