#pragma once 
#include "CMallocObject.h"
#include "TMallocAllocator.h"

namespace sqr
{
	class CError;

	class CErrLogJob : public CMallocObject
	{
	public:
		virtual ~CErrLogJob(){}
	};

	class CCreateErrLogJob : public CErrLogJob
	{
	public:
		CCreateErrLogJob(const wchar_t* szFileName, const char* szDeviceInfo);
	private:
		~CCreateErrLogJob();
		
		typedef basic_string<wchar_t, std::char_traits<wchar_t>, TMallocAllocator<wchar_t> >WString_t;
		typedef basic_string<char, std::char_traits<char>, TMallocAllocator<char> >String_t;
		WString_t	m_strFileName;
		String_t	m_strDeviceInfo;
	};

	class CCloseErrLogJob : public CErrLogJob
	{
	private:
		~CCloseErrLogJob();
	};


	class CWriteErrLogJob : public CErrLogJob
	{
	public:
		CWriteErrLogJob(const CError& exp,const char* szErrlogInfo);
	private:
		~CWriteErrLogJob();
		typedef basic_string<char, std::char_traits<char>, TMallocAllocator<char> >String_t;
		CError* pExp;
		String_t m_strErrlogInfo;
	};

}
