#include "stdafx.h"
#include "CEnmityMgr.h"
#include "CCharacterDictator.h"
#include "CNpcAI.h"
#include "CEntityServerManager.h"
#include "CBattleStateManager.h"
#include "CNpcEnmityMember.h"
#include "NpcInfoMgr.h"
#include "CoreCommon.h"
#include "CTeamServerManager.h"
#include "CCharacterMediator.h"
#include "TSqrAllocator.inl"
#include "CGameConfigServer.h"
#include "CFighterDictator.h"
#include "CDummyAI.h"
#include "Random.h"
#include "CMemberAI.h"
#include "CRelationMgrServer.h"

template class TPtRefee<CEnmityMgr,CNpcAI>;

CEnmityMgr::CEnmityMgr(CNpcAI *pOwner) : m_pOwner(pOwner)
{
	m_RefsByNpcAI.SetHolder(this);
	pOwner->SetNpcEnmityMgrHolder(this);
	m_uLockEnemyDis = m_pOwner->GetLockEnemyDis();
}

CEnmityMgr::~CEnmityMgr()
{
}

CNpcAI* CEnmityMgr::GetOwner()
{
	return m_pOwner;
}

void CEnmityMgr::UpDateNpcBattleState()
{
	CCharacterDictator* pOwnerCharacter = GetOwner()->GetCharacter();
	Ast (pOwnerCharacter);
	CFighterDictator* pOwnerFighter = pOwnerCharacter->GetFighter();
	Ast (pOwnerFighter);
	CBattleStateManager* pBattleStateMgr = pOwnerFighter->GetBattleStateMgr();
	Ast (pBattleStateMgr);
	bool bInBattleState = pOwnerFighter->GetCtrlState(eFCS_InBattle);
	ENpcAIType	eAIType = GetOwner()->GetAITypeID();
	bool bFighterNpc = NpcInfoMgr::BeFightNpc(eAIType);

	if (IsEnemyListEmpty())
	{
		if (!bFighterNpc)
			pBattleStateMgr->LeaveBattleState();
		GetOwner()->CheckLifeTimeByLeaveBattle();
		GetOwner()->SetTarget(NULL);
	}
	else
	{
		if (!bInBattleState)
			pBattleStateMgr->EnterBattleState();
	}

	if (GetOwner()->IsChaosEnmity() && GetFirstMember())
	{
		for (EnemyListIter iter = m_lstEnemy.begin(); iter != m_lstEnemy.end();++iter)
		{
			CNpcEnmityMember* pMember = (*iter);
			if (pMember)
				pMember->DetachAttackMsg();
		}
		GetFirstMember()->AttachAttackMsg();
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnEnterAttackEnemyState);
	}
}

//暴露给外部使用的仇恨相关接口
void CEnmityMgr::ForceLock(CCharacterDictator* pEnemy, uint32 uChaosTime)
{
	ForceAddFrontEnemy(pEnemy);
	CNpcEnmityMember* pMember = GetFirstMember();
	if (uChaosTime != 0 && pMember && GetOwner()->IsChaosEnmity())	//只有会乱仇恨顺序的Npc才需要注册tick
		pMember->CreateChaosTick(uChaosTime);
}

bool CEnmityMgr::AddEnemy(CCharacterDictator* pEnemy, bool bHurtEffect)
{
	bool bSucc = AddOnEnemyToEnmityList(pEnemy, bHurtEffect);
	if (!bSucc)
		return false;

	uint32 uTeamID = pEnemy->GetTeamID();
	if (uTeamID != 0)
	{
		const ESetType& sTeamSet = CTeamServerManager::GetInst()->CppGetTeamMember(uTeamID, eTT_Team);
		ESetTypeIter Team_Iter = sTeamSet.begin();
		for (; Team_Iter != sTeamSet.end(); ++Team_Iter)
		{
			CCharacterDictator* pEnemy = CCharacterDictator::GetCharacterByID(*Team_Iter);
			if (IsInEnmityDist(pEnemy))
				AddOnEnemyToEnmityList(pEnemy, false);
		}
		return true;
	}
	return true;
}

void CEnmityMgr::RemoveEnemy(CCharacterDictator* pEnemy)
{
	Ast(GetOwner());
	if (!pEnemy)
		return;

	uint32 uEntityID = pEnemy->GetEntityID();
	if (GetFirstEnemy() == pEnemy)
	{
		CNpcEnmityMember* pMember = *m_lstEnemy.begin();
		m_lstEnemy.pop_front();
		delete pMember;
		CCharacterDictator* pNewEnemy = GetFirstEnemy();
		if (pNewEnemy)
			GetOwner()->SetTarget(pNewEnemy->GetFighter());
		else
		{
			GetOwner()->SetTarget(NULL);
			CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnEnemyLost, eNECI_Zero);
		}
		UpDateNpcBattleState();			//这里有可能改变第一仇恨目标或者清空仇恨目标，所以需要UpDate
	}
	else
	{
		for (EnemyListIter iter = m_lstEnemy.begin(); iter != m_lstEnemy.end();)
		{
			if ((*iter)->GetEnemyEntityID() == uEntityID)
			{
				delete (*iter);
				m_lstEnemy.erase(iter++);
				break;
			}
			else
				++iter;
		}
	}
	GetOwner()->DealExpOwnerOnEnemyLost(pEnemy);
}



bool CEnmityMgr::BeTargetOutOfLockEnemyDis(CCharacterDictator* pCharacter)
{
	CCharacterDictator* pOwnerCharacter = GetOwner()->GetCharacter();
	Ast(pOwnerCharacter);
	if (!pOwnerCharacter || !pCharacter)
		return true;
	if (pOwnerCharacter->GetScene() != pCharacter->GetScene())
		return true;
	if (!GetOwner()->IsInLockEnemyRange(pCharacter->GetFighter()))
		return true;
	return false;
}

bool CEnmityMgr::IsInEnmityDist(CCharacterDictator* pEnmey)
{
	Ast (GetOwner());
	CCharacterDictator* pOwnerCharacter = GetOwner()->GetCharacter();
	if (!pEnmey || !pOwnerCharacter)
		return false;
	CPos OwnPos, EnemyPos;
	pOwnerCharacter->GetGridPos(OwnPos);
	pEnmey->GetGridPos(EnemyPos);
	if (pOwnerCharacter->GetScene() != pEnmey->GetScene())
		return false;
	if (OwnPos.Dist(EnemyPos) < GetOwner()->GetEnmityDist())
		return true;
	return false;
}

void CEnmityMgr::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	CNpcAI* pAI = GetOwner();
	Ast(pAI);
	CFighterDictator* pTargetFighter = (CFighterDictator*)(pSubject);
	Ast (pTargetFighter);
	CCharacterDictator* pTargetCharacter = pTargetFighter->GetCharacter();
	Ast (pTargetFighter);

	SQR_TRY
	{
		switch(uEvent)
		{
		case eCE_Die:
			{
				if (IsInEnmityList(pTargetCharacter->GetEntityID()))
					pAI->OnEnemyDead();
				RemoveFarawayEnemy(pTargetCharacter);

			}
			 break;
		case eCE_BattleRelationChange:
			{
				if (pAI->GetCharacter() == pTargetCharacter)
				{
					EnemyListType& lstEnmity = GetEnemyList();
					EnemyListType :: iterator iter = lstEnmity.begin();
					for (; iter!= lstEnmity.end(); )
					{
						CCharacterDictator* pCharacter = CEntityServerManager::GetInst()->GetCharacter((*iter)->GetEnemyEntityID());
						++iter;
						if (!pCharacter)
							return;

						if (pAI->CastToMemberAI())
						{
							CDummyAI* pLeaderAI = pAI->CastToMemberAI()->GetGroupLeaderDummyAI();
							if (pLeaderAI && !pLeaderAI->MemberCanFight(pCharacter))
							{
								pLeaderAI->GetEnmityMgr()->RemoveFarawayEnemy(pCharacter);
							}
						}
						else if ( !pAI->CanFight(pCharacter) || 
							(pAI->CastToDummyAI() && !pAI->CastToDummyAI()->MemberCanFight(pCharacter)) )
						{
							RemoveFarawayEnemy(pCharacter);
						}
					}
					break;
				}
				if ( pAI->CanFight(pTargetCharacter) || 
					(pAI->CastToDummyAI() && pAI->CastToDummyAI()->MemberCanFight(pTargetCharacter)) )
					return;
				RemoveFarawayEnemy(pTargetCharacter);
			}
			break;
		case eCE_Offline:
		case eCE_ChangeMapBegin:
			{
				RemoveFarawayEnemy(pTargetCharacter);
			}
			break;
		case eCE_EntryEnmity:
			{
				CFighterDictator* pEnemyFighter = reinterpret_cast<CFighterDictator*>(pArg);
				Ast (pEnemyFighter);
				CCharacterDictator* pEnemyCharacter = pEnemyFighter->GetCharacter();
				Ast (pEnemyCharacter);
				if (IsInEnmityList(pEnemyCharacter->GetEntityID()))
				{
					CNpcEnmityMember* pMember = GetMemberByID(pEnemyCharacter->GetEntityID());
					Ast (pMember);
					pMember->SetHurtEffect(true);
				}
				else
				{
					AddEnemy(pEnemyCharacter, true);
				}
			}
			break;
		case eCE_DebaseEnmity:
			{
				ForceAddBackEnemy(pTargetCharacter);
			}
			break;
		}
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

bool CEnmityMgr::AddOnEnemyToEnmityList(CCharacterDictator* pEnemy, bool bHurtEffect)
{
	if (!pEnemy || !GetOwner()->CanFight(pEnemy) || BeTargetOutOfLockEnemyDis(pEnemy))
		return false;
	uint32 uEntityID = pEnemy->GetEntityID();
	if (IsInEnmityList(uEntityID))
		return true;
	if (IsEnemyListEmpty())
		PushFrontEnemy(pEnemy, bHurtEffect);
	else
		PushBackEnemy(pEnemy, bHurtEffect);

	if (pEnemy->GetMaster())
		AddEnemy(pEnemy->GetMaster(), false);
	return true;
}

void CEnmityMgr::PushFrontEnemy(CCharacterDictator* pEnemy, bool bHurtEffect)
{
	CNpcAI* pOwnAI = GetOwner();
	Ast(pOwnAI);
	if (!pEnemy)
		return;
	CCharacterDictator* pOwnCharacter = pOwnAI->GetCharacter();
	Ast(pOwnCharacter);
	CNpcEnmityMember* pMember = new CNpcEnmityMember(pOwnCharacter, pEnemy, this, 
														bHurtEffect,float(pOwnAI->GetAIBaseData()->m_uLockEnemyDis) * eGridSpan);
	m_lstEnemy.push_front(pMember);
	pOwnAI->SetTarget(pEnemy->GetFighter());
	UpDateNpcBattleState();
}

void CEnmityMgr::PushBackEnemy(CCharacterDictator* pEnemy, bool bHurtEffect)
{
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast(pOwnCharacter);
	CNpcEnmityMember* pMember = new CNpcEnmityMember(pOwnCharacter, pEnemy, this, 
														bHurtEffect, float(GetOwner()->GetAIBaseData()->m_uLockEnemyDis) * eGridSpan);
	m_lstEnemy.push_back(pMember);
}

bool CEnmityMgr::ForceAddFrontEnemy(CCharacterDictator* pEnemy)
{
	if (!pEnemy || !JudgeIsEnemy(pEnemy))
		return false;

	uint32 uEntityID = pEnemy->GetEntityID();
	CFighterDictator* pEnemyFighter = pEnemy->GetFighter();
	Ast(pEnemyFighter);

	if (pEnemy == GetFirstEnemy())
		return true;

	// 判断Npc仇恨列表中是否存在该对象，存在就修改玩家在列表中的位置
	for (EnemyListIter iter = m_lstEnemy.begin(); iter != m_lstEnemy.end();)
	{
		if ((*iter)->GetEnemyEntityID() == uEntityID)
		{
			CNpcEnmityMember* pMember = (*iter);
			m_lstEnemy.erase(iter);
			m_lstEnemy.push_front(pMember);
			CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(pMember->GetEnemyEntityID());
			GetOwner()->SetTarget(pCharacter->GetFighter());
			UpDateNpcBattleState();
			return true;
		}
		else
			++iter;
	}
	PushFrontEnemy(pEnemy, false);
	return true;
}

bool CEnmityMgr::ForceAddBackEnemy(CCharacterDictator* pEnemy)
{
	if (!pEnemy || !GetOwner()->CanFight( pEnemy))
		return false;

	uint32 uEntityID = pEnemy->GetEntityID();
	CFighterDictator* pEnemyFighter = pEnemy->GetFighter();
	Ast(pEnemyFighter);

	EnemyListType::reverse_iterator iter = m_lstEnemy.rbegin();
	if (iter != m_lstEnemy.rend() && (*iter)->GetEnemyEntityID() == uEntityID)
		return true;

	for (EnemyListIter iter = m_lstEnemy.begin(); iter != m_lstEnemy.end();)
	{
		if ((*iter)->GetEnemyEntityID() == uEntityID)
		{
			CNpcEnmityMember* pMember = (*iter);
			m_lstEnemy.erase(iter);
			m_lstEnemy.push_back(pMember);
			CCharacterDictator* pNewEnemy = GetFirstEnemy();
			if (!pNewEnemy)
			{
				LogErr("CEnmityMgr::ForceAddBackEnemy Error 仇恨目标为空");
				GetOwner()->SetTarget(NULL);
				CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnEnemyLost, eNECI_Seventeen);
				UpDateNpcBattleState();
				return true;
			}
			GetOwner()->SetTarget(pNewEnemy->GetFighter());
			UpDateNpcBattleState();
			//如果当前不处于放技能中则切换目标，如果处于流程中则不立刻切换，状态机在进行下一次进攻的时候自动切换，防止打断当前流程
			if (!GetOwner()->GetIsDoingSkillRule()) 
				CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnTargetChange); 
			return true;
		}
		else
			++iter;
	}
	PushBackEnemy(pEnemy, false);
	return true;
}

void CEnmityMgr::RemoveFarawayEnemy(CCharacterDictator* pEnemy)
{
	CNpcAI* pOwnAI = GetOwner();
	Ast(pOwnAI);
	RemoveEnemy(pEnemy);
}

void CEnmityMgr::GetNearestEnemy()
{
	Ast (GetOwner());
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast (pOwnCharacter);

	CheckEnmityMember();

	if (GetEnemyNumber() == 0 || GetEnemyNumber() == 1)
		return;
	CNpcEnmityMember* pMember = GetFirstMember();
	Ast (pMember);
	if (!pMember->CanChangeEnemy())
		return;
	else						//list大于等于2
	{
		CCharacterDictator* pNearestEnemy = NULL;
		float fDis =  (float)4*m_uLockEnemyDis*eGridSpanForObj;//初始化为4倍锁敌距离，仇恨列表中不可能有比这个还大的数值
		CFPos ownpos;
		pOwnCharacter->GetPixelPos(ownpos);
		
		for (EnemyListIter iter = m_lstEnemy.begin(); iter != m_lstEnemy.end(); ++iter)
		{
			CNpcEnmityMember* pMember = (*iter);
			Ast (pMember);
			if (!pMember->BeHaveMadeHurtEffect())
				continue;
			CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(pMember->GetEnemyEntityID());
			Ast (pCharacter);
			CFPos enemypos;
			pCharacter->GetPixelPos(enemypos);
			float fLocalDis = ownpos.Dist(enemypos);
			if (fLocalDis < fDis)
			{
				pNearestEnemy = pCharacter;
				fDis = fLocalDis;
			}
		}

		if (pNearestEnemy)
			ForceAddFrontEnemy(pNearestEnemy);
	}
}

void CEnmityMgr::GetOtherNearstEnemy()
{
	Ast (GetOwner());
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast (pOwnCharacter);

	//如果出于流程当中就不切换目标，防止移动打断当前流程
	if (GetOwner()->GetIsDoingSkillRule())
		return;

	CheckEnmityMember();

	if (GetEnemyNumber() == 0 || GetEnemyNumber() == 1)
		return;
	CNpcEnmityMember* pMember = GetFirstMember();
	Ast (pMember);
	if (!pMember->CanChangeEnemy())
		return;
	else			//list数目大于等于2
	{
		CCharacterDictator* pOtherNearestEnemy = NULL;
		float fDis =  (float)4*m_uLockEnemyDis*eGridSpanForObj;	//初始化为4倍锁敌距离，仇恨列表中不可能有比这个还大的数值
		CFPos ownpos;
		pOwnCharacter->GetPixelPos(ownpos);

		EnemyListIter iter = m_lstEnemy.begin();
		Ast (iter != m_lstEnemy.end());
		iter++;

		for (; iter != m_lstEnemy.end(); ++iter)
		{
			CNpcEnmityMember* pOtherMember = (*iter);
			Ast (pOtherMember);
			if (!pOtherMember->BeHaveMadeHurtEffect())
				continue;
			CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(pOtherMember->GetEnemyEntityID());
			Ast (pCharacter);
			float fLocalDis = 0.0f;
			CFPos enemypos;
			pCharacter->GetPixelPos(enemypos);
			fLocalDis = ownpos.Dist(enemypos);
			if (fLocalDis < fDis)
			{
				pOtherNearestEnemy = pCharacter;
				fDis = fLocalDis;
			}
		}

		if (pOtherNearestEnemy)
		{
			ForceAddFrontEnemy(pOtherNearestEnemy);
			CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnTargetChange);
		}
	}
}

bool CEnmityMgr::CanChangeEnemy()
{
	CNpcEnmityMember* pMember = GetFirstMember();
	if (pMember && !pMember->CanChangeEnemy())
		return false;
	return true;
}

CNpcEnmityMember* CEnmityMgr::GetMemberByID(uint32 uEntityID)
{
	MapEnmityMemberIter iter = m_mapEnmityMemeber.find(uEntityID);
	if (iter != m_mapEnmityMemeber.end())
		return iter->second;
	else
		return NULL;
}

CCharacterDictator* CEnmityMgr::GetRandomEnemy()
{
	CheckEnmityMember();

	if(IsEnemyListEmpty())
		return NULL;
	uint32 num = (uint32)Random::Rand(0,(uint32)m_lstEnemy.size()-1);
	EnemyListType:: iterator iter = m_lstEnemy.begin();
	for(uint32 i = 0;i<num;i++)
		iter++;
	CCharacterDictator* pCharacter = CEntityServerManager::GetInst()->GetCharacter((*iter)->GetEnemyEntityID());
	return pCharacter;
}

CCharacterDictator* CEnmityMgr::GetRandomNotNpcEnemy()
{
	CheckEnmityMember();
	if(IsEnemyListEmpty())
		return NULL;
	vector<CCharacterDictator*> vecMedEnemy;
	for (EnemyListIter iter = m_lstEnemy.begin(); iter != m_lstEnemy.end(); iter++)
	{
		CCharacterDictator* pEnenmy = CCharacterDictator::GetCharacterByID((*iter)->GetEnemyEntityID());
		if (pEnenmy->CastToCharacterMediator())
		{
			vecMedEnemy.push_back(pEnenmy);
		}
	}
	if (vecMedEnemy.empty())
		return NULL;
	uint32 num = (uint32)Random::Rand(0,(uint32)vecMedEnemy.size()-1);
	CCharacterDictator* pCharacter = vecMedEnemy[num];
	return pCharacter;
}

bool CEnmityMgr::IsEnemyListEmpty()
{
	return m_lstEnemy.empty();
}

bool CEnmityMgr::IsInEnmityList(uint32 uEntityID)
{
	MapEnmityMemberIter iter = m_mapEnmityMemeber.find(uEntityID);
	return iter != m_mapEnmityMemeber.end();
}

CCharacterDictator* CEnmityMgr::GetFirstEnemy()
{
	if(IsEnemyListEmpty())
		return NULL;
	 return CCharacterDictator::GetCharacterByID(m_lstEnemy.front()->GetEnemyEntityID());
}

CNpcEnmityMember* CEnmityMgr::GetFirstMember()
{
	if (IsEnemyListEmpty())
		return NULL;
	return m_lstEnemy.front();
}

void CEnmityMgr::ShareEnmityWithFriend(CNpcAI* pFriendAI)
{
	if(!pFriendAI)
		return;
	if(IsEnemyListEmpty())
		return;
	for (EnemyListIter iter = m_lstEnemy.begin(); iter != m_lstEnemy.end(); iter++)
	{
		CCharacterDictator* pEnenmy = CCharacterDictator::GetCharacterByID((*iter)->GetEnemyEntityID());
		pFriendAI->OnBeAttacked(pEnenmy);
	}

}

void CEnmityMgr::AddEnmityMember(uint32 uEmenyID, CNpcEnmityMember* pMember)
{
	pair<MapEnmityMemberIter, bool> pairEnmityMap
		= m_mapEnmityMemeber.insert(make_pair(uEmenyID, pMember));
	Ast (pairEnmityMap.second);
}

void CEnmityMgr::DelEnmityMember(uint32 uEnemyID, CNpcEnmityMember* pMember)
{
	MapEnmityMemberIter iter = m_mapEnmityMemeber.find(uEnemyID);
	Ast (iter != m_mapEnmityMemeber.end());
	m_mapEnmityMemeber.erase(iter);
}

void CEnmityMgr::CheckEnmityMember()
{
	if (!CGameConfigServer::Inst()->GetCheckNpcEnmity())
		return;
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast (pOwnCharacter);
	string strErrType = "NpcEnmity Check Failed ! ";

	bool bMemberChange = false;
	for (EnemyListIter iter = m_lstEnemy.begin(); iter != m_lstEnemy.end();)
	{
		CNpcEnmityMember* pMember = (*iter);
		Ast (pMember);
		CCharacterDictator* pCharater = CCharacterDictator::GetCharacterByID(pMember->GetEnemyEntityID());
		if (!pCharater)
		{
			ostringstream strm;
			strm<<"Enmity member is not exist !";
			LogErr( strErrType, strm.str());
			
			bMemberChange = true;
			delete (*iter);
			m_lstEnemy.erase(iter++);
			continue;
		}
		if (!pCharater->CppIsLive())
		{
			ostringstream strm;
			strm<<"Enmity member is dead !";
			LogErr( strErrType, strm.str());

			bMemberChange = true;
			delete (*iter);
			m_lstEnemy.erase(iter++);
			continue;
		}
		if (!GetOwner()->CanFight( pCharater))
		{
			if (pOwnCharacter->CppIsLive())
			{
				ostringstream strm;
				strm<<"Enmity member can not fight !" << pOwnCharacter->GetNpcName() << "," << pCharater->GetNpcName();
				CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
				ERelationType eRelationType = relationMgr.GetRelationType(pOwnCharacter->GetFighter(), pCharater->GetFighter());
				if (eRelationType != eRT_Enemy)
				{
					strm << ", Relation:" << (uint32)eRelationType;
				}
				LogErr( strErrType, strm.str());
			}

			bMemberChange = true;
			delete (*iter);
			m_lstEnemy.erase(iter++);
			continue;
		}
		++iter;
	}
	if (bMemberChange)
		UpDateNpcBattleState();
}

