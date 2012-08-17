#if !defined _C_COOKIE_SINGLE_TON_H
#define _C_COOKIE_SINGLE_TON_H

#include "CMemoryManager.h"

namespace sqr
{
namespace Memory
{

template<class NameFun>
class CCookieSingleton
{
private:
	CMemoryCookie*	m_pCookie;

	CCookieSingleton(void)
			:m_pCookie(NULL)
	{
		m_pCookie=CMemoryManager::instance().AllocMemoryCookie();
	}


	~CCookieSingleton(void)
	{
		CMemoryManager::instance().dump(m_pCookie,NameFun::Name());
		CMemoryManager::instance().free_CMemoryCookie(m_pCookie);
	}
public:
	inline static CCookieSingleton<NameFun>& instance()
	{
		static CCookieSingleton<NameFun> cookie;
		return cookie;
	}

	CMemoryCookie* GetCookie()const
	{
		return m_pCookie;
	}
};

}
}

#endif

