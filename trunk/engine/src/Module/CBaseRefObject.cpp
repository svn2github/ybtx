#include "stdafx.h"
#include "CBaseRefObject.h"

CBaseRefObject::CBaseRefObject()
: m_nScriptRefNum(-1)
, m_uFlag(0)
{

}

CBaseRefObject::~CBaseRefObject()
{
	
}

void CBaseRefObject::SetObjectFlag(uint8 uFlagBit)
{
	m_uFlag = m_uFlag | uFlagBit;
}

bool CBaseRefObject::VisibleToScript()const
{
	return (m_uFlag & eBOSF_VisableToScript) != 0 ;
}

bool CBaseRefObject::CreatedByScript()const
{
	return (m_uFlag & eBOSF_CreatedByScript) != 0 ;
}

void CBaseRefObject::SetScriptRefNum(int32 num)
{
	m_nScriptRefNum = num;
}

bool CBaseRefObject::RefByScript() const
{
	return m_nScriptRefNum != -1;
}

int32 CBaseRefObject::GetScriptRefNum() const 
{
	return m_nScriptRefNum;
}

