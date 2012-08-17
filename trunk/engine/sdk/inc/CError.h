#pragma once
#include <string>
#include <sstream>
#include <cerrno>
#include "BaseDefs.h"
#include "ModuleDefs.h"
#include "CMallocObject.h"
#include "TMallocAllocator.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#endif

namespace sqr
{
	class CCallStackInfo;

	class MODULE_API CError : public CMallocObject
	{
	public:
		CError(const std::string& sErrorType) throw();
		CError(const std::string& sErrorType, const std::string& sStackInfo) throw();
		CError(const char* sErrorType) throw();
		CError(const char* sErrorType, const char* sStackInfo) throw();
		CError(const CError& exp) throw();

		~CError() throw();

		CError& operator =(const CError& exp) throw();

		const char* ErrorTitle() const throw();

		const char* ErrorMsg()const throw();
		
		const char* StackInfo()const throw();

		const char* Category() const throw();

		const char* ExpandInfo() const throw();

		const char* MemInfo() const throw();

		void AppendType(const std::string& sType, bool bAppendEnd = false) throw();
		void AppendType(const char* sType, bool bAppendEnd = false) throw();

		void AppendMsg(const std::string& strMsg, bool bAppendEnd = true) throw();
		void AppendMsg(const char* strMsg, bool bAppendEnd = true) throw();
		
		void AppendStackInfo(const std::string& strStackInfo) throw();
		void AppendStackInfo(const char* strStackInfo) throw();

		void SetCategory(const std::string& sCategory) throw();
		void SetCategory(const char* sCategory) throw();

		void AppendInfo(const std::string& strPos) throw();
		void AppendInfo(const char* strPos) throw();

	private:
		void GetMemInfo();

	protected:
		typedef std::basic_string<char,std::char_traits<char>,
			TMallocAllocator<char> >
			ErrorStr_t;

		ErrorStr_t		m_sErrorType;
		ErrorStr_t		m_sErrorMsg;
		ErrorStr_t		m_sStackInfo;
		ErrorStr_t		m_sCategory;
		ErrorStr_t		m_sExpandInfo;
		ErrorStr_t		m_sMemInfo;
	};

}

#ifdef _WIN32
#pragma warning (pop)
#endif
