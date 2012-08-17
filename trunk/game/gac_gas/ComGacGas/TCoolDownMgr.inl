#pragma once
#include "TCoolDownMgr.h"

template<typename App_t, typename Fight_t> uint32 TCoolDownMgr<App_t, Fight_t>::ms_uFightSkillCommonCDTime = 1000;
template<typename App_t, typename Fight_t> uint32 TCoolDownMgr<App_t, Fight_t>::ms_uItemSkillCommonCDTime = 0;

template<typename App_t, typename Fight_t>
TCoolDownMgr<App_t,Fight_t>::TCoolDownMgr(Fight_t* pFighter)
:m_uComCoolDownBeginTime(0),
m_uDrugItemSkillCDBeginTime(0),
m_uSpecialItemSkillCDBeginTime(0),
m_pFighter(pFighter)
{
}

template<typename App_t, typename Fight_t>
TCoolDownMgr<App_t,Fight_t>::~TCoolDownMgr(void)
{
	for(CoolDownInfoMap_t::iterator it = m_mapCoolDownInfo.begin(); it != m_mapCoolDownInfo.end() ; ++it)
		delete it->second;
}

template<typename App_t, typename Fight_t>
bool TCoolDownMgr<App_t,Fight_t>::IsCoolDown( const TCHAR* szName, ESkillCoolDownType eCoolDownType )const
{
	return GetCoolDownTime( szName, eCoolDownType ) == 0;
}

template<typename App_t,typename Fight_t>
CCoolDownInfo* TCoolDownMgr<App_t,Fight_t>::IntNewCoolDown( const TCHAR* szName, ESkillCoolDownType eCoolDownType, uint32 uCoolDownTime, bool bSwitchEquipSkill )
{
	uint64 uNow=const_cast<TCoolDownMgr*>(this)->GetFighter()->GetDistortedFrameTime();

	switch(eCoolDownType)
	{
	case eSCDT_FightSkill:	
	case eSCDT_TempSkill:
		{
			if(uCoolDownTime != 0)
				m_uComCoolDownBeginTime = uNow;	//重置公共冷却时间
		}
		break;
	case eSCDT_NoComCDFightSkill:
	case eSCDT_OtherSkill:
	case eSCDT_UnrestrainedSkill:
	case eSCDT_NonFightSkill:
	case eSCDT_EquipSkill:
	case eSCDT_OtherItemSkill:
	case eSCDT_UnrestrainedFightSkill:
		break;
	case eSCDT_DrugItemSkill:
		{
			ms_uItemSkillCommonCDTime = uCoolDownTime;
			m_uDrugItemSkillCDBeginTime = uNow;
		}
		break;
	case eSCDT_SpecialItemSkill:
		{
			m_uSpecialItemSkillCDBeginTime = uNow;
		}
		break;
	default:
		return NULL;
	}

	pair< CoolDownInfoMap_t::iterator , bool > p =
		m_mapCoolDownInfo.insert( make_pair( std::string(szName),static_cast<CCoolDownInfo*>(NULL) ) );

	CCoolDownInfo* pInfo;

	if( p.second )
	{
		pInfo = new CCoolDownInfo;
		pInfo->m_uBeginTime = uNow;
		pInfo->m_uEndTime = uNow + uCoolDownTime;
		pInfo->m_uCoolDownTime=uCoolDownTime;
		pInfo->m_eCoolDownType=eCoolDownType;
		pInfo->m_bSwitchEquipSkill = bSwitchEquipSkill;
		p.first->second = pInfo;
		return pInfo;
	}

	pInfo=p.first->second;
	pInfo->m_uCoolDownTime=uCoolDownTime;
	pInfo->m_bSwitchEquipSkill = bSwitchEquipSkill;
	
	if( pInfo->m_uEndTime <= uNow )
	{
		//上一个技能已经完全cooldown,当前技能必定可以使用
		pInfo->m_uBeginTime = uNow;
		pInfo->m_uEndTime = uNow + uCoolDownTime;
		return pInfo;
	}

	//上一个技能还没有完全cooldown

	if( pInfo->m_uBeginTime + uCoolDownTime < uNow )
	{
		if (uCoolDownTime == 0)//结束此技能的冷却
		{
			pInfo->m_uEndTime = uNow;
		}
		else//重新设置此技能的冷却
		{
			pInfo->m_uBeginTime = uNow;
			pInfo->m_uEndTime += uCoolDownTime;
		}
		return pInfo;
	}

	return NULL;
}

template<typename App_t,typename Fight_t>
uint32 TCoolDownMgr<App_t,Fight_t>::GetCoolDownTime( const TCHAR* szName, ESkillCoolDownType eCoolDownType )const
{
	uint64 uNow=const_cast<TCoolDownMgr*>(this)->GetFighter()->GetDistortedFrameTime();
	int32 nLeftTime = 0;

	switch(eCoolDownType)
	{
	case eSCDT_FightSkill:
	case eSCDT_TempSkill:
		{
			nLeftTime = (int32)((uint64)ms_uFightSkillCommonCDTime - (uNow - m_uComCoolDownBeginTime));
		}
		break;
	case eSCDT_NoComCDFightSkill:
	case eSCDT_OtherSkill:
	case eSCDT_UnrestrainedSkill:
	case eSCDT_NonFightSkill:
	case eSCDT_EquipSkill:
	case eSCDT_OtherItemSkill:
	case eSCDT_UnrestrainedFightSkill:
		break;
	case eSCDT_SpecialItemSkill:
		{
			nLeftTime = (int32)((uint64)ms_uFightSkillCommonCDTime - (uNow - m_uSpecialItemSkillCDBeginTime));
			CoolDownInfoMap_t::const_iterator it = m_mapCoolDownInfo.find(std::string(szName));
			if ( it != m_mapCoolDownInfo.end() )
			{
				CCoolDownInfo* pInfo = it->second;
				if( pInfo->m_uEndTime > uNow )
					nLeftTime = max(nLeftTime, int32(pInfo->m_uBeginTime + pInfo->m_uCoolDownTime - uNow));
			}
			if (nLeftTime <= 0)
			{
				nLeftTime = (int32)((uint64)ms_uFightSkillCommonCDTime - (uNow - m_uDrugItemSkillCDBeginTime));
				nLeftTime = nLeftTime<0 ? 0:nLeftTime;
			}
		}
		return nLeftTime;
	case eSCDT_DrugItemSkill:
		{
			if(m_uDrugItemSkillCDBeginTime == 0)
				nLeftTime = 0;
			else
				nLeftTime = (int32)((uint64)ms_uItemSkillCommonCDTime - (uNow - m_uDrugItemSkillCDBeginTime));

			if(nLeftTime <= 0)
			{
				nLeftTime = (int32)((uint64)ms_uFightSkillCommonCDTime - (uNow - m_uSpecialItemSkillCDBeginTime));
				nLeftTime = nLeftTime<0 ? 0:nLeftTime;
			}
		}
		return nLeftTime;
	default:
		return 0;
	}

	if(nLeftTime < 0)
	{
		nLeftTime = 0;
	}

	CoolDownInfoMap_t::const_iterator it = m_mapCoolDownInfo.find(std::string(szName));
	
	if ( it == m_mapCoolDownInfo.end() )
		return nLeftTime;

	CCoolDownInfo* pInfo = it->second;

	if( pInfo->m_uEndTime < uNow )
		return nLeftTime;

	nLeftTime = max(nLeftTime, (int32)(pInfo->m_uBeginTime + pInfo->m_uCoolDownTime - uNow));

	//if( nLeftTime <= 0 )
	//	return nLeftTime;

	return (uint32)nLeftTime;
}

template<typename App_t,typename Fight_t>
void TCoolDownMgr<App_t,Fight_t>::ClearCommonCD()
{
	m_uComCoolDownBeginTime = 0;
}

template<typename App_t,typename Fight_t>
bool TCoolDownMgr<App_t,Fight_t>::IsInCommonCD( const TCHAR* szName, ESkillCoolDownType eCoolDownType )
{
	uint64 uNow=const_cast<TCoolDownMgr*>(this)->GetFighter()->GetDistortedFrameTime();
	int32 nLeftTime = 0;

	bool bInCommonCD = true;
	switch(eCoolDownType)
	{
	case eSCDT_FightSkill:
	case eSCDT_TempSkill:
		{
			nLeftTime = (int32)((uint64)ms_uFightSkillCommonCDTime - (uNow - m_uComCoolDownBeginTime));
			if (nLeftTime <= 0)
				bInCommonCD = false;
		}
		break;
	default:
		bInCommonCD = false;
		break;
	}
	if (bInCommonCD)
	{
		CoolDownInfoMap_t::const_iterator it = m_mapCoolDownInfo.find(std::string(szName));
		if ( it == m_mapCoolDownInfo.end() )
			return bInCommonCD;

		CCoolDownInfo* pInfo = it->second;
		if( pInfo->m_uEndTime < uNow )
			return bInCommonCD;

		return false;
	}
	return bInCommonCD;
}

template<typename App_t,typename Fight_t>
bool TCoolDownMgr<App_t,Fight_t>::IsSwitchEquipSkill( const TCHAR* szName )
{
	CoolDownInfoMap_t::const_iterator it = m_mapCoolDownInfo.find(std::string(szName));

	if ( it == m_mapCoolDownInfo.end() )
		return false;
	return it->second->m_bSwitchEquipSkill;
}

template<typename App_t,typename Fight_t>
void TCoolDownMgr<App_t,Fight_t>::ResetAllCoolDown()
{
	uint64 uNow=const_cast<TCoolDownMgr*>(this)->GetFighter()->GetDistortedFrameTime();

	CoolDownInfoMap_t::iterator it = m_mapCoolDownInfo.begin();
	for (;it != m_mapCoolDownInfo.end(); ++it)
	{
		if (it->second->m_eCoolDownType != eSCDT_DrugItemSkill && 
			it->second->m_eCoolDownType != eSCDT_SpecialItemSkill && 
			it->second->m_eCoolDownType != eSCDT_OtherItemSkill)
		{
			it->second->m_uEndTime = uNow;
		}
	}
	m_uComCoolDownBeginTime = 0;
}

template<typename App_t,typename Fight_t>
void TCoolDownMgr<App_t,Fight_t>::ResetCoolDownByCoolDownType(ESkillCoolDownType eCoolDownType)
{
	uint64 uNow=App_t::Inst()->GetFrameTime();

	CoolDownInfoMap_t::iterator it = m_mapCoolDownInfo.begin();
	for (;it != m_mapCoolDownInfo.end(); ++it)
	{
		if (it->second->m_eCoolDownType == eCoolDownType)
		{
			it->second->m_uEndTime = uNow;
		}
	}
	if (eCoolDownType == eSCDT_DrugItemSkill || 
		eCoolDownType == eSCDT_SpecialItemSkill)
	{
		m_uComCoolDownBeginTime = 0;
	}
}

template<typename App_t,typename Fight_t>
CoolDownInfoMap_t& TCoolDownMgr<App_t,Fight_t>::GetCoolDownInfoMap()
{
	return m_mapCoolDownInfo;
}
