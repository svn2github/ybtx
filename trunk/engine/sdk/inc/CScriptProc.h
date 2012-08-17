#pragma once 
#include "EFuncParamType.h"
#include "CScriptMallocObject.h"
#include "CScriptAllocator.h"

struct lua_State;

namespace sqr
{

	class CBaseType;
	
	struct CallBaseData;

	class  CScript;

	class CScriptProc : public CScriptMallocObject
	{
	public:
		CScriptProc( CScript& Script, const char* szFormat, const char* szFunName);
		virtual ~CScriptProc();
		size_t GetParamBufSize();

	protected:
		CallBaseData*		m_pData;
	};

	class CFunction : public CScriptProc
	{
		friend class	CScript;
	public:
		CFunction( CScript& Script, const char* szFormat, const char* szFunName )
			: CScriptProc( Script, szFormat, szFunName ), m_bClass(false), m_sFunName( szFunName )
		{}
		const char* GetFunctionName(){ return m_sFunName.c_str(); }

		size_t GetFunctionNameSize() { return m_sFunName.size(); }

	protected:
		void	SetBeClassFunc(bool bClass) { m_bClass = bClass; }

		typedef basic_string<char, std::char_traits<char>, CScriptAllocator<char> > String_t;

		bool			m_bClass;
		String_t			m_sFunName;

		int32	GetParamFromLuaStack( lua_State* pState, char* pDataBuf, bool bPack = false );
		void	FillParamToCppStack( char* pDataBuf, char* pBuf, int32 nSize, bool bPack = false );
		void    FreeParamFromCppStack(char* pDataBuf);
		int32	PutResultToLuaStack( lua_State* pState, char* pDataBuf );
	};


	//CClassCallBack以及CUnpackCallBack会从这个类继承下来
	class CBaseCallBack : public CScriptProc
	{
	public:
		CBaseCallBack( CScript& Script, const char* szFormat, const char* szFunName );
		~CBaseCallBack();

	protected:
		typedef basic_string<char, std::char_traits<char>, CScriptAllocator<char> > String_t;

		String_t		m_sFunName;
		//这里保存了CCallBack被注册时所使用的coroutine,但Callback不应该总是使用该courtine作为自己的执行coroutine，
		//应该优先使用当前正在执行的coroutine作为自己的执行coroutine。
		lua_State*	m_pDefaultState;
		String_t		m_sArgBuf;
		void*		m_pObj;

		void GetFromCppStack( char* pDataBuf );
		void PushParam2LuaStack( lua_State* pState, char* pDataBuf );
	};


	template <typename BaseType>
	class CBaseFuncCall : public BaseType
	{
	public:
		typedef vector<size_t, CScriptAllocator<size_t> > VecParamOffset_t;
		typedef vector<int32, CScriptAllocator<int32> > VecParamType_t;
		typedef vector<size_t, CScriptAllocator<size_t> > VecParamLen_t;

		CBaseFuncCall(CScript& Script, const char* szFormat, const char* szFunName);
		~CBaseFuncCall(){}

		inline const VecParamOffset_t& GetParamOffset() const { return m_vecParamOffset; }
		inline VecParamOffset_t& GetParamOffset() { return m_vecParamOffset; }
		inline size_t GetRetOffset() const { return m_stRetOffset; }
	
	
		inline const VecParamLen_t& GetParamLen() const { return m_vecParamLen; }
		inline VecParamLen_t& GetParamLen() { return m_vecParamLen; }
		inline size_t GetRetLen() const { return m_stRetLen; }

	
		inline const VecParamType_t& GetParamType() const { return m_vecParamType; }
		inline VecParamType_t& GetParamType() { return m_vecParamType; }
		inline int32 GetRetType() const { return m_iRetType; }

	private:
		VecParamOffset_t	m_vecParamOffset;  //保存参数的偏移量，最左边的参数在第一个，如果是
											   //类成员函数调用，第一个为this的偏移
		size_t				m_stRetOffset;

		VecParamType_t		m_vecParamType;    //参数类型
		int32				m_iRetType;		   //返回值类型

		VecParamLen_t		m_vecParamLen;     //参数类型长度
		size_t				m_stRetLen;		   //结果类型长度
	};
}

