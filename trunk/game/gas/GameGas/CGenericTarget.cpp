#include "stdafx.h"
#include "CGenericTarget.h"
#include "CFighterDictator.h"
#include "CEntityServerManager.h"
#include "CoreCommon.h"

CGenericTarget::CGenericTarget(const CFPos& pos)
:m_eTargetType(eTT_Position)
,m_uEntityID(0)
{
	SetPos(pos);
}

CGenericTarget::CGenericTarget(CFighterDictator* pFighter)
:m_eTargetType(eTT_Fighter) 
{
	SetFighter(pFighter);
}

CGenericTarget::CGenericTarget()
:m_eTargetType(eTT_None)
, m_uEntityID(0) 
{

}

void CGenericTarget::SetPos(const CFPos& pos) 
{ 
	m_Pos.x = max(pos.x,0.0f); 
	m_Pos.y = max(pos.y,0.0f); 
	PixelPos2GridPos(m_GridPos,m_Pos);
	m_eTargetType = eTT_Position;
}
void CGenericTarget::SetFighter(CFighterDictator* pFighter) 
{ 
	m_uEntityID = pFighter?pFighter->GetEntityID():0; 
	m_eTargetType = eTT_Fighter;
}

CFighterDictator*	CGenericTarget::GetFighter() const					
{ 
	CEntityServer* pEntity = CEntityServerManager::GetInst()->GetEntityByID(m_uEntityID);
	CFighterDictator*	pFighter = pEntity?pEntity->GetFighter():NULL;
	return pFighter; 
}

const CFPos& CGenericTarget::GetPos()const
{
	return m_Pos;
}

const CPos& CGenericTarget::GetGridPos()const
{
	return m_GridPos;
}
