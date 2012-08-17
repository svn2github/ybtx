#include "stdafx.h"
#include "CallBackX.h"

namespace sqr
{

	void* NullGetCallBackData(CBaseObject*, const char* szFunName) { return NULL; }
	void  NullGetCallBackAttr(void*, size_t& stBufSize, size_t*&  pAyParamOffset,
							  size_t& iRetOffset){}
	bool  NullCallClassCallBack(void*, CBaseObject*, char*){ return false; }

	static GetCallBackDataType		gs_GetCallBackDataFunc		= &NullGetCallBackData;
	static GetCallBackAttrType		gs_GetCallBackAttrFunc		= &NullGetCallBackAttr;
	static CallClassCallBackType	gs_CallClassCallBackFunc	= &NullCallClassCallBack;


	void SetCallBackDataFunc(GetCallBackDataType Fun)
	{
		gs_GetCallBackDataFunc = Fun ? Fun : NullGetCallBackData;
	}
	GetCallBackDataType GetCallBackDataFunc()
	{
		return gs_GetCallBackDataFunc;
	}

	void SetCallBackAttrFunc(GetCallBackAttrType Fun)
	{
		gs_GetCallBackAttrFunc = Fun ? Fun : NullGetCallBackAttr;
	}
	GetCallBackAttrType GetCallBackAttrFunc()
	{
		return gs_GetCallBackAttrFunc;
	}

	void SetClassCallBackFunc(CallClassCallBackType Fun)
	{
		gs_CallClassCallBackFunc = Fun ? Fun : NullCallClassCallBack;
	}
	CallClassCallBackType CallClassCallBackFunc()
	{
		return gs_CallClassCallBackFunc;
	}

}
