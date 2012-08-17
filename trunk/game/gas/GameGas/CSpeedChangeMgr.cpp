#include "stdafx.h"
#include "CSpeedChangeMgr.h"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"

#define CASCADE_SPEED_BY_MUL

#ifdef CASCADE_SPEED_BY_MUL
	#define NOCHANGE_SPEED 1.0
	#define CHANGE_OPTR /=
#else
	#define NOCHANGE_SPEED 0.0
	#define CHANGE_OPTR -=
#endif

void CSpeedChangeMgr::InitChange()
{
	m_mapSpeedDownChange.clear();
	m_mapSpeedDownChange.insert(make_pair(NOCHANGE_SPEED, 1));
	m_mapSpeedUpChange.clear();
	m_mapSpeedUpChange.insert(make_pair(NOCHANGE_SPEED, 1));
}

void CSpeedChangeMgr::AddChange(double& fChange, bool bCascade)
{
	//如果可叠加则直接返回
	if(bCascade) return;

	if(fChange < NOCHANGE_SPEED)
	{
		double fMaxChange = m_mapSpeedDownChange.begin()->first;
		MapSpeedChange::iterator itr = m_mapSpeedDownChange.find(fChange);
		if(itr == m_mapSpeedDownChange.end())
		{
			m_mapSpeedDownChange.insert(make_pair(fChange, 1));
		}
		else
		{
			++itr->second;
		}

		if(fChange < fMaxChange)
		{                                                                                                                                             
			fChange CHANGE_OPTR fMaxChange;
		}
		else
		{
			fChange = NOCHANGE_SPEED;
		}
	}
	else if(fChange > NOCHANGE_SPEED)
	{                                           
		double fMaxChange = m_mapSpeedUpChange.rbegin()->first;
		MapSpeedChange::iterator itr = m_mapSpeedUpChange.find(fChange);
		if(itr == m_mapSpeedUpChange.end())
		{
			m_mapSpeedUpChange.insert(make_pair(fChange, 1));
		}
		else
		{
			++itr->second;
		}

		if(fChange > fMaxChange)
		{
			fChange CHANGE_OPTR fMaxChange;
		}
		else
		{
			fChange = NOCHANGE_SPEED;
		}
	}
}

void CSpeedChangeMgr::DecChange(double& fChange, bool bCascade)
{
	//如果可叠加则直接返回
	if(bCascade) return;

	if(fChange < NOCHANGE_SPEED)
	{
		MapSpeedChange::iterator itr = m_mapSpeedDownChange.find(fChange);
		if(itr == m_mapSpeedDownChange.end())
		{
			stringstream ExpStr;
			ExpStr << fChange;
			GenErr("魔法操作撤销的减速值不存在于变速管理器记录的减速速度中,减速值为：", ExpStr.str());
			return;
		}
		else
		{
			if(itr->second > 1)
			{
				--itr->second;
			}
			else
			{
				m_mapSpeedDownChange.erase(itr);
			}
		}

		double fMaxChange = m_mapSpeedDownChange.begin()->first;

		if(fChange < fMaxChange)
		{
			fChange CHANGE_OPTR fMaxChange;
		}
		else
		{
			fChange = NOCHANGE_SPEED;
		}
	}
	else if(fChange > NOCHANGE_SPEED)
	{
		MapSpeedChange::iterator itr = m_mapSpeedUpChange.find(fChange);
		if(itr == m_mapSpeedUpChange.end())
		{
			stringstream ExpStr;
			ExpStr << fChange;
			GenErr("魔法操作撤销的加速值不存在于变速管理器记录的加速速度中,加速值为：", ExpStr.str());
			return;
		}
		else
		{
			if(itr->second > 1)
			{
				--itr->second;
			}
			else
			{
				m_mapSpeedUpChange.erase(itr);
			}
		}

		double fMaxChange = m_mapSpeedUpChange.rbegin()->first;

		if(fChange > fMaxChange)
		{
			fChange CHANGE_OPTR fMaxChange;
		}
		else
		{
			fChange = NOCHANGE_SPEED;
		}
	}
}

#undef CHANGE_OPTR
#undef NOCHANGE_SPEED
#undef CASCADE_SPEED_BY_MUL

