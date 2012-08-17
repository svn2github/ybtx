#include "stdafx.h"
#include "CTeamClientManager.h"
#include "PtrlGas2GacCPPDef.h"
#include "CCharacterFollower.h"
#include "CFighterFollower.h"
#include "CSyncVariantClient.h"
#include "ITeamClientManagerHandler.h"
#include "TSqrAllocator.inl"

CSyncSceneInfo::CSyncSceneInfo(uint32 uEntityID)
:m_uEntityID(uEntityID)
{

}

void CSyncSceneInfo::UpdateInfo(const CGas2GacCPP_TeamSyncInfo* pCmd)
{
	m_uLevel = pCmd->uLevel;
	m_uBaseHP = pCmd->uBaseHP;
	m_uBaseMP = pCmd->uBaseMP;
	m_uBaseEP = pCmd->uBaseEP;
	m_uBaseRP = pCmd->uBaseRP;
	m_uHP = pCmd->uHP;
	m_uMP = pCmd->uMP;
	m_uEP = pCmd->uEP;
	m_uRP = pCmd->uRP;
	m_fPos = pCmd->fPos;
	m_bInBattle = pCmd->bInBattle;
}

CTeamClientManager::~CTeamClientManager()
{
	SafeDelCtn2(m_mapSyncInfo);
}

void CTeamClientManager::SetHandler( ITeamClientManagerHandler *pHandler )
{
	AdvDelRef(GetInst()->m_pHandler);
	GetInst()->m_pHandler = pHandler;
	AdvAddRef(GetInst()->m_pHandler);
}

uint32 CTeamClientManager::GetHP(uint32 uEntityID)
{
	uint32 uValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		uValue = pPlayer->GetFighter()->m_HealthPoint.LimitGet();
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			uValue = (it->second)->m_uHP;
		}
	
	}
	return uValue;
}

uint32 CTeamClientManager::GetMP(uint32 uEntityID)
{
	uint32 uValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		uValue = pPlayer->GetFighter()->m_ManaPoint.LimitGet();
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			uValue = (it->second)->m_uMP;
		}

	}
	return uValue;
}

uint32 CTeamClientManager::GetEP(uint32 uEntityID)
{
	uint32 uValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		uValue = pPlayer->GetFighter()->m_EnergyPoint.LimitGet();
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			uValue = (it->second)->m_uEP;
		}

	}
	return uValue;
}

uint32 CTeamClientManager::GetRP(uint32 uEntityID)
{
	uint32 uValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		uValue = pPlayer->GetFighter()->m_RagePoint.LimitGet();
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			uValue = (it->second)->m_uRP;
		}

	}
	return uValue;
}

uint32 CTeamClientManager::GetBaseHP(uint32 uEntityID)
{
	uint32 uValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		CFighterFollower* pFollower = pPlayer->GetFighter();
		uValue = pFollower->m_HealthPoint.Get(pFollower);
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			uValue = (it->second)->m_uBaseHP;
		}

	}
	return uValue;
}

uint32 CTeamClientManager::GetBaseMP(uint32 uEntityID)
{
	uint32 uValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		CFighterFollower* pFollower = pPlayer->GetFighter();
		uValue = pFollower->m_ManaPoint.Get(pFollower);
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			uValue = (it->second)->m_uBaseMP;
		}

	}
	return uValue;
}

uint32 CTeamClientManager::GetBaseEP(uint32 uEntityID)
{
	uint32 uValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		CFighterFollower* pFollower = pPlayer->GetFighter();
		uValue = pFollower->m_EnergyPoint.Get(pFollower);
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			uValue = (it->second)->m_uBaseEP;
		}

	}
	return uValue;
}

uint32 CTeamClientManager::GetBaseRP(uint32 uEntityID)
{
	uint32 uValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		CFighterFollower* pFollower = pPlayer->GetFighter();
		uValue = pFollower->m_RagePoint.Get(pFollower);
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			uValue = (it->second)->m_uBaseRP;
		}

	}
	return uValue;
}

void CTeamClientManager::GetPos(uint32 uEntityID,CFPos& Pos)
{
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		pPlayer->GetPixelPos(Pos);
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			Pos = it->second->GetPos();
		}

	}
}

uint32 CTeamClientManager::GetLevel(uint32 uEntityID)
{
	uint32 uLevel=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		uLevel=pPlayer->CppGetLevel();
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			uLevel = it->second->m_uLevel;
		}

	}
	return uLevel;
}

uint32 CTeamClientManager::GetHairResID(uint32 uEntityID)
{
	uint32 fValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		fValue = pPlayer->GetSyncVariant(eCTI_Static)->GetMember("Res")->GetMember("HairResID")->GetNumber<uint8>();
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			fValue = (it->second)->m_uHairResID;
		}

	}
	return fValue;
}
uint32 CTeamClientManager::GetFaceResID(uint32 uEntityID)
{
	uint32 fValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		fValue = pPlayer->GetSyncVariant(eCTI_Static)->GetMember("Res")->GetMember("FaceResID")->GetNumber<uint8>();
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			fValue = (it->second)->m_uFaceResID;
		}

	}
	return fValue;
}
uint32 CTeamClientManager::GetHeadIndexID(uint32 uEntityID)
{
	uint32 fValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		fValue = pPlayer->GetSyncVariant(eCTI_Static)->GetMember("Res")->GetMember("HeadIndexID")->GetNumber<uint32>();
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			fValue = (it->second)->m_uHeadIndexID;
		}

	}
	return fValue;
}
uint32 CTeamClientManager::GetBodyIndexID(uint32 uEntityID)
{
	uint32 fValue=0;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		fValue =pPlayer->GetSyncVariant(eCTI_Static)->GetMember("Res")->GetMember("BodyIndexID")->GetNumber<uint32>();
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			fValue = (it->second)->m_uBodyIndexID;
		}

	}
	return fValue;
}

bool CTeamClientManager::GetInBattle(uint32 uEntityID)
{
	bool bInBattle=false;
	CCharacterFollower* pPlayer = CCharacterFollower::GetCharacterByID(uEntityID);
	if (pPlayer)
	{
		bInBattle =pPlayer->GetFighter()->GetCtrlState(eFCS_InBattle);
	}
	else
	{
		MapSyncInfo MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
		MapSyncInfo::iterator it = MapSyncInfo.find(uEntityID);
		if (it!=MapSyncInfo.end())
		{
			bInBattle = (it->second)->m_bInBattle;
		}

	}
	return bInBattle;
}