#pragma once
#include "ScriptCommon.h"
#include "ScriptCppObj.h"
#include "CScriptMallocObject.h"
#include "CScriptAllocator.h"

#ifdef _WIN32
#include <hash_map>
#include <hash_set>
using namespace stdext;
#else
#include <ext/hash_map>
#include <ext/hash_set>
using namespace __gnu_cxx;

#endif

struct lua_State;

namespace sqr
{
	class CScript;

	enum EScriptClassType
	{
		eSCT_Isolated = 0,
		eSCT_Static,
		eSCT_Share,
		eSCT_Dynamic,
		eSCT_None,
	};

	class CClass : public CScriptMallocObject
	{
	public:
		friend class CScript;
		friend class CScriptData;

		CClass( CScript* pScript, const char* szClassName );
		~CClass();
		
		EScriptClassType GetType();

		bool IsIsolatedClass();
		bool IsStaticClass();
		bool IsShareClass();
		bool IsDynamicClass();

		void* Create(lua_State *pState);
		void Release(lua_State *pState, void * pObject );
		
		void RegCallBackFunc(const char* szFunName, void* pContext);
		void* GetCallBackFunc(const char* szFunName);

		bool IsCallBack();
		
		void AddBaseClass( CClass* pRegist );	

		const char* GetClassName()const;
		bool FindBase( CClass* pRegistBase );

		void SetClassRef(int32 iRef);
		int32 GetClassRef();

	private:
		bool IsAbstract();

		typedef basic_string<char, std::char_traits<char>, CScriptAllocator<char> > String_t;

		void* _FindCallBackFunc(const String_t& sFuncName);
		void _CheckRegCallBackFunc(CClass* pInfo, const char* szFunName);	
	
	private:
		CScript*	m_pScript; 
		String_t		m_sClassName;			// 类的名字
		
		typedef vector<CClass*, CScriptAllocator<CClass*> >		VecClass_t;
		VecClass_t		m_vpBaseRegist;         // 包含的基类
		
		REG_CREATE_FUN	m_pFunCreateObj;
		REG_DELETE_FUN	m_pFunDeleteObj;

		typedef map<String_t, void*, less<String_t>, 
			CScriptAllocator<pair<String_t, void*> > > MapCallBackFunc_t;
		MapCallBackFunc_t	m_mapCallBackFunc; //每个类自己上面的callback

		/*
			对于callback函数来说，在程序运行的时候我们会通过__FUNCTION__宏来使用
			而这个值是不变的，所以我们只用存储其指针就可以了
		*/

#ifdef _WIN32
		struct callback_less : public binary_function<const void*, const void*, bool>
		{
			bool operator()(const void* szLeft, const void* szRight) const 
			{
				return (size_t)szLeft < (size_t)szRight;
			}
		};

		typedef hash_map<const void*, void*, hash_compare<const void*, callback_less >, 
			CScriptAllocator<pair<const void*, void*> > >		MapRunningCallBack_t;
#else
		struct callback_hash     
		{                                                                                                                            
			size_t operator()(const void* str) const                                                                               
			{                                                                                                                        
				return (size_t)str;                                                                               
			}                                                                                                                        
		};    

		struct callback_equal
		{
			bool operator()(const void* szLeft, const void* szRight) const
			{
				return (size_t)szLeft == (size_t)szRight;
			}
		};

		typedef hash_map<const void*, void*, callback_hash, callback_equal, 
			CScriptAllocator<pair<const void*, void*> > >		MapRunningCallBack_t;
#endif
		//typedef map<const char*, void*, less<const char*>, 
		//	CScriptAllocator<pair<const char*, void*> > > MapRunningCallBack_t;

		MapRunningCallBack_t	m_mapRunningCallBack; //会保存自己以及父类所有的callback

		size_t		m_sizeOfClass;
		bool		m_bIsCallBack;

		int32		m_iClassRef;

		EScriptClassType	m_eType;
	};

}

