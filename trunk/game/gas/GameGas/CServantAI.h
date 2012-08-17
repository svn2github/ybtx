#pragma once
#include "CNpcAI.h"

class CServantBackToMaster;
class CCharacterDictator;

class CServantAI
	:public CNpcAI
{
friend class CServantBackToMaster;
public:
	CServantAI(CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType);
	void Follow();
	virtual void OnMasterDestroyed();	//主人死亡或者下线后会调用这个函数，召唤兽做相应的操作
	void StopFollowMaster();
	virtual void Attack(CCharacterDictator* pTarget);	//攻击
	virtual void Retreat();								//撤退
	virtual void Disband();								//解散
	virtual void Spread();								//展开
	virtual bool Move(uint32 x, uint32 y);				//移动
	//专门给战斗坐骑用的，其他地方不要用
	void TellMasterFaceEnemy();

	void BeginChaseOriginTrace();

	void CreateFollowMasterTick();
	void DestoryFollowMasterTick();

	void SetFollowState(bool bFollow);
	bool GetFollowState();
	bool BeInCommandMoveState();
	void MoveToLastMoveCommandPos();
	void CancelCommandMove();
	
	virtual void OnBeAttacked(CCharacterDictator* pAttacker, bool bHurtEffect = true);

public:
	CDistortedTick*						m_pBackToMaster;
private:
	int32						m_uAngle;
	bool						m_bFollowState;
	bool 						m_bCommandMoveState;
	CFPos						m_LastMoveCommanPos;
private:
	void UpdateMasterState(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);	
	void InitServantAttribute(CCharacterDictator* pMaster, CCharacterDictator* pServant);
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent, void* pArg);
	virtual ~CServantAI();
};

