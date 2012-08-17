#include "stdafx.h"
#include "CCfgAllStateCheck.h"
#include "CBaseState.h"
#include "BaseHelper.h"

CCfgAllStateCheck::CCfgAllStateCheck()
:m_strName("")
,m_eType(eBST_State)
,m_bPersistent(false)
,m_bNeedSaveToDB(false)
,m_pCancelEff(NULL)
,m_pDotEff(NULL)
,m_pFinalEff(NULL)
,m_strCascadeType("")
,m_strCascadeMax("")
{
}

CCfgAllStateCheck::~CCfgAllStateCheck()
{
}

void CCfgAllStateCheck::EndCheck()
{
	ClearMap(ms_mapState);
}

bool CCfgAllStateCheck::ExistDecreaseType(const string& sTypeName)
{
	return CBaseStateCfg::ms_mapDecreaseType.find(sTypeName) != CBaseStateCfg::ms_mapDecreaseType.end();
}

bool CCfgAllStateCheck::CheckExistAllState(const string& name)
{
	return ms_mapState.find(name) != ms_mapState.end();
}

CCfgAllStateCheck* CCfgAllStateCheck::GetStateByName(const string& name)
{
	if (ms_mapState.find(name) == ms_mapState.end())
	{
		return NULL;
	}
	return (ms_mapState.find(name))->second;
}

bool CCfgAllStateCheck::IsCancelEffOfState(CCfgMagicEffCheck* pCancelEff)
{
	MapState::iterator it = ms_mapState.begin();
	for (; it != ms_mapState.end(); ++it)
	{
		if (it->second->GetCancelEff() && pCancelEff == it->second->GetCancelEff())
			return true;
	}

	return false;
}
