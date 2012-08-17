#pragma once

#include "IObjPosObserverHandler.h"
#include "CNpcAIMallocObject.h"
#include "PatternCOR.h"

class CNpcEnmityMgr;
class CEnmityMgr;
class CNpcChaosCoolDownTick;
class CCharacterDictator;
class CFighterDictator;

namespace sqr
{
	class CDistortedTick;
}


/*
*================================================================================
*						Class : CNpcEnmityMember
*		          Description : Npc³ðºÞ¶ÔÏó
*================================================================================
*/

class CNpcEnmityMember
	: public IWatchHandler
	, public CPtCORHandler
	, public CNpcAIMallocObject
{
friend class CNpcChaosCoolDownTick;
public:
	CNpcEnmityMember(CCharacterDictator* pEntitySelf, CCharacterDictator* pEntityTarget, CEnmityMgr* pEnmityMgr, 
						bool bHaveHurtEffect, float fLockEnemyRangeInPixel);
	~CNpcEnmityMember();
	uint32 GetEnemyEntityID();
	void ReMoveSelfFromEnmityList();
	virtual void OnObserveeEntered();
	virtual void OnObserveeLeft();
	virtual void OnWatched(EWatchResult eResult);
	void AttachAttackMsg();
	void DetachAttackMsg();
	void UpDateCheckTick();
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void CreateChaosTick(uint32 uChaosTime);
	bool CanChangeEnemy();
	bool BeHaveMadeHurtEffect() const;
	void SetHurtEffect(bool bHaveHurtEffect){m_bHaveHurtEffect = bHaveHurtEffect;}
private:
	void NotifyEnemyEnterBattle() ;
	void NotifyEnemyLeaveBattle();
	void EnmityMgrDoAttach();
	void EnmityMgrDoDetach();
private:
	uint32					m_uTargetEntityID;		
	CEnmityMgr*				m_pEnmityMgr;
	CCharacterDictator*		m_pEntitySelf;
	CDistortedTick*			m_pChangeEnemyTick;
	CDistortedTick*			m_pChaosCoolDownTick;
	bool					m_bHaveAttackOnce;
	bool					m_bHaveHurtEffect;
};
