#include "stdafx.h"
#include "CDummyAI.h"
#include "CMemberAI.h"
#include "CNpcGroupMgr.h"
#include "CCharacterDictator.h"
#include "CFighterDictator.h"
#include "CDummyEnmityMgr.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "CMath.h"

CDummyAI::CDummyAI(CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType)
:CNpcAI(pCharacter, pBaseData, pStateTransit, eRealNpcType, eRealAIType, eObjAoiType)
,m_bIsWaitForAdjustFinish(false)
,m_bIsInitMemberPath(false)
,m_bIsMoveToFirstPoint(true)
{	
}	

CDummyAI::~CDummyAI()
{
	ClearAllEnmity();
	CCharacterDictator* pOwnCharacter = GetCharacter();
	if(pOwnCharacter)
		pOwnCharacter->RemoveAllGroupMember();
	for(MemberPathMapByID::iterator iter = m_mapMemberPathByID.begin(); iter != m_mapMemberPathByID.end(); iter++)
	{
		MemPathPointData *pMemData = (*iter).second;
		for (MemPathMapType::iterator iter = pMemData->m_mapMovePath.begin(); iter != pMemData->m_mapMovePath.end();iter++)
		{
			SafeDelete((*iter).second);
		}
		for (MemPathMapType::iterator iter = pMemData->m_mapRevesePointPath.begin(); iter != pMemData->m_mapRevesePointPath.end();iter++)
		{
			SafeDelete((*iter).second);
		}
		SafeDelete(pMemData);
	}
	DeleteNpcAIStateMachine();
}

CFPos CDummyAI::CalculateDistancePosToDest(CFPos beginPos,CFPos endPos,float fDistance)
{
	float fAngle = CalculateAngle(beginPos.x,beginPos.y,endPos.x,endPos.y);
	CFPos resultPos;
	if(abs(fAngle-(float)HALF_PI)<0.031f)
	{
		if(endPos.x>beginPos.x)
		{
			resultPos.x = endPos.x - fDistance;
			resultPos.y = endPos.y;
			return resultPos;
		}
		else
		{
			resultPos.x = endPos.x + fDistance;
			resultPos.y = endPos.y;
			return resultPos;
		}
	}
	else if(abs(fAngle-(float)PI)<0.031f || abs(fAngle)<0.031f || abs(fAngle - (float)2 * (float)PI)<0.031f )
	{
		if(endPos.y>beginPos.y)
		{
			resultPos.x = endPos.x;
			resultPos.y = endPos.y - fDistance;
			return resultPos;
		}
		else
		{
			resultPos.x = endPos.x;
			resultPos.y = endPos.y - fDistance;
			return resultPos;
		}
	}
	else
	{
		if(fAngle>0.0f && fAngle< (float)HALF_PI)
		{
			resultPos.x = endPos.x - fDistance*CMath::Sin(fAngle);
			resultPos.y = endPos.y - fDistance*CMath::Cos(fAngle);
			return resultPos;
		}
		else if(fAngle>(float)HALF_PI && fAngle< (float)PI)
		{
			resultPos.x = endPos.x - fDistance*CMath::Cos(fAngle);
			resultPos.y = endPos.y +  fDistance*CMath::Sin(fAngle);
			return resultPos;
		}
		else if(fAngle> (float)PI && fAngle< (float)3*(float)HALF_PI)
		{
			resultPos.x = endPos.x + fDistance*CMath::Cos(fAngle);
			resultPos.y = endPos.y +  fDistance*CMath::Sin(fAngle);
			return resultPos;
		}
		else 
		{
			resultPos.x = endPos.x + fDistance*CMath::Cos(fAngle);
			resultPos.y = endPos.y -  fDistance*CMath::Sin(fAngle);
			return resultPos;
		}
	}
}

float CDummyAI::CalculateAngle(float x1,float y1,float x2,float y2)
{
	float newdir; 
	if(abs(x1 - x2)>=10.0f && abs(y1 - y2)>=10.0f)
	{
		newdir= atan((x1 - x2)/(y1 - y2));
		if (x1 > x2)
		{
			if(y1 > y2)
				newdir += (float)PI;
			else
				newdir += (float)2*(float)PI;
		}
		else
		{
			if(y1 > y2)
				newdir += (float)PI;
		}
	}
	else
	{
		if(abs(x1 - x2)<10.0f)
			if(y1 > y2)
				newdir = (float)PI;
			else
				newdir = 0.0f;
		else
			if(x1 > x2)
				newdir = (float)3*(float)HALF_PI;
			else
				newdir = (float)HALF_PI;
	}
	return newdir;
}


void CDummyAI::CalculateMemberLocalPos()
{
	const MemberIDMapType& MemberMap = GetCharacter()->GetGroupMgr()->GetMemberIDMapType();
	MemberIDMapType::const_iterator it = MemberMap.begin();
	for(;it!=MemberMap.end();it++)
	{
		CMemberAI* pMemberAI = (*it).second->GetNpcAIHandler()->CastToMemberAI();
		if (pMemberAI)
		{
			CFPos worldPos(pMemberAI->m_InitPos.x - m_InitPos.x, pMemberAI->m_InitPos.y - m_InitPos.y);
			CFPos localPos;
			float fAngle = pMemberAI->m_fOldDir;
			localPos.x = worldPos.x * CMath::Cos(fAngle) + worldPos.y * (-CMath::Sin(fAngle));
			localPos.y = worldPos.x * CMath::Sin(fAngle)  + worldPos.y * CMath::Cos(fAngle);
			pMemberAI->m_LocalPos = localPos;
			CFPos localMirrorPos;
			localMirrorPos.x = -pMemberAI->m_LocalPos.x;
			localMirrorPos.y = pMemberAI->m_LocalPos.y;
			pMemberAI->m_LocalMirrorPos = localMirrorPos;
		}
	}
}
CFPos CDummyAI::ChangeLocalPosToWorldPos(CFPos originPos,CFPos localPos,float fAngle)
{
	CFPos worldPos;
	worldPos.x = localPos.x * CMath::Cos(fAngle) + localPos.y* CMath::Sin(fAngle) + originPos.x;
	worldPos.y = localPos.x * (-CMath::Sin(fAngle)) + localPos.y*CMath::Cos(fAngle) + originPos.y;
	return worldPos;
}

CFPos CDummyAI::GetMemberPathPoint(uint32 uMemID, uint8 uMovePoint,bool bMoveDir)
{
	MemPathPointData *pMemPathData = m_mapMemberPathByID[uMemID];
	CFPos resultPos;
	if(bMoveDir)
		resultPos = *pMemPathData->m_mapMovePath[uMovePoint];
	else
		resultPos = *pMemPathData->m_mapRevesePointPath[uMovePoint];
	return resultPos;
}

CFPos CDummyAI::GetMemberFirstDstPos(uint32 uMemID)
{
	MemPathPointData *pMemPathData = m_mapMemberPathByID[uMemID];
	CFPos resultPos;
	resultPos = pMemPathData->m_onceDstPos;
	return resultPos;
}


void CDummyAI::CreatePatrolPath()
{
	if(m_bIsInitMemberPath)
		return;
	if (!GetCharacter()->GetGroupMgr())
		return;
	CalculateMemberLocalPos();
	const MemberIDMapType& MemberMap = GetCharacter()->GetGroupMgr()->GetMemberIDMapType();
	MemberIDMapType::const_iterator it = MemberMap.begin();
	for(;it!=MemberMap.end();it++)
	{
		CMemberAI* pMemberAI = (*it).second->GetNpcAIHandler()->CastToMemberAI();
		if (pMemberAI)
		{
			MemPathPointData *pMemPathData = new MemPathPointData;
			pMemPathData->m_LocalPos = pMemberAI->m_LocalPos;
			pMemPathData->m_LocalMirrorPos = pMemberAI->m_LocalMirrorPos;

			if(GetMovePathPointCount() == 0)
			{
				uint32 uMemGroupID = pMemberAI->m_uGroupSelfID;
				m_mapMemberPathByID.insert(make_pair(uMemGroupID,pMemPathData));		
				continue;
			}

			CFPos leaderPos;
			GetCharacter()->GetPixelPos(leaderPos);
			float bornDir = CalculateAngle(leaderPos.x,leaderPos.y, m_mapMovePath[1]->x,m_mapMovePath[1]->y);
			CFPos bornPos = ChangeLocalPosToWorldPos(*m_mapMovePath[1],pMemberAI->m_LocalPos,bornDir);
			pMemPathData->m_onceDstPos = bornPos;
			if(m_eMoveType == ENpcMove_Trip)
			{
				for(uint8 i = 1;i<=m_mapMovePath.size();i++)
				{
					uint8 BeginIndex = i==1 ? i:(i-1);
					uint8 EndIndex	 = i==1 ? (i+1):i;
					float newdir = CalculateAngle(m_mapMovePath[BeginIndex]->x,m_mapMovePath[BeginIndex]->y,m_mapMovePath[EndIndex]->x,m_mapMovePath[EndIndex]->y); 
					CFPos resultPos = ChangeLocalPosToWorldPos(*m_mapMovePath[i],pMemberAI->m_LocalPos,newdir);
					CFPos *pPos = new CFPos(resultPos.x, resultPos.y);
					pMemPathData->m_mapMovePath.insert(make_pair(i, pPos));

					//返程线路
					BeginIndex = i==m_mapMovePath.size() ? i:(i+1);
					EndIndex = i==m_mapMovePath.size() ? (i-1):i;
					float backdir = CalculateAngle(m_mapMovePath[BeginIndex]->x,m_mapMovePath[BeginIndex]->y,m_mapMovePath[EndIndex]->x,m_mapMovePath[EndIndex]->y);
					CFPos backresultPos = ChangeLocalPosToWorldPos(*m_mapMovePath[i],pMemberAI->m_LocalMirrorPos,backdir);
					CFPos *pBackPos = new CFPos(backresultPos.x, backresultPos.y);
					pMemPathData->m_mapRevesePointPath.insert(make_pair(i, pBackPos));
				}
				uint32 uMemGroupID = pMemberAI->m_uGroupSelfID;
				m_mapMemberPathByID.insert(make_pair(uMemGroupID,pMemPathData));			
				
			}
			else if(m_eMoveType == ENpcMove_Cycle)
			{
				for(uint8 i = 1;i<=m_mapMovePath.size();i++)
				{
					uint8 BeforeIndex = i==1?m_mapMovePath.size():i-1;
					float newdir = CalculateAngle(m_mapMovePath[BeforeIndex]->x,m_mapMovePath[BeforeIndex]->y, m_mapMovePath[i]->x,m_mapMovePath[i]->y);
					CFPos resultPos = ChangeLocalPosToWorldPos(*m_mapMovePath[i],pMemberAI->m_LocalPos,newdir);
					CFPos *pPos = new CFPos(resultPos.x, resultPos.y);
					pMemPathData->m_mapMovePath.insert(make_pair(i, pPos));
				}
				uint32 uMemGroupID = pMemberAI->m_uGroupSelfID;
				m_mapMemberPathByID.insert(make_pair(uMemGroupID,pMemPathData));		
			}
		}
	}
	m_bIsInitMemberPath = true;
}

void CDummyAI::NotifyEventToMember(ENpcEvent eNpcEvent, ENpcEventCodeIndex eNpcEventCodeIndex)
{
	if (!GetCharacter()->GetGroupMgr())
		return;
	const MemberIDMapType& MemberMap = GetCharacter()->GetGroupMgr()->GetMemberIDMapType();
	MemberIDMapType::const_iterator it = MemberMap.begin();
	for(;it!=MemberMap.end();it++)
	{
		CMemberAI* pMemberAI = (*it).second->GetNpcAIHandler()->CastToMemberAI();
		if (pMemberAI)
			CNpcEventMsg::Create(pMemberAI, eNpcEvent, eNpcEventCodeIndex);
	}
}

void CDummyAI::MoveToDest(float x,float y)
{
	CNpcAI::MoveToDest(x, y);
	return;
}

void CDummyAI::OnMoveToPathPoint(uint8 point)
{
	if(GetMovePathPointCount() == 0)
		CNpcEventMsg::Create(this, eNpcEvent_OnMoveToDest);
	else
		CNpcAI::OnMoveToPathPoint(point);
}

void CDummyAI::SetMemberSynState(bool synState)
{
	if (!GetCharacter()->GetGroupMgr())
		return;
	const MemberIDMapType& MemberMap = GetCharacter()->GetGroupMgr()->GetMemberIDMapType();
	MemberIDMapType::const_iterator it = MemberMap.begin();
	for(;it!=MemberMap.end();it++)
	{
		CMemberAI* pMemberAI = (*it).second->GetNpcAIHandler()->CastToMemberAI();
		if(synState)
		{
			if(pMemberAI->m_bIsBeSynState)
				continue;
			pMemberAI->DoAttachLeader();
			pMemberAI->m_bIsBeSynState = true;
		}
		else
		{
			if(!pMemberAI->m_bIsBeSynState)
				continue;
			pMemberAI->DoDetachLeader();
			pMemberAI->m_bIsBeSynState = false;
		}
	}
}

bool CDummyAI::CheckMemberSynState()
{
	if (!GetCharacter()->GetGroupMgr())
		return true;
	CCharacterDictator* pOwnCharacter = GetCharacter();
	Ast(pOwnCharacter);
	CFighterDictator* pOwnFighter = pOwnCharacter->GetFighter();
	Ast(pOwnFighter);
	float fSpeed = m_bDefaultWalkState ?pOwnFighter->m_WalkSpeed.Get(pOwnFighter) : 
		pOwnFighter->m_RunSpeed.Get(pOwnFighter);
	bool bAllSynOK = true;
	const MemberIDMapType& MemberMap = GetCharacter()->GetGroupMgr()->GetMemberIDMapType();
	MemberIDMapType::const_iterator it = MemberMap.begin();
	for(;it!=MemberMap.end();it++)
	{
		CMemberAI* pMemberAI = (*it).second->GetNpcAIHandler()->CastToMemberAI();
		if(pMemberAI && pMemberAI->m_bIsBeSynState)
		{
			CCharacterDictator* pMemberCharacter = pMemberAI->GetCharacter();
			CFPos memberPos, memberDestPos;
			pMemberCharacter->GetPixelPos(memberPos);
			memberDestPos = pMemberAI->GetSelfCorrectPos();
			if(memberPos.Dist(memberDestPos)<=64.0f)
			{
				pMemberAI->m_bIsBeSynState = false;
				pMemberAI->DoDetachLeader();
				uint32 uMemGroupID = pMemberAI->m_uGroupSelfID;
				CFPos pos;
				if (m_bIsMoveToFirstPoint && m_uCurrentPoint == 1)
					pos = GetMemberFirstDstPos(uMemGroupID);
				else
				{
					if(m_eMoveType == ENpcMove_Trip)
					{
						if(m_uCurrentPoint == m_mapMovePath.size() || m_uCurrentPoint == 1)
							pos = GetMemberPathPoint(uMemGroupID,m_uCurrentPoint,false);
						else
							pos = GetMemberPathPoint(uMemGroupID,m_uCurrentPoint,m_bMoveDir);
					}
					else
						pos = GetMemberPathPoint(uMemGroupID,m_uCurrentPoint,true);
				}
				//cout<<"``````NPC ID = "<<pMemberAI->GetCharacter()->GetEntityID()<<" 定时器检测距离OK，速度设置回正常1"<<endl;
				pMemberAI->MoveToPixelInLinePath(pos,fSpeed,m_bDefaultWalkState);
			}
			else
				bAllSynOK = false;
		}
	}
	return bAllSynOK;
}

void CDummyAI::NotifyMemberBeAttacked(CCharacterDictator* pAttacker)
{
	if (!GetCharacter()->GetGroupMgr())
		return;
	if(!pAttacker)
		return;
	uint32 uEnemyID = pAttacker->GetEntityID();
	CEnmityMgr* pEnmityMgr = GetEnmityMgr();
	if(pEnmityMgr->IsInEnmityList(uEnemyID))
		return;
	if (pEnmityMgr->AddEnemy(pAttacker, false))
	{
		CNpcEventMsg::Create(this, eNpcEvent_OnGroupMemberBeAttacked,eNECI_Zero,reinterpret_cast<void*>(uEnemyID));
		const MemberIDMapType& MemberMap = GetCharacter()->GetGroupMgr()->GetMemberIDMapType();
		MemberIDMapType::const_iterator it = MemberMap.begin();
		for(;it!=MemberMap.end();it++)
		{
			CMemberAI* pMemberAI = (*it).second->GetNpcAIHandler()->CastToMemberAI();
			if(pMemberAI)
				pMemberAI->OnBeAttacked(pAttacker, false);
		}
	}
}

void CDummyAI::NotifyMemberRemoveEnemy(CCharacterDictator* pAttacker)
{
	if (!GetCharacter()->GetGroupMgr())
		return;
	const MemberIDMapType& MemberMap = GetCharacter()->GetGroupMgr()->GetMemberIDMapType();
	MemberIDMapType::const_iterator it = MemberMap.begin();
	for(;it!=MemberMap.end();it++)
	{
		CMemberAI* pMemberAI = (*it).second->GetNpcAIHandler()->CastToMemberAI();
		if(pMemberAI)
		{
			CEnmityMgr* pEnmityMgr = pMemberAI->GetEnmityMgr();
			pEnmityMgr->RemoveEnemy(pAttacker);
		}
	}
}

void CDummyAI::NotifyClearAllEnmity()
{
	if (!GetCharacter()->GetGroupMgr())
		return;
	const MemberIDMapType& MemberMap = GetCharacter()->GetGroupMgr()->GetMemberIDMapType();
	MemberIDMapType::const_iterator it = MemberMap.begin();
	for(;it!=MemberMap.end();it++)
	{
		CMemberAI* pMemberAI = (*it).second->GetNpcAIHandler()->CastToMemberAI();
		if(pMemberAI)
		{
			CEnmityMgr* pEnmityMgr = pMemberAI->GetEnmityMgr();
			if (!pEnmityMgr->IsEnemyListEmpty())
			{
				pEnmityMgr->ClearAllEnmity();
			}
		}
	}
}

CFPos CDummyAI::GetMemberReBornPosition(uint32 uMemID)
{
	MemPathPointData* pMemPathData = m_mapMemberPathByID[uMemID];
	CFPos rebornPos,dummyPos;
	GetCharacter()->GetPixelPos(dummyPos);
	uint uSubDir = GetCharacter()->GetActionDir();
	float newdir = (float)((float)uSubDir/255.0f * TWO_PI);
	if(m_eMoveType == ENpcMove_Trip && !m_bMoveDir && m_uCurrentPoint != m_mapMovePath.size())
		rebornPos = ChangeLocalPosToWorldPos(dummyPos,pMemPathData->m_LocalMirrorPos,newdir);
	else if(m_eMoveType == ENpcMove_Trip && m_bMoveDir && m_uCurrentPoint == 1)
		rebornPos = ChangeLocalPosToWorldPos(dummyPos,pMemPathData->m_LocalMirrorPos,newdir);
	else	
		rebornPos = ChangeLocalPosToWorldPos(dummyPos,pMemPathData->m_LocalPos,newdir);
	return rebornPos;
}

bool CDummyAI::IsInitMemberPathPoint(uint32 uMemID)
{
	MemberPathMapByID::iterator iter = m_mapMemberPathByID.find(uMemID);
	if(iter == m_mapMemberPathByID.end())
		return false;
	return true;

}

bool CDummyAI::MemberCanFight(CCharacterDictator* pTargetChar)
{
	if (!GetCharacter()->GetGroupMgr())
	{
		LogErr("群组Leader的GroupMgr为NULL");
		return false;
	}
	const MemberIDMapType& MemberMap = GetCharacter()->GetGroupMgr()->GetMemberIDMapType();
	MemberIDMapType::const_iterator it = MemberMap.begin();
	for(;it!=MemberMap.end();it++)
	{
		CMemberAI* pMemberAI = (*it).second->GetNpcAIHandler()->CastToMemberAI();
		if(pMemberAI)
		{
			return pMemberAI->CanFight(pTargetChar);
		}
	}
	return false;
}

void CDummyAI::OnMemberJoinGroup(CCharacterDictator* pMember)
{
	Ast (pMember);
	CMemberAI* pMemberAI =pMember->GetNpcAIHandler()->CastToMemberAI();
	if (pMemberAI)
	{
		CNpcEventMsg::Create(pMemberAI, eNpcEvent_OnMemberMoveTo, eNECI_Zero);
		CopyEnmityListToMember(pMemberAI);
		if(m_bIsInitMemberPath)
		{
			MemPathPointData* pMemPathData = m_mapMemberPathByID[pMemberAI->m_uGroupSelfID];
			pMemberAI->SetLocalPosAndMirrorPos(pMemPathData->m_LocalPos,pMemPathData->m_LocalMirrorPos);
		}
	}
}

void CDummyAI::CopyEnmityListToMember(CMemberAI* pMemAI, CNpcEventMsg* pEvent)
{
	CEnmityMgr* pLeaderEnmityMgr = GetEnmityMgr();
	if(!pLeaderEnmityMgr)
		return;
	CDummyEnmityMgr* pDummyEnmitMgr = class_cast<CDummyEnmityMgr*>(pLeaderEnmityMgr);
	if(!pDummyEnmitMgr)
		return;
	pDummyEnmitMgr->CopyEnmityListToMember(pMemAI, pEvent);
}

void CDummyAI::OnMemberAllDead()
{
	CEnmityMgr* pLeaderEnmityMgr = GetEnmityMgr();
	if(!pLeaderEnmityMgr)
		return;
	pLeaderEnmityMgr->ClearAllEnmity();
	CNpcEventMsg::Create(this, eNpcEvent_OnGroupMemberAllDead);

}
