#include "stdafx.h"
#include "CStateDBData.h"
#include "CStateDBData.inl"
#include "TSqrAllocator.inl"

CStateDBData::CStateDBData(EStateGlobalType eType, 
						   const TCHAR* sName, int32 iTime, int32 iRemainTime, int32 iCount, float fProbability,
						   uint32 uCancelableInstID, uint32 uDotInstID, uint32 uFinalInstID,
						   uint8 uSkillLevel, const TCHAR* sSkillName,
						   EAttackType eAttackType, bool bIsDot, bool bFromEqualsOwner)
						   :
m_eType(eType),
m_iTime(iTime),
m_iRemainTime(iRemainTime),
m_iCount(iCount),
m_fProbability(fProbability),
m_uCancelableInstID(uCancelableInstID),
m_uDotInstID(uDotInstID),
m_uFinalInstID(uFinalInstID),
m_uSkillLevel(uSkillLevel),
m_eAttackType(eAttackType),
m_bIsDot(bIsDot),
m_bFromEqualsOwner(bFromEqualsOwner)
{
	//m_sName = new TCHAR[256];
	//m_sSkillName = new TCHAR[256];
	//memcpy(m_sName, sName, sizeof(TCHAR) * 256);
	//memcpy(m_sSkillName, sSkillName,  sizeof(TCHAR) * 256);
	//m_sName = const_cast<TCHAR*>(sName);				//我保证不改成员的值我保证不改成员的值我保证不改成员的值- -
	//m_sSkillName = const_cast<TCHAR*>(sSkillName);	//我保证不改成员的值我保证不改成员的值我保证不改成员的值- -
	m_sName = sName;
	m_sSkillName = sSkillName;
}



CStateDBDataSet::CStateDBDataSet()
{
	m_pStateVec = new CStateDBDataVec;
	m_pStoredObjVec = new CStoredObjDBDataVec;
}
void CStateDBDataSet::Release()
{
	if(m_pStateVec) {delete m_pStateVec; m_pStateVec = NULL;}
	if(m_pStoredObjVec) {delete m_pStoredObjVec; m_pStoredObjVec = NULL; }
}
CStateDBDataSet::~CStateDBDataSet()
{
	Release();
}




CSkillCoolDownDBDataMgr::CSkillCoolDownDBDataMgr()
{
	m_pSkillCoolDownVec = new CSkillCoolDownDBDataVec;
}

void CSkillCoolDownDBDataMgr::Release()
{
	if (m_pSkillCoolDownVec){delete m_pSkillCoolDownVec; m_pSkillCoolDownVec = NULL;}
}

CSkillCoolDownDBDataMgr::~CSkillCoolDownDBDataMgr()
{
	Release();
}



CAureMagicDBDataMgr::CAureMagicDBDataMgr()
{
	m_pAureMagicVec = new CAureMagicDBDataVec;
}

CAureMagicDBDataMgr::~CAureMagicDBDataMgr()
{
	if(m_pAureMagicVec)
	{
		delete m_pAureMagicVec;
		m_pAureMagicVec = NULL;
	}
}
