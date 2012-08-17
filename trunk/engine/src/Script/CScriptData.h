#pragma once

#include "DataPack.h"
#include "CScriptSerial.h"
#include "CScriptAllocator.h"

namespace sqr
{

	class CClass;
	class CCompactAllocPool;

	class CVariant;

	//----------------------------------------------------
	// struct CScriptData
	//---------------------------------------------------
	class CScriptData : public CScriptMallocObject
	{
	public:
		CScriptData();
		~CScriptData();

		void PrepareFunctions();

		void InitHelper(EScriptHelper eScriptHelper);
		void UnitHelper();

		CPackMgr	m_PackMgr;
		lua_State*	m_pState;
		CScriptDebuger*	m_pDebuger;
		EScriptHelper	m_eScriptHelper;

		typedef basic_string<char, std::char_traits<char>, CScriptAllocator<char> > String_t;
		typedef map<String_t, CClass*, less<String_t>, 
			CScriptAllocator<pair<String_t, CClass*> > > MapClass_t;


		MapClass_t	m_mapClasses;

		typedef map<const char*, CClass*, less<const char*>, 
			CScriptAllocator<pair<const char*, CClass*> > > MapTypeClass_t;

		//因为这个map存储的是class的rtti名字，而rtti是不变的，所以我们可以直接存储其name的指针
		MapTypeClass_t	m_mapTypeClasses;

		//这个变量非常重要，必须要十分了解lua的调用机制(c++是可以随时控制在哪个coroutine上执行代码这点)，才能明白这个变量的作用。
		//用于记录CClassCallBack函数优先使用的lua_State。
		//如果一个CallBack是被一个CFunction间接触发的，那么CallBack应该优先使用CFunction正在使用lua_State，这样
		//做的效果是在某个coroutine调用一个被导出的c函数，那么这个c函数触发的回调也是在相同的coroutine上执行的。
		//如果这个变量为null，那么CallBack就是用他注册的时候的lua_State作为自己的执行coroutine。
		lua_State*	m_pRunningState;
	

		//用户注册的c++函数的全列表，退出的时候会自动删除所有被注册了的函数以及相关信息。
		typedef vector<CScriptProc*, CScriptAllocator<CScriptProc*> > VecFunction_t;
		VecFunction_t	m_vecFunctions;

		typedef map<String_t, int32, less<String_t>, 
			CScriptAllocator<pair<String_t, int32> > > MapSizeOfNum_t;

		MapSizeOfNum_t	m_mapSizeOfEnum;


		static int ErrorHandler(lua_State *pState);
		static int ExpLogHandler(lua_State *pState);

		String_t m_sStackInfo; //lua错误调用了error handler的时候记录堆栈

		typedef map<void*, void*, less<void*>, 
			CScriptAllocator<pair<void*, void*> > > MapContextStack_t;
		MapContextStack_t	m_mapContext2Stack;

		void SetContextStack(void* pContext, void* pStack);
		void* GetContextStack(void* pContext);
		void RemoveContextStack(void* pContext);

		CScriptSerial		m_ScriptSerial;

		CCompactAllocPool*		m_pAllocPool;

		typedef map<String_t, String_t, less<String_t>, 
			CScriptAllocator<pair<String_t, String_t> > > MapAliasPath_t;

		MapAliasPath_t		m_mapAliasToPath;

		CVariant* CreateVariant();
		void ReleaseVariant(CVariant* pVariant);
		void ClearStkVariant();

		typedef vector<CVariant*>	VecVariant_t;

		VecVariant_t* CreateVariantList();
		void ReleaseVariantList(VecVariant_t* pArgList);
		void ClearStkVariantList();

		typedef stack<CVariant*, 
			deque<CVariant*, CScriptAllocator<CVariant*> > >		StkVariant_t;
		typedef stack<VecVariant_t*, 
			deque<VecVariant_t*, CScriptAllocator<VecVariant_t*> > >		StkVariantList_t;

		StkVariant_t		m_stkVariant;
		StkVariantList_t	m_stkVariantList;

		bool	m_bCppMemLeakEnabled;	//是否检查lua创建c++对象的内存泄漏

		uint32 GetGCStepAndResetCounter();

		size_t	m_stCallbackCounter;
		size_t	m_stNormalUnpackCounter;
		size_t	m_stSerialUnpackCounter;

		float	m_fCallbackGCRatio;
		float	m_fNormalUnpackGCRatio;
		float	m_fSerialUnpackGCRatio;
	};

}
