#pragma once 
#include "ThreadTypes.h"
#include "CLogMallocObject.h"
#include "CLogMallocAllocator.h"

namespace sqr 
{

	class CSyncLog : public CLogMallocObject
	{
		friend class CLogThreadMgr;

	public:
		CSyncLog(uint32 uID, const char* sFileName, const char* szMode);
		~CSyncLog();

		void EnableTime(bool bEnable);
		void Write(const char* szLog, size_t stLen);
		void Flush();

	private:
		typedef basic_string<char, std::char_traits<char>, 
			CLogMallocAllocator<char> >
			String_t;

		uint32			m_uID;

		String_t		m_strFileName;
		
		String_t		m_strMode;

		FILE*			m_fdLog;
		bool			m_bLogTimeEnabled;
	};

}
