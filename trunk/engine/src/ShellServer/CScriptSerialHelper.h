#pragma once 

#include "ScriptDef.h"
#include "CScript.h"
#include "ScriptX.h"

namespace sqr
{

	class CSerialHelper
	{
	public:
		static void PostPackFun(uint32 uThreadIndex, const char* strType, size_t stTypeSize,
			const char* vecBuf, size_t stBufSize);
		static ON_UNPACKFUNCTION ms_OnUnPackFun;
	};

	class CThreadSerialHelper
	{
	public:
		static void PostPackFun(uint32 uThreadIndex, const char* strType, size_t stTypeSize, 
			const char* vecBuf, size_t stBufSize);
		static ON_UNPACKFUNCTION ms_OnUnPackFun;
	};

	template<typename Helper>
	class TRegisterSerial
	{
	public:
		typedef TRegisterSerial<Helper>		CRegisterSerial;
	
		static void RegisterSerial(CScript& Script, const char* szFunName)
		{
			REGIST_SERIALPACKFUN(Script, szFunName, &Helper::PostPackFun);
			
			if(!Helper::ms_OnUnPackFun)
				REGIST_SERIALUNPACKFUN(Script, &Helper::ms_OnUnPackFun);
		}
	};

#define REGISTER_SERIAL(Script, szFunName) \
	TRegisterSerial<CSerialHelper>::RegisterSerial(Script, szFunName)

#define REGISTER_THREAD_SERIAL(Script, szFunName) \
	TRegisterSerial<CThreadSerialHelper>::RegisterSerial(Script, szFunName)

}
