#pragma once

#include "CommonDefs.h"
#include "CStaticObject.h"
#include "CLogMallocObject.h"

namespace sqr
{
	
	/*
		简易log系统，用于处理自己的log
	*/
	class COMMON_API CLog
		: public CStaticObject
		, public CLogMallocObject
	{
	public:
		static CLog* CreateLog(const char* szFileName, const char* szMode = "wb");
	
		void Release();

		void EnableTime(bool bEnable);
		void Write(const char* szLog);
		void Write(const void* pBuffer, uint32 uLen);

		uint32 GetID() { return m_uID; }

	private:
		CLog();
		~CLog();

	private:
		static uint32	ms_uLogID;

		uint32	m_uID;
	};

}

