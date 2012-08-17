#include "stdafx.h"
#include "CStateCondFunctor.h"
#include "CBaseStateServer.h"

bool CStateCondForIcon::operator()(CBaseStateServer* pState) const
{
	CBaseStateCfgServer* pCfg = pState->GetCfg();
	switch(pCfg->GetIconCancelCond())
	{
	case eICC_All:
		return true;
	case eICC_Self:
		if(pState->GetInstaller() == pState->GetOwner()) return true;
		else return false;
	case eICC_None:
		return false;
	default:
		break;
	}
	return false;
}

bool CStateCondForDecreaseType::operator()(CBaseStateServer* pState) const
{
	if(m_iCount == 0) return false;
	CBaseStateCfgServer* pCfg = pState->GetCfg();
	if((m_bDelEqual && pCfg->GetDecreaseType() == m_eDecreaseType || !m_bDelEqual && pCfg->GetDecreaseType() != m_eDecreaseType)
		&& !pCfg->GetPersistent())
	{
		if(m_iCount > 0) --m_iCount;
		return true;
	}
	else
	{
		return false;
	}
}

bool CStateCondForDispellableDecreaseType::operator()(CBaseStateServer* pState) const
{
	if(m_iCount == 0) return false;
	CBaseStateCfgServer* pCfg = pState->GetCfg();
	if((m_bDelEqual && pCfg->GetDecreaseType() == m_eDecreaseType || !m_bDelEqual && pCfg->GetDecreaseType() != m_eDecreaseType)
		&& pCfg->GetDispellable()
		&& !pCfg->GetPersistent())
	{
		if(m_iCount > 0) --m_iCount;
		return true;
	}
	else
	{
		return false;
	}
}

bool CStateCondForAllDecrease::operator()(CBaseStateServer* pState) const
{
	if(m_iCount == 0) return false;
	CBaseStateCfgServer* pCfg = pState->GetCfg();
	if(pCfg->GetDecreaseType() >= eDST_Control
		&& !pCfg->GetPersistent())
	{
		if(m_iCount > 0) --m_iCount;
		return true;
	}
	else
	{
		return false;
	}
}

bool CStateCondForDispellableAllDecrease::operator()(CBaseStateServer* pState) const
{
	if(m_iCount == 0) return false;
	CBaseStateCfgServer* pCfg = pState->GetCfg();
	if(pCfg->GetDecreaseType() >= eDST_Control
		&& pCfg->GetDispellable()
		&& !pCfg->GetPersistent())
	{
		if(m_iCount > 0) --m_iCount;
		return true;
	}
	else
	{
		return false;
	}
}

bool CStateCondForNonPersistent::operator()(CBaseStateServer* pState) const
{
	return !pState->GetCfg()->GetPersistent();
}

bool CStateCondForSelfState::operator()(CBaseStateServer* pState) const
{
	return pState->GetInstaller() == m_pOwner;
}

bool CStateCondForCountDispellableAllDecrease::operator()(CBaseStateServer* pState) const
{
	if(m_iCount == 0) return false;
	CBaseStateCfgServer* pCfg = pState->GetCfg();
	if(pCfg->GetDecreaseType() >= eDST_Control
		&& pCfg->GetDispellable()
		&& !pCfg->GetPersistent())
	{
		++m_iAllCount;
	}
	return false;
}

bool CStateCondForRandDispellableAllDecrease::operator()(CBaseStateServer* pState) const
{
	if(m_iCount == 0) return false;
	CBaseStateCfgServer* pCfg = pState->GetCfg();
	if(pCfg->GetDecreaseType() >= eDST_Control
		&& pCfg->GetDispellable()
		&& !pCfg->GetPersistent())
	{
		if(m_iCount >= m_iAllCount || (float)rand()/(float)(RAND_MAX + 1) < (float)m_iCount / (float)m_iAllCount)
		{
			if(m_iCount > 0) --m_iCount;
			if(m_iAllCount > 0) --m_iAllCount;
			return true;
		}
		else
		{
			if(m_iAllCount > 0) --m_iAllCount;
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CStateCondForCountDispellableDecreaseType::operator()(CBaseStateServer* pState) const
{
	if(m_iCount == 0) return false;
	CBaseStateCfgServer* pCfg = pState->GetCfg();
	if((m_bDelEqual && pCfg->GetDecreaseType() == m_eDecreaseType || !m_bDelEqual && pCfg->GetDecreaseType() != m_eDecreaseType)
		&& pCfg->GetDispellable()
		&& !pCfg->GetPersistent())
	{
		++m_iAllCount;
	}
	return false;
}

bool CStateCondForRandDispellableDecreaseType::operator()(CBaseStateServer* pState) const
{
	if(m_iCount == 0) return false;
	CBaseStateCfgServer* pCfg = pState->GetCfg();
	if((m_bDelEqual && pCfg->GetDecreaseType() == m_eDecreaseType || !m_bDelEqual && pCfg->GetDecreaseType() != m_eDecreaseType)
		&& pCfg->GetDispellable()
		&& !pCfg->GetPersistent())
	{
		//float fRand =  (float)rand();
		//cout << "fRand = " << fRand << ",  ratio = " << fRand /(float)(RAND_MAX + 1) << endl;
		if(m_iCount >= m_iAllCount || (float)rand() /(float)(RAND_MAX + 1) < (float)m_iCount / (float)m_iAllCount)
		{
			if(m_iCount > 0) --m_iCount;
			if(m_iAllCount > 0) --m_iAllCount;
			return true;
		}
		else
		{
			if(m_iAllCount > 0) --m_iAllCount;
			return false;
		}
	}
	else
	{
		return false;
	}
}
