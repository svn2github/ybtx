#include "stdafx.h"
#include "CCfgCalcFunction.h"
#include "CFighterDictator.h"
#include "CFighterMediator.h"
#include "CMagicState.h"
#include "CTriggerState.h"
#include "CDamageChangeState.h"
#include "CCumuliTriggerState.h"
#include "CSpecialState.h"
#include "CAllStateMgr.h"
#include "CMagicStateMgr.h"
#include "CTriggerStateMgr.h"
#include "CDamageChangeStateMgr.h"
#include "CCumuliTriggerStateMgr.h"
#include "CSpecialStateMgr.h"
#include "COtherStateMgr.inl"
#include "CPos.h"
#include "CoreCommon.h"
#include "CEntityServer.h"
#include "CCfgCalc.inl"
#include "CCharacterDictator.h"

template<>
double CCfgGlobal::Get<CFighterDictator>(const CFighterDictator* pFrom, const CFighterDictator* pTo, uint32 index)
{
	if(m_bIsArrayConst)
	{
		if(m_bDeqCalculated[index])
		{
			return m_dVecResult[index];
		}
		else
		{
			m_bDeqCalculated[index] = true;
			return m_dVecResult[index] = m_pCfgCalc->GetDblValue(pFrom, pTo, index);
		}
	}
	else
	{
		return m_pCfgCalc->GetDblValue(pFrom, pTo, index);
	}
}

template<>
bool CCfgGlobal::ExistMagicState<CFighterDictator>(string& name, const CFighterDictator* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "服务器ExistMagicState的pFighter参数为空";
		GenErr(ExpStr.str());
		return false;
	}
	return pFighter->GetAllStateMgr()->GetMagicStateMgrServer()->ExistState(name);
}

template<>
bool CCfgGlobal::ExistTriggerState<CFighterDictator>(string& name, const CFighterDictator* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "服务器ExistTriggerState的pFighter参数为空";
		GenErr(ExpStr.str());
		return false;
	}
	return pFighter->GetAllStateMgr()->GetTriggerStateMgrServer()->ExistState(name);
}

template<>
bool CCfgGlobal::ExistDamageChangeState<CFighterDictator>(string& name, const CFighterDictator* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "服务器ExistDamageChangeState的pFighter参数为空";
		GenErr(ExpStr.str());
		return false;
	}
	return pFighter->GetAllStateMgr()->GetDamageChangeStateMgrServer()->ExistState(name);
}

template<>
bool CCfgGlobal::ExistSpecialState<CFighterDictator>(string& name, const CFighterDictator* pFighter)
{
	if(!pFighter)
	{
		stringstream ExpStr;
		ExpStr << "服务器ExistSpecialState的pFighter参数为空";
		GenErr(ExpStr.str());
		return false;
	}
	return pFighter->GetAllStateMgr()->GetSpecialStateMgrServer()->ExistState(name);
}

template<>
bool CCfgGlobal::ExistState<CFighterDictator>(string& name, const CFighterDictator* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "服务器ExistState的pFighter参数为空";
		GenErr(ExpStr.str());
		return false;
	}

	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(name);
	if(pCfg)
	{
		switch(pCfg->GetGlobalType())
		{
		case eSGT_MagicState:
			return pFighter->GetAllStateMgr()->GetMagicStateMgrServer()->ExistState(name);
		case eSGT_TriggerState:
			return pFighter->GetAllStateMgr()->GetTriggerStateMgrServer()->ExistState(name);
		case eSGT_DamageChangeState:
			return pFighter->GetAllStateMgr()->GetDamageChangeStateMgrServer()->ExistState(name);
		case eSGT_CumuliTriggerState:
			return pFighter->GetAllStateMgr()->GetCumuliTriggerStateMgrServer()->ExistState(name);
		case eSGT_SpecialState:
			return pFighter->GetAllStateMgr()->GetSpecialStateMgrServer()->ExistState(name);
		default:
			{
				stringstream ExpStr;
				ExpStr << "服务器ExistState(" << name << ")类型" << pCfg->GetGlobalType() << "不正确\n";
				GenErr("服务器ExistState名称类型不正确", ExpStr.str());
				//return false;
			}
		}
	}
	return false;
}

template<>
uint32 CCfgGlobal::StateCount<CFighterDictator>(string& name, const CFighterDictator* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "服务器StateCount的pFighter参数为空";
		GenErr(ExpStr.str());
		return 0;
	}

	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(name);
	if(pCfg)
	{
		switch(pCfg->GetGlobalType())
		{
		case eSGT_MagicState:
			return pFighter->GetAllStateMgr()->GetMagicStateMgrServer()->MagicStateCount(name);
		case eSGT_TriggerState:
			return (uint32)pFighter->GetAllStateMgr()->GetTriggerStateMgrServer()->ExistState(name);
		case eSGT_DamageChangeState:
			return (uint32)pFighter->GetAllStateMgr()->GetDamageChangeStateMgrServer()->ExistState(name);
		case eSGT_CumuliTriggerState:
			return (uint32)pFighter->GetAllStateMgr()->GetCumuliTriggerStateMgrServer()->ExistState(name);
		case eSGT_SpecialState:
			return (uint32)pFighter->GetAllStateMgr()->GetSpecialStateMgrServer()->ExistState(name);
		default:
			{
				stringstream ExpStr;
				ExpStr << "服务器StateCount" << name << "类型" << pCfg->GetGlobalType() << "不正确\n";
				GenErr("服务器StateCount类型不正确", ExpStr.str());
				//return false;
			}
		}
	}
	return 0;
}

template<>
uint32 CCfgGlobal::CurRlserStateCount<CFighterDictator>(string& name, const CFighterDictator* pFrom, const CFighterDictator* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "服务器CurRlserStateCount的pFighter参数为空";
		GenErr(ExpStr.str());
		return 0;
	}

	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(name);
	if(pCfg)
	{
		switch(pCfg->GetGlobalType())
		{
		case eSGT_MagicState:
			return pFighter->GetAllStateMgr()->GetMagicStateMgrServer()->MagicStateCountOfInstaller(name, pFrom);
		case eSGT_TriggerState:
			return (uint32)pFighter->GetAllStateMgr()->GetTriggerStateMgrServer()->ExistState(name);
		case eSGT_DamageChangeState:
			return (uint32)pFighter->GetAllStateMgr()->GetDamageChangeStateMgrServer()->ExistState(name);
		case eSGT_CumuliTriggerState:
			return (uint32)pFighter->GetAllStateMgr()->GetCumuliTriggerStateMgrServer()->ExistState(name);
		case eSGT_SpecialState:
			return (uint32)pFighter->GetAllStateMgr()->GetSpecialStateMgrServer()->ExistState(name);
		default:
			{
				stringstream ExpStr;
				ExpStr << "服务器CurRlserStateCount" << name << "类型" << pCfg->GetGlobalType() << "不正确\n";
				GenErr("服务器CurRlserStateCount类型不正确", ExpStr.str());
				//return false;
			}
		}
	}
	return 0;
}


template<>
uint32 CCfgGlobal::TriggerCount<CFighterDictator>(string& name, const CFighterDictator* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "服务器ExistState的pFighter参数为空";
		GenErr(ExpStr.str());
		return 0;
	}

	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(name);
	if(pCfg)
	{
		switch(pCfg->GetGlobalType())
		{
		case eSGT_MagicState:
			return 0;
		case eSGT_TriggerState:
			return (uint32)pFighter->GetAllStateMgr()->GetTriggerStateMgrServer()->TriggerCount(name);
		case eSGT_DamageChangeState:
			return (uint32)pFighter->GetAllStateMgr()->GetDamageChangeStateMgrServer()->TriggerCount(name);
		case eSGT_CumuliTriggerState:
			return (uint32)pFighter->GetAllStateMgr()->GetCumuliTriggerStateMgrServer()->TriggerCount(name);
		case eSGT_SpecialState:
			return 0;
		default:
			{
				stringstream ExpStr;
				ExpStr << "服务器TriggerCount(" << name << ")类型" << pCfg->GetGlobalType() << "不正确\n";
				GenErr("服务器TriggerCount类型不正确", ExpStr.str());
				//return false;
			}
		}
	}
	return 0;
}

template<>
int32 CCfgGlobal::StateLeftTime<CFighterDictator>(string& name, const CFighterDictator* pFrom, const CFighterDictator* pTo)
{
	if(!pTo)
	{
		strstream ExpStr;
		ExpStr << "服务器ExistState的pFighter参数为空";
		GenErr(ExpStr.str());
		return 0;
	}

	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(name);
	if(pCfg)
	{
		switch(pCfg->GetGlobalType())
		{
		case eSGT_MagicState:
			return pTo->GetAllStateMgr()->GetMagicStateMgrServer()->StateLeftTime(name, pFrom);
		case eSGT_TriggerState:
			return (uint32)pTo->GetAllStateMgr()->GetTriggerStateMgrServer()->StateLeftTime(name);
		case eSGT_DamageChangeState:
			return (uint32)pTo->GetAllStateMgr()->GetDamageChangeStateMgrServer()->StateLeftTime(name);
		case eSGT_CumuliTriggerState:
			return (uint32)pTo->GetAllStateMgr()->GetCumuliTriggerStateMgrServer()->StateLeftTime(name);
		case eSGT_SpecialState:
			return (uint32)pTo->GetAllStateMgr()->GetSpecialStateMgrServer()->StateLeftTime(name);
		default:
			{
				stringstream ExpStr;
				ExpStr << "服务器StateLeftTime(" << name << ")类型" << pCfg->GetGlobalType() << "不正确\n";
				GenErr("服务器StateLeftTime类型不正确", ExpStr.str());
				//return false;
			}
		}
	}
	return 0;
}

template<>
double CCfgGlobal::Distance<CFighterDictator>(const CFighterDictator* pFrom, const CFighterDictator* pTo)
{
	if(pFrom == NULL || pTo == NULL || pFrom == pTo) return 0.0;
	//CFPos posSelf = pFrom->GetPixelPos();
	//CFPos posTarget = pTo->GetPixelPos();
	//float x = posSelf.x - posTarget.x;
	//float y = posSelf.y - posTarget.y;
	//return sqrt(x * x + y * y) / eGridSpanForObj;

	CEntityServer* pEntityFrom = pFrom->GetEntity();
	CEntityServer* pEntityTo = pTo->GetEntity();
	if(pEntityFrom == NULL || pEntityTo == NULL)
		return 0.0;

	return pEntityFrom->GetEntityDistInGrid(pEntityTo);
}

template<>
bool CCfgGlobal::TargetIsNPC<CFighterDictator>(const CFighterDictator* pTo)
{
	CCharacterDictator* pCharTo = pTo->GetCharacter(); 
	if(pCharTo && !pCharTo->CastToCharacterMediator())
	{
		return true;
	}
	else
	{
		return false;
	}
}









//for CFighterMediator
//template<>
//double CCfgGlobal::Get<CFighterMediator>(const CFighterMediator* pFrom, const CFighterMediator* pTo, uint32 index)
//{
//	return Get(static_cast<const CFighterDictator*>(pFrom), static_cast<const CFighterDictator*>(pTo), index);
//}
//
//template<>
//bool CCfgGlobal::ExistMagicState<CFighterMediator>(string& name, const CFighterMediator* pFighter)
//{
//	return ExistMagicState(name, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//bool CCfgGlobal::ExistTriggerState<CFighterMediator>(string& name, const CFighterMediator* pFighter)
//{
//	return ExistTriggerState(name, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//bool CCfgGlobal::ExistDamageChangeState<CFighterMediator>(string& name, const CFighterMediator* pFighter)
//{
//	return ExistDamageChangeState(name, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//bool CCfgGlobal::ExistSpecialState<CFighterMediator>(string& name, const CFighterMediator* pFighter)
//{
//	return ExistSpecialState(name, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//bool CCfgGlobal::ExistState<CFighterMediator>(string& name, const CFighterMediator* pFighter)
//{
//	return ExistState(name, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//uint32 CCfgGlobal::StateCount<CFighterMediator>(string& name, const CFighterMediator* pFighter)
//{
//	return StateCount(name, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//uint32 CCfgGlobal::TriggerCount<CFighterMediator>(string& name, const CFighterMediator* pFighter)
//{
//	return TriggerCount(name, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//int32 CCfgGlobal::StateLeftTime<CFighterMediator>(string& name, const CFighterMediator* pFrom, const CFighterMediator* pTo)
//{
//	return StateLeftTime(name, static_cast<const CFighterDictator*>(pFrom), static_cast<const CFighterDictator*>(pTo));
//}
//
//template<>
//double CCfgGlobal::Distance<CFighterMediator>(const CFighterMediator* pFrom, const CFighterMediator* pTo)
//{
//	return Distance(static_cast<const CFighterDictator*>(pFrom), static_cast<const CFighterDictator*>(pTo));
//}
//
//template<>
//bool CCfgGlobal::TargetIsNPC<CFighterMediator>(const CFighterMediator* pTo)
//{
//	return TargetIsNPC(static_cast<const CFighterDictator*>(pTo));
//}

bool CCfgGlobal::IsActiveSkill(string& name, const CFighterDictator* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "服务器ExistMagicState的pFighter参数为空";
		GenErr(ExpStr.str());
		return false;
	}
	return pFighter->IsSkillActive(name, 0);
}

