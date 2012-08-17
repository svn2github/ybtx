#pragma once
#include "CDistortedTick.h"
#include "CFighterDictator.h"
#include "CFighterMallocObject.h"

class CLeaveBattleTick;

class CBattleStateManager
	:public CFighterMallocObject
{
public:
	CBattleStateManager(CFighterDictator* pFighter);
	~CBattleStateManager();
	
	// 玩家使用 by 玩家触发
	bool EnterBattleStateByPlayer();	// 玩家触发进入战斗状态，玩家出发标志 置true
	void LeaveBattleStateByForce();			// 强制离开战斗状态(如死)，玩家触发标志 和 NPC触发计数 都清

	// 玩家使用 by NPC 触发
	void AddBattleRefByNpc();		// NPC触发进入战斗状态,NPC触发计数+1
	void DelBattleRefByNpc();		// NPC触发离开战斗状态,NPC触发计数-1

	void EnterBattleState();
	void LeaveBattleState();

	void LeaveBattleStateOnTick();
	void LeaveBattleStateByDead();

private:
	bool GetFinalBattleState() const;
	void RegOutBattleTick();
	void UnRegOutBattleTick();
	void NotifyServantEnterBattle();
	void NotifyServantLeaveBattle();
private:
	CFighterDictator*	m_pFighter;
	CLeaveBattleTick*	m_pTick;

	bool				m_bBattleWithPlay;
	int16				m_nBattleStateCount;
};

// 6 秒脱离战斗 Tick
class CLeaveBattleTick 
	: public CDistortedTick
	, public CFighterMallocObject
{
public:
	CLeaveBattleTick(CFighterDictator* pFighter) : m_pFighter(pFighter) {}
	void OnTick();

private:
	CFighterDictator* m_pFighter;
};

