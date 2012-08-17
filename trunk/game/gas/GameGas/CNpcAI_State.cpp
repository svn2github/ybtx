#include "stdafx.h"
#include "CNpcAI.h"
#include "CNpcStateMetaData.h"

//×¢²á×´Ì¬
#define REGIST_NPCSTATE(name) \
	do{\
	CNpcStateMetaData *pData = new CNpcStateMetaData(CNpcStateMetaDataMgr::GetInst()->GetSize(), #name, new CNpcStateCreator(&CNpcState##name::Create)); \
	if (!CNpcStateMetaDataMgr::GetInst()->AddEntity(pData)) \
		{\
		ostringstream strm;\
		strm<<"ÖØ¸´×¢²áNpc×´Ì¬£º"<< #name << endl;\
		GenErr(strm.str()); \
		}\
	}while(0)

bool CNpcAI::RegistAllState()
{
	REGIST_NPCSTATE(ExitSubMachine);
	REGIST_NPCSTATE(Idle);
	REGIST_NPCSTATE(Wander);
	REGIST_NPCSTATE(ActiveState);
	REGIST_NPCSTATE(LockEnemy);
	REGIST_NPCSTATE(ChaseEnemy);
	REGIST_NPCSTATE(AttackEnemy);
	REGIST_NPCSTATE(ChaseBack);
	REGIST_NPCSTATE(Task);
	REGIST_NPCSTATE(Building);
	REGIST_NPCSTATE(PatrolForNpc);
	REGIST_NPCSTATE(ActiveBossInit);
	REGIST_NPCSTATE(PassivityBossInit);
	REGIST_NPCSTATE(BossPatrol);
	REGIST_NPCSTATE(Theater);
	REGIST_NPCSTATE(DoNothingState);
	REGIST_NPCSTATE(PatrolForGroupMember);
	REGIST_NPCSTATE(PatrolForGroupLeader);

	REGIST_NPCSTATE(TotemCaughtCharacter);
	REGIST_NPCSTATE(TotemLostCharacter);
	REGIST_NPCSTATE(NpcSleep);

	REGIST_NPCSTATE(InitPersonality);
	REGIST_NPCSTATE(LikeState);
	REGIST_NPCSTATE(AbhorState);
	REGIST_NPCSTATE(CowardState);
	REGIST_NPCSTATE(SleepyState);
	REGIST_NPCSTATE(GreetState);
	REGIST_NPCSTATE(AttractState);

	REGIST_NPCSTATE(FacialState);
	REGIST_NPCSTATE(FacialColdState);
	REGIST_NPCSTATE(AlertState);
	REGIST_NPCSTATE(TowerWander);
	REGIST_NPCSTATE(TowerLockEnemy);
	REGIST_NPCSTATE(TowerLeaveBattle);
	REGIST_NPCSTATE(DeathOrDestoryState);
	REGIST_NPCSTATE(EscapeState);
	REGIST_NPCSTATE(LittlePet);
	//ÕÙ»½ÊÞÏà¹Ø
	REGIST_NPCSTATE(ServantDefenseState);
	REGIST_NPCSTATE(ServantActiveState);
	REGIST_NPCSTATE(ServantPassivityState);

	return true;
}
#undef REGIST_NPCSTATE

const string& CNpcAI::GetCurrentStateName() const
{
	return CNpcStateMetaDataMgr::GetInst()->GetEntity(GetCurrentStateMachine()->GetCurrentState()->GetID())->GetName();
}

CNpcEventMsg* CNpcAI::PushSubMachine(CNpcStateMachine* pStateMachine, CNpcEventMsg* pEvent)
{
	if (!pEvent)
		return NULL;
	GetCurrentStateMachine()->Suspend(pEvent);
	m_lstStateMachineStack.push_back(pStateMachine);
	GetCurrentStateMachine()->Start(pEvent);

	return NULL;
}

CNpcEventMsg* CNpcAI::PopSubMachine(CNpcEventMsg* pEvent)
{
	//²»É¾³ý×îºóÒ»¸ö×´Ì¬»ú
	if (m_lstStateMachineStack.size() <= 1)
		return pEvent;

	GetCurrentStateMachine()->Stop(pEvent);
	delete m_lstStateMachineStack.back();
	m_lstStateMachineStack.pop_back();
	GetCurrentStateMachine()->Resume(pEvent);
	GetCurrentStateMachine()->OnEvent(pEvent);

	return pEvent;
}

void CNpcAI::DeleteNpcAIStateMachine()
{
	CNpcEventMsg *pEvent = CNpcEventMsg::Create(this,eNpcEvent_OnStop);
	if(!m_lstStateMachineStack.empty())
	{
		for (;;)
		{
			CNpcStateMachine* pStateMachine = m_lstStateMachineStack.back();
			pStateMachine->Stop(pEvent);
			delete pStateMachine;
			m_lstStateMachineStack.pop_back();

			if (m_lstStateMachineStack.empty())
				break;

			m_lstStateMachineStack.back()->Resume(pEvent);
		}
	}
}

