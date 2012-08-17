#include "stdafx.h"
#include "CDataType.h"
#include "CBufAcr.h"
#include "ScriptCommon.h"
#include "ScriptCppObj.h"
#include "CClass.h"
#include "CScriptData.h"
#include "CBaseObject.h"
#include "ScriptKey.h"
#include "CVariant.h"

//===========================		CTypeString		==================================
CTypeString::CTypeString( bool bLongStr ) 
: m_bLongStr(bLongStr) 
{}

size_t CTypeString::GetPackBufLen( char* pDataBuf )		
{ 
	const char *pStr = *(char**)( pDataBuf + m_uOffset );

	if( pStr == NULL )
	{
		GenErr("Rpc 参数为 nil");
	}

	
	return strlen( pStr ) + 1 + m_bLongStr + 1;
}

void CTypeString::GetPacker( char* pDataBuf, char* pDes )	
{ 
	const char* pStr = *(char**)( pDataBuf + m_uOffset );
	if( pStr == NULL )
	{
		GenErr("Rpc 参数为 nil");
	}

	size_t size = strlen( pStr ); 
	if( m_bLongStr )
	{	
		if( size >= 65536 )
		{
			ostringstream strm;
			strm << "实际长度: " << size;
			GenErr("Lua传入的字符串长度大于注册的最大长度65535", strm.str().c_str());
		}

		*(uint16*)pDes = (uint16)size;
		memcpy( pDes + 2, pStr, size + 1);
	}
	else
	{
		if( size >= 256 )
		{
			ostringstream strm;
			strm << "实际长度: " << size;
			GenErr("Lua传入的字符串长度大于注册的最大长度255", strm.str().c_str());
		}

		*(uint8*)pDes = (uint8)size;
		memcpy( pDes + 1, pStr, size + 1);
	}
}

int32 CTypeString::UnPack( char* pDataBuf, char* pValue, size_t size )
{
	if( size < (size_t)( 1 + m_bLongStr ) )
		return 0;

	size_t uStrLen = m_bLongStr ? *(uint16*)pValue : *(uint8*)pValue;
	char* pStr = pValue + m_bLongStr; 
	if( size < 1 + m_bLongStr + uStrLen + 1)
		return 0;

	pStr += 1;
	//memmove( pStr, pStr + 1, uStrLen );
	pStr[uStrLen] = 0;		
	*(const char**)( pDataBuf + m_uOffset ) = pStr;	
	return (int32)(1 + m_bLongStr + uStrLen + 1);
}

//===========================		CTypeCppObj		==================================
CTypeCppObj::CTypeCppObj( CClass* pClassInfo, CBaseType::EFlag eFlag )
: m_pClassInfo( pClassInfo )
{	
	m_Flag = m_Flag | eFlagValue | eFlagCppObj | eFlag;	
}

const char* CTypeCppObj::GetClassName() const
{
	return m_pClassInfo->GetClassName();
}

void CTypeCppObj::GetFromLuaStack( lua_State* pState, char* pDataBuf, int32 LuaStkId )
{
	int32 absIdx = absLuaStackIdx(pState, LuaStkId);
	
	if( lua_isnil( pState, LuaStkId ) )
	{
		*(size_t*)( pDataBuf + m_uOffset ) = 0;	
		return;
	}
	
#ifdef _DEBUG
	if(m_pClassInfo->IsIsolatedClass())
	{
		//对于unsafe对象，参数类型传错了我们在这里检查，对于safe对象我们在c++层面就可以检查了
		lua_getmetatable(pState, absIdx);
		lua_getfield(pState, -1, "__info");
		CClass* pClass = (CClass*)lua_touserdata(pState, -1);
		Ast(pClass);
		lua_pop(pState, 2);
		if(pClass != m_pClassInfo)
		{
			ostringstream str;
			str << "传入参数类型为: " << pClass->GetClassName() << endl
				<< "实际参数类型为: " << m_pClassInfo->GetClassName() << endl
				<< "参数类型不匹配，请检查!" << endl;
			GenErr(str.str().c_str());
		}
	}
#endif
		
	if (lua_isuserdata(pState, LuaStkId))
	{
		*(size_t*)( pDataBuf + m_uOffset ) = *(size_t*)( lua_touserdata( pState, LuaStkId ) );
	}
	else
	{
		lua_pushlightuserdata(pState, GetCppObjPtrKey());
		lua_gettable(pState, absIdx);

		if(lua_isnil(pState, -1))
		{
			*(size_t*)( pDataBuf + m_uOffset ) = 0;		
		}
		else if(lua_islightuserdata(pState, -1)) 
		{	
			*(size_t*)( pDataBuf + m_uOffset ) = (size_t)( lua_touserdata( pState, -1 ) );
		}	
		else
		{
			*(size_t*)( pDataBuf + m_uOffset ) = *(size_t*)( lua_touserdata( pState, -1 ) );
		}
		lua_pop( pState, 1 );
	}
}

static bool GetRegClass(lua_State* pState, CBaseObject* pObject, CClass* pClassInfo)
{
	//注册的时候可能是父类，传入的参数实际是子类，这里我们用子类的class，这样不用cast了
	CClass* pInfo = _S( pState ).GetClassInfoWithType(typeid(*pObject).name());
	if(!pInfo)
	{
		//如果info为null，表明实际的类型并没有注册给lua，这里我们就使用注册的classinfo
		if (!pClassInfo)
		{
			//如果也没有注册的classinfo，那么我们压入nil
			lua_pushnil(pState);
			return false;
		}

		pInfo = pClassInfo;
	}

	Ast(!pInfo->IsIsolatedClass());

	lua_rawgeti(pState, LUA_REGISTRYINDEX, pInfo->GetClassRef());
	Ast( !lua_isnil( pState, -1 ) );			//szClass必须被注册

	return true;
}

namespace sqr 
{
	void SetUserDataObjMeta(lua_State* pState);
}

static void PushCppObj2Script(lua_State* pState, CBaseObject* pObject, CClass* pClassInfo)
{
	if( pObject == NULL )
	{
		lua_pushnil( pState );
		return;
	}

	EBaseObjectType eType = pObject->GetBaseObjectType();
	Ast(eType != eBOT_None);

	if (eType == eBOT_Share)
	{
		uint32 uID = pObject->GetBaseObjectID();
		GetObjectByID(pState, uID);
		if(!lua_isnil(pState, -1))
		{
			return;
		}
		else
		{
			lua_pop(pState, 1);
		}

		if(!GetRegClass(pState, pObject, pClassInfo))
			return;

		void** ppObject = (void**)lua_newuserdata(pState, sizeof(void*)); //class, userdata
		*ppObject = pObject;

		SetUserDataObjMeta(pState); //userdata

		lua_pushvalue(pState, -1); //userdata, userdata

		MapObjectToID(pState, uID);	//userdata
		
		return;
	}
	else
	{
		if(pObject->VisibleToScript())
		{
			//如果是c++创建传入lua的对象
			if(!pObject->CreatedByScript())
			{
				int iRef = pObject->GetScriptRefNum();
				lua_rawgeti(pState, LUA_REGISTRYINDEX, iRef);
			}
			else
			{
				GetObjectByUserdata(pState, pObject); //obj		
			}
		}
		else
		{
			if(!GetRegClass(pState, pObject, pClassInfo))
				return;

			if (eType == eBOT_Dynamic)
			{
				lua_newtable( pState );// class, table

				lua_pushvalue(pState, -2); //class, table, class
				lua_setmetatable( pState, -2 );			//class, table
				lua_remove(pState, -2);	//table
				SetCppObjPtr(pState, pObject);
			}
			else
			{
				void** ppObject = (void**)lua_newuserdata(pState, sizeof(void*)); //class, userdata
				*ppObject = pObject;
				
				SetUserDataObjMeta(pState); //userdata
			}
			//因为该对象对应的table不能被lua垃圾回收，
			//所以我们要将table给ref住
			lua_pushvalue( pState, -1 );
			int ref = luaL_ref(pState, LUA_REGISTRYINDEX);

			pObject->SetScriptRefNum(ref);
			pObject->SetObjectFlag( eBOSF_VisableToScript );
		}	
	}
}

void CTypeCppObj::Push2LuaStack( lua_State* pState, char* pDataBuf )
{
	if(m_pClassInfo->IsIsolatedClass())
	{
		GenErr("注册的isolated类不能在c++里面创建对象并传递到lua里面使用", m_pClassInfo->GetClassName());
	}

	CBaseObject* pObj = (CBaseObject*)( *(size_t*)( pDataBuf + m_uOffset ) );
		
	PushCppObj2Script(pState, pObj, m_pClassInfo);
}

//===========================		CTypeMemory	==================================
CTypeMemory::CTypeMemory()
{
	m_Flag = m_Flag | eFlagMemory;
}

void CTypeMemory::GetFromLuaStack( lua_State* pState, char* pDataBuf, int32 LuaStkId )
{
	int32 absIdx = absLuaStackIdx( pState, LuaStkId );

	if( lua_isnil( pState, absIdx ) )
	{
		*(void**)(pDataBuf + m_uOffset) = NULL;
		return;
	}

	lua_getfield( pState, absIdx, "__buf" );
	Ast( !lua_isnil(pState, -1) );
	void **pAddr = (void**)(lua_touserdata( pState, -1) );
	MapObjectToUserdata( pState, pAddr );

	memcpy( pDataBuf + m_uOffset, &pAddr, GetLen() );		
}

void CTypeMemory::Push2LuaStack( lua_State* pState, char* pDataBuf )
{
	void* pAddr = *( (void**)(pDataBuf + m_uOffset) );	//pDataBuf + m_uOffset保存的是 __ptr 的地址
	if( pAddr == NULL )
	{
		lua_pushnil( pState );
		return;
	}

	lua_newtable( pState );
	int nTop = lua_gettop( pState );
	__makeTable2CBufAcr( pState, nTop, pAddr );
}

void CTypeMemory::SetValue( char* pDataBuf, void* pValue )
{
	memcpy( pDataBuf + m_uOffset, pValue, GetLen() );
}

void CTypeMemory::GetValue( char* pDataBuf, void* pDes )
{
	memcpy( pDes, pDataBuf + m_uOffset, GetLen() );
}

/************************************************************************/
/*  
	CTypeVariant
*/
/************************************************************************/
CTypeVariant::CTypeVariant()
:m_pCurVar(NULL)
{
	
}

CTypeVariant::~CTypeVariant()
{
	
}

void CTypeVariant::Release()
{
	
}

static void GetVarFromLuaStack(CVariant *&pCurVar, lua_State* pState, int32 absIdx)
{
	int iLuaType = lua_type(pState, absIdx);

	//Ast(iLuaType == LUA_TNIL || iLuaType == LUA_TNUMBER 
	//	|| iLuaType == LUA_TSTRING || iLuaType == LUA_TBOOLEAN);

	switch(iLuaType)
	{
	case LUA_TNUMBER:
		{
			double val = lua_tonumber(pState, absIdx);
			pCurVar->SetNumber(val);
		}
		break;
	case LUA_TBOOLEAN:
		{
			bool val = (lua_toboolean(pState, absIdx) != 0);
			pCurVar->SetNumber(val);
		}
		break;
	case LUA_TSTRING:
		{
			const char* val = lua_tostring(pState, absIdx);
			pCurVar->SetCStr(val);
		}
		break;
	case LUA_TNIL:
		{
			pCurVar->SetEmptyTable();  //如果参数为nil，我们用一个空table表示
		}
		break;
	case LUA_TTABLE:
		{
			lua_pushlightuserdata(pState, GetCppObjPtrKey());
			lua_rawget(pState, -2);
			void* pPointer = NULL;
			if(lua_islightuserdata(pState, -1))
			{
				pPointer = lua_touserdata(pState, -1);
			}
			else if(lua_isuserdata(pState, -1))
			{
				pPointer = *(void**)lua_touserdata(pState, -1);
			}
			lua_pop(pState, 1);

			pCurVar->SetPtr(pPointer);
		}
		break;
	default:
		GenErr("CTypeVariant 错误的数据类型");
		break;
	}
}

void CTypeVariant::GetFromLuaStack(lua_State* pState, char* pDataBuf, int32 LuaStkId )
{
	m_pCurVar = CScript::Inst()->GetData()->CreateVariant();

	int32 absIdx = absLuaStackIdx( pState, LuaStkId );

	GetVarFromLuaStack(m_pCurVar, pState, absIdx);
		
	*(size_t*)( pDataBuf + m_uOffset ) = (size_t)( m_pCurVar );
}

static void SetVarToLuaStack(CVariant *pObj, lua_State* pState)
{
	if (!pObj)
	{
		lua_pushnil(pState);
		return;
	}

	EValueVariantType eVarType = pObj->GetType();

	switch(eVarType)
	{
	case eVVT_String:
		{
			const char* str = pObj->GetString();
			lua_pushstring(pState, str);
		}
		break;
	case eVVT_CStr:
		{
			const char* str = pObj->GetCStr();
			lua_pushstring(pState, str);
		}
		break;
	case eVVT_Bool:
		{
			bool bVal = pObj->GetNumber<bool>();
			lua_pushboolean(pState, bVal);
		}
		break;
	case eVVT_Int8:
		{
			double dVal = pObj->GetNumber<int8>();
			lua_pushnumber(pState, dVal);
		}
		break;
	case eVVT_Uint8:
		{
			double dVal = pObj->GetNumber<uint8>();
			lua_pushnumber(pState, dVal);
		}
		break;
	case eVVT_Int16:
		{
			double dVal = pObj->GetNumber<int16>();
			lua_pushnumber(pState, dVal);
		}		
		break;
	case eVVT_Uint16:
		{
			double dVal = pObj->GetNumber<uint16>();
			lua_pushnumber(pState, dVal);
		}		
		break;
	case eVVT_Int32:
		{
			double dVal = pObj->GetNumber<int32>();
			lua_pushnumber(pState, dVal);
		}		
		break;
	case eVVT_Uint32:
		{
			double dVal = pObj->GetNumber<uint32>();
			lua_pushnumber(pState, dVal);
		}		
		break;
	case eVVT_Int64:
		{
			double dVal =double( pObj->GetNumber<int64>());
			lua_pushnumber(pState, dVal);
		}		
		break;
	case eVVT_Uint64:
		{
			double dVal =double( pObj->GetNumber<uint64>());
			lua_pushnumber(pState, dVal);
		}		
		break;
	case eVVT_Float:
		{
			double dVal = pObj->GetNumber<float>();
			lua_pushnumber(pState, dVal);
		}		
		break;
	case eVVT_Double:
		{
			double dVal = pObj->GetNumber<double>();
			lua_pushnumber(pState, dVal);
		}		
		break;
	case eVVT_Ptr:
		{
			CBaseObject* pObject = (CBaseObject*)(const_cast<void*>(pObj->GetPtr()));
			PushCppObj2Script(pState, pObject, NULL);
		}
		break;
	default:
		GenErr("CVaraintObject::Push2LuaStack 错误的数据类型");
		break;
	}
}

void CTypeVariant::Push2LuaStack(lua_State* pState, char* pDataBuf )
{
	CVariant* pObj = (CVariant*)( *(size_t*)( pDataBuf + m_uOffset ) );
	
	SetVarToLuaStack(pObj, pState);
	
}

/************************************************************************/
/*                                                                      
	CTypeArgList
*/
/************************************************************************/
CTypeArgList::CTypeArgList()
:m_pvecVarList(NULL)
{
	
}

CTypeArgList::~CTypeArgList()
{
	//Release();
}

void CTypeArgList::Release()
{
	
}

void CTypeArgList::GetArgListFromLuaStack(lua_State *pState, int32 LuaStkId)
{

	int32 absIdx = absLuaStackIdx( pState, LuaStkId );
	int32 iTop = lua_gettop(pState);

	m_pvecVarList = CScript::Inst()->GetData()->CreateVariantList();
	
	for (int index = absIdx; index <= iTop; index++)
	{
		CVariant* pVar = CScript::Inst()->GetData()->CreateVariant();
		GetVarFromLuaStack(pVar, pState, index);
		m_pvecVarList->push_back(pVar);
	}

}

void CTypeArgList::GetFromLuaStack(lua_State* pState, char* pDataBuf, int32 LuaStkId )
{
	*(size_t*)( pDataBuf + m_uOffset ) = (size_t)( m_pvecVarList );
}

void CTypeArgList::Push2LuaStack(lua_State* pState, char* pDataBuf )
{
	vector<CVariant*> *pObj = (vector<CVariant*> *)( *(size_t*)( pDataBuf + m_uOffset ) );

	if (!pObj)
	{
		m_iParamNumber = 1;
		lua_pushnil(pState);
		return;
	}

	//得到参数个数
	m_iParamNumber = (int32)pObj->size();

	luaL_checkstack(pState, m_iParamNumber, "CTypeArgList Push2LuaStack Error");

	vector<CVariant*>::iterator iter, end;
	iter = pObj->begin();
	end = pObj->end();

	while (iter != end)
	{
		SetVarToLuaStack((*iter), pState);
		iter++;
	}

}

