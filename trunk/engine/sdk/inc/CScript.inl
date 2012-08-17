#pragma once 
#include "CScript.h"
#include "TFuncCall.inl"
#include "CClassCallBack.inl"

namespace sqr
{

//-----------------------------------------------------
// 注册单个被脚本调用的函数
//-----------------------------------------------------

template<typename Func>	
void CScript::RegCallFunc( const char* szArg, Func pFunc, const char* szFunctionName )
{
	TFuncCall<Func>* pByScript = new TFuncCall<Func>( *this, szArg, pFunc, false, szFunctionName );
	_RegCallFunc(pByScript, szFunctionName, TFuncCall<Func>::Call);
}

//-----------------------------------------------------
// 注册类成员函数
//-----------------------------------------------------
template<typename Func>
void CScript::RegClassCallFunc( const char* szArg, Func pFunc, const char* szClass, const char* szFunctionName )
{
	TFuncCall<Func>* pByScript = new TFuncCall<Func>( *this, szArg, pFunc, true, ( string( szClass ) + "::" + szFunctionName ).c_str() );
	_RegClassCallFunc(pByScript, szClass, szFunctionName, TFuncCall<Func>::Call);
}
}

//-----------------------------------------------------
// 注册类回调函数
//-----------------------------------------------------
template<typename Func>
void CScript::RegClassCallback( const char* szArg, Func pFunc, const char* szClass, const char* szFunctionName, bool bPureCallBackFunc )
{
	CClassCallBack* pToScript = new CClassCallBack( *this, szClass, szFunctionName, szArg, bPureCallBackFunc);
	pToScript->CheckCallBackFunc(pFunc);

	_RegClassCallback(pToScript);
}

