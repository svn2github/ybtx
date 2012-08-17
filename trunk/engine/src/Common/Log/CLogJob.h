#pragma once 
#include "CLogMallocObject.h"
#include "CLogMallocAllocator.h"

namespace sqr 
{
	class CLogJob : public CLogMallocObject
	{
	public:
		CLogJob();
		virtual ~CLogJob();

		void Add();
	};

	class CCreateLogJob : public CLogJob
	{
	public:
		CCreateLogJob(uint32 uID, const char* szFileName, const char* szMode);
		~CCreateLogJob();

	private:
		uint32	m_uID;

		typedef basic_string<char, std::char_traits<char>, 
			CLogMallocAllocator<char> >
			String_t;

		String_t	m_sFileName;
		String_t	m_sMode;
	};

	class CDestroyLogJob : public CLogJob
	{
	public:
		CDestroyLogJob(uint32 uID);
		~CDestroyLogJob();

	private:
		uint32	m_uID;
	};

	class CWriteLogJob : public CLogJob
	{
	public:
		CWriteLogJob(uint32 uID, const char* szMsg);
		CWriteLogJob(uint32 uID, const void* pBuffer, uint32 uLen);
		~CWriteLogJob();

	private:
		uint32	m_uID;
		typedef basic_string<char, std::char_traits<char>, 
			CLogMallocAllocator<char> >
			String_t;
		String_t	m_sMsg;
	};

	class CEnableTimeLogJob : public CLogJob
	{
	public:
		CEnableTimeLogJob(uint32 uID, bool bEnabled);
		~CEnableTimeLogJob();

	private:
		uint32	m_uID;
		bool	m_bEnabled;
	};
}
