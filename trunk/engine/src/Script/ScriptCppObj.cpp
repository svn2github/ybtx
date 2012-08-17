#include "stdafx.h"

#include "ScriptCppObj.h"
#include "ExpHelper.h"
#include "DataMake.h"
#include "CScriptData.h"
#include "CBaseType.h"
#include "CDataType.h"
#include "CScript.h"
#include "CClass.h"
#include "CBaseObject.h"
#include "ErrLogHelper.h"
#include "ScriptKey.h"

namespace sqr
{
	static void StoreCppObjMemInfo( lua_State* pState )
	{
		bool bEnabled = CScript::Inst()->GetData()->m_bCppMemLeakEnabled;

		if (!bEnabled)
		{
			return;
		}

		int32 nTop = lua_gettop(pState); //userdata

		GetCppMemLeakTable(pState); //userdata, memtbl

		Ast(lua_istable(pState, -1));

		lua_Debug ld;
		if(lua_getstack (pState, 2, &ld))
		{
			lua_getinfo(pState, "Sl", &ld);
			char buffer[1024];
			int32 count = sprintf(buffer, "%s:%d", ld.source, ld.currentline);
			lua_pushvalue(pState, nTop);		//userdata, memtbl, userdata
			lua_pushlstring(pState, buffer, count); //userdata, memtbl, userdata, name
			lua_settable(pState, -3);		//userdata, memtbl
		}	

		lua_settop(pState, nTop);
	}

	static void* CreateCppObject(lua_State* pState, CClass* pClass)
	{
		void** ppObject = (void**)lua_newuserdata(pState, sizeof(void*));
		void* pObject = pClass->Create(pState); //class, userdata
		*ppObject = pObject;

		StoreCppObjMemInfo(pState);

		return pObject;
	}

	void SetUserDataObjMeta(lua_State* pState)
	{
		lua_pushvalue(pState, -2);	//class, userdata, class
		lua_setmetatable(pState, -2);	//class, userdata

		lua_remove(pState, -2);	//userdata
	}

	int32 CreateIsolatedObjFromLua(lua_State* pState)
	{
		CClass* pClass = (CClass*)lua_touserdata(pState, lua_upvalueindex(1));
		Ast(pClass->IsIsolatedClass());

		void* pObject = CreateCppObject(pState, pClass);

		SetUserDataObjMeta(pState);
	
		lua_pushvalue(pState, -1); //userdata, userdata

		MapObjectToUserdata(pState, pObject); //userdata

		return 1;
	}

	int32 CreateStaticObjFromLua(lua_State* pState)
	{
		CClass* pClass = (CClass*)lua_touserdata(pState, lua_upvalueindex(1));
		Ast(pClass->IsStaticClass());

		void* pObject = CreateCppObject(pState, pClass); //class, userdata

		CBaseObject* cBase = (CBaseObject*) (pObject);
		cBase->SetObjectFlag( eBOSF_VisableToScript | eBOSF_CreatedByScript );

		SetUserDataObjMeta(pState);

		lua_pushvalue(pState, -1); //userdata, userdata

		MapObjectToUserdata(pState, pObject); //userdata
		
		return 1;
	}
	

	static int32 _SetCppObjFromLua(lua_State* pState, CClass* pClass)
	{
		lua_pushvalue(pState, -2); //class, obj, class
		lua_setmetatable(pState, -2); //class, obj

		void* pObject = CreateCppObject(pState, pClass); //class, obj, userdata

		CBaseObject* cBase = (CBaseObject*) (pObject);
		cBase->SetObjectFlag( eBOSF_VisableToScript | eBOSF_CreatedByScript );

		lua_pushvalue(pState, -3);  //class, obj, userdata, class
		lua_setmetatable(pState, -2); //class, obj, userdata

		lua_remove(pState, -3); //obj, userdata

		lua_pushlightuserdata(pState, GetCppObjPtrKey());  //obj, userdata, key
		lua_pushvalue(pState, -2);                         //obj, userdata, key, userdata
		lua_rawset(pState, -4);                            //obj, userdata
		lua_pop(pState, 1);       

		lua_pushvalue(pState, -1);
		MapObjectToUserdata(pState, pObject);  //obj


		return 1; //stack top = 1, 返回Obj, 留在堆栈里
	}

	int32 CreateDynamicObjFromLua(lua_State* pState)
	{
		CClass* pClass = (CClass*)lua_touserdata(pState, lua_upvalueindex(1));
		Ast(pClass->IsDynamicClass());

		lua_newtable(pState); //class, obj
		
		_SetCppObjFromLua(pState, pClass);

		return 1; //stack top = 1, 返回Obj, 留在堆栈里
	}
	
	int32 InitCppObjFromLua(lua_State* pState)
	{
		CClass* pClass = (CClass*)lua_touserdata(pState, lua_upvalueindex(1)); //class, obj table
		Ast(pClass->IsDynamicClass() || pClass->IsStaticClass());

		Ast(lua_istable(pState, 2));

		_SetCppObjFromLua(pState, pClass);

		return 1; //stack top = 1, 返回Obj, 留在堆栈里		
	}

	int DestroyCppObjFromLua( lua_State* pState )
	{
		CClass* pClass = (CClass*)lua_touserdata(pState, lua_upvalueindex(1));
		void* pObject = *(void**)lua_touserdata( pState, -1 );
		if(!pObject)
			return 0;

		pClass->Release(pState, pObject );	
		
		return 0;
	}	

}
