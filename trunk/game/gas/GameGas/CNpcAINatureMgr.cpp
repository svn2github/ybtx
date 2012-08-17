#include "stdafx.h"
#include "CNpcAINatureMgr.h"
#include "TSqrAllocator.inl"

MapString2Nature CNpcNatureMgr::map_s2Nature;
MapEvent2String CNpcNatureMgr::map_EventToNature;

template class TPtRefee<CNpcNatureMgr,CNpcAI>;


CNpcNatureMgr::CNpcNatureMgr(CNpcAI* pAI)
:m_pAI(pAI)
,m_pNature(NULL)
,m_bDisableNature(true)
,m_bColdTimeEnd(true)
{
	if (map_s2Nature.empty())
	{
		map_s2Nature["Œ¸“˝"]		= eNP_Attract;
		map_s2Nature["œ≤∫√"]		= eNP_Like;            
		map_s2Nature["—·∂Ò"]		= eNP_Abhor;        
		map_s2Nature["«”≈≥"]		= eNP_Coward;       
		map_s2Nature[" »ÀØ"]		= eNP_Sleepy;       
		map_s2Nature["œ‡æ€"]		= eNP_Greet;
	}
	if (map_EventToNature.empty())
		InitNatureInterest();

	m_pNatureMgrTick = new NatureMgrTick(this);
	m_RefsByNpcAI.SetHolder(this);
	pAI->SetNpcNatureMgrHolder(this);
}

CNpcNatureMgr::~CNpcNatureMgr()
{
	SetNature(NULL);
	if (m_pNatureMgrTick)
	{
		GetNpcAI()->GetCharacter()->UnRegistDistortedTick(m_pNatureMgrTick);
		delete m_pNatureMgrTick;
		m_pNatureMgrTick = NULL;
	}
	GetNpcAI()->SetNpcNatureMgrHolder(NULL);
}

void CNpcNatureMgr::AfterSatisfied()
{
	if (NULL == GetNature())
		return;
	string& act = GetNature()->GetAction();
	if (act == "doskill")
	{
		if (GetNature()->GetLevel() != 0 && GetNature()->GetActionArg() != "")
		{
			GetNpcAI()->GetCharacter()->DoNPCSkill(GetNature()->GetActionArg().c_str(), GetNpcAI()->GetCharacter());
			//cout<<name<<" : "<<GetNature()->GetAction()<<" : "<<GetNature()->GetActionArg()<<" Level: "<<GetNature()->GetLevel()<<endl;
		}
	}
}

CNpcNature* CNpcNatureMgr::CreateNpcNature(const TCHAR* sNatureName, const TCHAR* sNatureArg)
{
	switch(map_s2Nature[sNatureName])
	{
	case eNP_Attract:
		return new CNpcNatureAttract(sNatureArg, GetNpcAI());
	case eNP_Like:
		return new CNpcNatureLike(sNatureArg, GetNpcAI());
	case eNP_Abhor:
		return new CNpcNatureAbhor(sNatureArg, GetNpcAI());
	case eNP_Coward:
		return new CNpcNatureCoward(sNatureArg, GetNpcAI());
	case eNP_Sleepy:
		return new CNpcNatureSleepy(sNatureArg, GetNpcAI());
	case eNP_Greet:
		return new CNpcNatureGreet(sNatureArg, GetNpcAI());
	}
	return NULL;
}



void CNpcNatureMgr::Handle(CNpcEventMsg* pEvent)
{
	if (!m_bDisableNature || !m_bColdTimeEnd) 
		return;
	
	string szNatureName = EventListenedNature(pEvent->GetID());

	if (szNatureName.empty())
	{
		return;
	}

	if (GetNature())
	{
		SetNature(NULL);
	}

	SetNature(CNpcNatureMgr::CreateNpcNature(szNatureName.c_str(), m_sNatureData.szNatureArg.c_str()));
	GetNature()->SetColdTime(m_sNatureData.uColdTime);
	GetNature()->SetExtraArg(m_sNatureData.uNatureExtraArg);
	GetNature()->SetAction(m_sNatureData.szAction.c_str());
	GetNature()->SetActionArg(m_sNatureData.szActionArg.c_str());
	GetNature()->SetLevel(m_sNatureData.uLevel);
	GetNature()->SetNatureAction(m_sNatureData.szNatureAction.c_str());
	uint32 event_id = pEvent->GetID();
	GetNature()->SetEvent( ENpcEvent(event_id), (size_t)pEvent->GetTag());	
	CNpcEventMsg::Create(GetNpcAI(), eNpcEvent_OnEnterPersonality, eNECI_Zero, reinterpret_cast<void*>(event_id));
	NatureColdBegin();
}

void CNpcNatureMgr::NatureColdBegin()
{
	m_bColdTimeEnd = false;
	GetNpcAI()->GetCharacter()->RegistDistortedTick(m_pNatureMgrTick, GetNature()->GetColdTime() * 1000);
}

void CNpcNatureMgr::NatureColdEnd()
{
	m_bColdTimeEnd = true;
}

void CNpcNatureMgr::DisableNature()
{
	m_bDisableNature = false;	
}

void CNpcNatureMgr::EnableNature()
{
	m_bDisableNature = true;
}

bool CNpcNatureMgr::CNpcSatisfied()
{
	return m_bDisableNature == false;
}

void CNpcNatureMgr::SetNature(CNpcNature* pNature)
{
	if (pNature)
	{
		m_pNature = pNature;
	}
	else
	{
		delete m_pNature;
		m_pNature = NULL;
	}
}

CNpcNature* CNpcNatureMgr::GetNature()
{	
	return m_pNature;
}

CNpcEventMsg* CNpcNatureMgr::Dispatch(CNpcEventMsg* pEvent)
{
	CNpcNature* pNature = GetNature();
	if (NULL != pNature)
		return pNature->DoEvent(pEvent);
	return NULL;
}

bool CNpcNatureMgr::Interest(CNpcNature* pNature, uint32 uEventID)
{
	if (!pNature)
		return false;
	return pNature->Interest(uEventID);
}

void CNpcNatureMgr::SetNatureData(const string& szNatureName, const string& szNatureArg, uint32 uNatureExtraArg, 
								  const string& szNatureAction, uint16 uColdTime, const string& szAction, const string& szActionArg, uint16 uLevel)
{
	CNpcNatureData		s;
	s.szNatureArg		= szNatureArg;
	s.uNatureExtraArg	= uNatureExtraArg;
	s.szNatureAction	= szNatureAction;
	s.uColdTime			= uColdTime;
	s.szAction			= szAction;
	s.szActionArg		= szActionArg;
	s.uLevel			= uLevel;

	m_mapNpcNatureData[szNatureName] = s;
}

bool CNpcNatureMgr::IsEventListend(ENpcEvent e)
{
	for(size_t i = 0; i < map_EventToNature[e].size(); ++i)
	{
		MapString2Data::iterator it = m_mapNpcNatureData.find(map_EventToNature[e][i]);
		if (it != m_mapNpcNatureData.end())
			return true;
	}
	return false;
}

string CNpcNatureMgr::EventListenedNature(ENpcEvent e)
{
	string szNatureName = "";
	for(size_t i = 0; i < map_EventToNature[e].size(); ++i)
	{
		MapString2Data::iterator iter = m_mapNpcNatureData.find(map_EventToNature[e][i]);
		if (iter != m_mapNpcNatureData.end())
		{
			m_sNatureData = iter->second;
			szNatureName = iter->first;
		}
	}
	return szNatureName;
}

void CNpcNatureMgr::InitNatureInterest()
{
	VecStringNature obj_insight_vec;
	obj_insight_vec.push_back("œ≤∫√");
	obj_insight_vec.push_back("Œ¸“˝");
	obj_insight_vec.push_back("—·∂Ò");
	map_EventToNature.insert(make_pair(eNpcEvent_OnObjectInSight, obj_insight_vec));

	VecStringNature npc_insight_vec;
	npc_insight_vec.push_back("œ‡æ€");
	map_EventToNature.insert(make_pair(eNpcEvent_OnNpcInSight, npc_insight_vec));

	VecStringNature sleep_vec;
	sleep_vec.push_back(" »ÀØ");
	map_EventToNature.insert(make_pair(eNpcEvent_OnSleepyIdle, sleep_vec));

	VecStringNature other_dead_vec;
	other_dead_vec.push_back("«”≈≥");
	map_EventToNature.insert(make_pair(eNpcEvent_OnOtherDead, other_dead_vec));
}
