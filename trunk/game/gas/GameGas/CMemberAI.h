#pragma once
#include "CNpcAI.h"

class CGridPosChangeTick;

class CMemberAI
	:public CNpcAI
{
public:
	CMemberAI(CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType);
	~CMemberAI();
	virtual ECharacterInnerHandlerType GetType() const { return ECharacterInnerHandlerType_Member;}
	void InitMemberAIDate();			//初始化数据(要在设置了群组master后调用)
	CDummyAI* GetGroupLeaderDummyAI();
	CFPos GetSelfCorrectPos();
	void DoAttachLeader();
	void DoDetachLeader();
	void MemberAutoSynPos(bool bExtraMove = false);
	void SetLocalPosAndMirrorPos(CFPos localPos,CFPos mirrorPos);
	virtual bool IsInLockEnemyRange(CFighterDictator* pTarget);

	virtual void BeginChaseOriginTrace();

public:
	bool				m_bMemberReady;
	bool				m_bIsBeSynState;
	CFPos			m_LocalPos;
	CFPos			m_LocalMirrorPos;
	float				m_fOldDir;
	CGridPosChangeTick*			m_pGridPosChangeTick;
};
