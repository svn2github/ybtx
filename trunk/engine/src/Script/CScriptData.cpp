#include "stdafx.h"
#include "CScriptData.h"
#include "ScriptDebugHelper.h"
#include "CClass.h"
#ifdef _WIN32
#include "Console.h"
#endif
#include "CScriptDebuger.h"
#include "CPkgFile.h"
#include "CBuf.h"
#include "CBufAcr.h"
#include "luaprofiler/LuaProfiler.h"
#include <algorithm>
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "CScriptDebugerMgr.h"
#include "ScriptKey.h"
#include "ScriptFileComm.h"
#include "ScriptCoroutine.h"
#include "ScriptLog.h"
#include "ScriptRegComm.h"
#include "MemoryHelper.h"
#include "ThreadHelper.h"
#include "CCompactAllocPool.h"
#include "CSqrAllocPoolMgr.h"
#include "CVariant.h"
#include "TSqrAllocator.inl"
#include "StringHelper.h"

namespace sqr
{

	lua_State* _L( CScript& Script )
	{
		return Script.GetData()->m_pState;
	}

}

//----------------------------------------------------
// struct CScriptData
//---------------------------------------------------

int profiler_init(lua_State *L);

extern "C"
{
	static void *LuaAlloc (void *ud, void *ptr, size_t osize, size_t nsize)
	{
		return static_cast<CCompactAllocPool*>(ud)->Realloc(ptr,osize, nsize);
	}
}

CScriptData::CScriptData()
: m_pDebuger(NULL) 
, m_eScriptHelper(eSH_None)
, m_pRunningState(NULL)
, m_bCppMemLeakEnabled(true)
{
	static int pool_num = 0;
	char buf[64];
	sprintf(buf, "CScriptPoolObject%d", ++pool_num);

	m_pAllocPool = new CCompactAllocPool(1024 * 1024 * 10,buf);

	m_pState = 	lua_newstate(LuaAlloc, m_pAllocPool);
	//m_pState = luaL_newstate();

	luaL_openlibs( m_pState );

	PrepareFunctions();

	m_stCallbackCounter=0;
	m_stNormalUnpackCounter=0;
	m_stSerialUnpackCounter=0;

	lua_gc(m_pState, LUA_GCSTOP, 0);
}

CScriptData::~CScriptData()
{
	lua_gc(m_pState, LUA_GCRESTART, 0);

	lua_close( m_pState );

	for( MapClass_t::iterator it = m_mapClasses.begin();
		it != m_mapClasses.end(); it++ )
		delete it->second;

	for( VecFunction_t::iterator it = m_vecFunctions.begin();
		it != m_vecFunctions.end(); it++ )
		delete *it;

	ClearStkVariant();
	ClearStkVariantList();

	delete m_pAllocPool;
}

void CScriptData::InitHelper(EScriptHelper eScriptHelper)
{
	m_eScriptHelper=eScriptHelper;

	switch ( m_eScriptHelper )
	{
	case eSH_Profiler:
		profiler_init(m_pState);
		break;
	case eSH_Debuger:
		LoadDebugBreaks();
		m_pDebuger=new CScriptDebuger(m_pState);
		CScriptDebugerMgr::Inst().AddDebuger(m_pDebuger);
	default:
		break;
	}
}

void CScriptData::UnitHelper()
{
	if(m_pDebuger)
	{
		SaveDebugBreaks();

		CScriptDebugerMgr::Inst().RemoveDebuger(m_pDebuger);	
		delete m_pDebuger;
		m_pDebuger=NULL;	
	}

	//if( m_eScriptHelper == eSH_Profiler )
	//	exit_profiler(m_pState);

	m_eScriptHelper = eSH_None;
}

namespace sqr
{

	static inline void __newGlobalReferenceTable( lua_State* L )
	{
		lua_newtable( L );

		lua_pushlightuserdata(L, GetGlobalRefTableKey());
		lua_pushvalue(L, -2);
		lua_settable(L, LUA_GLOBALSINDEX);

		lua_setglobal( L, GetGlobalRefTableKeyName() );
	}

	/*
	当我们的c++对象在c++里面创建并且传入到lua使用了之后，如果
	在c++中删除了该对象，那么我们会清空相应的lua里面对应table的一些数据
	但是这个时候lua里面对应的table是不会删除的，因为还没有被垃圾回收，
	所以我们得提供一个函数用来判断在lua使用的这个对象是不是在c++里面已经被删除了

	由于在c++与lua之间相互传递的类对象只可能是safe对象，而safe对象删除的时候，
	会把table里面的对应的meta table清空
	*/
	static int IsCppBound(lua_State* pState)
	{
		if(lua_isnil(pState, -1))
		{	
			lua_pushboolean(pState, 0);
			return 1;
		}

		if(!lua_getmetatable(pState, -1))
		{
			lua_pushboolean(pState, 0);
		}
		else
		{
			lua_pushboolean(pState, 1);
			lua_remove(pState, -2);
		}
		return 1;
	}

	static int GetAllMemInfo(lua_State* pState)
	{
		ostringstream strm;

		CSqrAllocPoolMgr::Inst().GetAllMemInfo(strm);

		lua_pushstring(pState, strm.str().c_str());

		return 1;
	}

	static int GetCurrentPID(lua_State* pState)
	{
		uint32 pid = (uint32)GetCurPID();

		lua_pushnumber(pState, pid);

		return 1;
	}

	static int GetCurrentThreadID(lua_State* pState)
	{
		uint32 tid = (uint32)GetCurTID();

		lua_pushnumber(pState, tid);

		return 1;
	}

	static int32 EnableCppMemLeakCheck( lua_State* pState )
	{
		bool bEnabled = (lua_toboolean(pState, -1) == 1);

		CScriptData* pData = CScript::Inst()->GetData();
		pData->m_bCppMemLeakEnabled = bEnabled;
		return 0;
	}

	static int32 ChangeMetaTable(lua_State* pState)
	{
		//obj, class
		if (lua_istable(pState, -2) || lua_isuserdata(pState, -2))
		{
			lua_setmetatable(pState, -2); //obj
		}
		else
		{
			luaL_error(pState, "对象不是userdata或者table，不能更换metatable");
		}
		return 0;
	}

	static int SetCoroutineESH(lua_State* pState)
	{
#ifdef _WIN32
		if(CatchErrorEnabled())
			_set_se_translator(TransESHFun);
#endif
		return 0;
	}

	static int SubUTF8String(lua_State* pState)
	{
		Ast(lua_isstring(pState, 1));
		string strIn = lua_tostring(pState, 1);
		size_t stStart = (size_t)lua_tonumber(pState,2);
		size_t stLen = (size_t)lua_tonumber(pState,3);
		string strOut;
		substr_u8(strOut,strIn,stStart,stLen);
		lua_pushstring(pState, strOut.c_str());
		return 1;
	}
}

static void RegScriptClassType(lua_State* pState)
{
	lua_newtable(pState);
	lua_pushvalue(pState, -1);
	lua_setglobal(pState, "EScriptClassType"); //table

	lua_pushnumber(pState, eSCT_Isolated);
	lua_setfield(pState, -2, "eSCT_Isolated");

	lua_pushnumber(pState, eSCT_Static);
	lua_setfield(pState, -2, "eSCT_Static");

	lua_pushnumber(pState, eSCT_Share);
	lua_setfield(pState, -2, "eSCT_Share");

	lua_pushnumber(pState, eSCT_Dynamic);
	lua_setfield(pState, -2, "eSCT_Dynamic");

	lua_pop(pState, 1);
}

void CScriptData::PrepareFunctions()
{
	int top = lua_gettop(m_pState);

	RegScriptClassType(m_pState);	
	
	lua_register( m_pState, "SetCoroutineESH", &SetCoroutineESH );

	lua_register( m_pState, "LoadFile", &ASLoadFileExport );
	lua_register( m_pState, "DoFile", &ASDoFile );

	lua_register( m_pState, "AliasLoadFile", &AliasLoadFile );
	lua_register( m_pState, "AliasDoFile", &AliasDoFile );

	//运行建立类机制的脚本
	RegDefClass( m_pState);

	//运行创建DebugPrint函数的脚本
	RegDebugPrint( m_pState );

	//运行本引擎提供的lua扩展过功能
	RegExtend( m_pState );

	lua_newtable( m_pState );
	lua_newtable( m_pState );
	lua_pushstring( m_pState, "v" );
	lua_setfield( m_pState, -2, "__mode" );
	lua_setmetatable( m_pState, -2 ); //tbl

	SetGlobalObjTable( m_pState );

	__newGlobalReferenceTable( m_pState );

	//设置cpp mem leak check表，用来存放
	lua_newtable( m_pState );
	lua_newtable( m_pState );
	lua_pushstring( m_pState, "k" );
	lua_setfield( m_pState, -2, "__mode" );
	lua_setmetatable( m_pState, -2 );	//cpp table

	SetCppMemLeakTable(m_pState);
	lua_register( m_pState, "EnableCppMemLeakCheck", EnableCppMemLeakCheck );	

	lua_register( m_pState, "is_cpp_member_var", (lua_CFunction)&CMemberVariable::IsCppMemberVar);
	lua_register( m_pState, "db", &sqr::DebugBreak );
	lua_register( m_pState, "ErrorHandler", &CScriptData::ErrorHandler );
	lua_register( m_pState, "ExpLogHandler", &CScriptData::ExpLogHandler );
	lua_register( m_pState, "CreateCBuf", &__createCBuf );
	lua_register( m_pState, "CreateCBufAcr", &__createCBufAcr );

	lua_register( m_pState, "uint32", &ToUint32 );
	lua_register( m_pState, "int32", &ToInt32 );
	lua_register( m_pState, "uint16", &ToUint16 );
	lua_register( m_pState, "int16", &ToInt16 );
	lua_register( m_pState, "uint8", &ToUint8 );
	lua_register( m_pState, "int8", &ToInt8 );
	lua_register( m_pState, "BitAnd", &BitAnd );
	lua_register( m_pState, "BitOr", &BitOr );
	lua_register( m_pState, "BitNot", &BitNot );
	lua_register( m_pState, "BitXor", &BitXor );

	lua_register( m_pState, "IsCppBound", &IsCppBound );
	lua_register( m_pState, "GetAllMemInfo", &GetAllMemInfo );
	lua_register( m_pState, "GetCurrentPID", &GetCurrentPID );
	lua_register( m_pState, "GetCurrentThreadID", &GetCurrentThreadID );
	lua_register( m_pState, "TraverseTable", &TraverseTable );
	lua_register( m_pState, "ChangeMetaTable", &ChangeMetaTable );

	lua_register( m_pState, "LogErr", &LogScriptErr );
	lua_register( m_pState, "CfgLogErr", &CfgLogScriptErr );
	lua_register( m_pState, "LogCategoryErr", &LogCategoryScriptErr );


	lua_register( m_pState, "BeforeResume", &BeforeResume );
	lua_register( m_pState, "AfterResume", &AfterResume );

	lua_register( m_pState, "SubUTF8String", &SubUTF8String );

	//注册profiler
	lua_newtable(m_pState);
	lua_pushvalue(m_pState, -1);
	lua_setglobal(m_pState, LUAPROFILERNAME);

	lua_pushcfunction(m_pState, &profiler_init);
	lua_setfield(m_pState, -2, "Start");

	lua_pushcfunction(m_pState, &profiler_stop);
	lua_setfield(m_pState, -2, "Stop");

	lua_settop(m_pState, top);
}

int CScriptData::ExpLogHandler(lua_State *pState)
{
	CScriptData* pThis= CScript::Inst()->GetData();

	if( pThis->m_pDebuger )
	{
		cout << lua_tostring( pState, -1 ) << endl;
		pThis->m_pDebuger->Debug(pState);
		return 1;
	}

	LogScriptExp(pState);

	return 1;
}

int CScriptData::ErrorHandler(lua_State *pState)
{
	CScriptData* pThis= CScript::Inst()->GetData();

	if( !pThis->m_pDebuger )
	{
		//如果关闭了调试器，我们同时记录lua堆栈
		ostringstream strm;
		for(int i = 0; PrintFrame(pState, i, strm); i++);	
		pThis->m_sStackInfo = strm.str().c_str();
	}
	else
	{

#ifdef _WIN32
		ShowConsole(true);	//控制台程序运行到这里的时候一定会返回false
#endif	
		string szError =lua_tostring( pState, -1 );
		cout << szError.c_str() << endl;

		pThis->m_pDebuger->Debug(pState);

#ifdef _WIN32
		ShowConsole(false);	//控制台程序运行到这里的时候一定会返回false
#endif

	}

	return 1;
}

void CScriptData::SetContextStack(void* pContext, void* pStack)
{
	if(pStack)
		m_mapContext2Stack[pContext] = pStack;
	else
		m_mapContext2Stack.erase(pContext);
}

void* CScriptData::GetContextStack(void* pContext)
{
	MapContextStack_t::iterator iter = m_mapContext2Stack.find(pContext);
	if(iter == m_mapContext2Stack.end())
		return NULL;

	return iter->second;
}

void CScriptData::RemoveContextStack(void* pContext)
{
	m_mapContext2Stack.erase(pContext);
}

CVariant* CScriptData::CreateVariant()
{
	if (m_stkVariant.empty())
	{
		void* buffer = SqrNew(sizeof(CVariant), CScriptMallocObject::GetPool());
		CVariant* pVariant = new (buffer)CVariant;
		return pVariant;
	}
	else
	{
		CVariant* pVariant = m_stkVariant.top();
		m_stkVariant.pop();
		return pVariant;
	}
}

void CScriptData::ReleaseVariant(CVariant* pVariant)
{
	if (m_stkVariant.size() > 100)
	{
		pVariant->~CVariant();
		SqrDelete(pVariant, CScriptMallocObject::GetPool());
	}
	else
	{
		pVariant->SetEmptyTable();
		m_stkVariant.push(pVariant);
	}
}

void CScriptData::ClearStkVariant()
{
	while (!m_stkVariant.empty())
	{
		CVariant* pVariant = m_stkVariant.top();
		m_stkVariant.pop();

		pVariant->~CVariant();
		SqrDelete(pVariant, CScriptMallocObject::GetPool());
	}
}

CScriptData::VecVariant_t* CScriptData::CreateVariantList()
{
	if (m_stkVariantList.empty())
	{
		void* buffer = SqrNew(sizeof(VecVariant_t), CScriptMallocObject::GetPool());
		VecVariant_t* pVariantList = new (buffer)VecVariant_t;
		return pVariantList;
	}
	else
	{
		VecVariant_t* pVariantList = m_stkVariantList.top();
		m_stkVariantList.pop();
		return pVariantList;
	}
}

void CScriptData::ReleaseVariantList(VecVariant_t* pArgList)
{
	size_t stSize = pArgList->size();
	for (size_t i = 0; i < stSize; i++)
	{
		CVariant* pVar = pArgList->at(i);
		ReleaseVariant(pVar);
	}

	if (m_stkVariantList.size() > 100)
	{
		pArgList->~VecVariant_t();
		SqrDelete(pArgList, CScriptMallocObject::GetPool());
	}
	else
	{
		pArgList->clear();
		m_stkVariantList.push(pArgList);
	}
}

void CScriptData::ClearStkVariantList()
{
	while(!m_stkVariantList.empty())
	{
		VecVariant_t* pArgList = m_stkVariantList.top();
		m_stkVariantList.pop();

		pArgList->~VecVariant_t();
		SqrDelete(pArgList, CScriptMallocObject::GetPool());
	}
}

uint32 CScriptData::GetGCStepAndResetCounter()
{
	const float fStep=
		m_stCallbackCounter*m_fCallbackGCRatio			+
		m_stNormalUnpackCounter*m_fNormalUnpackGCRatio	+
		m_stSerialUnpackCounter*m_fSerialUnpackGCRatio;

	m_stCallbackCounter=0;
	m_stNormalUnpackCounter=0;
	m_stSerialUnpackCounter=0;

	return uint32( fStep );
}
