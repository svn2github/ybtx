#include "stdafx.h"
#include "CSkillRule.h"
#include "CSkillRuleMgr.h"
#include "CSkillRuleServer.h"
#include "CSkillRuleBaseData.h"
#include "PatternRef.inl"
#include "CNpcAI.h"
#include "CNpcEventMsg.h"
#include "CCharacterDictator.h"
#include "CPhaseChgRule.h"
#include "CTargetChangeRule.h"
#include "CExclaimRule.h"
#include "CMultiSkillRule.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "ErrLogHelper.h"
#include "CSkillAroundRule.h"
#include "CGoBackExclaimRule.h"
#include "CDoSceneSkillRule.h"
#include "CAlertExclaimRule.h"
#include "CNotNpcTargetChangeRule.h"
#include "CDoSkillOnStateCondRule.h"

template class TPtRefer<CSkillRuleMgr,CNpcAI>;

CSkillRuleMgr::CSkillRuleMgr(CNpcAI* pAI) : m_pOwner(pAI)
{
	m_uCurrentPage = 1;
	m_pSkillAnimaionTick = NULL;
	m_pCurrentSkill = NULL;
	m_fCurrentHp = GetCharacter()->CppGetMaxHP();
	m_fFrontHp = m_fCurrentHp;
	m_fCurrentMp = GetCharacter()->CppGetMaxMP();
	m_fFrontMp = m_fCurrentMp;
	m_bSkillMgrActive = false;
	m_iHpChange = 0;
	m_RefsByNpcAI.SetHolder(this);
	pAI->SetSkillRuleMgrHolder(this);
	pAI->GetFightBaseData()->GetSkillRuleName(m_strSkillRuleName);

	MakeSkillRuleData();
	InitWanderAndDeadSkillRule();
}

CSkillRuleMgr::~CSkillRuleMgr()
{
	if (m_pSkillAnimaionTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pSkillAnimaionTick);
		delete m_pSkillAnimaionTick;
		m_pSkillAnimaionTick = NULL;
	}
	for (MapSkillRulePage::iterator iter = m_mapSkilRuleByPage.begin();iter!=m_mapSkilRuleByPage.end();++iter)
	{
		ListSkillRule lstSkillRule = iter->second;
		for (ListSkillRule::iterator iterSR = lstSkillRule.begin();iterSR!=lstSkillRule.end();)
		{
			delete (*iterSR);
			lstSkillRule.erase(iterSR++);
		}
	}
	m_mapSkilRuleByPage.clear();
	for (ListSkillRule::iterator iter = m_lstWanderSkill.begin();iter != m_lstWanderSkill.end();)
	{
		delete(*iter);
		m_lstWanderSkill.erase(iter++);
	}
	for (ListSkillRule::iterator iter = m_lstDeadSkill.begin(); iter != m_lstDeadSkill.end();)
	{
		delete (*iter);
		m_lstDeadSkill.erase(iter++);
	}
	m_lstBattleSkillRuleData.clear();
	m_lstPhaseBattleSkillRuleData.clear();
	m_lstWanderAndDeadSkillRuleData.clear();
}

CNpcAI* CSkillRuleMgr::GetOwner() const
{
	return m_pOwner;
}

CCharacterDictator* CSkillRuleMgr::GetCharacter()const
{
	return m_pOwner->GetCharacter();
}

void CSkillRuleMgr::MakeSkillRuleData()
{
	bool bResult = CSkillRuleServer::IsHaveSkillRule(m_strSkillRuleName);
	if(!bResult)
		return;
	LstRuleDataType::iterator iter = CSkillRuleServer::GetSkillRuleDataByName(m_strSkillRuleName)->begin();
	LstRuleDataType::iterator iterEnd = CSkillRuleServer::GetSkillRuleDataByName(m_strSkillRuleName)->end();
	for (;iter != iterEnd;++iter)
	{
		if ((*iter)->m_iSkillPage == 0)
		{
			m_lstBattleSkillRuleData.push_back(*iter);
		}
		else if((*iter)->m_iSkillPage == -1)
		{
			m_lstWanderAndDeadSkillRuleData.push_back(*iter);
		}
		else
		{
			m_lstPhaseBattleSkillRuleData.push_back(*iter);
		}
	}
}


void CSkillRuleMgr::InstallBattleSkillRule()
{
	if (m_bSkillMgrActive)
		return;

	m_fCurrentHp = GetCharacter()->CppGetMaxHP();
	m_fFrontHp = m_fCurrentHp;
	m_fCurrentMp = GetCharacter()->CppGetMaxMP();
	m_fFrontMp = m_fCurrentMp;
	InitBattleSkillRule();
	MapID2SkillRulePage::iterator iter = m_mapEvent2RulePage.begin();
	for (;iter!=m_mapEvent2RulePage.end();++iter)
	{
		uint32 uEventID = iter->first;
		GetOwner()->AddEventHandler(uEventID,this);
	}
	m_uCurrentPage = 1;
	m_bSkillMgrActive = true;
	GetOwner()->SetNormalAttackFirstTime(true);
}

void CSkillRuleMgr::UnInstallBattleSkillRule()
{
	if (!m_bSkillMgrActive)
		return;
	MapID2SkillRulePage::iterator iter = m_mapEvent2RulePage.begin();
	for (;iter!=m_mapEvent2RulePage.end();++iter)
	{
		uint32 uEventID = iter->first;
		GetOwner()->RemoveEventHandler(uEventID,this);
	}
	m_mapEvent2RulePage.clear();
	if(m_pCurrentSkill)
		m_pCurrentSkill->StopDoMoveSkill();
	m_pCurrentSkill = NULL;
	UnInstallPhase(0);
	UnInstallPhase(m_uCurrentPage);
	m_uCurrentPage = 1;
	if (m_pSkillAnimaionTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pSkillAnimaionTick);
		SafeDelete(m_pSkillAnimaionTick);
	}
	GetOwner()->SetIsDoingSkillRule(false);

	for (MapSkillRulePage::iterator iter = m_mapSkilRuleByPage.begin();iter!=m_mapSkilRuleByPage.end();++iter)
	{
		ListSkillRule& lstSkillRule = iter->second;
		for (ListSkillRule::iterator iterSR = lstSkillRule.begin();iterSR!=lstSkillRule.end();)
		{
			delete (*iterSR);
			lstSkillRule.erase(iterSR++);
		}
	}
	m_mapSkilRuleByPage.clear();
	m_bSkillMgrActive = false;
}

void CSkillRuleMgr::InstallWanderSkillRule()
{

	MapID2SkillRule::iterator iter = m_mapWanderRuleHandler.begin();
	for (;iter!=m_mapWanderRuleHandler.end();++iter)
	{
		uint32 uEventID = iter->first;
		GetOwner()->AddEventHandler(uEventID,this);
	}
}

void CSkillRuleMgr::UnInstallWanderSkillRule()
{
	MapID2SkillRule::iterator iter = m_mapWanderRuleHandler.begin();
	for (;iter != m_mapWanderRuleHandler.end();++iter)
	{
		uint32 uEventID = iter->first;
		GetOwner()->RemoveEventHandler(uEventID,this);
	}
	for (ListSkillRule::iterator iterSR = m_lstWanderSkill.begin();iterSR != m_lstWanderSkill.end();++iterSR)
	{
		(*iterSR)->UnInstallBeginCond();
		(*iterSR)->UnInstallContinueCond();
	}
	for (ListSkillRule::iterator iter = m_lstWanderSkill.begin();iter != m_lstWanderSkill.end();)
	{
		delete(*iter);
		m_lstWanderSkill.erase(iter++);
	}
}

void CSkillRuleMgr::ExecuteDeadSkill()
{

	for (ListSkillRule::iterator iter = m_lstDeadSkill.begin();iter != m_lstDeadSkill.end();++iter)
	{
		(*iter)->DoNoRuleSkill();
	}
}

CSkillRule* CSkillRuleMgr::CreateSkillRule(ESkillRuleTypeName eRuleType,CSkillRuleData* pSkillRuleData)
{
	CSkillRule* pSkillRule = NULL;
	switch(eRuleType)
	{
	case eSKRT_Normal:
		pSkillRule = new CSkillRule(this, pSkillRuleData);
		break;
	case eSKRT_PhaseChange:
		pSkillRule = new CPhaseChangeRule(this,pSkillRuleData);
		break;
	case eSKRT_TargetChange:
		pSkillRule = new CTargetChangeRule(this,pSkillRuleData);
		break;
	case eSKRT_NotNpcTargetChange:
		pSkillRule = new CNotNpcTargetChangeRule(this,pSkillRuleData);
		break;
	case eSKRT_Exclaim:
		pSkillRule = new CExclaimRule(this,pSkillRuleData, true);
		break;
	case eSKRT_GoBackExclaim:
		pSkillRule = new CGoBackExclaimRule(this, pSkillRuleData);
		break;
	case eSKRT_CMultiSkill:
		pSkillRule = new CMultiSkillRule(this,pSkillRuleData);
		break;
	case eSKRT_SkillAround:
		pSkillRule = new CSkillAroundRule(this, pSkillRuleData);
		break;
	case eSKRT_Exclaim_NoEffect:
		pSkillRule = new CExclaimRule(this,pSkillRuleData, false);
		break;
	case eSKRT_AlertExclaim:
		pSkillRule = new CAlertExclaimRule(this, pSkillRuleData);
		break;
	case eSKRT_DoSceneSkill:
		pSkillRule = new CDoSceneSkillRule(this, pSkillRuleData);
		break;
	case eSKRT_DoSkillOnStateCond:
		pSkillRule = new CDoSkillOnStateCondRule(this, pSkillRuleData);
		break;

	default:
		break;
	}
	return pSkillRule;
}

void CSkillRuleMgr::InitBattleSkillRule()
{
	LstRuleDataType::const_iterator iter = m_lstBattleSkillRuleData.begin();
	LstRuleDataType::const_iterator iterEnd = m_lstBattleSkillRuleData.end();
	for (;iter != iterEnd;++iter)
	{
		CSkillRuleData* pSkillRuleData = *iter;
		ESkillRuleTypeName eRuleType = pSkillRuleData->m_eSRTypeName;
		CSkillRule* pSkillRule = CreateSkillRule(eRuleType, (*iter));
		m_mapSkilRuleByPage[pSkillRule->m_iSkillPage].push_back(pSkillRule);
	}

	LstRuleDataType::const_iterator Phaseiter = m_lstPhaseBattleSkillRuleData.begin();
	LstRuleDataType::const_iterator PhaseiterEnd = m_lstPhaseBattleSkillRuleData.end();
	for (;Phaseiter != PhaseiterEnd;++Phaseiter)
	{
		CSkillRuleData* pSkillRuleData = *Phaseiter;
		ESkillRuleTypeName eRuleType = pSkillRuleData->m_eSRTypeName;
		CSkillRule* pSkillRule = CreateSkillRule(eRuleType, (*Phaseiter));
		m_mapSkilRuleByPage[pSkillRule->m_iSkillPage].push_back(pSkillRule);
	}
	MapSkillRulePage::const_iterator iter1 = m_mapSkilRuleByPage.begin();
	for (;iter1 != m_mapSkilRuleByPage.end();++iter1)
	{
		ListSkillRule lstSkillRule = iter1->second;
		for (ListSkillRule::iterator iterSR = lstSkillRule.begin();iterSR!=lstSkillRule.end();++iterSR)
		{
			(*iterSR)->CreateEventHandler();
		}
	}
}

void CSkillRuleMgr::InitWanderAndDeadSkillRule()
{
	LstRuleDataType::const_iterator iter = m_lstWanderAndDeadSkillRuleData.begin();
	LstRuleDataType::const_iterator iterEnd = m_lstWanderAndDeadSkillRuleData.end();
	for (; iter != iterEnd; iter++)
	{
		CSkillRule* pSkillRule = new CSkillRule(this, (*iter));
		if (pSkillRule->GetSkillRuleKind() == eSRK_Wander)
		{
			m_lstWanderSkill.push_back(pSkillRule);
			pSkillRule->CreateEventHandler();
		}
		else
		{
			m_lstDeadSkill.push_back(pSkillRule);
		}
	}
}

bool CSkillRuleMgr::ExecuteSkill()
{
	bool bSuccess = false;
	if(!m_pCurrentSkill && m_lstPreparedSkill.size() != 0)
	{
		m_pCurrentSkill = m_lstPreparedSkill.front();
		m_lstPreparedSkill.pop_front();
		bSuccess = m_pCurrentSkill->CheckAndDoSkill();
		//cout<<"npc do skill : "<<m_pCurrentSkill->m_strSkillName<<endl;
	}
	return bSuccess;
}

void CSkillRuleMgr::Handle(CNpcEventMsg* pEvent)
{
	ENpcEvent eEvent = pEvent->GetID();
	OnEvent(eEvent);
}

void CSkillRuleMgr::OnEvent(ENpcEvent eEvent)
{
	switch(eEvent)
	{
	case eNpcEvent_OnHpChanged:
		CalculateHp();
		break;
	case eNpcEvent_OnMpChanged:
		CalculateMp();
	    break;
	case eNpcEvent_OnEnterBattleState:
		m_uStartBattleTime = GetCharacter()->GetDistortedFrameTime();
		break;
	case eNpcEvent_OnEnterWander:
		{
			for (ListSkillRule::iterator iter = m_lstWanderSkill.begin();iter != m_lstWanderSkill.end();iter++)
			{
				(*iter)->Handle(eEvent);
			}
			return;
		}
		break;
	default:
	    break;
	}
	SQR_TRY
	{
		if (m_mapEvent2RulePage.count(eEvent))
		{
			MapSkillRulePage& lstSkillPage = m_mapEvent2RulePage[eEvent];
			ListSkillRule& lstSkillRule = lstSkillPage[0];
			ListSkillRule::iterator iter = lstSkillRule.begin();
			for (;iter != lstSkillRule.end();iter++)
			{
				(*iter)->Handle(eEvent);
				if (m_mapEvent2RulePage.empty())
					return;
			}

			ListSkillRule& lstPhaseSkillRule = lstSkillPage[m_uCurrentPage];
			ListSkillRule::iterator iter1 = lstPhaseSkillRule.begin();
			for (;iter1 != lstPhaseSkillRule.end();iter1++)
			{
				(*iter1)->Handle(eEvent);
				if (m_mapEvent2RulePage.empty())
					return;
			}
		}
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;
		strm<<"NPC Name【" << GetOwner()->m_pBaseData->GetName() << "】" <<endl;
		exp.AppendMsg(strm.str().c_str());
		LogExp(exp);
	}
	SQR_TRY_END;
}

void CSkillRuleMgr::ChangeNextPhase()
{
	UnInstallPhase(m_uCurrentPage);
	++m_uCurrentPage;
	if(!m_mapSkilRuleByPage.count(m_uCurrentPage))
		m_uCurrentPage = 1;
	InstallPhase(m_uCurrentPage);
	return;
}

void CSkillRuleMgr::InstallPhase(uint32 uPage)
{
	if(!m_mapSkilRuleByPage.count(uPage))
		return;
	ListSkillRule lstRule = m_mapSkilRuleByPage[uPage];
	ListSkillRule::iterator iter = lstRule.begin();
	for (;iter != lstRule.end();++iter)
	{
		(*iter)->LoadBeginCond();
	}
}

void CSkillRuleMgr::UnInstallPhase(uint32 uPage)
{
	if(!m_mapSkilRuleByPage.count(uPage))
		return;
	ListSkillRule lstRule = m_mapSkilRuleByPage[uPage];
	ListSkillRule::iterator iter = lstRule.begin();
	for (;iter != lstRule.end();++iter)
	{
		(*iter)->UnInstallBeginCond();
		(*iter)->UnInstallContinueCond();
	}
	
	if (uPage == 0)
		m_lstPreparedSkill.clear();
	else
		ClearPhasePreparedSkillList();
}

void CSkillRuleMgr::ClearPhasePreparedSkillList()
{
	ListSkillRule::iterator iter1 = m_lstPreparedSkill.begin();
	for (; iter1 != m_lstPreparedSkill.end(); )
	{
		if ((*iter1)->GetSkillRuleKind() != eSRK_Fight)
			m_lstPreparedSkill.erase(iter1++);
		else
			++iter1;
	}
}

void  CSkillRuleMgr::AddEventHandler(uint32 uEventID,CSkillRule* pSkillRule)
{
	if (uEventID == eNpcEvent_OnEnterWander)
	{
		m_mapWanderRuleHandler[uEventID].push_back(pSkillRule);
		return;
	}
	if (!m_mapEvent2RulePage.count(uEventID))
	{
		m_mapEvent2RulePage[uEventID][pSkillRule->m_iSkillPage].push_back(pSkillRule);
	}
	else
	{
		m_mapEvent2RulePage[uEventID][pSkillRule->m_iSkillPage].push_back(pSkillRule);
	}
}

void CSkillRuleMgr::AddPreparedSkillFront(CSkillRule* pSkillRule)
{
	if (!pSkillRule->SkillRuleCoolDown())
		return;
	m_lstPreparedSkill.push_back(pSkillRule);
	CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnEnterAttackEnemyState);
}

void CSkillRuleMgr::AddPreparedSkillBack(CSkillRule* pSkillRule, bool bRepeat)
{
	if (!pSkillRule->SkillRuleCoolDown())
		return;
	if (!bRepeat)	//如果不能重复
	{
		ListSkillRule::iterator iter = m_lstPreparedSkill.begin();
		for (; iter != m_lstPreparedSkill.end(); iter++)
		{
			if ((*iter) == pSkillRule)
				return;
		}
	}
	m_lstPreparedSkill.push_back(pSkillRule);
	CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnEnterAttackEnemyState);
}

void CSkillRuleMgr::CalculateHp()
{
	CCharacterDictator* pCharacter = GetCharacter();
	m_fFrontHp = m_fCurrentHp;
	m_fCurrentHp = pCharacter->CppGetHP();
}

void CSkillRuleMgr::CalculateMp()
{
	CCharacterDictator* pCharacter = GetCharacter();
	m_fFrontMp = m_fCurrentMp;
	m_fCurrentMp = pCharacter->CppGetMP();
}

void CSkillRuleMgr::OnBeHurt(int32 iHpChange)
{
	m_iHpChange = iHpChange;
	OnEvent(eNpcEvent_OnBehurt);
}

void CSkillRuleMgr::ClearCurSkill()
{
	if (m_pCurrentSkill)
	{
		m_pCurrentSkill->StopDoMoveSkill();
		m_pCurrentSkill = NULL;
	}
	DestorySkillAniTick();
}	

void CSkillRuleMgr::CreateSkillAniTick()
{
	struct AnimationTick
		: public CDistortedTick
		, public CNpcAIMallocObject 
	{
		AnimationTick(CSkillRuleMgr* pSkillMgr) : m_pSkillMgr(pSkillMgr) {}
		void OnTick()
		{
			m_pSkillMgr->GetCharacter()->UnRegistDistortedTick(this);
			Ast(m_pSkillMgr);
			Ast(m_pSkillMgr->GetOwner());
			CNpcEventMsg::Create(m_pSkillMgr->GetOwner(),eNpcEvent_OnSkillAnimationEnd);
		}
		CSkillRuleMgr* m_pSkillMgr;
	};

	if (!m_pCurrentSkill)
	{
		if (!GetOwner()->GetIsDoingSkillRule())
			return;
		else
		{
			//对于像主角召唤兽这样的Npc可能会被主角强行拉出战斗，但是Npc消息是异步的，所所以这里出现这种情况就通知脱离战斗
			CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnEnemyLost, eNECI_Thirteen);
			return;
		}
	}
	uint8 uSkillAniKeyFrame = GetOwner()->GetCharacter()->GetSkillCastAniFrameCnt(m_pCurrentSkill->m_strSkillName.c_str());
	if(uSkillAniKeyFrame == 0)
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnSkillAnimationEnd);
	else
	{
		if (!m_pSkillAnimaionTick)
			m_pSkillAnimaionTick = new AnimationTick(this);
		GetCharacter()->RegistDistortedTick(m_pSkillAnimaionTick, uSkillAniKeyFrame*33);
	}
}

void CSkillRuleMgr::DestorySkillAniTick()
{
	if (m_pSkillAnimaionTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pSkillAnimaionTick);
		SafeDelete(m_pSkillAnimaionTick);
	}
}
