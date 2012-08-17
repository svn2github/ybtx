#include "stdafx.h"
#include "CFightStatisticMgr.h"
#include "TimeHelper.h"
#include "CSkillServer.h"
#include "CAppServer.h"
#include "ErrLogHelper.h"

CFightStatisticMgr::CFightStatisticMgr()
{
	ResetData(m_FightStatisticData);
}

CFightStatisticMgr::~CFightStatisticMgr()
{
	ClearAllStatisticData();
}

uint32 CFightStatisticMgr::GetStatisticFightHurt(const TCHAR* sName)
{
	if (strcmp(sName,"") == 0)
	{
		return m_FightStatisticData.m_uFightHurt;
	}
	else
	{
		MapFightStatisticData::iterator iter = m_mapFightStatisticData.find(sName);
		if (iter == m_mapFightStatisticData.end())
		{
			LogErr("GetStatisticFightHurt error", sName);
			return 0;
		}
		return iter->second.m_uFightHurt;
	}
	
}

uint32 CFightStatisticMgr::GetStatisticFightHeal(const TCHAR* sName)
{
	if (strcmp(sName,"") == 0)
	{
		return m_FightStatisticData.m_uFightHeal;
	}
	else
	{
		MapFightStatisticData::iterator iter = m_mapFightStatisticData.find(sName);
		if (iter == m_mapFightStatisticData.end())
		{
			LogErr("GetStatisticFightHeal error", sName);
			return 0;
		}
		return iter->second.m_uFightHurt;
	}
}

uint32 CFightStatisticMgr::GetStatisticBeSubHurt(const TCHAR* sName)
{
	if (strcmp(sName,"") == 0)
	{
		return m_FightStatisticData.m_uBeSubHurt;
	}
	else
	{
		MapFightStatisticData::iterator iter = m_mapFightStatisticData.find(sName);
		if (iter == m_mapFightStatisticData.end())
		{
			LogErr("GetStatisticBeSubHurt error", sName);
			return 0;
		}
		return iter->second.m_uBeSubHurt;
	}
}

uint32 CFightStatisticMgr::GetStatisticFightTime(FightStatisticData& data)
{
	uint32 uBattleTime = 0;
	if (data.m_uBattleBeginTime != 0)
	{
		uBattleTime = m_FightStatisticData.m_uBattleTime + (uint32)(CAppServer::Inst()->GetFrameTime() - data.m_uBattleBeginTime);
	}
	else
	{
		uBattleTime = m_FightStatisticData.m_uBattleTime;
	}
	return uBattleTime;
}

float CFightStatisticMgr::GetStatisticDps(const TCHAR* sName)
{
	if (strcmp(sName,"") == 0)
	{
		uint32 uBattleTime = GetStatisticFightTime(m_FightStatisticData);
		if (uBattleTime != 0)
			return ((float)m_FightStatisticData.m_uFightHurt / (float)uBattleTime) * 1000;
	}
	else
	{
		MapFightStatisticData::iterator iter = m_mapFightStatisticData.find(sName);
		if (iter != m_mapFightStatisticData.end())
		{
			uint32 uBattleTime = GetStatisticFightTime(iter->second);
			if (uBattleTime != 0)
				return ((float)(iter->second.m_uFightHurt) / (float)(uBattleTime)) * 1000;
		}
		else
		{
			LogErr("GetStatisticDps error", sName);
		}
	}
	return 0;
}

float CFightStatisticMgr::GetStatisticHps(const TCHAR* sName)
{
	if (strcmp(sName,"") == 0)
	{
		uint32 uBattleTime = GetStatisticFightTime(m_FightStatisticData);
		if (uBattleTime != 0)
			return ((float)m_FightStatisticData.m_uFightHeal / (float)uBattleTime) * 1000;
	}
	else
	{
		MapFightStatisticData::iterator iter = m_mapFightStatisticData.find(sName);
		if (iter != m_mapFightStatisticData.end())
		{
			uint32 uBattleTime = GetStatisticFightTime(iter->second);
			if (uBattleTime != 0)
				return ((float)(iter->second.m_uFightHeal) / (float)(uBattleTime)) * 1000;
		}
		else
		{
			LogErr("GetStatisticHps error", sName);
		}
	}
	return 0;
}

uint32 CFightStatisticMgr::GetStatisticDeadTimes(const TCHAR* sName)
{
	if (strcmp(sName,"") == 0)
	{
		return m_FightStatisticData.m_uDeadTimes;
	}
	else
	{
		MapFightStatisticData::iterator iter = m_mapFightStatisticData.find(sName);
		if (iter == m_mapFightStatisticData.end())
		{
			LogErr("GetStatisticDeadTimes error", sName);
			return 0;
		}
		return iter->second.m_uDeadTimes;
	}
}

bool CFightStatisticMgr::BeBeginStatistic(FightStatisticData& data)
{
	return data.m_bStatistic;
}

void CFightStatisticMgr::ResetData(FightStatisticData& data)
{
	data.m_uFightHurt = 0;
	data.m_uFightHeal = 0;
	data.m_uBeSubHurt = 0;
	data.m_uDeadTimes = 0;
	data.m_bStatistic = false;
	data.m_uBattleBeginTime = 0;
	data.m_uBattleTime = 0;
}

void CFightStatisticMgr::BeginStatisticData(FightStatisticData& data)
{
	ResetData(data);
	data.m_bStatistic = true;
}

void CFightStatisticMgr::EndStatisticData(FightStatisticData& data)
{
	data.m_bStatistic = false;
	data.m_uBattleBeginTime = 0;
}

void CFightStatisticMgr::BeginStatistic(const TCHAR* sName)
{
	if (strcmp(sName,"") == 0)
	{
		BeginStatisticData(m_FightStatisticData);
	}
	else
	{
		if (m_mapFightStatisticData.size() >= 6)
		{
			LogErr("战斗统计过多");
		}
		MapFightStatisticData::iterator iter = m_mapFightStatisticData.find(sName);
		if (iter == m_mapFightStatisticData.end())
		{
			FightStatisticData data;
			BeginStatisticData(data);
			m_mapFightStatisticData.insert(make_pair(sName,data));
			return;
		}
		BeginStatisticData(iter->second);
	}
}

void CFightStatisticMgr::EndStatistic(const TCHAR* sName)
{
	if (strcmp(sName,"") == 0)
	{
		EndStatisticData(m_FightStatisticData);
	}
	else
	{
		MapFightStatisticData::iterator iter = m_mapFightStatisticData.find(sName);
		if (iter != m_mapFightStatisticData.end())
		{
			EndStatisticData(iter->second);
		}
	}
}

void CFightStatisticMgr::ClearAllStatisticData()
{
	ResetData(m_FightStatisticData);
	m_mapFightStatisticData.clear();
}

void CFightStatisticMgr::StatisticOnBattleStateChanged(bool bEnterBattle, FightStatisticData& data)
{
	if (!BeBeginStatistic(data))
		return;
	if (!bEnterBattle)
	{
		if (data.m_uBattleBeginTime != 0)
		{
			data.m_uBattleTime = data.m_uBattleTime + (uint32)(CAppServer::Inst()->GetFrameTime() - data.m_uBattleBeginTime);
		}
	}
	else
	{
		data.m_uBattleBeginTime = CAppServer::Inst()->GetFrameTime();
	}
}

void CFightStatisticMgr::OnBattleStateChanged(bool bEnterBattle)
{
	StatisticOnBattleStateChanged(bEnterBattle, m_FightStatisticData);
	MapFightStatisticData::iterator iter = m_mapFightStatisticData.begin();
	for ( ; iter != m_mapFightStatisticData.end(); iter++)
	{
		StatisticOnBattleStateChanged(bEnterBattle, iter->second);
	}
}

void CFightStatisticMgr::StatisticHurt(EFightInfoIndex eFightInfoIndex, uint32 uHurt, FightStatisticData& data)
{
	if (!BeBeginStatistic(data))
		return;
	if (IsStatisticFightInfoIndex(eFightInfoIndex))
	{
		if 	(eFightInfoIndex == EFII_Heal || eFightInfoIndex == EFII_StrikeHeal)
		{
			data.m_uFightHeal = data.m_uFightHeal + uHurt;
		}
		else
		{
			data.m_uFightHurt = data.m_uFightHurt + uHurt;
		}
	}
}

void CFightStatisticMgr::StatisticSubHp(EFightInfoIndex eFightInfoIndex, uint32 uBeSubHp, FightStatisticData& data)
{
	if (!BeBeginStatistic(data))
		return;
	switch(eFightInfoIndex)
	{
	case EFII_Hurt:
	case EFII_StrikeHurt:
	case EFII_Resist:
	case EFII_Block:
	case EFII_NonTypeAttackHit:
	case EFII_StateEffectHit:
	case EFII_MultiAbsorb:
	case EFII_StateEffectStrikeHit:
		{
			data.m_uBeSubHurt += uBeSubHp;
			return;
		}
	default:
		return;
	}
}

void CFightStatisticMgr::OnFightHurt(EFightInfoIndex eFightInfoIndex, uint32 uHurt)
{
	StatisticHurt(eFightInfoIndex, uHurt, m_FightStatisticData);
	MapFightStatisticData::iterator iter = m_mapFightStatisticData.begin();
	for ( ; iter != m_mapFightStatisticData.end(); iter++)
	{
		StatisticHurt(eFightInfoIndex, uHurt, iter->second);
	}
}

void CFightStatisticMgr::OnBeSubHp(EFightInfoIndex eFightInfoIndex, uint32 uBeSubHp)
{
	StatisticSubHp(eFightInfoIndex, uBeSubHp, m_FightStatisticData);
	MapFightStatisticData::iterator iter = m_mapFightStatisticData.begin();
	for ( ; iter != m_mapFightStatisticData.end(); iter++)
	{
		StatisticSubHp(eFightInfoIndex, uBeSubHp, iter->second);
	}
}

void CFightStatisticMgr::OnDead()
{
	if (BeBeginStatistic(m_FightStatisticData))
		++m_FightStatisticData.m_uDeadTimes;
	MapFightStatisticData::iterator iter = m_mapFightStatisticData.begin();
	for ( ; iter != m_mapFightStatisticData.end(); iter++)
	{
		if (BeBeginStatistic(iter->second))
			++((iter->second).m_uDeadTimes);
	}
}

bool CFightStatisticMgr::IsStatisticFightInfoIndex(EFightInfoIndex eFightInfoIndex)
{
	switch(eFightInfoIndex)
	{
	case EFII_Hurt:
	case EFII_StrikeHurt:
	case EFII_Resist:
	case EFII_Block:
	case EFII_NonTypeAttackHit:
	case EFII_StateEffectHit:
	case EFII_MultiAbsorb:
	case EFII_Heal:
	case EFII_StrikeHeal:
	case EFII_StateEffectStrikeHit:
		{
			return true;
		}
	default:
		return false;
	}
}
