#include "stdafx.h"
#include "AgilePropertyServer.h"
#include "CFighterDictator.h"
#include "FighterProperty.inl"
#include "CTriggerEvent.h"

template class TAgileProperty<CManaPointServer,ePID_ManaPoint,TBaseIntegerAMBProperty,&CStaticAttribs::m_MaxManaPoint>;

template class TAgileProperty<CHealthPointServer,ePID_HealthPoint,TBaseIntegerAMBProperty,&CStaticAttribs::m_MaxHealthPoint>;

template class TAgileProperty<CRagePointServer,ePID_RagePoint,TBaseIntegerAMBProperty,&CStaticAttribs::m_MaxRagePoint>;

template class TAgileProperty<CEnergyPointServer,ePID_EnergyPoint,TBaseIntegerAMBProperty,&CStaticAttribs::m_MaxEnergyPoint>;

template class TAgileProperty<CComboPointServer,ePID_ComboPoint,TBaseIntegerAMBProperty,&CStaticAttribs::m_MaxComboPoint>;


void CRenewHPTick::OnTick()
{
	int32 CurHP = m_pFighter->m_HealthPoint.LimitGet();
	if (m_pFighter->GetCtrlState(eFCS_InBattle))
	{
		CurHP += (int32)(m_pFighter->m_RevertPer.Get(m_pFighter) * m_pFighter->m_HPUpdateRate.Get(m_pFighter) * m_pFighter->m_HealthPoint.Get(m_pFighter));
	}
	else
	{
		CurHP += (int32)(m_pFighter->m_HPUpdateRate.Get(m_pFighter) * m_pFighter->m_HealthPoint.Get(m_pFighter));
	}
	m_pFighter->m_HealthPoint.LimitSet(CurHP, m_pFighter);
}

void CRenewMPTick::OnTick()
{
	int32 CurMP = m_pFighter->m_ManaPoint.LimitGet();
	if (m_pFighter->GetCtrlState(eFCS_InBattle))
	{
		CurMP += (int32)(m_pFighter->m_RevertPer.Get(m_pFighter) * m_pFighter->m_MPUpdateRate.Get(m_pFighter) * m_pFighter->m_ManaPoint.Get(m_pFighter));
	}
	else
	{
		CurMP += (int32)(m_pFighter->m_MPUpdateRate.Get(m_pFighter) * m_pFighter->m_ManaPoint.Get(m_pFighter));
	}
	m_pFighter->m_ManaPoint.LimitSet(CurMP, m_pFighter);
}

void CReduceRPTick::OnTick()
{
	int32 CurRP = m_pFighter->m_RagePoint.LimitGet();
	CurRP -= (int32)(m_pFighter->m_RPUpdateValue.Get(m_pFighter));
	m_pFighter->m_RagePoint.LimitSet(CurRP, m_pFighter);
}

void CRenewEPTick::OnTick()
{
	int32 CurEP = m_pFighter->m_EnergyPoint.LimitGet();
	if (m_pFighter->IsMoving())
	{
		if (m_pFighter->GetCtrlState(eFCS_InBattleHorse) || m_pFighter->GetCtrlState(eFCS_InNormalHorse))
			CurEP += (int32)(3 * m_pFighter->m_EPUpdateValue.Get(m_pFighter));
		else
			CurEP += (int32)(m_pFighter->m_EPUpdateValue.Get(m_pFighter));
	}
	else
	{
		CurEP += (int32)(m_pFighter->m_EPUpdateValue.Get(m_pFighter));
	}
	m_pFighter->m_EnergyPoint.LimitSet(CurEP, m_pFighter);
}

void CHealthPointServer::Unit()
{
	if(m_pTick)
	{
		m_pTick->GetFighter()->UnRegistDistortedTick(m_pTick);
		delete m_pTick;
		m_pTick = NULL;
	}
}

void CHealthPointServer::OnLimitSet(int32 nValue, const CFighterDictator* pFighter)
{
	int32 nHealthPoint = int32(pFighter->m_HealthPoint.Get(pFighter));
	if (nHealthPoint == 0)
		return;

	if (pFighter->CppGetClass() == eCL_OrcWarrior)
	{
		uint8 uCurPercentOfLostHP;
		if (nValue < nHealthPoint)
			uCurPercentOfLostHP = (nHealthPoint - nValue) * 10 / nHealthPoint;
		else
			uCurPercentOfLostHP = 0;

		if (uCurPercentOfLostHP != m_uPercentOfLostHP)
		{
			CTriggerEvent::MessageEvent(eTST_Trigger, const_cast<CFighterDictator*>(pFighter), NULL, eSET_HPChanged);
			m_uPercentOfLostHP = uCurPercentOfLostHP;
		}
	}
}

void CHealthPointServer::OnRenewAgileValue(const CFighterDictator* pFighter)
{
	int32 nMaxHP = int32(pFighter->m_HealthPoint.Get(pFighter));
	int32 nCurHP = int32(pFighter->m_HealthPoint.LimitGet());
	if (nCurHP < nMaxHP)
	{
		if (!m_bRenewHP)
		{
			if (!m_pTick)
				m_pTick = new CRenewHPTick(const_cast<CFighterDictator*>(pFighter));

			m_pTick->GetFighter()->RegistDistortedTick(m_pTick, RENEW_TIME);
			m_bRenewHP = true;		
		}
	}
	else
	{
		StopRenewHP();
	}
}

void CHealthPointServer::ResetRenewHP(const CFighterDictator* pFighter)
{	
	StopRenewHP();
	
	if (pFighter->m_HealthPoint.LimitGet() < pFighter->m_HealthPoint.Get(pFighter))
	{
		if (!m_pTick)
			m_pTick = new CRenewHPTick(const_cast<CFighterDictator*>(pFighter));

		m_pTick->GetFighter()->RegistDistortedTick(m_pTick, RENEW_TIME);
		m_bRenewHP = true;
	}
}

void CHealthPointServer::StopRenewHP()
{
	if (m_pTick && m_bRenewHP)
	{
		m_pTick->GetFighter()->UnRegistDistortedTick(m_pTick);
		m_bRenewHP = false;
	}
}


void CManaPointServer::Unit()
{
	if(m_pTick)
	{
		m_pTick->GetFighter()->UnRegistDistortedTick(m_pTick);
		delete m_pTick;
		m_pTick = NULL;
	}
}

void CManaPointServer::OnLimitSet(int32 iValue,const CFighterDictator* pFighter)
{
}

void CManaPointServer::OnRenewAgileValue(const CFighterDictator* pFighter)
{
	int32 nMaxMP = int32(pFighter->m_ManaPoint.Get(pFighter));
	int32 nCurMP = int32(pFighter->m_ManaPoint.LimitGet());
	if (nCurMP < nMaxMP)
	{
		if (!m_bRenewMP)
		{
			if (!m_pTick)
				m_pTick = new CRenewMPTick(const_cast<CFighterDictator*>(pFighter));

			m_pTick->GetFighter()->RegistDistortedTick(m_pTick, RENEW_TIME);
			m_bRenewMP = true;		
		}
	}
	else
	{
		StopRenewMP();
	}
}

void CManaPointServer::ResetRenewMP(const CFighterDictator* pFighter)
{
	StopRenewMP();

	if (pFighter->m_ManaPoint.LimitGet() < pFighter->m_ManaPoint.Get(pFighter))
	{
		if (!m_pTick)
			m_pTick = new CRenewMPTick(const_cast<CFighterDictator*>(pFighter));

		m_pTick->GetFighter()->RegistDistortedTick(m_pTick, RENEW_TIME);
		m_bRenewMP = true;
	}
}

void CManaPointServer::StopRenewMP()
{
	if (m_pTick && m_bRenewMP)
	{
		m_pTick->GetFighter()->UnRegistDistortedTick(m_pTick);
		m_bRenewMP = false;
	}
}

void CRagePointServer::Unit()
{
	if(m_pTick)
	{
		m_pTick->GetFighter()->UnRegistDistortedTick(m_pTick);
		delete m_pTick;
		m_pTick = NULL;
	}
}

void CRagePointServer::OnLimitSet(int32 iValue, const CFighterDictator* pFighter)
{
	if (iValue == 100)
	{
		CTriggerEvent::MessageEvent(eTST_Trigger, const_cast<CFighterDictator*>(pFighter), NULL, eSET_RagePointIsFull);
	}

	if (0 < iValue && !pFighter->GetCtrlState(eFCS_InBattle))
	{
		if (!m_bRenewRP)
		{
			if (!m_pTick)
				m_pTick = new CReduceRPTick(const_cast<CFighterDictator*>(pFighter));

			m_pTick->GetFighter()->RegistDistortedTick(m_pTick, RENEW_TIME);
			m_bRenewRP = true;
		}
	}
	else
	{
		StopReduceRP();
	}
}

void CRagePointServer::ResetReduceRP(const CFighterDictator* pFighter)
{
	if (pFighter->Get(&CFighterSyncToDirectorCalInfo::m_RPConsumeRate) == 0.0f)
		return;

	StopReduceRP();

	if (pFighter->m_RagePoint.LimitGet() > 0)
	{
		if (!m_pTick)
			m_pTick = new CReduceRPTick(const_cast<CFighterDictator*>(pFighter));

		m_pTick->GetFighter()->RegistDistortedTick(m_pTick, RENEW_TIME);
		m_bRenewRP = true;
	}
}

void CRagePointServer::StopReduceRP()
{
	if (m_pTick && m_bRenewRP)
	{
		m_pTick->GetFighter()->UnRegistDistortedTick(m_pTick);
		m_bRenewRP = false;
	}
}

void CEnergyPointServer::Unit()
{
	if(m_pTick)
	{
		m_pTick->GetFighter()->UnRegistDistortedTick(m_pTick);
		delete m_pTick;
		m_pTick = NULL;
	}
}

void CEnergyPointServer::OnLimitSet(int32 iValue,const CFighterDictator* pFighter)
{
	if (iValue < int32(pFighter->m_EnergyPoint.Get(pFighter)))
	{
		if (!m_bRenewEP)
		{
			if (!m_pTick)
				m_pTick = new CRenewEPTick(const_cast<CFighterDictator*>(pFighter));

			m_pTick->GetFighter()->RegistDistortedTick(m_pTick, RENEW_TIME);
			m_bRenewEP = true;
		}
	}
	else
	{
		StopRenewEP();
	}
}

void CEnergyPointServer::ResetRenewEP(const CFighterDictator* pFighter)
{
	StopRenewEP();

	if (pFighter->m_EnergyPoint.LimitGet() < pFighter->m_EnergyPoint.Get(pFighter))
	{
		if (!m_pTick)
			m_pTick = new CRenewEPTick(const_cast<CFighterDictator*>(pFighter));

		m_pTick->GetFighter()->RegistDistortedTick(m_pTick, RENEW_TIME);
		m_bRenewEP = true;
	}
}

void CEnergyPointServer::StopRenewEP()
{
	if (m_pTick && m_bRenewEP)
	{
		m_pTick->GetFighter()->UnRegistDistortedTick(m_pTick);
		m_bRenewEP = false;
	}
}
