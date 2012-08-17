#include "stdafx.h"
#include "CAllStateMgr.h"
#include "CSpecialState.h"
#include "CSpecialStateCfg.h"
#include "CMagicEffServer.h"
#include "CFighterDictator.h"
#include "CSkillInstServer.h"
#include "CSpecialStateMgr.h"
#include "ErrLogHelper.h"
#include "BaseHelper.h"
#include "CConnServer.h"
#include "FighterProperty.inl"
#include "DebugHelper.h"
#include "TSqrAllocator.inl"

using namespace std;






//以下是CSpecialStateBase的操作
CSpecialStateBase::CSpecialStateBase(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
	CSpecialStateMgrServer* pMgr, const CSpecialStateCfgServerSharedPtr& pCfg, int32 iTime, int32 iRemainTime)
:CBaseStateServer(pSkillInst, pInstaller, iTime, iRemainTime)
,m_pMgr(pMgr)
,m_pCfg(new CSpecialStateCfgServerSharedPtr(pCfg))
,m_pStateEventBundle(NULL)
{	
	if(!GetCfgSharedPtr()->GetModelStr().empty()) GetAllMgr()->SetModelStateId(GetCfgSharedPtr()->GetId());
	m_pSkillInst->AddRef();
}

CSpecialStateBase::~CSpecialStateBase()
{
	CAllStateMgrServer* pAllMgr = GetAllMgr();
	pAllMgr->ChangeSizeRate(GetCfg(), -1);

	//是否考虑把DetachInstaller()的调入语句全部移入这里？
	if(!GetCfgSharedPtr()->GetModelStr().empty()) pAllMgr->ClearModelStateId();
	m_pSkillInst->DelRef();
	//需要检测这里是否能进入
	SafeDelete(m_pCfg);
}



void CSpecialStateBase::Start(bool bFromDB)
{
#ifdef COUT_STATE_INFO
	cout << "新建特殊状态并开始计时\n";
#endif

	StartDo();
	StartTime(bFromDB);
}

void CSpecialStateBase::StartDo()
{
	GetAllMgr()->ChangeSizeRate(GetCfg(), 1);
}

void CSpecialStateBase::StartTime(bool bFromDB)
{
	if(!bFromDB)
	{
		m_iTime = GetCfgSharedPtr()->GetTime()->GetIntValue(GetInstaller(), GetOwner(), m_pSkillInst->GetLevelIndex());

		if (GetInstaller() != GetOwner() && (m_iTime != -1))
		{
			switch(GetCfgSharedPtr()->GetDecreaseType())
			{
			case eDST_Control:
				m_iTime = uint32(m_iTime * GetOwner()->m_ControlDecreaseTimeRate.Get(GetOwner()) + 0.5f);
				break;
			case eDST_Pause:
				m_iTime = uint32(m_iTime * GetOwner()->m_PauseDecreaseTimeRate.Get(GetOwner()) + 0.5f);
				break;
			case eDST_Crippling:
				m_iTime = uint32(m_iTime * GetOwner()->m_CripplingDecreaseTimeRate.Get(GetOwner()) + 0.5f);
				break;
			case eDST_Special:
				m_iTime = uint32(m_iTime * GetOwner()->m_SpecialDecreaseTimeRate.Get(GetOwner()) + 0.5f);
				break;
			default:
				break;
			}
		}
	}

	if(m_iTime < -1)
	{
		stringstream ExpStr;
		ExpStr << "特殊状态的作用时间不能小于-1：" << m_iTime << endl;
		GenErr("特殊状态的作用时间不能小于-1", ExpStr.str());
		return;
	}
	if(!bFromDB)
	{
		m_iRemainTime = m_iTime;
	}

	m_uStartTime = GetOwner()->GetDistortedFrameTime();

	if (m_iRemainTime >= 0)
	{
		GetOwner()->RegistDistortedTick(this, m_iRemainTime * 1000);
	}
};

void CSpecialStateBase::OnTick()
{
#ifdef COUT_STATE_INFO
	cout << "因时间到了而删除自己\n";
#endif

	DeleteSelf();
}

void CSpecialStateBase::DeleteSelf()
{
	PrepareDeleteSelf();
	//以下为删除自身的操作，注意这句必须放在函数最末尾
	DeleteItr();
	delete this;
}

void CSpecialStateBase::DeleteSelfExceptItr()
{
	PrepareDeleteSelf();
	//以下为删除自身的操作，注意这句必须放在最末尾
	delete this;
}


void CSpecialStateBase::PrepareDeleteSelf()
{
	//GetOwner()->UnRegistDistortedTick(this);

	if (m_listSSItr != m_pStateBundleByType->m_listState.end()) m_pStateBundleByType->m_listState.erase(m_listSSItr);

	//自定义观察者模式Detach
	if(m_pStateEventBundle && m_listStateItr != m_pStateEventBundle->m_listState.end()) m_pStateEventBundle->m_listState.erase(m_listStateItr);

	uint32 uId = GetCfgSharedPtr()->GetId();
	CAllStateMgrServer* pAllMgr = GetAllMgr();
	pAllMgr->OnDeleteState(uId, m_uDynamicId);
	pAllMgr->DelStateByDId(m_uDynamicId);

	//为ClearAll的中途ExistState等函数准备的（ExistState等函数要检测itr->second != NULL）
	if (m_mapSSItr != m_pMgr->m_mapState.end()) m_mapSSItr->second = NULL;

	pAllMgr->PrintFightInfo(false, GetInstallerID(), GetCfgSharedPtr().get());
}

void CSpecialStateBase::DeleteItr()
{
	if (m_mapSSItr != m_pMgr->m_mapState.end()) m_pMgr->m_mapState.erase(m_mapSSItr);
}

//uint32 CSpecialStateBase::GetDynamicId()
//{
//	return m_pCfg->GetId();
//}





CFighterDictator* CSpecialStateBase::GetOwner()
{
	return m_pMgr->m_pOwner;
}

CAllStateMgrServer* CSpecialStateBase::GetAllMgr()
{
	return m_pMgr->GetAllMgr();
}

void CSpecialStateBase::RegisterEvent()
{
	m_pStateEventBundle = NULL;
}



CBaseStateCfgServer* CSpecialStateBase::GetCfg()
{
	return m_pCfg->get();
}

CSpecialStateCfgServerSharedPtr& CSpecialStateBase::GetCfgSharedPtr()const
{
	return *m_pCfg;
}


//以下是CReflectStateServer的操作
CReflectStateServer::CReflectStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
	CSpecialStateMgrServer* pMgr, const CSpecialStateCfgServerSharedPtr& pCfg, int32 iTime, int32 iRemainTime)
: CSpecialStateBase(pSkillInst, pInstaller, pMgr, pCfg, iTime, iRemainTime) 
{

}

//以下是CRepeatStateServer
CRepeatStateServer::CRepeatStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
	CSpecialStateMgrServer* pMgr, const CSpecialStateCfgServerSharedPtr& pCfg, int32 iTime, int32 iRemainTime)
: CSpecialStateBase(pSkillInst, pInstaller, pMgr, pCfg, iTime, iRemainTime) 
{

}

CDeadBodyStateServer::CDeadBodyStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
	CSpecialStateMgrServer* pMgr, const CSpecialStateCfgServerSharedPtr& pCfg, int32 iTime, int32 iRemainTime)
: CSpecialStateBase(pSkillInst, pInstaller, pMgr, pCfg, iTime, iRemainTime) 
{

}

