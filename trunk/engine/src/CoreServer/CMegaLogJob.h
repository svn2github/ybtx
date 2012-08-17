#pragma once 
#include "CMegaLogAllocator.h"

namespace sqr 
{

	class CMegaLogJob
		:public CMegaLogMallocObject
	{
	public:
		CMegaLogJob(uint32 uID);
		virtual ~CMegaLogJob();

		void Add();

		virtual void Do()=0;

	protected:
		uint32 m_uID;
	};

	class CMegaLogCreateJob : public CMegaLogJob
	{
	public:
		CMegaLogCreateJob(uint32 uID, const char* szFileName, const char* szLogVersion, 
			uint32 uMaxFileSize, uint32 uInterval);
		
		void Do();

	private:
		typedef basic_string<char, std::char_traits<char>, CMegaLogAllocator<char> >	String_t;
		
		String_t	m_sFileName;
		String_t	m_sLogVersion;
		uint32		m_uMaxFileSize;
		uint32		m_uInterval;
	};

	class CMegaLogDestroyJob : public CMegaLogJob
	{
	public:
		CMegaLogDestroyJob(uint32 uID);
		
		void Do();
	};

	class CMegaLogWriteJob : public CMegaLogJob
	{
	public:
		CMegaLogWriteJob(uint32 uID, const char* szMsg);
		
		void Do();
	private:
		typedef basic_string<char, std::char_traits<char>, CMegaLogAllocator<char> >	String_t;
		
		String_t	m_sMsg;
	};
}
