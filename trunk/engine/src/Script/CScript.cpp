#include "stdafx.h"
#include "CScript.h"
#include "ThreadHelper.h"
#include "CScriptDebuger.h"
#include "ExpHelper.h"
#include "ScriptCppObj.h"
#include "ScriptCommon.h"
#include "DataMake.h"
#include "ScriptX.h"
#include "CRunScript.h"
#include "CScriptData.h"
#include "CClass.h"
#include "ThreadHelper.h"
#include "CClassCallBack.h"
#include "ScriptDebugHelper.h"
#include "ScriptKey.h"
#include "CSqrAllocPool.h"
#include "TSqrAllocator.inl"
#include "CCallBackProfiler.h"

namespace sqr
{
	////============================		reference	table			======================================
	//const char AS_GLOBAL_REFERENCE_TABLE_NAME[]	=	"__global_reference_table";
	//const char AS_REF_NUM_FILED_NAME[]			=	"__ref_num";
	//const char AS_REF_DATA_FILED_NAME[]			=	"__ref_data";


	static std::string GetStringFromTopOfLuaStack( lua_State* pState )
	{
		string sError;
		const char* pstr = lua_tostring( pState, -1 );
		if(pstr)
			sError.assign(pstr);
		lua_pop( pState, 1);
		return sError;		
	}

	//-----------------------------------------------------
	// 将HScriptState句柄转换为lua的lua_State类型
	//-----------------------------------------------------

	static string LoadFile( lua_State* pState, const char* szFile )
	{
		//载入文件
		if ( ASLoadFile( pState, szFile ) )
			return GetStringFromTopOfLuaStack( pState );
		return "";
	}

	static string LoadString( lua_State* pState, const char* szString )
	{
		//载入文件
		if ( luaL_loadstring( pState, szString ) )
			return GetStringFromTopOfLuaStack( pState );
		return "";
	}

	static string LoadFunction( lua_State* pState, const char* szFunction )
	{
		lua_getglobal( pState, szFunction );
		return "";
	}

	//-----------------------------------------------------
	// 调用脚本函数
	//-----------------------------------------------------
	typedef string (*LOADER_FUN)(lua_State* pState,const char*);

	static std::string LoadAndCallFunction( lua_State* pState, LOADER_FUN Loader, const char* szName, const char* szFormat, va_list vlArgs )
	{
		int32 nErrFunIndex=0;
		lua_pushcfunction(pState,&CScriptData::ErrorHandler );
		nErrFunIndex=lua_gettop(pState);

		string sResult = Loader( pState, szName );
		if(!sResult.empty())
		{
			lua_pop( pState, 1 );
			return sResult;
		}

		CRunScript runner( _S( pState ), szFormat, (char*)vlArgs );
		char* pDataBuf = (char*)alloca( runner.GetParamBufSize() );
		if( !runner.RunScript( pState, pDataBuf, nErrFunIndex ) )
			return GetStringFromTopOfLuaStack( pState );

		//如果此时我们调用了profiler，则必须在脚本运行结束之后运行profiler，结束profiler
		//不要等到整个函数结束之后运行，不然会导致lua_gettop(pState)==0)的Ast
		//if(_S(pState).GetHelperType() == eSH_Profiler)
		//{
		//	exit_profiler(pState);
		//}

		lua_remove( pState, nErrFunIndex );

		// 如果porfiler正常运行则弹出栈底，否则在Ast检测时出错
		if (lua_toboolean( pState, 1 ))
			lua_pop( pState, 1 );

		Ast(lua_gettop(pState)==0);
		return "";
	}

}

CScript* CScript::ms_pInst=NULL;

HTLSKEY	CScript::ms_tlsKey;


CScript* CScript::Inst()
{
	CScript* pScript = (CScript*)TLS_GetValue(ms_tlsKey);
	if(pScript)
		return pScript;

	return NULL;
}


void CScript::Init(EScriptHelper eScriptHelper,float fCallbackGCRatio,float fNormalUnpackGCRatio,float fSerialUnpackGCRatio)
{
	if( ms_pInst )
		GenErr("CScript::Init can not run twice.");

	ms_pInst=new CScript(eScriptHelper,fCallbackGCRatio,fNormalUnpackGCRatio,fSerialUnpackGCRatio);

	TLS_CreateKey(&ms_tlsKey);

	ms_pInst->SetRunningThread(GetCurTID());

	//下面这个函数是用来给调试器用的，用来显示lua的调用堆栈
	//初始化的时候，调用一下这个函数，防止release编译的时候被编译器给优化掉了
	sbt();

}

void CScript::Unit()
{
	delete ms_pInst;
	ms_pInst=NULL;

	TLS_DestroyKey(ms_tlsKey);
}


//-----------------------------------------------------
// 构造函数
//-----------------------------------------------------
CScript::CScript(EScriptHelper eScriptHelper,float fCallbackGCRatio,float fNormalUnpackGCRatio,float fSerialUnpackGCRatio)
:m_pData(NULL)
{
	m_hThreadID = GetCurTID();	//这赋值没用，SetRunningThread 才是真正赋值

	//把lua标准库载入到脚本虚拟机中
	m_pData= new CScriptData();
	m_pData->InitHelper(eScriptHelper);

	m_pData->m_fCallbackGCRatio=fCallbackGCRatio;
	m_pData->m_fNormalUnpackGCRatio=fNormalUnpackGCRatio;
	m_pData->m_fSerialUnpackGCRatio=fSerialUnpackGCRatio;

	m_pCallBackProfiler = new CCallBackProfiler();
}

//-----------------------------------------------------
// 析构函数
//-----------------------------------------------------
CScript::~CScript()
{
	m_pData->UnitHelper();
	delete m_pData;

	delete m_pCallBackProfiler;

	CScript* pScript = (CScript*)TLS_GetValue(ms_tlsKey);
	if(pScript)
		TLS_SetValue(ms_tlsKey, NULL);
}

//强制垃圾回收
void CScript::GCAll()
{
	lua_gc(m_pData->m_pState, LUA_GCRESTART, 0);
	lua_gc( m_pData->m_pState, LUA_GCCOLLECT, 0 );
	lua_gc(m_pData->m_pState, LUA_GCSTOP, 0);
}

//得到lua的内存占用
int CScript::GCCount()
{
	return lua_gc( m_pData->m_pState, LUA_GCCOUNT, 0 );
}

void CScript::GCStep(uint32 uMinStep)
{
	const int nStep=max<int>( int(uMinStep),int(m_pData->GetGCStepAndResetCounter()) );
	
	lua_gc(m_pData->m_pState, LUA_GCRESTART, 0);
	SQR_TRY
	{
		//如果这里面发生了异常，并且没有catch的话，会导致垃圾回收功能被打开
		lua_gc(m_pData->m_pState, LUA_GCSTEP, nStep );
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
	lua_gc(m_pData->m_pState, LUA_GCSTOP, 0);
}

//设置gc的属性
void CScript::SetGCStepMul(int stepmul)
{
	lua_gc(m_pData->m_pState, LUA_GCSETSTEPMUL, stepmul * 100);
}

void CScript::SetGCPause(int pause)
{
	lua_gc(m_pData->m_pState, LUA_GCSETPAUSE, pause * 100);
}

//得到Script数据
CScriptData* CScript::GetData() const
{
	return m_pData;
}

std::string CScript::RunFile( const char* szFile, const char* szFormat, ... )
{
	va_list vlArgs;
	va_start(vlArgs,szFormat);
	string sResult=LoadAndCallFunction( m_pData->m_pState, &LoadFile, szFile,szFormat,vlArgs );
	va_end(vlArgs);
	return sResult;
}

std::string CScript::RunString( const char* szString, const char* szFormat, ... )
{
	va_list vlArgs;
	va_start(vlArgs,szFormat);
	string sResult=LoadAndCallFunction( m_pData->m_pState, &LoadString, szString, szFormat, vlArgs );
	va_end(vlArgs);
	return sResult;
}

std::string CScript::RunFunction( const char* szFunction, const char* szFormat, ... )
{
	va_list vlArgs;
	va_start(vlArgs,szFormat);
	string sResult=LoadAndCallFunction( m_pData->m_pState, &LoadFunction, szFunction, szFormat,vlArgs );
	va_end(vlArgs);
	return sResult;
}


static void CleanPathName(string& sResult)
{
	//将"\\"替换成"/"
	replace(sResult.begin(), sResult.end(), '\\', '/');

	for(;;)
	{
		string::size_type stPos;

		//将"//"替换成"/"
		stPos=sResult.find("//");
		if(stPos != string::npos)
		{
			sResult.erase(stPos,1);
			continue;
		}

		//将"./"去掉
		stPos=sResult.find("./");
		if(  (stPos!=string::npos)  &&  ((stPos==0) || (sResult[stPos-1]=='/'))  )
		{
			sResult.erase(stPos,2);
			continue;
		}

		break;
	}
}


void CScript::SetLoadPath(const char* szPathAlias, const char* szPath)
{
	Ast(szPathAlias);
	Ast(szPath);

	lua_State* pState = m_pData->m_pState;
	int32 top = lua_gettop(pState);
	lua_getglobal(pState, "LoadPathTbl");
	
	Ast(lua_istable(pState, -1));

	string sPath(szPath);
	if (!sPath.empty())
	{
		sPath.append("/");
	}
	CleanPathName(sPath);

	m_pData->m_mapAliasToPath[szPathAlias] = sPath.c_str();

	lua_pushstring(pState, sPath.c_str());
	lua_setfield(pState, -2, szPathAlias);

	//设置全局的载入函数，require加上alias的前缀
	char func_buf[1024];
	sprintf(func_buf, "_G[\"%s_require\"] = function(file_name) return RequireFile(\"%s\", file_name) end", szPathAlias, szPathAlias);

	luaL_dostring(pState, func_buf);

	lua_settop(pState, top);
}

const char* CScript::GetLoadPath(const char* szPathAlias)
{
	Ast(szPathAlias);

	return m_pData->m_mapAliasToPath[szPathAlias].c_str();
}


//-----------------------------------------------------
// 注册类成员变量
//-----------------------------------------------------
void CScript::RegClassMember( const char* szArg, int32 nOffset, const char* szClass, const char* szMemberName )
{
	Ast(GetClassInfo(szClass)->IsIsolatedClass());

	CMemberVariable* pScriptMember = new CMemberVariable( *this, szArg, nOffset );
	m_pData->m_vecFunctions.push_back( pScriptMember );

	lua_getglobal( m_pData->m_pState, szClass );
	Ast( !lua_isnil( m_pData->m_pState, -1 ) ); 

	lua_pushlightuserdata( m_pData->m_pState, pScriptMember );
	lua_pushcclosure( m_pData->m_pState, (lua_CFunction)CMemberVariable::GetCppMemberVar, 1 );
	lua_setfield( m_pData->m_pState, -2, szMemberName );
	lua_pop( m_pData->m_pState, 1 );
}

//-----------------------------------------------------
//得到注册的类信息
//-----------------------------------------------------
CClass* CScript::GetClassInfo( const char* szClassName )
{
	CScriptData::MapClass_t::iterator it = m_pData->m_mapClasses.find( szClassName );
	if( it == m_pData->m_mapClasses.end() )
		return NULL;
	return it->second;
}

//-----------------------------------------------------
//通过rtti的name得到注册的类信息
//-----------------------------------------------------
CClass* CScript::GetClassInfoWithType( const char* szClassType )
{
	CScriptData::MapTypeClass_t::iterator it = m_pData->m_mapTypeClasses.find( szClassType );
	if( it == m_pData->m_mapTypeClasses.end() )
		return NULL;
	return it->second;
}

CClass* CScript::CreateClass(REG_CREATE_FUN pCreateFun, REG_DELETE_FUN pDeleteFun, 
							 size_t sizeOfClass, const char* szTypeClass, 
							 const char* szClass, int32 eClassType)
{
	CClass*& pClass = GetData()->m_mapClasses[ szClass ];
	if(pClass)
	{
		//已被注册
		ostringstream strm;
		strm<<"Class \""<<szClass<<"\" is already registered.";
		GenErr(strm.str());
	}

	pClass = new CClass( this, szClass );

	Ver(GetData()->m_mapTypeClasses.insert(
		make_pair(szTypeClass, pClass)).second);

	pClass->m_sizeOfClass	= sizeOfClass;

	
	pClass->m_pFunCreateObj = pCreateFun;
	pClass->m_pFunDeleteObj = pDeleteFun;
	pClass->m_eType = (EScriptClassType)eClassType;

	return pClass;
}

static void CheckClassNotReg(lua_State* pState, const char* szClass)
{
	lua_getglobal( pState, szClass );		//获和Cpp对象同名的lua类（table）	//stack: class(function) class
	if( !lua_isnil( pState, -1 ) )
	{
		//lua的全局表中有与szClass同名的对象存在
		ostringstream strm;
		strm<<"lua中类\""<<szClass<<"\"已经被注册，不允许再次注册";
		GenErr(strm.str());
	}
	//nil也是可以被放在堆栈里的，所以要pop出来		
	lua_pop( pState, 1 );						
}


static void SetClassBaseInfo(lua_State* pState, CClass* pClass, const char* szClass, 
							 lua_CFunction pCreateFunc, lua_CFunction pDeleteFunc)
{
	lua_pushvalue(pState, -1);		//class, class
	int iClassRef = luaL_ref(pState, LUA_REGISTRYINDEX);
	pClass->SetClassRef(iClassRef);	//class

	lua_pushvalue( pState, -1 );			//stack: class class
	lua_setglobal( pState, szClass );		//stack: class

	//给类设置类属性结构
	lua_pushlightuserdata( pState, pClass );		//stack: class  pClass
	lua_setfield( pState, -2, "__info" );	//class[__info] = pClass，此时stack: class
	
	if (pCreateFunc)
	{
		lua_pushlightuserdata(pState, pClass);
		lua_pushcclosure( pState, pCreateFunc, 1 );
		lua_setfield( pState, -2, "ctor" );
	}

	if (pDeleteFunc)
	{
		lua_pushlightuserdata(pState, pClass);  //class, pclass
		lua_pushcclosure(pState, pDeleteFunc, 1);	//class, gc
		lua_setfield( pState, -2, "__gc" );					//class, 
	}



	lua_pushlightuserdata(pState, pClass);
	lua_pushcclosure(pState, InitCppObjFromLua, 1);
	lua_setfield(pState, -2, "init_obj");

	EScriptClassType eType = pClass->GetType();
	lua_pushnumber(pState, (int32)eType);
	lua_setfield(pState, -2, "__ClassType");

	//设置类名
	lua_pushstring(pState,pClass->GetClassName());
	lua_setfield(pState, -2, "__ClassName");

	//将堆栈上新构造出来的类弹出堆栈,弹出此已经构造好的类
	lua_pop( pState, 1 );
}

void CScript::RegIsolatedClass( REG_CREATE_FUN pCreateFun, REG_DELETE_FUN pDeleteFun, size_t sizeOfClass, 
							   const char* szTypeClass, const char* szClass )
{
	CClass* pClass = CreateClass(pCreateFun, pDeleteFun, sizeOfClass, 
		szTypeClass, szClass, eSCT_Isolated);
	
	lua_State* pState = GetData()->m_pState;

	CheckClassNotReg(pState, szClass);

	lua_getglobal( pState, "isolated_class" );			
	Ast( !lua_isnil( pState, -1 ) );	

	lua_call(pState, 0, 1);	//class

	SetClassBaseInfo(pState, pClass, szClass, CreateIsolatedObjFromLua, DestroyCppObjFromLua);
}

void CScript::RegClass( int32 eClassType, REG_CREATE_FUN pCreateFun, REG_DELETE_FUN pDeleteFun,
					   size_t sizeOfClass, const char* szTypeClass, 
					   const char* szClass, ... )
{
	va_list va;
	va_start( va, szClass );

	EBaseObjectType eType = (EBaseObjectType)eClassType;

	if(eType == eBOT_Share && pCreateFun)
	{
		GenErr("share 类只允许注册成abstrace", szClass);
	}

	EScriptClassType eScriptClassType = eSCT_None;

	lua_CFunction pfnCreate = NULL;
	lua_CFunction pfnDelete = NULL;

	switch(eType)
	{
	case eBOT_Static:
		eScriptClassType = eSCT_Static;
		pfnCreate = CreateStaticObjFromLua;
		pfnDelete = DestroyCppObjFromLua;
		break;
	case eBOT_Share:
		eScriptClassType = eSCT_Share;
		break;
	case eBOT_Dynamic:
		eScriptClassType = eSCT_Dynamic;
		pfnCreate = CreateDynamicObjFromLua;
		pfnDelete = DestroyCppObjFromLua;
		break;
	default:
		GenErr("Invalid BaseObject Type");
		break;
	}

	CClass* pClass = CreateClass(pCreateFun, pDeleteFun, sizeOfClass, 
		szTypeClass, szClass, eScriptClassType);

	//调用完毕之后，lua stack 还有一个值，新的class
	lua_State* pState = GetData()->m_pState;

	CheckClassNotReg(pState, szClass);

	lua_getglobal( pState, "cpp_class" );			
	Ast( !lua_isnil( pState, -1 ) );			

	lua_pushnumber(pState, eScriptClassType);

	//生成全局类名
	int32 nParam = 1;

	const char* szBaseClass;
	while( ( szBaseClass = va_arg(va,const char*) )!=NULL )
	{
		CClass* pBaseClass=GetClassInfo( szBaseClass );
		if( ! pBaseClass )
		{
			ostringstream strm;
			strm<<"\""<<szBaseClass<<"\" not exists.";
			GenErr(strm.str());
		}
		
		pClass->AddBaseClass(pBaseClass);

		lua_getglobal( pState, szBaseClass );
		if( lua_isnil(pState,-1) )
			GenErr("Base class does not exist.");
		++ nParam;
	}

	va_end( va );

	lua_call( pState, nParam, 1 );			//stack: class

	SetClassBaseInfo(pState, pClass, szClass, pfnCreate, pfnDelete);
}

//-----------------------------------------------------
//注册函数参数打包函数集
//-----------------------------------------------------
HPACKAGE CScript::RegPackPakage( PACK_FUNCTION pPackFun, void* pCppContext, const char* szTableName )
{
	return m_pData->m_PackMgr.RegPackPakage( *this, pPackFun, pCppContext, szTableName );
}

//-----------------------------------------------------
//注册函数参数打包打包函数集的函数
//-----------------------------------------------------
void CScript::RegPackFunction( HPACKAGE hPakageHandle, const char* szFunction, uint32 uID, bool bFirstArgAsCallContext, const char* szFormat )
{
	m_pData->m_PackMgr.RegPackFunction( *this, hPakageHandle, szFunction, uID, bFirstArgAsCallContext, szFormat );
}

//-----------------------------------------------------
//注册函数参数解包函数集
//-----------------------------------------------------
HPACKAGE CScript::RegUnPackPakage( const char* szTableName, UNPACK_FUNCTION* pUnPackFun )
{
	return m_pData->m_PackMgr.RegUnPackPakage( *this, szTableName, pUnPackFun );
}

//-----------------------------------------------------
//注册函数参数解包函数集的解包函数
//-----------------------------------------------------
void CScript::RegUnPackFunction( HPACKAGE hPakageHandle, const char* szFunction, uint32 uID, bool bFirstArgAsCallContext, const char* szFormat )
{
	m_pData->m_PackMgr.RegUnPackFunction( *this, hPakageHandle, szFunction, uID, bFirstArgAsCallContext, szFormat );
}


//注册serial函数参数打包函数
void CScript::RegSerialPackFun(const char* szFunName, POST_PACKFUNCTION pPostPackFun)
{
	m_pData->m_ScriptSerial.RegPackFun(*this, szFunName, pPostPackFun);
}

//注册serial函数参数解包函数
void CScript::RegSerialUnPackFun(ON_UNPACKFUNCTION* pOnUnPackFun)
{
	m_pData->m_ScriptSerial.RegUnPackFun(*this, pOnUnPackFun);
}


//-----------------------------------------------------
//注册常量		
//-----------------------------------------------------	
void CScript::RegConstant( const char* szTableName, const char* szFeild, int32 nValue )
{
	lua_State* L = m_pData->m_pState;
	if( szTableName && szTableName[0] )
	{
		if( szFeild && szFeild[0] )
		{
			lua_getglobal( L, szTableName );
			if( lua_isnil( L, -1 ) )
			{
				lua_pop( L, 1 );
				lua_newtable( L );
				lua_pushvalue( L, -1 );
				lua_setglobal( L, szTableName );
			}
			lua_pushinteger( L, nValue );
			lua_setfield( L, -2, szFeild );
			lua_pop( L, 1 );
		}
		else
			m_pData->m_mapSizeOfEnum[szTableName] = nValue  ;
	}
	else
	{
		lua_pushinteger( L, nValue );
		lua_setglobal( L, szFeild );
	}
}

void CScript::AstInSameThread()
{
	Ast( InSameThread() );
}

inline bool CScript::InSameThread()
{
	HTHREADID id = GetCurTID();
	return id == m_hThreadID;
}

void CScript::SetRunningThread(HTHREADID id)
{
	m_hThreadID = id;

	TLS_SetValue(ms_tlsKey, this);

	m_pCallBackProfiler->Init(m_hThreadID);
}

void CScript::DetachRunningThread()
{
	TLS_SetValue(ms_tlsKey, NULL);
}


void CScript::UnlinkCppObj( void* pObj )
{
    //AstInSameThread();
	
	lua_State* L = m_pData->m_pState;
	int nTop = lua_gettop( L );

	GetObjectByUserdata(L, pObj);//stack:...,Obj

	if(!lua_isnil(L, -1))
	{
		lua_pushnil(L);
		lua_setmetatable(L, -2);

		if (lua_istable(L, -1))
		{
			lua_pushlightuserdata(L, GetCppObjPtrKey()); //obj, key
			lua_pushvalue(L, -1);                   //obj, key, key
			lua_rawget(L, -3);  //get userdata           //obj, key, userdata
			void** ppObject = (void**)lua_touserdata(L, -1);
			*ppObject = NULL;						//清空userdata里面指向的指针

			lua_pop(L, 1);                          //obj, key

			lua_pushnil(L);           
			lua_rawset(L, -3);   
		}
		else
		{
			void** ppObject = (void**)lua_touserdata(L, -1);
			*ppObject = NULL;						
		}

		GetGlobalObjTable( L );
		lua_pushlightuserdata(L, pObj);
		lua_pushnil(L);
		lua_settable(L, -3);
	}
	
	lua_settop( L, nTop );
}

void CScript::UnlinkCppObj( CBaseObject* pObject )
{
	EBaseObjectType eType = pObject->GetBaseObjectType();

	Ast(eType == eBOT_Static || eType == eBOT_Dynamic);

	//如果这个对象是c++创建并传入lua的
	if(!pObject->CreatedByScript())
	{
		//AstInSameThread();
	
		lua_State* L = m_pData->m_pState;

		int iRefNum = pObject->GetScriptRefNum();
		lua_rawgeti(L, LUA_REGISTRYINDEX, iRefNum); //obj
		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			return;
		}

		lua_pushnil(L);
		lua_setmetatable(L, -2);

		if (eType == eBOT_Dynamic)
		{
			lua_pushlightuserdata(L, GetCppObjPtrKey());  
			lua_pushnil(L);           
			lua_rawset(L, -3);    
		}
		else
		{
			void** ppObject = (void**)lua_touserdata(L, -1);
			*ppObject = NULL;
		}
		      
		luaL_unref(L, LUA_REGISTRYINDEX, iRefNum);
	
		lua_remove(L, -1);
	}
	else
	{
		UnlinkCppObj((void*)pObject);
	}
}


//=====================================================================
//	输出： 把 AS_GLOBAL_REFERENCE_TABLE 放到栈顶
//=====================================================================
static inline void AsPushGlobalRefTable( lua_State* L )
{
	lua_pushlightuserdata(L, GetGlobalRefTableKey());
	lua_gettable(L, LUA_GLOBALSINDEX);
}
//
////=====================================================================
////	输出： 把 userdata索引的table   放到栈顶
////=====================================================================
//static void AsGetBeRefTable( lua_State* L, void* pUserData )
//{
//	AsPushGlobalRefTable( L );//stack:..., Glb_Ref_Tbl
//	int32 gTableIdx = lua_gettop( L );
//	lua_pushlightuserdata( L, pUserData );//stack:..., Glb_Ref_Tbl, userdata 
//	lua_gettable( L, -2 );	//Glb_Ref_Tbl[lightuserdata] = Ref_Tbl  //stack:..., Glb_Ref_Tbl, Ref_Tbl
//	lua_remove( L, gTableIdx );//stack:..., Ref_Tbl
//}
//
////=======================================================================
////	把userdata作为索引，向全局索引表__global_reference_table添加lua对象索引,
////	防止C++引用的lua对象被lua垃圾回收：
////			__global_reference_table[userdata] = tbl
////	其中：
////			tbl =	{
////						__ref_num = lua对象被引用的次数
////						__ref_data = lua对象
////					}
////=======================================================================
//static void _AddBeRefTable( lua_State*L, void* pUserData, int nObj )
//{
//	AsPushGlobalRefTable( L );//stack:...,Glb_Ref_Tbl
//
//	lua_pushlightuserdata( L, pUserData );//stack: ..., Glb_Ref_Tbl, userdata 
//	lua_newtable( L );// stack:...,Glb_Ref_Tbl, userdata, tbl
//
//	lua_pushinteger( L, 1 );// stack:..., Glb_Ref_Tbl, userdata, tbl, 1
//	lua_setfield( L, -2, AS_REF_NUM_FILED_NAME );//  tbl["__ref_num"] = 1  //stack:..., Glb_Ref_Tbl, userdata, tbl
//
//	lua_pushvalue(L, nObj);//stack:..., Glb_Ref_Tbl, userdata, tbl, Obj
//
//	lua_setfield( L, -2, AS_REF_DATA_FILED_NAME );//tbl["__ref_data"] //stack:..., Glb_Ref_Tbl, userdata, tbl
//
//	lua_settable( L, -3 );//Glb_Ref_Tbl[userdata] = tbl
//}
//
////=====================================================================
////	输出：	AS_GLOBAL_REFERENCE_TABLE[pUserData] = __ObjTable[pUserData]
////			如果__ObjTable[pUserData] = nil, 直接返回
////			堆栈不发生改变
////=====================================================================
//static void AddBeRefTable( lua_State*L, void* pUserData )
//{
//	int nTop = lua_gettop( L );
//	GetObjectByUserdata( L, pUserData );
//	if( !lua_isnil( L, -1) )
//	{
//		_AddBeRefTable(L, pUserData, nTop + 1);
//	}
//	lua_settop( L, nTop );
//}
//
////=====================================================================
////	输出： 删除 AS_GLOBAL_REFERENCE_TABLE 的一个索引,堆栈不发生改变
////=====================================================================
//static void RemoveRefTable( lua_State*L, void* pUserData )
//{
//	AsPushGlobalRefTable( L );
//	lua_pushlightuserdata( L, pUserData );
//	lua_pushnil( L );
//	lua_settable( L, -3 );
//}


//=====================================================================
//	输出：	增加 索引 计数， 如果 被引用的 table 不存在， 添加一个table 到
//			AS_GLOBAL_REFERENCE_TABLE, 并且其计数为1.
//			堆栈不发生改变 
//			当 pUserData == NULL 时， 直接返回
//=====================================================================
void CScript::RefScriptObj( void* pObj )
{
	if( pObj == NULL )
		return;

	lua_State* pState = m_pData->m_pState;

	int32 nTop = lua_gettop(pState);

	GetObjectByUserdata(pState, pObj);	//obj

	if (lua_isnil(pState, -1))
	{
		lua_pop(pState, 1);
		return;
	}

	AsPushGlobalRefTable(pState);		//obj, ref tbl

	lua_pushvalue(pState, -2);			//obj, ref tbl, obj	
	
	lua_rawget(pState, -2);				//obj, ref tbl, ref number

	int32 nRefNum = 0;

	if (lua_isnil(pState, -1))
	{
		nRefNum = 1;
	}
	else
	{
		nRefNum = (int32)lua_tonumber(pState, -1) + 1;
	}

	lua_pop(pState, 1);					//obj, ref tbl

	lua_pushvalue(pState, -2);			//obj, ref tbl, obj
	lua_pushnumber(pState, nRefNum);	//obj, ref tbl, obj, ref num
	lua_settable(pState, -3);			//obj, ref tbl

	lua_settop(pState, nTop);

	//lua_State* L = m_pData->m_pState;
	//int nTop = lua_gettop( L );
	//AsGetBeRefTable( L, pObj );//stack: ..., Ref_Tbl
	//if( lua_isnil( L, -1 ) )
	//{
	//	AddBeRefTable( L, pObj );
	//}
	//else
	//{
	//	lua_getfield( L, -1, AS_REF_NUM_FILED_NAME );//stack: ...,Ref_Tbl, num = Ref_Tbl["__ref_num"]  
	//	int32 nRef = (int32)lua_tointeger( L, -1 );
	//	lua_pushinteger( L, nRef + 1 );//stack: ...,Ref_Tbl, num+1
	//	lua_setfield( L, -3, AS_REF_NUM_FILED_NAME );//Ref_Tbl["__ref_num"] = num + 1
	//}
	//lua_settop( L, nTop );
}

void CScript::RefScriptObj(CBaseObject *pObject)
{
	if(!pObject || !pObject->CreatedByScript())
		return;
	
	RefScriptObj((void*)pObject);
}

//=====================================================================
//	输出：	减少 索引 计数， 如果 被引用的 table 计数为1， 从
//			AS_GLOBAL_REFERENCE_TABLE中 删除该table, 并且其计数为1.
//			计数为0时，直接返回.
//			堆栈不发生改变 
//=====================================================================
void CScript::UnrefScriptObj( void* pObj )
{
	if( pObj == NULL )
		return;

	//AstInSameThread();

	lua_State* pState = m_pData->m_pState;

	int32 nTop = lua_gettop(pState);

	GetObjectByUserdata(pState, pObj);	//obj

	if (lua_isnil(pState, -1))
	{
		lua_pop(pState, 1);
		return;
	}

	AsPushGlobalRefTable(pState);		//obj, ref tbl

	lua_pushvalue(pState, -2);			//obj, ref tbl, obj	

	lua_rawget(pState, -2);				//obj, ref tbl, ref number

	if (!lua_isnil(pState, -1))
	{
		int32 nRefNum = (int32)lua_tonumber(pState, -1) - 1;
		lua_pop(pState, 1);				//obj, ref tbl
		if (nRefNum == 0)
		{
			lua_pushvalue(pState, -2);	//obj, ref tbl, obj
			lua_pushnil(pState);		//obj, ref tbl, obj, nil
			lua_settable(pState, -3);
		}
		else
		{
			lua_pushvalue(pState, -2);	//obj, ref tbl, obj
			lua_pushnumber(pState, nRefNum);
			lua_settable(pState, -3);
		}
	}

	lua_settop(pState, nTop);



	//lua_State* L = m_pData->m_pState;
	//int nTop = lua_gettop( L );

	//AsGetBeRefTable( L, pObj );
	//if( lua_isnil( L, -1 ) == 0 )
	//{
	//	lua_getfield( L, -1, AS_REF_NUM_FILED_NAME );
	//	int32 nRef = (int32)lua_tointeger( L, -1 );
	//	if( nRef ==1 )
	//	{
	//		RemoveRefTable( L, pObj );
	//	}
	//	else
	//	{
	//		lua_pushinteger( L, nRef - 1 );
	//		lua_setfield( L, -3, AS_REF_NUM_FILED_NAME );
	//	}
	//}
	//lua_settop( L, nTop );
}

void CScript::UnrefScriptObj(CBaseObject *pObject)
{
	if(!pObject || !pObject->CreatedByScript())
		return;

	UnrefScriptObj((void*)pObject);
}

EScriptHelper CScript::GetHelperType()const
{
	return m_pData->m_eScriptHelper;
}

#ifdef _WIN32
extern "C" int compile(int nArgc,char* aryArgv[]);
#endif

int CScript::Compile(int nArgc,char* aryArgv[])
{
#ifdef _WIN32
	return compile(nArgc,aryArgv);
#else
	return 0;
#endif
}

void CScript::_RegCallFunc(void* pContext, const char* szFunctionName, int32 (*pFunc)(lua_State*))
{
	CScriptProc* pByScript = static_cast<CScriptProc*>(pContext);

	m_pData->m_vecFunctions.push_back( pByScript );

	lua_pushlightuserdata( m_pData->m_pState, pByScript );
	lua_pushcclosure( m_pData->m_pState, (lua_CFunction)pFunc, 1 );
	lua_setglobal( m_pData->m_pState, szFunctionName );
}

void CScript::_RegClassCallFunc(void *pContext, const char *szClass, const char *szFunctionName, int32 (*pFunc)(lua_State *))
{
	CScriptProc* pByScript = static_cast<CScriptProc*>(pContext);
	
	m_pData->m_vecFunctions.push_back( pByScript );

	lua_getglobal( m_pData->m_pState, szClass );
	Ast( !lua_isnil( m_pData->m_pState, -1 ) ); 

	lua_pushlightuserdata( m_pData->m_pState, pByScript );
	lua_pushcclosure( m_pData->m_pState, (lua_CFunction)pFunc, 1 );
	lua_setfield( m_pData->m_pState, -2, szFunctionName );
	lua_pop( m_pData->m_pState, 1 );
}

void CScript::_RegClassCallback(void* pContext)
{
	CScriptProc* pToScript = static_cast<CScriptProc*>(pContext);

	m_pData->m_vecFunctions.push_back( pToScript );
}

void* CScript::GetCallBackData(CBaseObject* pObject, const char* szFunName)
{
	return CClassCallBack::GetCallBackData(pObject, szFunName);	
}

typedef vector<size_t, CScriptAllocator<size_t> > VecParamOffset_t;

void  CScript::GetCallBackAttr(void* pCall, size_t& stBufSize, size_t*& pAyParamOffset, 
				 			   size_t& iRetOffset)
{
	CClassCallBack* pData = static_cast<CClassCallBack*>(pCall);

	stBufSize		= pData->GetParamBufSize();

	VecParamOffset_t& vecOffset = pData->GetParamOffset();

	if (vecOffset.empty())
	{
		pAyParamOffset = NULL;
	}
	else
	{
		pAyParamOffset = &(vecOffset[0]);
	}
	iRetOffset	= pData->GetRetOffset();
}

bool CScript::CallClassCallBack(void* pCall, CBaseObject* pObject, char* pDataBuf)
{
	CClassCallBack* pData = static_cast<CClassCallBack*>(pCall);
	return pData->CallClassCallBack(pObject, pDataBuf);
}

void CScript::GetStack(ostream& strm)
{
	CScript* pScript = CScript::Inst();
	if(!pScript)
		return;

	lua_State* pState;

	if(pScript->m_pData->m_pRunningState)
	{	 
		pState=pScript->m_pData->m_pRunningState;
	}
	else
	{
		pState=_L(*pScript);
	}

	for(int i = 0; PrintFrame(pState, i, strm); i++);

}

void CScript::CallBackProfileBegin(const char* szFunName)
{
	if (m_pCallBackProfiler->IsOpen())
	{
		m_pCallBackProfiler->ProfileBegin(szFunName);
	}
}

void CScript::CallBackProfileEnd(const char* szFunName)
{
	if (m_pCallBackProfiler->IsOpen())
	{
		m_pCallBackProfiler->ProfileEnd(szFunName);
	}
}

