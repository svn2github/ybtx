#include "stdafx.h"
#include "CBurstSoulMgrServer.h"
#include "CBurstSoulCfgServer.h"
#include "BaseHelper.h"
#include "CSkillInstServer.h"
#include "TSqrAllocator.inl"
#include "CCharacterDictator.h"
#include "PtrlGas2GacCPPDef.h"

CBurstSoulMgrServer::CBurstSoulMgrServer(CFighterDictator* pFighter)
:m_pFighter(pFighter)
,m_uBurstSoulTimes(0)
,m_uContinuousBurstTimes(0)
{
	m_uMaxBurstSoulTimes = GetMaxBurstTimes();
	pFighter->Attach(this, eCE_ChangeMapEnd);
}

CBurstSoulMgrServer::~CBurstSoulMgrServer(void)
{
	ClearVector(m_vecBurstSoulServer);
}

void CBurstSoulMgrServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	CFighterDictator* pFrom = GetFighter();
	Ast(pFrom);
	switch(uEvent)
	{
	case eCE_ChangeMapEnd:
		{
			VecBurstSoulServer::iterator it = m_vecBurstSoulServer.begin();
			for (; it != m_vecBurstSoulServer.end(); ++it)
			{
				(*it)->m_pCfg->get()->GetMagicEff()->Do((*it)->m_pSkillInst, pFrom, pFrom);
			}
			ClearVector(m_vecBurstSoulServer);
		}
		return;
	default:
		break;
	}	
}

uint32 CBurstSoulMgrServer::GetBurstSoulTimes()
{
	if (!m_vecBurstSoulServer.empty())
	{
		CFighterDictator* pFrom = GetFighter();
		Ast(pFrom);
		VecBurstSoulServer::iterator it = m_vecBurstSoulServer.begin();
		for (; it != m_vecBurstSoulServer.end(); ++it)
		{
			(*it)->m_pCfg->get()->GetMagicEff()->Do((*it)->m_pSkillInst, pFrom, pFrom);
		}
		ClearVector(m_vecBurstSoulServer);
	}
	return m_uBurstSoulTimes;	
}

uint32 CBurstSoulMgrServer::DoBurstSoulExtraEff(CSkillInstServer* pInst, CFighterDictator* pTo, EBurstSoulType eBurstSoulType)
{
	CFighterDictator* pFrom = GetFighter();
	Ast(pFrom);
	Ast(pTo);

	uint32 uRet = eHR_Fail;
	CCharacterDictator* pCharacter = pTo->GetCharacter();
	Ast(pCharacter);
	float fRandomRate = ((float)rand())/RAND_MAX;
	float fCurrentRate = 0.0f;
	CBurstSoulCfgServer::MapBurstSoulCfg::iterator itr = CBurstSoulCfgServer::ms_mapBurstSoulCfg.begin();
	for (; itr != CBurstSoulCfgServer::ms_mapBurstSoulCfg.end(); ++itr)
	{
		CBurstSoulCfgServer* pBurstSoulCfgServer = itr->second->get();
		fCurrentRate = float(pBurstSoulCfgServer->GetProbability()->GetDblValue(pFrom));
		if (fCurrentRate >= fRandomRate && pBurstSoulCfgServer->GetBurstSoulType() == eBurstSoulType
			&& pBurstSoulCfgServer->GetMagicEff())
		{
			uint32 uCount = pBurstSoulCfgServer->GetSoulCount()->GetIntValue(pFrom);
			pTo->ServerRelSoulBullet(pFrom->GetEntityID(), pBurstSoulCfgServer->GetBulletMagicName().c_str(),
				pBurstSoulCfgServer->GetBurstSoulType(), uCount);
			for (uint32 i=1; i<=uCount; i++)
			{
				CBurstSoulServer* pBurstSoulServer = new CBurstSoulServer(pInst, *(itr->second));
				m_vecBurstSoulServer.push_back(pBurstSoulServer);
			}
			uRet = eHR_Success;
		}
	}
	return uRet;
}

uint32 CBurstSoulMgrServer::DoDoubleBurstPrize(CSkillInstServer* pInst, CFighterDictator* pTo, uint32 uCount)
{
	CFighterDictator* pFrom = GetFighter();
	Ast(pFrom);
	CBurstSoulCfgServer::MapBurstSoulCfg::iterator itr = CBurstSoulCfgServer::ms_mapBurstSoulCfg.begin();
	for (; itr != CBurstSoulCfgServer::ms_mapBurstSoulCfg.end(); ++itr)
	{
		if (itr->second->get()->GetDoubleBurst()->GetDblValue(pFrom) == uCount)
		{
			for (uint32 i=1; i<=2; i++)
			{
				pTo->ServerRelSoulBullet(pFrom->GetEntityID(), itr->second->get()->GetBulletMagicName().c_str(), itr->second->get()->GetBurstSoulType(),1);
				CBurstSoulServer* pBurstSoulServer = new CBurstSoulServer(pInst, *(itr->second));
				m_vecBurstSoulServer.push_back(pBurstSoulServer);
			}
			return eHR_Success;
		}
	}
	return eHR_Fail;
}

void CBurstSoulMgrServer::NotifyBurstSoulArrived(EBurstSoulType eBurstSoulType)
{
	CFighterDictator* pFrom = GetFighter();
	Ast(pFrom);
	if (m_vecBurstSoulServer.empty())
		return;
	/*CBurstSoulServer* pBurstSoulServer = m_vecBurstSoulServer.back();
	m_vecBurstSoulServer.pop_back();
	pBurstSoulServer->m_pCfg->get()->GetMagicEff()->Do(pBurstSoulServer->m_pSkillInst, pFrom, pFrom);
	delete pBurstSoulServer;*/
	VecBurstSoulServer::iterator itrBegin = m_vecBurstSoulServer.begin();
	VecBurstSoulServer::iterator itrEnd = m_vecBurstSoulServer.end();
	for(; itrBegin!=itrEnd; itrBegin++)
	{
		if ((*itrBegin)->m_pCfg->get()->GetBurstSoulType() == eBurstSoulType)
		{
			(*itrBegin)->m_pCfg->get()->GetMagicEff()->Do((*itrBegin)->m_pSkillInst, pFrom, pFrom);
			delete *itrBegin;
			m_vecBurstSoulServer.erase(itrBegin);
			return;
		}
	}
}

void CBurstSoulMgrServer::AddBurstSoulTimes(uint32 uTimes)
{
	m_uBurstSoulTimes = m_uBurstSoulTimes+uTimes;
	m_uBurstSoulTimes = m_uBurstSoulTimes>m_uMaxBurstSoulTimes ? m_uMaxBurstSoulTimes:m_uBurstSoulTimes;
	OnBurstSoulTimesChanged();
}

void CBurstSoulMgrServer::ConsumeBurstSoulTimes(uint32 uTimes)
{
	if (uTimes>m_uBurstSoulTimes)
	{
		uTimes = m_uBurstSoulTimes;
	}
	m_uBurstSoulTimes = m_uBurstSoulTimes - uTimes;
	OnBurstSoulTimesChanged();
}

void CBurstSoulMgrServer::OnBurstSoulTimesChanged()
{
	CFighterDictator* pFrom = GetFighter();
	Ast(pFrom);
	CGas2GacCPP_OnBurstSoulTimesChanged Cmd;
	Cmd.uObjID = pFrom->GetEntityID();
	Cmd.uBurstSoulTimes = m_uBurstSoulTimes;
	pFrom->SendCmdToGac(&Cmd,eOnlyToDirector);
}

uint32 CBurstSoulMgrServer::GetNeedBurstTimesByLevel(uint32 uLevel)
{
	CBurstSoulCfgServer::MapBurstSoulCfg::iterator itr = CBurstSoulCfgServer::ms_mapBurstSoulCfg.begin();
	for (; itr != CBurstSoulCfgServer::ms_mapBurstSoulCfg.end(); ++itr)
	{
		if (itr->second->get()->GetLevel() && itr->second->get()->GetLevel()->GetIntValue() == uLevel)
		{
			return itr->second->get()->GetNeedBurstTimes()->GetIntValue();
		}
	}
	GenErr("不存在这个等级的爆魂蓄力值\n");
	return 5000;
}

uint32 CBurstSoulMgrServer::GetMaxBurstTimes()
{
	uint32 uMaxBurstTimes = 0;
	CBurstSoulCfgServer::MapBurstSoulCfg::iterator itr = CBurstSoulCfgServer::ms_mapBurstSoulCfg.begin();
	for (; itr != CBurstSoulCfgServer::ms_mapBurstSoulCfg.end(); ++itr)
	{
		if (itr->second->get()->GetNeedBurstTimes())
		{
			uint32 uBurstTimes = itr->second->get()->GetNeedBurstTimes()->GetIntValue();
			uMaxBurstTimes = uBurstTimes>uMaxBurstTimes ? uBurstTimes:uMaxBurstTimes;
		}
	}
	return uMaxBurstTimes;
}

void CBurstSoulMgrServer::AddContinuousBurstTimes(int32 uChangeTimes)
{
	m_uContinuousBurstTimes = (m_uContinuousBurstTimes+uChangeTimes)>0?m_uContinuousBurstTimes+uChangeTimes:0;
}

CBurstSoulServer::CBurstSoulServer(CSkillInstServer* pInst, const CBurstSoulCfgServerSharedPtr& pCfg)
:m_pSkillInst(pInst)
,m_pCfg(new CBurstSoulCfgServerSharedPtr(pCfg))
{
	m_pSkillInst->AddRef();
}

CBurstSoulServer::~CBurstSoulServer()
{
	m_pSkillInst->DelRef();
	SafeDelete(m_pCfg);
}

