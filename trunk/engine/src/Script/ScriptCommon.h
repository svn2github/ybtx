#pragma once

extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
}

#include "CScript.h"

struct lua_State;

namespace sqr
{
	class CClass;
	class CScriptDebuger;
	class CBaseType;

	int32  ToUint32( lua_State* L );
	int32  ToInt32( lua_State* L );
	int32  ToUint16( lua_State* L );
	int32  ToInt16( lua_State* L );
	int32  ToUint8( lua_State* L );
	int32  ToInt8( lua_State* L );
	int32  BitAnd( lua_State* L );
	int32  BitOr( lua_State* L );
	int32  BitNot( lua_State* L );
	int32  BitXor( lua_State* L );

	int ASLoadFile( lua_State*L, const char* szFileName );

	CScript&		_S( lua_State* pState );
	lua_State*		_L( CScript& Script );

	//extern const char AS_GLOBAL_REFERENCE_TABLE_NAME[];
	//extern const char AS_REF_NUM_FILED_NAME[];
	//extern const char AS_REF_DATA_FILED_NAME[];

	void		MapObjectToUserdata( lua_State* pState, void* pUserData );
	void		GetObjectByUserdata( lua_State* pState, void* pUserData );

	void		MapObjectToID( lua_State* pState, uint32 uID );
	void		GetObjectByID( lua_State* pState, uint32 uID );


	//把栈顶table里面cpp obj ptr的field设置成pObj  obj["__CppObjPtr"] = pObj
	void SetCppObjPtr(lua_State* pState, void* pObj);

	//得到栈顶table里面cpp obj ptr的field的值，压入lua堆栈  return obj["__CppObjPtr"]
	void GetCppObjPtr(lua_State* pState);

	//在全局table里把obj tbl的field设置为指向的table  _G["__ObjTable"] = objtbl
	void SetGlobalObjTable(lua_State* pState);

	//在全局table里得到obj tbl的field的值，压入lua堆栈 return _G["__ObjTable"]
	void GetGlobalObjTable(lua_State* pState);

	//设置cpp mem leak table
	void SetCppMemLeakTable(lua_State* pState);

	//获取cpp mem leak table
	void GetCppMemLeakTable(lua_State* pState);

	//遍历某一个table里面所有的key value，并输出到相应文件
	int TraverseTable(lua_State* pState);

	void OnScriptError(lua_State* pState, int32 nErrorCode);
}
