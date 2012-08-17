#include "stdafx.h"
#include "CMagicOpCfgArg.h"
#include "CCfgCalc.h"
#include "CCfgColChecker.inl"

CPlayerSkillCfgArg::CPlayerSkillCfgArg(CNormalSkillServerSharedPtr* pCfg,uint32 uSkillLevel)
:m_pCfg(pCfg)
,m_uSkillLevel(uSkillLevel)
{
}

CTriggerStateCfgArg::CTriggerStateCfgArg(CTriggerStateCfgServerSharedPtr* pCfg)
:m_pCfg(pCfg)
{
}

CAureMagicCfgArg::CAureMagicCfgArg(CAureMagicCfgServerSharedPtr* pCfg)
:m_pCfg(pCfg)
{
}

CMagicStateCfgArg::CMagicStateCfgArg(CMagicStateCfgServerSharedPtr* pCfg)
:m_pCfg(pCfg)
{
}

CMagicStateCfgMultiArg::CMagicStateCfgMultiArg(CMagicStateCfgServerSharedPtr* pCfg, const string& szArg)
:CMagicStateCfgArg(pCfg)
{
	m_pArg = new CCfgCalc();
	m_pArg->InputString(szArg);
}

CMagicStateCfgMultiArg::~CMagicStateCfgMultiArg()
{
	delete m_pArg;
}

CDamageChangeStateCfgArg::CDamageChangeStateCfgArg(CDamageChangeStateCfgServerSharedPtr* pCfg)
:m_pCfg(pCfg)
{
}

CCumuliTriggerStateCfgArg::CCumuliTriggerStateCfgArg(CCumuliTriggerStateCfgServerSharedPtr* pCfg)
:m_pCfg(pCfg)
{
}

CSpecialStateCfgArg::CSpecialStateCfgArg(CSpecialStateCfgServerSharedPtr* pCfg)
:m_pCfg(pCfg)
{
}

CMoveMagicCfgArg::CMoveMagicCfgArg(CMoveMagicCfgServerSharedPtr* pCfg)
:m_pCfg(pCfg)
{
}

CMoveMagicCfgMultiArg::CMoveMagicCfgMultiArg(CMoveMagicCfgServerSharedPtr* pCfg, const string& szArg)
:m_pCfg(pCfg)
{
	m_pArg = new CCfgCalc();
	m_pArg->InputString(szArg);
}

CMoveMagicCfgMultiArg::~CMoveMagicCfgMultiArg()
{
	delete m_pArg;
}

CTransferableMagicCfgArg::CTransferableMagicCfgArg(CTransferableMagicCfgServerSharedPtr* pCfg)
:m_pCfg(pCfg)
{
}

CBulletMagicCfgArg::CBulletMagicCfgArg(CBulletMagicCfgServerSharedPtr* pCfg)
:m_pCfg(pCfg)
{
}

CPositionMagicCfgArg::CPositionMagicCfgArg(CPositionMagicCfgServerSharedPtr* pCfg, const string& szArg)
:m_pCfg(pCfg)
{
	m_pArg = new CCfgCalc();
	m_pArg->InputString(szArg);
}

CPositionMagicCfgArg::~CPositionMagicCfgArg()
{
	delete m_pArg;
}

CDelayPositionMagicCfgArg::CDelayPositionMagicCfgArg(uint32 uDelayTime, CPositionMagicCfgServerSharedPtr* pCfg)
:m_uDelayTime(uDelayTime)
,m_pCfg(pCfg)
{
}

CDelayPositionMagicCfgArg::~CDelayPositionMagicCfgArg()
{
}

CShockWaveMagicCfgArg::CShockWaveMagicCfgArg(CShockWaveMagicCfgServerSharedPtr* pCfg)
:m_pCfg(pCfg)
{
}

CBarrierMagicCfgArg::CBarrierMagicCfgArg(CBarrierMagicCfgServerSharedPtr* pCfg)
:m_pCfg(pCfg)
{
}

CMagicEffCfgArg::CMagicEffCfgArg(CMagicEffServerSharedPtr* pCfg, const string& szArg)
:m_pCfg(pCfg)
{
	m_pArg = new CCfgCalc();
	m_pArg->InputString(szArg);
}

CMagicEffCfgArg::~CMagicEffCfgArg()
{
	delete m_pArg;
}

CBaseStateCfgArg::CBaseStateCfgArg(CBaseStateCfg* pCfg)
:m_pCfg(pCfg)
{
}

CDisplayMsgCfgArg::CDisplayMsgCfgArg(uint32 uArg1, const string& szArg2)
:m_uArg1(uArg1)
,m_strArg2(szArg2)
{
}


CTwoCfgCalcArg::CTwoCfgCalcArg(const string& szArg1, const string& szArg2)
{
	m_pArg1 = new CCfgCalc();
	m_pArg1->InputString(szArg1);
	if (szArg2 == "")
	{
		m_pArg2 = NULL;
	}
	else
	{
		m_pArg2 = new CCfgCalc();
		m_pArg2->InputString(szArg2);
	}
}

CTwoCfgCalcArg::~CTwoCfgCalcArg()
{
	delete m_pArg1;
	delete m_pArg2;
}

CTwoStringArg::CTwoStringArg(const string& szArg1, const string& szArg2)
:m_strArg1(szArg1),m_strArg2(szArg2)
{
}

CPosDelayCreateNPCArg::CPosDelayCreateNPCArg(uint32 uArg1, const string& szArg2)
:m_uArg1(uArg1)
,m_strArg2(szArg2)
{
}
