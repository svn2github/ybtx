#include "stdafx.h"
#include "CBaseStateServer.h"
#include "CCfgCalc.inl"
#include "CFighterDictator.h"
#include "CEntityServerManager.h"
#include "BaseHelper.h"

CBaseStateServer::CBaseStateServer(CSkillInstServer* pSkillInst, CFighterDictator* pInstaller) :
m_uDynamicId(0),
m_uInstallerID(0),
m_iTime(0),
m_iRemainTime(0),
m_uStartTime(0),
m_pSkillInst(pSkillInst),
m_pVariantPointerHolder(NULL)
{
	if(pInstaller)
	{
		m_uInstallerID = pInstaller->GetEntityID();
	}
}

CBaseStateServer::CBaseStateServer(CSkillInstServer* pSkillInst, CFighterDictator* pInstaller, int32 iTime, int32 iRemainTime) :
m_uDynamicId(0),
m_uInstallerID(0),
m_iTime(iTime),
m_iRemainTime(iRemainTime),
m_uStartTime(0),
m_pSkillInst(pSkillInst),
m_pVariantPointerHolder(NULL)

{
	if(pInstaller)
	{
		m_uInstallerID = pInstaller->GetEntityID();
	}
}

CBaseStateServer::~CBaseStateServer()
{
	SafeDelete(m_pVariantPointerHolder);
}

CFighterDictator* CBaseStateServer::GetInstaller()
{
	CEntityServer* pEntity = CEntityServerManager::GetEntityByID(m_uInstallerID);
	if(pEntity)
		return pEntity->GetFighter();
	else
		return NULL;
}

void CBaseStateServer::DeleteVaraintPointerHolder()
{
	SafeDelete(m_pVariantPointerHolder);
}

int32 CBaseStateServer::GetLeftTime()
{
	if(m_iRemainTime == -1)
	{
		return -1;
	}
	else
	{
		if(m_uStartTime == 0)
		{
			return 0;
		}
		else
		{
			uint64 uNow = GetOwner()->GetDistortedFrameTime();
			int32 iRemainTime = m_iRemainTime - (int32(uNow - m_uStartTime) + 500) / 1000;
			if(iRemainTime < 0)
			{
				return 0;
			}
			else
			{
				return iRemainTime;
			}
		}
	}
}









