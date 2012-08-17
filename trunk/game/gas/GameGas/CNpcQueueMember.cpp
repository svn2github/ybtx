#include "stdafx.h"
#include "CNpcQueueMember.h"

CNpcQueueMember::CNpcQueueMember(int32 uAngle, uint32 uEntityID)
:m_uAngle(uAngle)
,m_uEntityID(uEntityID)
{	
}

uint32 CNpcQueueMember::GetEntityID()
{
	return m_uEntityID;
}

void CNpcQueueMember::SetEntityID(uint32 uEntityID)
{
	m_uEntityID = uEntityID;
}
 
int32 CNpcQueueMember::GetAngle()
{
	return m_uAngle;
}

