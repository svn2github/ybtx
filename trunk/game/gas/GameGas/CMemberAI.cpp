#include "stdafx.h"
#include "CMemberAI.h"
#include "CDummyAI.h"
#include "CCharacterDictator.h"
#include "CFighterDictator.h"
#include "CDir.h"
#include "CDistortedTick.h"
#include "CoreCommon.h"
#include "CNpcAIMallocObject.h"

class CGridPosChangeTick 
	: public CDistortedTick
	, public CNpcAIMallocObject
{
public:
	CGridPosChangeTick(CMemberAI* pAI) : m_pAI(pAI){}
	void OnTick()
	{
		m_pAI->MemberAutoSynPos(true);
	}
private:
	CMemberAI* m_pAI;
};

CMemberAI::CMemberAI(CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData,const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType)
: CNpcAI(pCharacter, pBaseData, pStateTransit, eRealNpcType, eRealAIType, eObjAoiType)
, m_bMemberReady(true)
,m_bIsBeSynState(false)
{	
	m_pGridPosChangeTick = new CGridPosChangeTick(this);
}	

CMemberAI::~CMemberAI()
{
	CCharacterDictator* pLeader = GetCharacter()->GetGroupLeader();
	if(pLeader)
		pLeader->RemoveGroupMember(GetCharacter());
	GetCharacter()->SetGroupLeader(NULL);
	DeleteNpcAIStateMachine();
	if (m_pGridPosChangeTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pGridPosChangeTick);
		delete m_pGridPosChangeTick;
	}
}

CDummyAI*  CMemberAI::GetGroupLeaderDummyAI()
{
	CCharacterDictator* pLeader = GetCharacter()->GetGroupLeader();
	if (pLeader && pLeader->GetNpcAIHandler()) 
		return pLeader->GetNpcAIHandler()->CastToDummyAI();
	return NULL;
}

void CMemberAI::InitMemberAIDate()
{
	CDummyAI* pDummyAI = GetGroupLeaderDummyAI();
	if (pDummyAI)
	{
		uint uSubDir = pDummyAI->GetCharacter()->GetActionDir();
		if(pDummyAI->GetMovePathPointCount())
			GetCharacter()->SetAndSyncActionDir(uSubDir);
		m_fOldDir = (float)((float)uSubDir/255.0f * TWO_PI);
	}
}

CFPos CMemberAI::GetSelfCorrectPos()
{
	CFPos destPos(0.0f,0.0f);
	CDummyAI* pDummyAI = GetGroupLeaderDummyAI();
	if (!pDummyAI)
		return destPos;
	CFPos dummyPos;
	pDummyAI->GetCharacter()->GetPixelPos(dummyPos);
	uint uSubDir = pDummyAI->GetCharacter()->GetActionDir();
	float newdir = (float)((float)uSubDir/255.0f * TWO_PI);
	
	if(pDummyAI->m_eMoveType == ENpcMove_Trip && !pDummyAI->m_bMoveDir && pDummyAI->m_uCurrentPoint != pDummyAI->m_mapMovePath.size())
		destPos = pDummyAI->ChangeLocalPosToWorldPos(dummyPos,m_LocalMirrorPos,newdir);
	else if(pDummyAI->m_eMoveType == ENpcMove_Trip && pDummyAI->m_bMoveDir && pDummyAI->m_uCurrentPoint == 1)
		destPos = pDummyAI->ChangeLocalPosToWorldPos(dummyPos,m_LocalMirrorPos,newdir);
	else	
		destPos = pDummyAI->ChangeLocalPosToWorldPos(dummyPos,m_LocalPos,newdir);
	return destPos;
}

void CMemberAI::DoAttachLeader()
{
	//cout<<"````NPC ID ="<<GetCharacter()->GetEntityID()<<" 添加速度监视绑定"<<endl;
	CDummyAI* pDummyAI = GetGroupLeaderDummyAI();
	if (!pDummyAI)
		return;
	CCharacterDictator* pLeaderCharacter = pDummyAI->GetCharacter();
	if(!pLeaderCharacter)
		return;
	CFighterDictator* pLeaderFighter = pLeaderCharacter->GetFighter();
	if(!pLeaderFighter)
		return;
	GetCharacter()->RegistDistortedTick(m_pGridPosChangeTick,500);
}
void CMemberAI::DoDetachLeader()
{
	//cout<<"````NPC ID ="<<GetCharacter()->GetEntityID()<<" 解除速度监视绑定"<<endl;
	CDummyAI* pDummyAI = GetGroupLeaderDummyAI();
	if (!pDummyAI)
		return;
	CCharacterDictator* pLeaderCharacter = pDummyAI->GetCharacter();
	if(!pLeaderCharacter)
		return;
	CFighterDictator* pLeaderFighter = pLeaderCharacter->GetFighter();
	if(!pLeaderFighter)
		return;
	GetCharacter()->UnRegistDistortedTick(m_pGridPosChangeTick);
}

void CMemberAI::MemberAutoSynPos(bool bExtraMove)
{
	CDummyAI* pDummyAI = GetGroupLeaderDummyAI();
	if (!pDummyAI)
		return;
	CCharacterDictator* pDummyCharacter = pDummyAI->GetCharacter();
	if (!pDummyCharacter)
		return;
	CFighterDictator* pDummyFighter = pDummyCharacter->GetFighter();
	if(!pDummyFighter)
		return;
	CFPos destPos = GetSelfCorrectPos();
	CFPos ownPos;
	GetCharacter()->GetPixelPos(ownPos);
	bool bWalkState = pDummyAI->m_bDefaultWalkState;
	float fSpeed = bWalkState ?pDummyFighter->m_WalkSpeed.Get(pDummyFighter) : 
		pDummyFighter->m_RunSpeed.Get(pDummyFighter);

	if (ownPos.Dist(destPos)<eGridSpanForObj/2)
	{
		if (bExtraMove)
			pDummyAI->CheckMemberSynState();
		else
			MoveToPixelInLinePath(destPos,fSpeed,bWalkState);
		return;
	}

	CVector2f vecf;
	vecf.x = destPos.x - ownPos.x;
	vecf.y = destPos.y - ownPos.y;
	CDir dir;
	dir.Set(vecf);
	dir.Get(vecf);

	if(bExtraMove)
	{
		float fLineDst = fSpeed*0.5f*eGridSpanForObj;
		destPos.x = destPos.x + fLineDst*vecf.x;
		destPos.y = destPos.y + fLineDst*vecf.y;
	}

	fSpeed += 0.3f;
	MoveToPixelInLinePath(destPos,fSpeed,bWalkState);
}

bool CMemberAI::IsInLockEnemyRange(CFighterDictator* pTarget)
{
	return true;
}

void CMemberAI::SetLocalPosAndMirrorPos(CFPos localPos,CFPos mirrorPos)
{
	m_LocalPos = localPos;
	m_LocalMirrorPos = mirrorPos;
}

void CMemberAI::BeginChaseOriginTrace()
{
}
