#pragma once

#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition.hpp>

#include "EFuncParamType.h"
#include "TFuncCallHelper.h"
#include "CallBackHelper.h"

namespace sqr
{
/*
	由于采用了boost库的pp用于生成代码，所以下面的代码不怎么好理解
	所以这里写上宏扩展之后的代码，以函数有返回值，同时有两个模板参数为例

	template<typename RT, typename Callee, typename P0, typename P1> 
	inline RT CallClassCallBack(bool& bSuc, const char* szFunName, Callee* pThis, P0 p0, P1 p1)
	{	
		TClassCallBack* pCall = GetCallBackDataFunc(pState, pThis, szFunName);
		
		if(pCall == NULL)
		{	
			bSuc = false; 
			return RetValue(TypeWrapper<RT>());	
		}
		
		size_t stBufSize;
		vector<size_t> vecParamOffset;
		size_t iRetOffset;

		GetCallBackAttrFunc(pCall, stBufSize, vecParamOffset, iRetOffset);
		char* pDataBuf = (char*)alloca(stBufSize);
		
		ScriptSetValue(TypeWrapper<P0)>(), p0, pDataBuf, vecParamOffset[0]);
		ScriptSetValue(TypeWrapper<P1)>(), p1, pDataBuf, vecParamOffset[1]);

		bSuc = CallClassCallBackFunc(pCall, pThis, pDataBuf);
		
		if(!bSuc)
		{
			return RetValue(TypeWrapper<RT>());	
		}
		return ScriptGetValue(TypeWrapper<RT>(), pDataBuf, iRetOffset);
	}
*/

#ifndef MAX_FUNCTION_PARAMS 
#define MAX_FUNCTION_PARAMS 12
#endif

#define FUNC_PARAM_LIST(Z, N, _)\
		BOOST_PP_COMMA_IF(N) BOOST_PP_CAT(P, N) BOOST_PP_CAT(p, N)

#define GET_VALUE_FROM_PARAM(Z, N, _)\
		ScriptSetValue(TypeWrapper<BOOST_PP_CAT(P, N)>(), BOOST_PP_CAT(p, N), pDataBuf, pAyParamOffset[N]);


#define CLASS_CALLBACK(Z, N, _)\
	template<typename RT, typename Callee BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)> \
	inline RT CallClassCallBack(bool& bSuc, const char* szFunName, Callee* pThis BOOST_PP_COMMA_IF(N) BOOST_PP_REPEAT(N, FUNC_PARAM_LIST, _))\
	{	void* pCall = GetCallBackDataFunc()(pThis, szFunName);\
		if(pCall == NULL)\
		{	bSuc = false; return RetValue(TypeWrapper<RT>());	}\
		size_t stBufSize;\
		size_t* pAyParamOffset;\
		size_t iRetOffset;\
		GetCallBackAttrFunc()(pCall, stBufSize, pAyParamOffset, iRetOffset);\
		char* pDataBuf = (char*)alloca(stBufSize);\
		BOOST_PP_REPEAT(N, GET_VALUE_FROM_PARAM, _) \
		bSuc = CallClassCallBackFunc()(pCall, pThis, pDataBuf);\
		if(!bSuc)\
		{	return RetValue(TypeWrapper<RT>());	}\
		return ScriptGetValue(TypeWrapper<RT>(), pDataBuf, iRetOffset);\
	}


	BOOST_PP_REPEAT(MAX_FUNCTION_PARAMS, CLASS_CALLBACK, _)


#undef CLASS_CALLBACK
#undef CLASSCALLBACK
#undef GET_VALUE_FROM_PARAM
	

//windows下面__FUNCTION__含有类的信息，要去掉，linux下面只有函数名字
inline const char* Script_GetFunName(const char* szFunName, size_t stNameSize)
	{
#ifdef _WIN32
		size_t i = stNameSize - 1;
		while(i >= 0)
		{
			if (szFunName[i] == ':')
			{
				break;
			}
			i--;
		}

		const char* szName = szFunName + i + 1;
		return szName;
#else
		(void)stNameSize;
		return szFunName;
#endif
	}



//--------------------------------------------------------------------------
//callback回调宏

#define CALLBACK_GET_FUNC_NAME\
	static const char* ls_szFunName = Script_GetFunName(__FUNCTION__, sizeof(__FUNCTION__)); \
	bool bSuc = false; 

#define CALL_CLASS_CALLBACK()\
	{ CALLBACK_GET_FUNC_NAME\
	CallClassCallBack<void>(bSuc, ls_szFunName, this); if(bSuc) return; } 


#define CALL_CLASS_CALLBACK_1(p0)\
	{ CALLBACK_GET_FUNC_NAME\
	CallClassCallBack<void>(bSuc, ls_szFunName, this, p0); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_2(p0, p1)\
	{ CALLBACK_GET_FUNC_NAME\
	CallClassCallBack<void>(bSuc, ls_szFunName, this, p0, p1); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_3(p0, p1, p2)\
	{ CALLBACK_GET_FUNC_NAME\
	CallClassCallBack<void>(bSuc, ls_szFunName, this, p0, p1, p2); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_4(p0, p1, p2, p3)\
	{ CALLBACK_GET_FUNC_NAME\
	CallClassCallBack<void>(bSuc, ls_szFunName, this, p0, p1, p2, p3); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_5(p0, p1, p2, p3, p4)\
	{ CALLBACK_GET_FUNC_NAME\
	CallClassCallBack<void>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_6(p0, p1, p2, p3, p4, p5)\
	{ CALLBACK_GET_FUNC_NAME\
	CallClassCallBack<void>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4, p5); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_7(p0, p1, p2, p3, p4, p5, p6)\
	{ CALLBACK_GET_FUNC_NAME\
	CallClassCallBack<void>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4, p5, p6); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_8(p0, p1, p2, p3, p4, p5, p6, p7)\
	{ CALLBACK_GET_FUNC_NAME\
	CallClassCallBack<void>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4, p5, p6, p7); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_9(p0, p1, p2, p3, p4, p5, p6, p7, p8)\
	{ CALLBACK_GET_FUNC_NAME\
	CallClassCallBack<void>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4, p5, p6, p7, p8); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_10(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)\
	{ CALLBACK_GET_FUNC_NAME\
	CallClassCallBack<void>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9); if(bSuc) return; } 


#define CALL_CLASS_CALLBACK_RET(RetType)\
	{ CALLBACK_GET_FUNC_NAME\
	RetType ret = CallClassCallBack<RetType>(bSuc, ls_szFunName, this); if(bSuc) return ret;} 


#define CALL_CLASS_CALLBACK_1_RET(RetType, p0)\
	{ CALLBACK_GET_FUNC_NAME\
	RetType ret =  CallClassCallBack<RetType>(bSuc, ls_szFunName, this, p0);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_2_RET(RetType, p0, p1)\
	{ CALLBACK_GET_FUNC_NAME\
	RetType ret =  CallClassCallBack<RetType>(bSuc, ls_szFunName, this, p0, p1);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_3_RET(RetType, p0, p1, p2)\
	{ CALLBACK_GET_FUNC_NAME\
	RetType ret =  CallClassCallBack<RetType>(bSuc, ls_szFunName, this, p0, p1, p2);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_4_RET(RetType, p0, p1, p2, p3)\
	{ CALLBACK_GET_FUNC_NAME\
	RetType ret =  CallClassCallBack<RetType>(bSuc, ls_szFunName, this, p0, p1, p2, p3);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_5_RET(RetType, p0, p1, p2, p3, p4)\
	{ CALLBACK_GET_FUNC_NAME\
	RetType ret =  CallClassCallBack<RetType>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_6_RET(RetType, p0, p1, p2, p3, p4, p5)\
	{ CALLBACK_GET_FUNC_NAME\
	RetType ret =  CallClassCallBack<RetType>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4, p5);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_7_RET(RetType, p0, p1, p2, p3, p4, p5, p6)\
	{ CALLBACK_GET_FUNC_NAME\
	RetType ret =  CallClassCallBack<RetType>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4, p5, p6);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_8_RET(RetType, p0, p1, p2, p3, p4, p5, p6, p7)\
	{ CALLBACK_GET_FUNC_NAME\
	RetType ret =  CallClassCallBack<RetType>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4, p5, p6, p7);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_9_RET(RetType, p0, p1, p2, p3, p4, p5, p6, p7, p8)\
	{ CALLBACK_GET_FUNC_NAME\
	RetType ret =  CallClassCallBack<RetType>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4, p5, p6, p7, p8);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_10_RET(RetType, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)\
	{ CALLBACK_GET_FUNC_NAME\
	RetType ret =  CallClassCallBack<RetType>(bSuc, ls_szFunName, this, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);if(bSuc) return ret;} 


//-------------------------------------------------------------------
	
#define CALL_CLASS_CALLBACK_OVERLOAD(FuncName)\
	{ bool bSuc = false;\
	CallClassCallBack<void>(bSuc, FuncName, this); if(bSuc) return; } 


#define CALL_CLASS_CALLBACK_1_OVERLOAD(FuncName, p0)\
	{ bool bSuc = false;\
	CallClassCallBack<void>(bSuc, FuncName, this, p0); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_2_OVERLOAD(FuncName, p0, p1)\
	{ bool bSuc = false;\
	CallClassCallBack<void>(bSuc, FuncName, this, p0, p1); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_3_OVERLOAD(FuncName, p0, p1, p2)\
	{ bool bSuc = false;\
	CallClassCallBack<void>(bSuc, FuncName, this, p0, p1, p2); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_4_OVERLOAD(FuncName, p0, p1, p2, p3)\
	{ bool bSuc = false;\
	CallClassCallBack<void>(bSuc, FuncName, this, p0, p1, p2, p3); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_5_OVERLOAD(FuncName, p0, p1, p2, p3, p4)\
	{ bool bSuc = false;\
	CallClassCallBack<void>(bSuc, FuncName, this, p0, p1, p2, p3, p4); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_6_OVERLOAD(FuncName, p0, p1, p2, p3, p4, p5)\
	{ bool bSuc = false;\
	CallClassCallBack<void>(bSuc, FuncName, this, p0, p1, p2, p3, p4, p5); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_7_OVERLOAD(FuncName, p0, p1, p2, p3, p4, p5, p6)\
	{ bool bSuc = false;\
	CallClassCallBack<void>(bSuc, FuncName, this, p0, p1, p2, p3, p4, p5, p6); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_8_OVERLOAD(FuncName, p0, p1, p2, p3, p4, p5, p6, p7)\
	{ bool bSuc = false;\
	CallClassCallBack<void>(bSuc, FuncName, this, p0, p1, p2, p3, p4, p5, p6, p7); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_9_OVERLOAD(FuncName, p0, p1, p2, p3, p4, p5, p6, p7, p8)\
	{ bool bSuc = false;\
	CallClassCallBack<void>(bSuc, FuncName, this, p0, p1, p2, p3, p4, p5, p6, p7, p8); if(bSuc) return; } 

#define CALL_CLASS_CALLBACK_10_OVERLOAD(FuncName, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)\
	{ bool bSuc = false;\
	CallClassCallBack<void>(bSuc, FuncName, this, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9); if(bSuc) return; } 


#define CALL_CLASS_CALLBACK_RET_OVERLOAD(FuncName, RetType)\
	{ bool bSuc = false;\
	RetType ret = CallClassCallBack<RetType>(bSuc, FuncName, this);if(bSuc) return ret;} 


#define CALL_CLASS_CALLBACK_1_RET_OVERLOAD(FuncName, RetType, p0)\
	{ bool bSuc = false;\
	RetType ret =  CallClassCallBack<RetType>(bSuc, FuncName, this, p0);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_2_RET_OVERLOAD(FuncName, RetType, p0, p1)\
	{ bool bSuc = false;\
	RetType ret =  CallClassCallBack<RetType>(bSuc, FuncName, this, p0, p1);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_3_RET_OVERLOAD(FuncName, RetType, p0, p1, p2)\
	{ bool bSuc = false;\
	RetType ret =  CallClassCallBack<RetType>(bSuc, FuncName, this, p0, p1, p2);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_4_RET_OVERLOAD(FuncName, RetType, p0, p1, p2, p3)\
	{ bool bSuc = false;\
	RetType ret =  CallClassCallBack<RetType>(bSuc, FuncName, this, p0, p1, p2, p3);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_5_RET_OVERLOAD(FuncName, RetType, p0, p1, p2, p3, p4)\
	{ bool bSuc = false;\
	RetType ret =  CallClassCallBack<RetType>(bSuc, FuncName, this, p0, p1, p2, p3, p4);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_6_RET_OVERLOAD(FuncName, RetType, p0, p1, p2, p3, p4, p5)\
	{ bool bSuc = false;\
	RetType ret =  CallClassCallBack<RetType>(bSuc, FuncName, this, p0, p1, p2, p3, p4, p5);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_7_RET_OVERLOAD(FuncName, RetType, p0, p1, p2, p3, p4, p5, p6)\
	{ bool bSuc = false;\
	RetType ret =  CallClassCallBack<RetType>(bSuc, FuncName, this, p0, p1, p2, p3, p4, p5, p6);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_8_RET_OVERLOAD(FuncName, RetType, p0, p1, p2, p3, p4, p5, p6, p7)\
	{ bool bSuc = false;\
	RetType ret =  CallClassCallBack<RetType>(bSuc, FuncName, this, p0, p1, p2, p3, p4, p5, p6, p7);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_9_RET_OVERLOAD(FuncName, RetType, p0, p1, p2, p3, p4, p5, p6, p7, p8)\
	{ bool bSuc = false;\
	RetType ret =  CallClassCallBack<RetType>(bSuc, FuncName, this, p0, p1, p2, p3, p4, p5, p6, p7, p8);if(bSuc) return ret;} 

#define CALL_CLASS_CALLBACK_10_RET_OVERLOAD(FuncName, RetType, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)\
	{ bool bSuc = false;\
	RetType ret =  CallClassCallBack<RetType>(bSuc, FuncName, this, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);if(bSuc) return ret;} 

	

}

