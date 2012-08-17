#pragma once 
#include "ThreadTypes.h"
#include "CMegaLogMallocObject.h"

namespace sqr 
{
	class CMegaLog
		: public CMegaLogMallocObject
	{
	public:
		static CMegaLog* CreateLog(const char* szFileName, const char* szLogVersion, 
								   uint32 uMaxFileSize, uint32 uInterval);

		void Release();

		void Write(const char* szMsg);

		static void Init();
		static void Unit();
		
	private:
		CMegaLog(const char* szFileName, const char* szLogVersion, 
			uint32 uMaxFileSize, uint32 uInterval);
		~CMegaLog();
	
	private:
		static volatile int32	ms_nLogID;

		uint32	m_uID;
	};
}
