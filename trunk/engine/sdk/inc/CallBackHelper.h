#pragma once

namespace sqr
{
	class CBaseObject;

	typedef void* (*GetCallBackDataType)(CBaseObject*, const char* szFunName);
	typedef void (*GetCallBackAttrType)(void*, size_t& stBufSize, size_t*& pAyParamOffset, 
										size_t& iRetOffset);
	typedef bool (*CallClassCallBackType)(void*, CBaseObject*, char*);

	void SetCallBackDataFunc(GetCallBackDataType);
	GetCallBackDataType GetCallBackDataFunc();

	void SetCallBackAttrFunc(GetCallBackAttrType);
	GetCallBackAttrType GetCallBackAttrFunc();

	void SetClassCallBackFunc(CallClassCallBackType);
	CallClassCallBackType CallClassCallBackFunc();

}

