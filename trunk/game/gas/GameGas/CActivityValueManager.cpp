#include "stdafx.h"
#include "CActivityValueManager.h"

CActivityValueManager::CActivityValueManager()
:m_uHPCount(0)
{
	
}

CActivityValueManager::~CActivityValueManager()
{

}

void CActivityValueManager::ChangeTargetHP(int32 uHP)
{
	if (uHP>=0)
	{
		m_uHPCount +=uHP;
	} 
	else
	{
		m_uHPCount +=-1*uHP;
	}
}

void CActivityValueManager::ClearActivityValue()
{
	m_uHPCount=0;
}

uint32 CActivityValueManager::GetActivityValue()
{
	return m_uHPCount;
}

