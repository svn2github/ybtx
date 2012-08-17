#include "stdafx.h"
#include "CNpcAI.h"
#include "CCharacterDictator.h"
#include "ICoreObjectServerHandler.h"
#include "CIntObjServer.h"
#include "CAoiDimMgr.h"
#include "ICharacterDictatorCallbackHandler.h"
#include "CSceneCommon.h"
#include "CSkillRuleMgr.h"
#include "CFighterDictator.h"
#include "CNpcAINatureMgr.h"
#include "CCoreSceneServer.h"
#include "NpcInfoMgr.h"
#include "NormalAttack.h"
#include "CNpcResServerData.h"
#include "Random.h"

void CNpcAI::OnCreateEnded()
{
	CCharacterDictator* pCharacter = GetCharacter();
	Ast(pCharacter);
	pCharacter->GetPixelPos(m_InitPos);
	m_RandomMoveBeginPos = m_InitPos;
	CNpcEventMsg *pEvent = CNpcEventMsg::Create(this,eNpcEvent_OnStart);
	GetCurrentStateMachine()->Start(pEvent);
	//cout<<m_uPlayNumInSleepDim<<"  视野玩家@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
	SetBornTrigger();
}

void CNpcAI::OnDead()
{
	//CCharacterDictator* pCharacterDictator = GetCharacter();
	//Ast(pCharacterDictator);
	//CPos pos;
	//pCharacterDictator->GetGridPos(pos);

	////通知周围的人，自己被打死了
	//vector<CCoreObjectServer* > vecObject;
	//
	//CFPos pixelPos = pCharacterDictator->GetPixelPos();
	//pCharacterDictator->GetScene()->QueryObject(vecObject, pixelPos, pCharacterDictator->GetEyeSight());
	//for(vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	//{
	//	if ( *it == NULL ) 
	//		return;
	//	ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
	//	if ( pHandler == NULL )
	//		return;
	//	size_t eType = (size_t)(pHandler->GetTag());
	//	switch(eType)
	//	{
	//	case eCOHT_Character:
	//		CCharacterDictator *pCharacter = CCharacterDictator::GetCharacterByCoreObj(*it);
	//		if (pCharacter != NULL && pCharacter->GetNpcAIHandler() && GetFirstEnenmyFighter())
	//			pCharacter->GetNpcAIHandler()->OnOtherDead(GetFirstEnenmyFighter()->GetEntityID(), GetName());
	//	}
	//}
	//ClearAllEnmity();
	CNpcEventMsg::Create(this,eNpcEvent_OnDead);
}

void CNpcAI::OnDestoryBegin()
{
	CNpcEventMsg::Create(this,eNpcEvent_OnDestoryBegin);
}

void CNpcAI::OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	CCoreObjectServer *pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);

	if ( pCoreObj == NULL ) 
		return;
	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if ( pHandler == NULL )
		return;

	size_t eType = (size_t)(pHandler->GetTag());

	switch( eType )
	{
	case eCOHT_Character:
		{
			CCharacterDictator *pCharacter = CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
			if(pCharacter)
			{
				RedirectForCaughtViewSightOf(pCharacter, uDimension);
			}
			break;
		}
	case eCOHT_Obj:
		{
			CIntObjServer* pIntObj = CIntObjServer::GetIntObjServer(pCoreObj);
			CNpcEventMsg::Create(this,eNpcEvent_OnObjectInSight,eNECI_Zero,reinterpret_cast<void*>(pIntObj->GetEntityID()));
			m_mapEntity[pIntObj->GetEntityID()] = eType;
			break;
		}
	}
}

void CNpcAI::RedirectForCaughtViewSightOf(CCharacterDictator *pCharacter, uint32 uDimension)
{
	if (uDimension == CAoiDimFilter::m_uNpcSleepDim)
	{
		m_uPlayNumInSleepDim ++;
		CNpcEventMsg::Create(this,eNpcEvent_OnPlayInNpcSleepDimEyeSight);
		AddPlayToSleepDim();
	}
	else
	{
		uint32 uEntityID = pCharacter->GetEntityID();
		CNpcEventMsg::Create(this,eNpcEvent_OnCharacterInSight,eNECI_Zero,reinterpret_cast<void*>(uEntityID));
		m_mapEntity[uEntityID] = eCOHT_Character;
	}

}

void CNpcAI::OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	if(pCoreObj == NULL)
		return;
	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if ( pHandler == NULL )
		return;

	size_t eType = (size_t)(pHandler->GetTag());

	switch( eType )
	{
	case eCOHT_Character:
		{
			CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
			if(pCharacter)
			{
				RedirectForLostViewSightOf(pCharacter, uDimension);
			}	
		}
		break;
	case eCOHT_Obj:
		{
			CIntObjServer* pIntObj = CIntObjServer::GetIntObjServer(pCoreObj);
			if(pIntObj)
			{
				uint32 uEntityID = pIntObj->GetEntityID();
				if(m_mapEntity.find(uEntityID) != m_mapEntity.end())
				{
					CNpcEventMsg::Create(this,eNpcEvent_OnObjectOutOfSight,eNECI_Zero,reinterpret_cast<void*>(uEntityID));
					m_mapEntity.erase(m_mapEntity.find(uEntityID));
				}
			}
		}
		break;
	}
}

void CNpcAI::RedirectForLostViewSightOf(CCharacterDictator *pCharacter, uint32 uDimension)
{
	if (uDimension == CAoiDimFilter::m_uNpcSleepDim)
	{
		m_uPlayNumInSleepDim --;
		CNpcEventMsg::Create(this,eNpcEvent_OnPlayOutNpcSleepDimEyeSight);
		DelPlayToSleepDim();
	}
	else
	{
		uint32 uEntityID = pCharacter->GetEntityID();
		MapEntityIDInNpcSight::iterator itr = m_mapEntity.find(uEntityID);
		if(itr != m_mapEntity.end())
		{
			CNpcEventMsg::Create(this,eNpcEvent_OnCharacterOutOfSight,eNECI_Zero,reinterpret_cast<void*>(uEntityID));
			m_mapEntity.erase(itr);
		}
	}
}

void CNpcAI::OnBeAttacked(CCharacterDictator *pAttacker, bool bHurtEffect)
{
	if(!pAttacker || pAttacker == GetCharacter() || BeInChaseBackState())
		return;
	DropAdscriptionJudgement(pAttacker);
	bool bInEnemyList = GetEnmityMgr()->IsInEnmityList(pAttacker->GetEntityID());
	if (!bInEnemyList)
	{
		if (GetEnmityMgr()->JudgeIsEnemy(pAttacker))
			OnAddEnemyToEnmityMgr(pAttacker, bHurtEffect);
	}
	if (!GetIsDoingSkillRule() && !GetIsDoingAttack())
			CNpcEventMsg::Create(this, eNpcEvent_OnBeAttacked);
}

void CNpcAI::OnOtherDead(uint32 EntityID, const string& NpcName)
{
	if (NpcName == GetName())
	{
		CNpcEventMsg::Create(this, eNpcEvent_OnOtherDead );
	}
}

void CNpcAI::OnAgileValueChanged(uint32 uValueTypeID, float fValue)	
{
	if (!GetSkillRuleMgr() || !GetSkillRuleMgr()->BeActiveState())
		return;

	switch (uValueTypeID)
	{
	case (ePID_HealthPoint * ePFT_Count + ePFT_Agile):
		{
			GetSkillRuleMgr()->OnEvent(eNpcEvent_OnHpChanged);
		}
		break;
	case (ePID_ManaPoint * ePFT_Count + ePFT_Agile):
		{
			GetSkillRuleMgr()->OnEvent(eNpcEvent_OnMpChanged);
		}
		break;
	default:
		break;
	}
}

void CNpcAI::OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet)
{
	CNpcAI* pOwner = const_cast<CNpcAI*>(this);
	switch (uValueTypeID)
	{
	case (eFCS_ForbitNormalAttack):
		if (bSet)
		{
			CNpcEventMsg::Create(pOwner,eNpcEvent_OnForbitNormalAttack);
		}			
		else
		{
			CNpcEventMsg::Create(pOwner,eNpcEvent_OnCanNormalAttack);
		}		
		break;
	case (eFCS_ForbitMove):
		if (bSet)
		{
			CNpcEventMsg::Create(pOwner,eNpcEvent_OnForbitMove);
		}			
		else
		{
			CNpcEventMsg::Create(pOwner,eNpcEvent_OnCanMove);
		}			
		break;
	}
}

void CNpcAI::OnDoSkillCtrlStateChanged(uint32 uState, bool bSet)
{
	CNpcAI* pOwner = const_cast<CNpcAI*>(this);
	switch (uState)
	{
	case (eDSCS_ForbitUseSkill):
		if (bSet)
		{
			CNpcEventMsg::Create(pOwner,eNpcEvent_OnForbitUseSkill);
		}

		else
		{
			CNpcEventMsg::Create(pOwner,eNpcEvent_OnCanUseSkill);
		}		
		break;
	}
}

void CNpcAI::OnSkillBegin()
{
	CNpcEventMsg::Create(this,eNpcEvent_OnSkillBegin);
}

void CNpcAI::OnSkillSuccessEnd()
{
	CNpcEventMsg::Create(this,eNpcEvent_OnSkillSuccessEnd);
}

void CNpcAI::OnSkillFailEnd()
{
	CNpcEventMsg::Create(this,eNpcEvent_OnSkillFailEnd);
}

void CNpcAI::CheckLifeTimeByLeaveBattle()
{
	if (!GetCharacter())
		return;
	if (m_bLifeTimeEnd && GetCharacter()->CppIsLive())
		GetCharacter()->KillNpcByLifeTimeEnd();
}

void CNpcAI::SetDeadAfterBattle()
{
	if (!GetCharacter())
		return;
	CFighterDictator* pFighter = GetCharacter()->GetFighter();
	bool bInBattle = pFighter->GetCtrlState(eFCS_InBattle);
	if (bInBattle)
		m_bLifeTimeEnd = true;
	else
		GetCharacter()->KillNpcByLifeTimeEnd();
}

void CNpcAI::SetDefaultWalkState(bool walkState)
{
	m_bDefaultWalkState = walkState;
}

void CNpcAI::SetMoveRange(uint32 range)
{
	m_uMoveRange = range;
}

void CNpcAI::OnBeginRandomMove()
{
	CNpcEventMsg::Create(this,eNpcEvent_OnBeginRandomMove);
}

void CNpcAI::OnSetState(uint32 global_id)
{
	CNpcEventMsg::Create(this,eNpcEvent_OnCharacterSetState,eNECI_Zero,reinterpret_cast<void*>(global_id));
}

void CNpcAI::OnDeleteState(uint32 global_id)
{
	CNpcEventMsg::Create(this,eNpcEvent_OnCharacterDeleteState,eNECI_Zero,reinterpret_cast<void*>(global_id));
}

void CNpcAI::OnFacialAction(uint32 uEntityID, const string& szFacialName)
{
	CCharacterDictator *pCharacter = CCharacterDictator::GetCharacterByID(uEntityID);
	CCharacterDictator* pOwnCharacterDictator = GetCharacter();
	Ast(pOwnCharacterDictator);
	CFighterDictator* pOwnFighterDictator = pOwnCharacterDictator->GetFighter();
	Ast(pOwnFighterDictator);
	pOwnFighterDictator->SetTarget(pCharacter->GetFighter());
	SetTarget(pCharacter->GetFighter());
	CNpcEventMsg::Create(this,eNpcEvent_OnFacialActionHappened);
}

void CNpcAI::OnSetNatureData(const string& szNatureName, const string& szNatureArg, uint32 uNatureExtraArg, 
							 const string& NatureAction, uint16 uColdTime, const string& szAction, const string& szActionArg, uint16 uLevel)
{
	GetNatureMgr()->SetNatureData(szNatureName, szNatureArg, uNatureExtraArg, NatureAction, uColdTime, szAction, szActionArg, uLevel);
}

void CNpcAI::SendNpcEvent(ENpcEvent eEvent)
{
	CNpcEventMsg::Create(this,eEvent);
}

uint32 CNpcAI::GetOneActorAround()
{
	vector<CCoreObjectServer* > vecObject;
	vector<uint32> vecAllChar;
	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	CFPos pixelPos = pCharacterDictator->GetPixelPos();
	pCharacterDictator->GetScene()->QueryObject(vecObject, pixelPos, 10.0f);

	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( *it == NULL ) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator *pChar = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (!pChar->GetNpcAIHandler())
			{
				vecAllChar.push_back(pChar->GetEntityID());
			}
		}
	}
	if (!vecAllChar.empty())
	{
		uint32 num = (uint32)Random::Rand(0,(uint32)vecAllChar.size()-1);
		return vecAllChar[num];
	}

	return 0;
}

uint32 CNpcAI::GetOneNpcAround()
{
	vector<CCoreObjectServer* > vecObject;
	vector<uint32> vecAllNpc;
	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	CFPos pixelPos = pCharacterDictator->GetPixelPos();
	pCharacterDictator->GetScene()->QueryObject(vecObject, pixelPos, 10.0f);

	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( *it == NULL ) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator *pChar = CCharacterDictator::GetCharacterByCoreObj(*it);

			if (GetCharacter()->GetEntityID() != pChar->GetEntityID() && pChar->GetNpcAIHandler())
			{
				vecAllNpc.push_back(pChar->GetEntityID());
			}
		}
	}
	if (!vecAllNpc.empty())
	{
		uint32 num = (uint32)Random::Rand(0,(uint32)vecAllNpc.size()-1);
		return vecAllNpc[num];
	}

	return 0;
}

uint32 CNpcAI::GetRandomOneAround()
{
	vector<CCoreObjectServer* > vecObject;
	vector<uint32> vecAllObj;
	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	CFPos pixelPos = pCharacterDictator->GetPixelPos();
	pCharacterDictator->GetScene()->QueryObject(vecObject, pixelPos,10.0f);

	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( *it == NULL ) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator *pChar = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (CanFight(pChar))
			{
				vecAllObj.push_back(pChar->GetEntityID());
			}
		}
	}
	if (!vecAllObj.empty())
	{
		uint32 num = (uint32)Random::Rand(0,(uint32)vecAllObj.size()-1);
		return vecAllObj[num];
	}

	return 0;
}

void CNpcAI::ClearAllEnmity()
{
	CEnmityMgr* pOwnEnmityMgr = GetEnmityMgr();
	Ast(pOwnEnmityMgr);
	pOwnEnmityMgr->ClearAllEnmity();
}

void CNpcAI::ReSetNpcExistTick(uint32 uExitTime)
{
	if (!GetCharacter() || !GetCharacter()->CppIsLive() )
		return;
	if (uExitTime == 0)
	{
		if (NULL != m_pExistTick)
		{
			GetCharacter()->UnRegistDistortedTick(m_pExistTick);
			delete m_pExistTick;
			m_pExistTick = NULL;
		}
	}
	else
	{
		if (NULL != m_pExistTick)
		{
			GetCharacter()->UnRegistDistortedTick(m_pExistTick);
			GetCharacter()->RegistDistortedTick(m_pExistTick, uExitTime * 1000);
		}
		else
		{
			m_pExistTick = new ExistTick(this);
			GetCharacter()->RegistDistortedTick(m_pExistTick, uExitTime*1000);
		}
	}
}

EAttackType CNpcAI::GetNpcAttackType()
{
	//Ast(GetFightBaseData());
	//return GetFightBaseData()->m_eAttackType;
	CCharacterDictator* pOwnCharacter = GetCharacter();
	Ast(pOwnCharacter);
	CFighterDictator* pOwnFighter = pOwnCharacter->GetFighter();
	Ast(pOwnFighter);
	return pOwnFighter->GetNormalAttackMgr()->GetMHNormalAttack()->GetCfgAttackType();
}

void CNpcAI::SetGroupMemberID(uint32 id)
{
	m_uGroupSelfID = id;
}

CNpcNatureMgr* CNpcAI::GetNatureMgr()
{
	return m_pNpcNatureMgrHandler.Get();
}

void CNpcAI::CheckPlayInNpcSleepDim()
{
	if ( !m_bSleep)
		return;
	Ast (m_uPlayNumInSleepDim >= 0);
	if (m_uPlayNumInSleepDim == 0)
	{
		CNpcEventMsg::Create(this,eNpcEvent_OnNpcSleepBegin);
	}
}

bool CNpcAI::HaveBeSetPath()
{
	return m_mapMovePath.size() == 0 ? false : true;
}

void CNpcAI::RemoveOneEnemy(CCharacterDictator *pEnemy)
{
	if (GetEnmityMgr())
	{
		GetEnmityMgr()->RemoveEnemy(pEnemy);
	}
}


void CNpcAI::SetNpcAoiAndNature()
{
	/********************************************************************************************************************/
	/*		必须先设置第一重AOI，再构造个性管理器，因为在构造个性管理器的时候可能会设置第二重AOI，必须在设置第一重AOI之后进行 
	*	同时，设置第一重AOI时候也有时序依赖，必须先设置睡眠层，再设置AoiType再设置EyeSize和Size*/
	/********************************************************************************************************************/
	CCharacterDictator* pCharacter = GetCharacter();
	Ast (pCharacter);
	if (GetNpcType() == ENpcType_BattleHorse)
	{
		pCharacter->SetObjectAoiType(EAoi_BattleHorse);
		pCharacter->SetEyeSight(0);
		CCharacterDictator* pMaster = pCharacter->GetMaster();
		Ast(pMaster);
		pCharacter->SetSize(pMaster->GetSize());
		pCharacter->SetStealth(0);
		pCharacter->SetKeenness(0);
		pCharacter->SetBottomSize(GetResBaseData()->m_fBottomSize);
		//pCharacter->SetBarrierSize(GetNpcSize());
	}
	else
	{
		pCharacter->InitNpcEyeSizeForNpcSleepDim(m_bSleep);
		pCharacter->SetObjectAoiType(m_eObjAoiType);
		uint32 uBirthFrame = (uint32)pCharacter->GetNpcBirthAniFrameCnt();
		if (uBirthFrame != 0)
		{
			m_pSetNpcAoiTick = new DelaySetEyeSizeAndNatureTick(this);
			pCharacter->RegistDistortedTick(m_pSetNpcAoiTick, uBirthFrame*33);
		}
		else
			pCharacter->SetEyeSight(GetAIBaseData()->m_uEyeSize);
		pCharacter->SetSize(GetNpcSize());
		pCharacter->SetStealth(0);
		pCharacter->SetKeenness(0);
		pCharacter->SetBottomSize(GetResBaseData()->m_fBottomSize);
		//pCharacter->SetBarrierSize(GetNpcSize());
	}
}

void CNpcAI::SetNpcBirthDir(uint8 uDir)
{
	m_uBirthDir = uDir;
}

void CNpcAI::OnEnterBattleState()
{
	if (GetSkillRuleMgr())
		GetSkillRuleMgr()->InstallBattleSkillRule();
	CNpcEventMsg::Create(this,eNpcEvent_OnEnterBattleState);
}

void CNpcAI::OnLeaveBattleState()
{
	if(GetSkillRuleMgr())
		GetSkillRuleMgr()->UnInstallBattleSkillRule();
}

void CNpcAI::MasterNotifyLeaveBattle()
{
	CNpcEventMsg::Create(this, eNpcEvent_OnEnemyLost, eNECI_Three);
}


void CNpcAI::NpcMoveInPath()
{
	if (m_mapMovePath.size() == 0 || m_eMoveType != ENpcMove_MoveInPath)
		return;
	m_bSleep = false;	//禁止休眠
	if (GetRealAIType() != ENpcAIType_Task)
	{
		ostringstream strm;
		strm<<"Npc:【"<<GetCharacter()->m_sNpcName<<"】是非任务Npc不允许设置 组合路径巡逻 类型！"<<endl;
		LogErr("Npc巡逻类型设置错误！", strm.str().c_str());
		return;
	}
	vector<CFPos> vecPath;
	MapMovePath::const_iterator iter = m_mapMovePath.begin();
	CFPos curPos;
	GetCharacter()->GetPixelPos(curPos);
	if (curPos != (*(*iter).second))
	{
		vecPath.push_back(curPos);
	}
	for (uint32 i = 1; i < m_mapMovePath.size(); i++)
	{
		MapMovePath::const_iterator iterIndex = m_mapMovePath.find(i);
		CFPos pos = (*(iterIndex->second));
		iter++;
		vecPath.push_back(pos);
	}
	CFighterDictator* pFighter = GetCharacter()->GetFighter();
	float fSpeed = m_bDefaultWalkState ?pFighter->m_WalkSpeed.Get(this->GetCharacter()->GetFighter()) : 
		pFighter->m_RunSpeed.Get(this->GetCharacter()->GetFighter());
	GetCharacter()->MoveToInCustomPath(vecPath, fSpeed, 0);
}

uint32 CNpcAI::GetNpcDoSkillLevel()
{
	return GetCharacter()->GetNpcSkillLevel() == 0 ? GetCharacter()->CppGetLevel() : GetCharacter()->GetNpcSkillLevel();
}

void CNpcAI::OnBeHurt(int32 iHpChange)
{
	//这里直接通知技能规则被伤害的的消息，减少消息数目，防止异步导致拿到的伤害数值不对
	if (GetSkillRuleMgr() && GetSkillRuleMgr()->BeActiveState())
		GetSkillRuleMgr()->OnBeHurt(iHpChange);
}

void CNpcAI::SetDoSpecialActionProbability(uint32 uProbability)
{
	m_uSpecialActionProbability = uProbability;
}

