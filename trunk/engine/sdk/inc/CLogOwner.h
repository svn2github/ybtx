#pragma once
#include "CDynamicObject.h"
#include "TMallocAllocator.h"
#include "ModuleDefs.h"

namespace sqr
{
#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#endif

	class CLogOwnerImp;

	class MODULE_API CLogOwner : virtual public CDynamicObject
	{
	public:

		typedef basic_string<char,std::char_traits<char>,
			TMallocAllocator<char> >	LogString_t;

		void SetValue(const char* szKey, const char* szValue);

		//传入的是如下类型字串 key:value
		void SetValue(const char* str);
		void SetValue(const char* str, string& strKey, string& strValue);

		const char* GetValue(const char* szKey);

		void EraseValue(const char* szKey);

		void GetLog(LogString_t& str);
		void Clear();

		CLogOwner();
		~CLogOwner();

	private:
		CLogOwnerImp*	m_pImp;

		typedef std::map<LogString_t, LogString_t, 
			less<LogString_t>, 
			TMallocAllocator<pair<LogString_t, LogString_t> > >
			MapLogValue_t;

	};

#ifdef _WIN32
#pragma warning(pop)
#endif
}
