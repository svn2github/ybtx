#pragma once
#include "ModuleDefs.h"
#include "ThreadTypes.h"
#include "ErrLogTypes.h"

namespace sqr
{

	class CError;

	class CErrLogMgr
	{
	public:
		CErrLogMgr();
		~CErrLogMgr();
		static CErrLogMgr& Inst()
		{
			static CErrLogMgr ls_Inst;
			return ls_Inst;
		}

		void SetLogFileName(const wchar_t* szFileName);
		void SetErrLogEnabled(bool bEnabled);
		bool ErrLogEnabled(){return m_bErrLogEnabled;}
		void SetInterval(uint32 uInterval);
		uint32 GetInterval(){return m_uInterval;}


		void WriteDumpErrLog(CError& exp);

		void WriteErrLog(CError& exp);

		void WriteErrInfoLog(const char* szErrInfo);

		void WriteDeviceInfoLog(const char* szDeviceInfo);


	private:
		void CreateErrLogFile();
		typedef basic_string<char,std::char_traits<char>,
			TMallocAllocator<char> >	ErrMsg_t;

		bool			m_bErrLogEnabled;

		CLogString			m_strLogFile;

		uint32		m_uInterval;
		uint64		m_uLastLogTime; //上一次记录log的时间

		ErrMsg_t	m_strErrInfo;
		ErrMsg_t	m_strDeviceInfo;
	};
}
