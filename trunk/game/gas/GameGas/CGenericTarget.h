#pragma once
#include "FightDef.h"
#include "BaseTypes.h"
#include "CPos.h"
#include "CFighterMallocObject.h"

class CFighterDictator;
class CEntityServer;

class CGenericTarget 
	: public CFighterMallocObject
{
public:
	CGenericTarget(const CFPos& pos);
	CGenericTarget(CFighterDictator* pFighter);
	CGenericTarget();
	void SetPos(const CFPos& pos);
	void SetFighter(CFighterDictator* pFighter);
	ETargetType			GetType() const						{ return m_eTargetType; }
	CFighterDictator*	GetFighter() const;
	uint32 GetID()const { return m_uEntityID;}
	const CFPos&		GetPos() const;
	const CPos&		GetGridPos() const;

private:
	ETargetType			m_eTargetType;	
	uint32					m_uEntityID;
	CFPos					m_Pos;
	CPos						m_GridPos;
};

