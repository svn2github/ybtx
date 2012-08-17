#include "stdafx.h"
#include "CNpcAI.h"
#include "CNpcEventMetaData.h"
#include "CCharacterDictator.h"
#include "INpcEventHandler.h"

//注册事件
#define REGIST_NPCEVENT(name) \
	do{\
	CNpcEventMetaData *pData = new CNpcEventMetaData(eNpcEvent_##name, #name); \
	if(!CNpcEventMetaDataMgr::GetInst()->AddEntity(pData)) \
		{\
		ostringstream strm;\
		strm<<"重复注册Npc事件："<< #name << endl;\
		GenErr(strm.str()); \
		}\
	}while(0)

bool CNpcAI::RegistAllEvent()
{
	REGIST_NPCEVENT(OnStart);
	REGIST_NPCEVENT(OnStop);
	REGIST_NPCEVENT(OnSuspend);
	REGIST_NPCEVENT(OnDead);
	REGIST_NPCEVENT(OnCharacterInSight);
	REGIST_NPCEVENT(OnCharacterOutOfSight);
	REGIST_NPCEVENT(OnMoveEnded);
	REGIST_NPCEVENT(OnBeAttacked);
	REGIST_NPCEVENT(OnBeginAttack);
	REGIST_NPCEVENT(OnBeginRandomMove);
	REGIST_NPCEVENT(OnEnemyLost);
	REGIST_NPCEVENT(OnTargetOutWatchDis);
	REGIST_NPCEVENT(OnEnterBattleState);
	REGIST_NPCEVENT(OnLeaveBattleState);
	REGIST_NPCEVENT(OnMoveToDest);
	REGIST_NPCEVENT(OnWeaponCoolDownEnd);
	REGIST_NPCEVENT(OnAttackAnimationEnd);
	REGIST_NPCEVENT(OnSkillAnimationEnd);
	REGIST_NPCEVENT(OnSkillBegin);
	REGIST_NPCEVENT(OnSkillSuccessEnd);
	REGIST_NPCEVENT(OnSkillFailEnd);
	REGIST_NPCEVENT(OnExitChaseBack);
	REGIST_NPCEVENT(OnHpChanged);
	REGIST_NPCEVENT(OnBehurt);
	REGIST_NPCEVENT(OnMpChanged);
	REGIST_NPCEVENT(OnForbitMove);
	REGIST_NPCEVENT(OnForbitUseSkill);
	REGIST_NPCEVENT(OnForbitNormalAttack);
	REGIST_NPCEVENT(OnCanMove);
	REGIST_NPCEVENT(OnCanUseSkill);
	REGIST_NPCEVENT(OnCanNormalAttack);
	REGIST_NPCEVENT(OnCanActive);
	REGIST_NPCEVENT(OnMemberReady);
	REGIST_NPCEVENT(OnMemberAllReady);
	REGIST_NPCEVENT(OnMemberMoveTo);
	REGIST_NPCEVENT(OnEnterAttackEnemyState);
	REGIST_NPCEVENT(OnNeedToChangeTarget);
	REGIST_NPCEVENT(OnPatrolStateEnd);
	REGIST_NPCEVENT(OnDestoryBegin);
	REGIST_NPCEVENT(OnEnterPhase);
	REGIST_NPCEVENT(OnEnterWander);
	REGIST_NPCEVENT(OnTraceFailed);
	REGIST_NPCEVENT(OnTraceSuccess);
	REGIST_NPCEVENT(OnEnemyDead);
	REGIST_NPCEVENT(OnSpeedChange);
	REGIST_NPCEVENT(OnTaskDialogBegin);
	REGIST_NPCEVENT(OnTaskDialogAllEnd);
	REGIST_NPCEVENT(OnGroupMemberBeAttacked);
	REGIST_NPCEVENT(OnGroupMemberAllDead);
	REGIST_NPCEVENT(OnBeginAttackEnemy);
	REGIST_NPCEVENT(OnEnemyOutOfAttackRange);
	REGIST_NPCEVENT(OnInAttackScope);
	REGIST_NPCEVENT(OnExitTowerAttackSubMachine);
	REGIST_NPCEVENT(OnPlayInNpcSleepDimEyeSight);
	REGIST_NPCEVENT(OnPlayOutNpcSleepDimEyeSight);
	REGIST_NPCEVENT(OnNpcSleepBegin);
	REGIST_NPCEVENT(OnNpcSleepEnd);
	REGIST_NPCEVENT(OnLike);
	REGIST_NPCEVENT(OnAttract);
	REGIST_NPCEVENT(OnSeek);
	REGIST_NPCEVENT(OnAbhor);
	REGIST_NPCEVENT(OnFear);
	REGIST_NPCEVENT(OnCoward);
	REGIST_NPCEVENT(OnSleepy);
	REGIST_NPCEVENT(OnGregarious);
	REGIST_NPCEVENT(OnCharacterSetState);
	REGIST_NPCEVENT(OnCharacterDeleteState);
	REGIST_NPCEVENT(OnObjectInSight);
	REGIST_NPCEVENT(OnObjectOutOfSight);
	REGIST_NPCEVENT(OnOtherDead);
	REGIST_NPCEVENT(OnIdle);
	REGIST_NPCEVENT(OnEnterPersonality);
	REGIST_NPCEVENT(OnExitNature);
	REGIST_NPCEVENT(OnNpcNature2NormalAttack);
	REGIST_NPCEVENT(OnExitSubMachine);
	REGIST_NPCEVENT(OnSleepyIdle);
	REGIST_NPCEVENT(OnPlayerInSight);
	REGIST_NPCEVENT(OnHide);
	REGIST_NPCEVENT(OnGreet);
	REGIST_NPCEVENT(OnNpcInSight);
	REGIST_NPCEVENT(OnTargetChange);
	REGIST_NPCEVENT(OnExitAlertState);
	REGIST_NPCEVENT(OnFacialActionHappened);
	REGIST_NPCEVENT(OnFacialCold);
	REGIST_NPCEVENT(OnWarning);
	REGIST_NPCEVENT(OnChangeModel);
	REGIST_NPCEVENT(OnServantDoAttack);
	REGIST_NPCEVENT(OnTargetOutOfRange);
	REGIST_NPCEVENT(OnEnterAlertTarget);
	REGIST_NPCEVENT(OnLeaveAlertTarget);

	return true;
}
#undef REGIST_NPCEVENT

void CNpcAI::AddEventHandler(uint32 uEventID, INpcEventHandler* pHandler)
{
	MapEvent2Handler::iterator MapIter = m_mapEventHandler.find(uEventID);
	if (MapIter == m_mapEventHandler.end())
		m_mapEventHandler.insert(make_pair(uEventID, SetEventHandler()));
	SetEventHandler& setHandler = m_mapEventHandler[uEventID];
	setHandler.insert(pHandler);
}
void CNpcAI::RemoveEventHandler(uint32 uEventID, INpcEventHandler* pHandler)
{
	MapEvent2Handler::iterator MapIter = m_mapEventHandler.find(uEventID);
	if (MapIter == m_mapEventHandler.end())
		return;
	SetEventHandler& setHandler = m_mapEventHandler[uEventID];
	Ver(setHandler.erase(pHandler));
	if (setHandler.empty())
	{
		m_mapEventHandler.erase(MapIter);
	}
}

TPtRefee<CNpcAI, CNpcEventMsg>& CNpcAI::GetRefByEventMsg()
{
	return m_RefsByMsg;
}

void CNpcAI::DispatchEventToNpcAI(CNpcEventMsg* pEvent)
{
	DispatchEventToLisner(pEvent);
	DispatchEventToStateMachine(pEvent);
}

void CNpcAI::DispatchEventToStateMachine(CNpcEventMsg* pEvent)
{
	Ast (GetCurrentStateMachine());
	GetCurrentStateMachine()->OnEvent(pEvent);
}

void CNpcAI::DispatchEventToLisner(CNpcEventMsg* pEvent)
{
	if (GetCharacter()->CppIsLive() && m_mapEventHandler.find(pEvent->GetID()) != m_mapEventHandler.end())
	{
		const SetEventHandler& setHandler = m_mapEventHandler[pEvent->GetID()];
		for_each(setHandler.begin(), setHandler.end(), bind2nd(mem_fun(&INpcEventHandler::Handle), pEvent));
	}
}


