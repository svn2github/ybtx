#include "stdafx.h"
#include "BaseMath.h"
#include "CFighterBaseInfo.h"

CFighterBaseInfo::CFighterBaseInfo()
:m_uLevel(1)
,m_eClass(eCL_Soldier)
,m_eBirthCamp(eCamp_Monster)
,m_eCamp(eCamp_Monster)
,m_iGameCamp(0)
,m_bPKState(false)
//,m_ePKState(ePKS_Safety)
//,m_eZoneType(eZT_WarZone)
{
}

EClass CFighterBaseInfo::CppGetClass()const
{
	return m_eClass;
}

void CFighterBaseInfo::SetClass(EClass eClass)
{
	m_eClass=eClass;
}

uint32 CFighterBaseInfo::CppGetLevel()const
{
	return m_uLevel;
}

void CFighterBaseInfo::SetLevel(uint32 uLevel)
{
	m_uLevel=limit2(uLevel,uint32(1),uint32(eMaxLevel));
}

ECamp CFighterBaseInfo::CppGetBirthCamp()const
{
	return m_eBirthCamp;
}
void CFighterBaseInfo::SetBirthCamp(ECamp eCamp)
{
	m_eBirthCamp = eCamp;
}

ECamp CFighterBaseInfo::CppGetCamp()const
{
	return m_eCamp;
}

void CFighterBaseInfo::SetCamp(ECamp eCamp)
{
	m_eCamp = eCamp;
}

int32 CFighterBaseInfo::CppGetGameCamp() const
{
	return m_iGameCamp;
}

void CFighterBaseInfo::SetGameCamp(int32 iGameCamp)
{
	m_iGameCamp = iGameCamp;
}

bool CFighterBaseInfo::CppGetPKState() const
{
	return m_bPKState;
}

void CFighterBaseInfo::SetPKState(bool bPKState)
{
	m_bPKState = bPKState;
}

//EPKState CFighterBaseInfo::CppGetPKState() const
//{
//	return m_ePKState;
//}
//
//void CFighterBaseInfo::SetPKState(EPKState ePKState)
//{
//	m_ePKState = ePKState;
//}
//
//EZoneType CFighterBaseInfo::CppGetZoneType() const
//{
//	return m_eZoneType;
//}
//
//void CFighterBaseInfo::SetZoneType(EZoneType eZoneType)
//{
//	m_eZoneType = eZoneType;
//}
