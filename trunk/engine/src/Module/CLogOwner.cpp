#include "stdafx.h"
#include "CLogOwner.h"
#include "ExpHelper.h"
#include "CMallocObject.h"

namespace sqr
{
	
	class CLogOwnerImp : public CMallocObject
	{
	public:
		typedef basic_string<char,std::char_traits<char>,
			TMallocAllocator<char> >	LogString_t;

		typedef std::map<LogString_t, LogString_t, 
			less<LogString_t>, TMallocAllocator<pair<LogString_t, LogString_t> > >
			MapLogValue_t;

		MapLogValue_t	m_mapLogValue;
	};

}

CLogOwner::CLogOwner()
{
	m_pImp = new CLogOwnerImp;
}

CLogOwner::~CLogOwner()
{
	delete m_pImp;
}

void CLogOwner::SetValue(const char* szKey, const char* szValue)
{
	Ast(szKey);

	MapLogValue_t::iterator iter = m_pImp->m_mapLogValue.find(szKey);
	if(iter == m_pImp->m_mapLogValue.end())
	{
		if(szValue)		
			m_pImp->m_mapLogValue.insert(make_pair<LogString_t, LogString_t>(szKey, szValue));
	}
	else
	{
		if(szValue)
			iter->second = szValue;
		else
			m_pImp->m_mapLogValue.erase(iter);
	}
}

void CLogOwner::SetValue(const char* s)
{
	string strKey;
	string strValue;

	SetValue(s, strKey, strValue);
}

void CLogOwner::SetValue(const char* s, string& strKey, string& strValue)
{
	string str(s);
	size_t stPos = str.find(':');
	if(stPos == string::npos)
		return;

	strKey = str.substr(0, stPos).c_str();
	strValue = str.substr(stPos + 1).c_str();

	SetValue(strKey.c_str(), strValue.c_str());
	
}

const char* CLogOwner::GetValue(const char* szKey)
{
	Ast(szKey);

	MapLogValue_t::iterator iter = m_pImp->m_mapLogValue.find(szKey);
	if(iter == m_pImp->m_mapLogValue.end())
	{
		return NULL;
	}
	else
	{
		return iter->second.c_str();
	}
}

void CLogOwner::EraseValue(const char* szKey)
{
	Ast(szKey);

	m_pImp->m_mapLogValue.erase(szKey);
}

void CLogOwner::GetLog(LogString_t &str)
{
	MapLogValue_t::iterator iter = m_pImp->m_mapLogValue.begin();
	MapLogValue_t::iterator iter2 = m_pImp->m_mapLogValue.end();

	while(iter != iter2)
	{
		str.append("[").append(iter->first.c_str()).append(":").append(iter->second.c_str()).append("]\t");
		++iter;
	}
}

void CLogOwner::Clear()
{
	m_pImp->m_mapLogValue.clear();
}

