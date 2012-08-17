#include "stdafx.h"
#include "CMagicCfgServer.h"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "CSkillCfg.h"

CSkillCfg::MapOperateObjectType CSkillCfg::ms_mapOperateObjectType;

CMagicCfgServer::CMagicCfgServer()
:m_pRadius(NULL)
,m_pAmountLimit(NULL)
,m_pTime(NULL)
,m_uId(0)
{
}

CMagicCfgServer::CMagicCfgServer(const CMagicCfgServer& cfg)
:m_pRadius(cfg.m_pRadius?new CCfgCalc(*(cfg.m_pRadius)):NULL)
,m_pAmountLimit(cfg.m_pAmountLimit?new CCfgCalc(*(cfg.m_pAmountLimit)):NULL)
,m_pTime(cfg.m_pTime?new CCfgCalc(*(cfg.m_pTime)):NULL)
,m_sName(cfg.m_sName)
,m_fSpeed(cfg.m_fSpeed)
,m_eOperateObject(cfg.m_eOperateObject)
,m_uId(cfg.m_uId)
{
}

CMagicCfgServer::~CMagicCfgServer()
{
	SafeDelete(m_pRadius);
	SafeDelete(m_pAmountLimit);
	SafeDelete(m_pTime);
}

const vector<string> CMagicCfgServer::Split(const string& src, string delimit)  
{  
	vector<string> v;
	if( src.empty() || delimit.empty() )
	{
		v.push_back("");
		v.push_back("");
		return v;
	}

	int32 m,n;
	m=src.find_first_of("\"");  
	n=src.find_last_of("\"");  
	string str=src.substr(m>=0?m+1:0,n-1>0?n-1:src.size());  

	basic_string<TCHAR>::size_type delim_len = delimit.size();
	size_t index = std::string::npos, last_search_position = 0;  
	while((index=str.find(delimit, last_search_position)) != std::string::npos)  
	{  
		if(index == last_search_position)  
			v.push_back("");  
		else  
			v.push_back( str.substr(last_search_position, index - last_search_position) );  
		last_search_position = index + delim_len;  
	}  
	string last_one = str.substr(last_search_position);  
	v.push_back( last_one.empty()? "":last_one );
	return v;  
}


