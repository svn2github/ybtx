#include "stdafx.h"
#include <iostream>
#include "StringHelper.h"
#include "CTalent.h"
#include "TSqrAllocator.inl"
#include "ErrLogHelper.h"

CTalentHolder::SetGlobalName		CTalentHolder::m_setGlobalName;
CTalentHolder::CTalentHolder()
:m_pHandler(NULL)
{

}

uint32 CTalentHolder::GetPoint(const string&  szName)const
{
	MapPoint::const_iterator	it_Talent	=	m_mapPoint.find(szName);
	if(it_Talent!=m_mapPoint.end())
		return it_Talent->second;
	else
	{
		return 0;
	}
}

void CTalentHolder::InsertTalent(const string& szName, uint32 uPoint)
{
	m_mapPoint[szName] = uPoint;
	if(m_pHandler) m_pHandler->OnTalentAdded(szName,uPoint);
}

void CTalentHolder::ChangeTalent(const string& szName, int32 iPoint)
{
	string strName = szName;
	tolower(strName);
	MapPoint::iterator itr = m_mapPoint.find(strName);
	if (iPoint == 0)
	{
		ostringstream strErr; 
		strErr << szName <<endl;
		LogErr("不能改变天赋点为零的天赋",strErr.str());
	}
	if(itr == m_mapPoint.end())
	{
		if (iPoint>0)
		{
			InsertTalent(szName,iPoint);
		}
		else
		{
			ostringstream strErr; 
			strErr << szName <<endl;
			LogErr("不能插入天赋点为负的天赋",strErr.str());
		}
	}
	else
	{
		itr->second += iPoint;
		uint32 uPoint = itr->second;
		if(uPoint <= 0)
		{
			RemoveTalent(szName);
			if (uPoint<0)
			{
				ostringstream strErr; 
				strErr << szName <<endl;
				LogErr("不能删除天赋点为负的天赋",strErr.str());
			}
			return;
		}
		if(m_pHandler) m_pHandler->OnTalentChanged(strName,iPoint);
	}
}

void CTalentHolder::RemoveTalent(const string& szName)
{
	string strName = szName;
	tolower(strName);
	MapPoint::iterator itr = m_mapPoint.find(strName);
	if(itr != m_mapPoint.end())
	{
		if(m_pHandler) m_pHandler->OnTalentRemove(szName,itr->second);
		m_mapPoint.erase(itr);
	}
}

void CTalentHolder::InsertName(const TCHAR* sName)
{
	m_setGlobalName.insert(sName);
}

bool CTalentHolder::ExistName(const string& szName)
{
	string strName = szName;
	tolower(strName);
	return m_setGlobalName.find(strName) != m_setGlobalName.end();

}

