#include "stdafx.h"

#include "ScriptCommon.h"
#include "ScriptKey.h"
#include "CClass.h"
#include "CLog.h"
#include "ThreadHelper.inl"
#include "TimeHelper.h"

namespace sqr
{
	int32 ToUint32( lua_State* pState )
	{
		double n = lua_tonumber( pState, -1 );
		lua_pop( pState, 1 );
		lua_pushnumber( pState, (uint32)n );
		return 1;
	}

	int32 ToInt32( lua_State* pState )
	{
		double n = lua_tonumber( pState, -1 );
		lua_pop( pState, 1 );
		lua_pushnumber( pState, (int32)(uint32)n );
		return 1;
	}

	int32 ToUint16( lua_State* pState )
	{
		double n = lua_tonumber( pState, -1 );
		lua_pop( pState, 1 );
		lua_pushnumber( pState, (uint16)(uint32)n );
		return 1;
	}

	int32 ToInt16( lua_State* pState )
	{
		double n = lua_tonumber( pState, -1 );
		lua_pop( pState, 1 );
		lua_pushnumber( pState, (int16)(uint32)n );
		return 1;
	}

	int32 ToUint8( lua_State* pState )
	{
		double n = lua_tonumber( pState, -1 );
		lua_pop( pState, 1 );
		lua_pushnumber( pState, (uint8)(uint32)n );
		return 1;
	}

	int32 ToInt8( lua_State* pState )
	{
		double n = lua_tonumber( pState, -1 );
		lua_pop( pState, 1 );
		lua_pushnumber( pState, (int8)(uint32)n );
		return 1;
	}

	int32 BitAnd( lua_State* pState )
	{
		uint32 n = 0xffffffff;
		int32 nTop = lua_gettop( pState );
		for( int32 i = 1; i <= nTop; i++ )
			n &= (uint32)lua_tonumber( pState, i );
		lua_pop( pState, nTop );
		lua_pushnumber( pState, n );
		return 1;
	}

	int32 BitOr( lua_State* pState )
	{
		uint32 n = 0;
		int32 nTop = lua_gettop( pState );
		for( int32 i = 1; i <= nTop; i++ )
			n |= (uint32)lua_tonumber( pState, i );
		lua_pop( pState, nTop );
		lua_pushnumber( pState, n );
		return 1;
	}

	int32 BitNot( lua_State* pState )
	{
		double n = lua_tonumber( pState, -1 );
		lua_pop( pState, 1 );
		lua_pushnumber( pState, ~((size_t)n) );
		return 1;
	}

	int32 BitXor( lua_State* pState )
	{
		uint32 n = 0;
		int32 nTop = lua_gettop( pState );
		for( int32 i = 1; i <= nTop; i++ )
			n ^= (uint32)lua_tonumber( pState, i );
		lua_pop( pState, nTop );
		lua_pushnumber( pState, n );
		return 1;
	}

	CScript& _S( lua_State* pState )
	{
		(void)pState;
		return *CScript::Inst();
	}


	int32 LuaGetTop(lua_State* pState)
	{
		return lua_gettop(pState);
	}

	void LuaSetTop(lua_State* pState, int32 top)
	{
		lua_settop(pState, top);
	}

	void LuaPop(lua_State* pState, int32 pop)
	{
		lua_pop(pState, pop);
	}

	void LuaPushNumber(lua_State* pState, double value)
	{
		lua_pushnumber(pState, value);
	}

	void LuaPushString(lua_State* pState, const char* str)
	{
		lua_pushstring(pState, str);
	}

	void LuaPushBoolean(lua_State* pState, int value)
	{
		lua_pushboolean(pState, value);
	}

	void LuaPushLightUserData(lua_State* pState, void* pData)
	{
		lua_pushlightuserdata(pState, pData);
	}

	int32 LuaToBoolean(lua_State* pState, int32 index)
	{
		return lua_toboolean(pState, index);
	}

	double LuaToNumber(lua_State* pState, int32 index)
	{
		return lua_tonumber(pState, index);
	}

	const char* LuaToString(lua_State* pState, int32 index)
	{
		return lua_tostring(pState, index);
	}

	void* LuaToUserData(lua_State* pState, int32 index)
	{
		return lua_touserdata(pState, index);
	}

	int32 LuaUpValueIndex(int32 index)
	{
		return lua_upvalueindex(index);
	}

	void LuaL_Error(lua_State* pState, const char* str)
	{
		luaL_error(pState, str);
	}

	int32 LuaIsNil(lua_State* pState, int32 index)
	{
		return lua_isnil(pState, index);
	}

	int32 LuaIsNumber(lua_State* pState, int32 index)
	{
		return lua_isnumber(pState, index);
	}

	int32 LuaIsString(lua_State* pState, int32 index)
	{
		return lua_isstring(pState, index);
	}

	int32 LuaIsTable(lua_State* pState, int32 index)
	{
		return lua_istable(pState, index);
	}

	int32 LuaIsUserData(lua_State* pState, int32 index)
	{
		return lua_isuserdata(pState, index);
	}

	void MapObjectToUserdata( lua_State* pState, void* pUserData )
	{
		Ast(pUserData);
		GetGlobalObjTable( pState );  //obj, global
		lua_pushlightuserdata( pState, pUserData );  //obj, global, userdata
		lua_pushvalue( pState, -3 );  //obj, global, userdata, obj
		lua_settable( pState, -3 );   //obj, global
		lua_pop( pState, 2 );
	}

	void GetObjectByUserdata( lua_State* pState, void* pUserData )
	{
		GetGlobalObjTable( pState );
		lua_pushlightuserdata( pState, pUserData );
		lua_gettable( pState, -2 );
		lua_remove( pState, -2 );
	}

	void MapObjectToID( lua_State* pState, uint32 uID )
	{
		GetGlobalObjTable(pState); //obj, global
		lua_pushnumber(pState, uID); //obj, global, id
		lua_pushvalue(pState, -3);  //obj, global, id, obj
		lua_settable(pState, -3);  //obj, global
		lua_pop(pState, 2);
	}

	void GetObjectByID( lua_State* pState, uint32 uID )
	{
		GetGlobalObjTable(pState);
		lua_pushnumber(pState, uID);
		lua_gettable(pState, -2);
		lua_remove(pState, -2);
	}

	//把栈顶table里面cpp obj ptr的field设置成pObj
	void SetCppObjPtr(lua_State* pState, void* pObj)
	{
		lua_pushlightuserdata(pState, GetCppObjPtrKey());  //..., obj, key	
		lua_pushlightuserdata(pState, pObj);           //..., obj, key, pObj
		lua_rawset(pState, -3);                        //..., 
	}


	//得到栈顶table里面cpp obj ptr的field的值，压入lua堆栈
	void GetCppObjPtr(lua_State* pState)
	{
		lua_pushlightuserdata(pState, GetCppObjPtrKey());  //obj, key
		lua_rawget(pState, -2);
	}

	//在全局table里把obj tbl的field设置为index指向的table
	void SetGlobalObjTable(lua_State* pState)
	{
		lua_pushlightuserdata(pState, GetGlobalObjTblKey()); //tbl, key
		lua_pushvalue(pState, -2);							 //tbl, key, tbl
		lua_settable(pState, LUA_GLOBALSINDEX);              //tbl
	
		lua_setglobal(pState, GetGlobalObjTblKeyName());	
	}

	//在全局table里得到obj tbl的field的值，压入lua堆栈
	void GetGlobalObjTable(lua_State* pState)
	{
		lua_pushlightuserdata(pState, GetGlobalObjTblKey());  //key
		lua_gettable(pState, LUA_GLOBALSINDEX);
	}


	void SetCppMemLeakTable(lua_State* pState)
	{
		lua_pushlightuserdata(pState, GetCppMemLeakTblKey()); //tbl, key
		lua_pushvalue(pState, -2);							 //tbl, key, tbl
		lua_settable(pState, LUA_GLOBALSINDEX);              //tbl
		lua_setglobal(pState, GetCppMemLeakTblKeyName());				
	}

	void GetCppMemLeakTable(lua_State* pState)
	{
		lua_pushlightuserdata(pState, GetCppMemLeakTblKey());  //key
		lua_gettable(pState, LUA_GLOBALSINDEX);
	}

	static void LogTableInfo(lua_State* pState, int index, string& msg)
	{
		char buf[1024];

		int32 type = lua_type(pState, index);
		switch(type)
		{
		case LUA_TNUMBER:
			{
				double num = lua_tonumber(pState, index);
				sprintf(buf, LUA_NUMBER_FMT, num);
				msg.append(buf);
			}
			break;
		case LUA_TSTRING:
			{
				msg.append(lua_tostring(pState, index));
			}
			break;
		case LUA_TBOOLEAN:
			{
				bool bSuc = lua_toboolean(pState, index) == 1;
				const char* type = bSuc ? "true" : "false";
				msg.append(type);
			}
			break;
		case LUA_TTABLE:
			{
				const void* p = lua_topointer(pState, index);
				sprintf(buf, "table:%p", p);
				msg.append(buf);
			}
			break;
		case LUA_TFUNCTION:
			{
				const void* p = lua_topointer(pState, index);
				sprintf(buf, "function:%p", p);
				msg.append(buf);
			}
			break;
		case LUA_TUSERDATA:
			{
				const void* p = lua_topointer(pState, index);
				sprintf(buf, "userdata:%p", p);
				msg.append(buf);
			}
			break;
		case LUA_TLIGHTUSERDATA:
			{
				const void* p = lua_topointer(pState, index);
				sprintf(buf, "lightuserdata:%p", p);
				msg.append(buf);
			}
			break;
		case LUA_TTHREAD:
			{
				const void* p = lua_topointer(pState, index);
				sprintf(buf, "thread:%p", p);
				msg.append(buf);		
			}
			break;
		default:
			{
				msg.append("LuaData");
			}
			break;
		}
	
		msg.append("\t");
	}

	static void _TraverseTable(lua_State* pState, set<const void*>& tbl, CLog* pLog, int& level, int nMaxLevel, uint32& uDataNum);

	void _TraverseFunc(lua_State* pState, set<const void*>& tbl, CLog* pLog, int& level, int nMaxLevel, uint32& uUpvalueNum)
	{
		uUpvalueNum = 0;

		const void* p = lua_topointer(pState, -1);

		bool bScu = tbl.insert(p).second;
		if (!bScu)
		{
			return;
		}

		//如果是c function，我们不处理
		if(	lua_iscfunction(pState, -1) )
		{
			return;
		}

		if (level > nMaxLevel)
		{
			return;
		}

		level++;

		int32 top = lua_gettop(pState);
		int32 nFuncIndex = top;

		int32 n = 1;

		const char* name = NULL;

		string msg;

		lua_checkstack(pState, 1);
		while ((name = lua_getupvalue(pState, nFuncIndex, n++)) != NULL)
		{
			++uUpvalueNum;
	
			msg.clear();
			msg.append(level, '\t');
			msg.append(name);
			msg.append("\t");

			LogTableInfo(pState, -1, msg);

			pLog->Write(msg.c_str());

			switch(lua_type(pState, -1))
			{
			case LUA_TTABLE:
				{
					uint32 uDataNum = 0;
					_TraverseTable(pState, tbl, pLog, level, nMaxLevel, uDataNum);
					if (uDataNum > 0)
					{
						char buf[32];
						sprintf(buf, "\t DataNum: %u", uDataNum);
						msg.append(buf);
						pLog->Write(msg.c_str());
					}
				}
				break;
			case LUA_TFUNCTION:
				{
					uint32 uUpValueNum = 0;
					_TraverseFunc(pState, tbl, pLog, level, nMaxLevel, uUpValueNum);
					if (uUpValueNum > 0)
					{
						char buf[32];
						sprintf(buf, "\t UpValueNum: %u", uUpValueNum);
						msg.append(buf);
						pLog->Write(msg.c_str());
					}
				}
				break;
			default:
				break;
			}

			lua_pop(pState, 1);
		}

		lua_settop(pState, top);
		level--;
	}

	void _TraverseTable(lua_State* pState, set<const void*>& tbl, CLog* pLog, int& level, int nMaxLevel, uint32& uDataNum)
	{
		uDataNum = 0;

		const void* p = lua_topointer(pState, -1);
		//防止循环遍历table
		bool bSuc = tbl.insert(p).second;
		if(!bSuc)
			return;

		//如果大于最大深度了，我们停止遍历
		if (level > nMaxLevel)
		{
			return;
		}

		level++;

		int top = lua_gettop(pState);
	
		lua_checkstack(pState, 3);

		lua_pushnil(pState);
		
		string msg;

		while(lua_next(pState, -2) != 0)
		{
			++uDataNum;

			msg.clear();

			msg.append(level, '\t');
			LogTableInfo(pState, -2, msg);
			LogTableInfo(pState, -1, msg);
			pLog->Write(msg.c_str());

			int32 type = lua_type(pState, -1);
			switch(type)
			{
			case LUA_TTABLE:
				{
					uint32 uDataNum = 0;
					_TraverseTable(pState, tbl, pLog, level, nMaxLevel, uDataNum);
					if (uDataNum > 0)
					{
						char buf[32];
						sprintf(buf, "\t DataNum: %u", uDataNum);
						msg.append(buf);
						pLog->Write(msg.c_str());
					}
				}
				break;
			case LUA_TFUNCTION:
				{
					uint32 uUpValueNum = 0;
					_TraverseFunc(pState, tbl, pLog, level, nMaxLevel, uUpValueNum);
					if (uUpValueNum > 0)
					{
						char buf[32];
						sprintf(buf, "\t UpValueNum: %u", uUpValueNum);
						msg.append(buf);
						pLog->Write(msg.c_str());
					}
				}
				break;
			default:
				break;
			}

			lua_pop(pState, 1);
		}

		lua_settop(pState, top);
		level--;
	}

	int TraverseTable(lua_State* pState)
	{
		if(!lua_istable(pState, 1))
			return 0;

		string name;
		if (lua_isstring(pState, 2))
		{
			name = lua_tostring(pState, 2);
		}
		else
		{
			char buf[50];
			static int32 nNum = 0;
			sprintf(buf, "Trav%u_%u.log", Atomic_FetchAndAdd(&nNum, 1), GetCurPID());
			name = buf;
		}
		const char* mode = (lua_isstring(pState, 3)) ? (lua_tostring(pState, 3)) : "w";
		
		const int ls_nMaxTravelLevel = 10;

		int nMaxLevel = (lua_isnumber(pState, 4)) ? (int)lua_tonumber(pState, 4) : ls_nMaxTravelLevel;

		set<const void*> tbl;
		int level = -1;

		uint64 uBegin = GetProcessTime();

		//在travel的时候必须得关闭thread watch功能，因为travel可能会很慢
		HTHREAD hThread;
		GetCurrentThread(&hThread);
		EnableWatchThread(hThread, false);

		if (lua_istable(pState, 5))
		{
			//filter object table
			lua_checkstack(pState, 3);

			lua_pushnil(pState);

			while (lua_next(pState, -2))
			{
				const void* p = lua_topointer(pState, -1);
				tbl.insert(p);
				lua_pop(pState, 1);
			}
		}

		lua_settop(pState, 1);

		CLog* pLog = CLog::CreateLog(name.c_str(), mode);
		pLog->EnableTime(false);

		uint32 uDataNum = 0;
		_TraverseTable(pState, tbl, pLog, level, nMaxLevel, uDataNum);
		EnableWatchThread(hThread, true);

		uint64 uEnd = GetProcessTime();
		
		ostringstream strm;
		strm << endl;
		strm << "遍历花费时间: " << uEnd - uBegin << " 毫秒" << endl;

		pLog->Write(strm.str().c_str());

		pLog->Release();

		return 0;
	}      

	void OnScriptError(lua_State* pState, int32 nErrorCode)
	{
		(void)pState;
		if (nErrorCode == LUA_ERRMEM)
		{
			ShowErrorMsgAndExit("Not Enough Memory, the program will be shutdown");
		}
	}
}
