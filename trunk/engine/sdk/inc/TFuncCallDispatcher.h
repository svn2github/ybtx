#pragma once 

//开始用boost了，发现preprocessor这个真的很强大
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition.hpp>

#include "TFuncCallHelper.h"
#include "CScriptAllocator.h"


/*
	由于采用了boost库的pp用于生成代码，所以下面的代码不怎么好理解
	所以这里写上宏扩展之后的代码，以函数有返回值，同时有两个模板参数为例

	template<typename Ret>
	class TFuncCallDispatcher
	{
	public:   
		template<typename RT, typename P0, typename P1>  //其他参数的一样扩展
		static void Call(RT (*pFunc)(P0, P1), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset)
		{
			RT ret = pFunc(ScriptGetValue(TypeWrapper<P0)>(), pDataBuf, vecParamOffset[0]),
						   ScriptGetValue(TypeWrapper<P1)>(), pDataBuf, vecParamOffset[1]));
			ScriptSetValue(TypeWrapper<RT>(), ret, pDataBuf, stRetOffset);
		}

		template<typename Callee, typename RT, typename P0, typename P1>
		static void Call( RT (Callee::*pFunc)( P0, P1 ), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset)
		{
			Callee* pThis = GetThis(TypeWrapper<Callee>(), pDataBuf, vecParamOffset); 
			if( !pThis ) 
			{
				GenErr(" self is nil. "); 
			}
			RT ret = (pThis->*pFunc)(ScriptGetValue(TypeWrapper<P0)>(), pDataBuf, vecParamOffset[1]),
								     ScriptGetValue(TypeWrapper<P1)>(), pDataBuf, vecParamOffset[2]));
			ScriptSetValue(TypeWrapper<RT>(), ret, pDataBuf, stRetOffset);
		}
	
		template<typename Callee, typename RT, typename P0, typename P1>
		static void Call( RT (Callee::*pFunc)( P0, P1 ) const, char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset)
		{
			Callee* pThis = GetThis(TypeWrapper<Callee>(), pDataBuf, vecParamOffset); 
			if( !pThis ) 
			{
				GenErr(" self is nil. "); 
			}
			RT ret = (pThis->*pFunc)(ScriptGetValue(TypeWrapper<P0)>(), pDataBuf, vecParamOffset[1]),
								     ScriptGetValue(TypeWrapper<P1)>(), pDataBuf, vecParamOffset[2]));
			ScriptSetValue(TypeWrapper<RT>(), ret, pDataBuf, stRetOffset);
		}
	}


	template<>
	class TFuncCallDispatcher<void>
	{
	public:   
		//为什么这里还要写RT，是因为如果参数为0，模板是不允许template<>的实例化的
		template<typename RT, typename P0, typename P1>  //其他参数的一样扩展
		static void Call(RT (*pFunc)(P0, P1), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset)
		{
			pFunc(ScriptGetValue(TypeWrapper<P0)>(), pDataBuf, vecParamOffset[0]),
				  ScriptGetValue(TypeWrapper<P1)>(), pDataBuf, vecParamOffset[1]));
		}

		template<typename Callee, typename RT, typename P0, typename P1>
		static void Call( RT (Callee::*pFunc)( P0, P1 ), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset)
		{
			Callee* pThis = GetThis(TypeWrapper<Callee>(), pDataBuf, vecParamOffset); 
			if( !pThis ) 
			{
				GenErr(" self is nil. "); 
			}
			(pThis->*pFunc)(ScriptGetValue(TypeWrapper<P0)>(), pDataBuf, vecParamOffset[1]),
				  	        ScriptGetValue(TypeWrapper<P1)>(), pDataBuf, vecParamOffset[2]));
		}
	
		template<typename Callee, typename RT, typename P0, typename P1>
		static void Call( RT (Callee::*pFunc)( P0, P1 ) const, char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset)
		{
			Callee* pThis = GetThis(TypeWrapper<Callee>(), pDataBuf, vecParamOffset); 
			if( !pThis ) 
			{
				GenErr(" self is nil. "); 
			}
			(pThis->*pFunc)(ScriptGetValue(TypeWrapper<P0)>(), pDataBuf, vecParamOffset[1]),
						    ScriptGetValue(TypeWrapper<P1)>(), pDataBuf, vecParamOffset[2]));
		}
	}

	template<typename RT, typename P0, typename P1>
	void CallDispatcher(RT (*pFunc)( P0, P1 ), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset) 
	{
		TFuncCallDispatcher<RT>::Call(pFunc, pDataBuf, vecParamOffset, stRetOffset);
	}

	template<typename Callee, typename RT, typename P0, typename P1>
	void CallDispatcher(RT (Callee::*pFunc)( P0, P1 ), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset) 
	{
		TFuncCallDispatcher<RT>::Call(pFunc, pDataBuf, vecParamOffset, stRetOffset);
	}

	template<typename Callee, typename RT, typename P0, typename P1>
	void CallDispatcher(RT (Callee::*pFunc)( P0, P1 ) const, char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset) 
	{
		TFuncCallDispatcher<RT>::Call(pFunc, pDataBuf, vecParamOffset, stRetOffset);
	}
*/


namespace sqr
{
	typedef vector<size_t, CScriptAllocator<size_t> > ScriptVecParamOffset_t;
	typedef vector<int32, CScriptAllocator<int32> >	ScriptVecParamType_t;
	typedef vector<size_t, CScriptAllocator<size_t> > ScriptVecParamLen_t;

	//------------------------------------------------------------------------------

	template<typename Callee>
	inline Callee* GetThis( TypeWrapper<Callee>, char* pDataBuf, const ScriptVecParamOffset_t& vecOffset)
	{
		if( vecOffset.empty() )
			return NULL;

		return ScriptGetValue(TypeWrapper<Callee*>(), pDataBuf, vecOffset[0]);
	}

	

#ifndef MAX_FUNCTION_PARAMS 
#define MAX_FUNCTION_PARAMS 12
#endif

#define Func_Get_Value(Z, N, _)\
	BOOST_PP_COMMA_IF(N) ScriptGetValue(TypeWrapper<BOOST_PP_CAT(P, N)>(), pDataBuf, vecParamOffset[N]) 

//由于member function的vector param offset第一个参数为this指针，所以我们要从第二个开始
#define MemFunc_Get_Value(Z, N, _)\
	BOOST_PP_COMMA_IF(N) ScriptGetValue(TypeWrapper<BOOST_PP_CAT(P, N)>(), pDataBuf, vecParamOffset[BOOST_PP_INC(N)])

//-----------------------------------------------------------------
//for call dispatcher 

#define CALL_GLOBAL_FUNCTION(Z, N, _) \
	template<typename RT BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)>\
	static void Call( RT (*pFunc)( BOOST_PP_ENUM_PARAMS( N, P ) ), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset ) \
	{\
		RT ret = pFunc(BOOST_PP_REPEAT(N, Func_Get_Value, _));\
		ScriptSetValue(TypeWrapper<RT>(), ret, pDataBuf, stRetOffset);\
	}


#define CALL_MEMBER_FUNCTION(Z, N, _)\
	template<typename Callee, typename RT BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)>\
	static void Call( RT (Callee::*pFunc)( BOOST_PP_ENUM_PARAMS( N, P ) ), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset )\
	{\
		Callee* pThis = GetThis(TypeWrapper<Callee>(), pDataBuf, vecParamOffset);\
		if( !pThis )\
		{\
			GenErr("self is nil.");\
		}\
		RT ret = (pThis->*pFunc)(BOOST_PP_REPEAT(N, MemFunc_Get_Value, _));\
		ScriptSetValue(TypeWrapper<RT>(), ret, pDataBuf, stRetOffset);\
	}


#define CALL_MEMBER_CONST_FUNCTION(Z, N, _)\
	template<typename Callee, typename RT BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)>\
	static void Call( RT (Callee::*pFunc)( BOOST_PP_ENUM_PARAMS( N, P ) ) const, char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset )\
	{\
		Callee* pThis = GetThis(TypeWrapper<Callee>(), pDataBuf, vecParamOffset);\
		if( !pThis )\
		{\
			GenErr("self is nil.");\
		}\
		RT ret = (pThis->*pFunc)(BOOST_PP_REPEAT(N, MemFunc_Get_Value, _));\
		ScriptSetValue(TypeWrapper<RT>(), ret, pDataBuf, stRetOffset);\
	}

template<typename Ret>
class TFuncCallDispatcher
{
public:
	BOOST_PP_REPEAT(MAX_FUNCTION_PARAMS, CALL_GLOBAL_FUNCTION, _)
	BOOST_PP_REPEAT(MAX_FUNCTION_PARAMS, CALL_MEMBER_FUNCTION, _)
	BOOST_PP_REPEAT(MAX_FUNCTION_PARAMS, CALL_MEMBER_CONST_FUNCTION, _)
};

#undef CALL_GLOBAL_FUNCTION
#undef CALL_MEMBER_FUNCTION
#undef CALL_MEMBER_CONST_FUNCTION

//------------------------------------------------------------------
//TFunCallDispatcher specialization for no return

#define CALL_GLOBAL_FUNCTION_NORET(Z, N, _) \
	template<typename RT BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)>\
	static void Call( RT (*pFunc)( BOOST_PP_ENUM_PARAMS( N, P ) ), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset ) \
	{\
		pFunc(BOOST_PP_REPEAT(N, Func_Get_Value, _));\
	}


#define CALL_MEMBER_FUNCTION_NORET(Z, N, _)\
	template<typename Callee, typename RT BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)>\
	static void Call( RT (Callee::*pFunc)( BOOST_PP_ENUM_PARAMS( N, P ) ), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset )\
	{ \
		Callee* pThis = GetThis(TypeWrapper<Callee>(), pDataBuf, vecParamOffset); \
		if( !pThis ) \
		{ \
			GenErr("self is nil. "); \
		}\
		(pThis->*pFunc)(BOOST_PP_REPEAT(N, MemFunc_Get_Value, _));\
	}


#define CALL_MEMBER_CONST_FUNCTION_NORET(Z, N, _)\
	template<typename Callee, typename RT BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)>\
	static void Call( RT (Callee::*pFunc)( BOOST_PP_ENUM_PARAMS( N, P ) ) const, char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset )\
	{ \
		Callee* pThis = GetThis(TypeWrapper<Callee>(), pDataBuf, vecParamOffset); \
		if( !pThis ) \
		{ \
			GenErr("self is nil. "); \
		}\
		(pThis->*pFunc)(BOOST_PP_REPEAT(N, MemFunc_Get_Value, _));\
	}


template<>
class TFuncCallDispatcher<void>
{
public:
	BOOST_PP_REPEAT(MAX_FUNCTION_PARAMS, CALL_GLOBAL_FUNCTION_NORET, _)
	BOOST_PP_REPEAT(MAX_FUNCTION_PARAMS, CALL_MEMBER_FUNCTION_NORET, _)
	BOOST_PP_REPEAT(MAX_FUNCTION_PARAMS, CALL_MEMBER_CONST_FUNCTION_NORET, _)
};

#undef CALL_GLOBAL_FUNCTION_NORET
#undef CALL_MEMBER_FUNCTION_NORET
#undef CALL_MEMBER_CONST_FUNCTION_NORET


#undef Func_Get_Value
#undef MemFunc_Get_Value

//-----------------------------------------------------------------------------

#define CALL_GLOBAL_FUNC_CD(Z, N, _)\
	template<typename RT BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)>\
	void CallDispatcher(RT (*pFunc)( BOOST_PP_ENUM_PARAMS(N, P) ), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset ) \
	{\
		TFuncCallDispatcher<RT>::Call(pFunc, pDataBuf, vecParamOffset, stRetOffset);\
	}


#define	CALL_MEMBER_FUNC_CD(Z, N, _)\
	template<typename Callee, typename RT BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)>\
	void CallDispatcher(RT (Callee::*pFunc)( BOOST_PP_ENUM_PARAMS(N, P) ), char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset ) \
	{\
		TFuncCallDispatcher<RT>::Call(pFunc, pDataBuf, vecParamOffset, stRetOffset);\
	}


#define CALL_MEMBER_CONST_FUNC_CD(Z, N, _)\
	template<typename Callee, typename RT BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)>\
	void CallDispatcher(RT (Callee::*pFunc)( BOOST_PP_ENUM_PARAMS(N, P) ) const, char* pDataBuf, const ScriptVecParamOffset_t& vecParamOffset, size_t stRetOffset) \
	{\
		TFuncCallDispatcher<RT>::Call(pFunc, pDataBuf, vecParamOffset, stRetOffset);\
	}


BOOST_PP_REPEAT(MAX_FUNCTION_PARAMS, CALL_GLOBAL_FUNC_CD, _)
BOOST_PP_REPEAT(MAX_FUNCTION_PARAMS, CALL_MEMBER_FUNC_CD, _)
BOOST_PP_REPEAT(MAX_FUNCTION_PARAMS, CALL_MEMBER_CONST_FUNC_CD, _)

#undef CALL_GLOBAL_FUNC_CD
#undef CALL_MEMBER_FUNC_CD
#undef CALL_MEMBER_CONST_FUNC_CD

}

