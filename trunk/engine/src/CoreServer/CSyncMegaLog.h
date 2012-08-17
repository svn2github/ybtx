#pragma once 
#include "CMegaLogAllocator.h"

namespace sqr 
{
	class CSyncMegaLog : public CMegaLogMallocObject
	{
	public:
		CSyncMegaLog(uint32 uID, const char* szFileName, const char* szLogVersion, 
			uint32 uMaxFileSize, uint32 uInterval);
		~CSyncMegaLog();

		void Release();

		void Write(const char* szMsg, size_t stLen);

		void Flush();

		static CSyncMegaLog* GetLog(uint32 uID);
		static void AddLog(CSyncMegaLog* pLog, uint32 uID);
		static CSyncMegaLog* RemoveLog(uint32 uID);

	private:
		bool CreateLog();

	private:
		typedef basic_string<char, std::char_traits<char>, CMegaLogAllocator<char> >	String_t;

		uint32		m_uID;

		String_t	m_sFileName;
		String_t	m_sLogVersion;
		uint32		m_uMaxFileSize;
		uint32		m_uCurFileSize;
		uint32		m_uInterval;
		uint64		m_uLastLogTime;
		
		FILE*		m_fdLog;

#ifdef _WIN32
		typedef hash_map<uint32, CSyncMegaLog*, hash_compare<uint32, less<uint32> >, 
			CMegaLogAllocator<pair<uint32, CSyncMegaLog*> > >	MapID2Log_t;
#else
		typedef hash_map<uint32, CSyncMegaLog*, hash<uint32>, equal_to<uint32>, 
			CMegaLogAllocator<pair<uint32, CSyncMegaLog*> > > MapID2Log_t;
#endif

		static MapID2Log_t				ms_mapID2Log;
	};
}
