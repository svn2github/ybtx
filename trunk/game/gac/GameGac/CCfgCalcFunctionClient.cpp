#include "stdafx.h"
#include "CCfgCalcFunction.h"
#include "CFighterDirector.h"
#include "CMagicStateClient.h"
#include "COtherStateClient.inl"
#include "CEntityClient.h"

template<>
double CCfgGlobal::Get<CFighterDirector>(const CFighterDirector* pFrom,const CFighterDirector* pTo, uint32 index)
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
bool CCfgGlobal::ExistMagicState<CFighterDirector>(string& name, const CFighterDirector* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "客户端ExistMagicState的pFighter参数为空";
		GenErr(ExpStr.str());
		return false;
	}
	return pFighter->GetMagicStateMgrClient()->ExistState(name);
}

template<>
bool CCfgGlobal::ExistTriggerState<CFighterDirector>(string& name, const CFighterDirector* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "客户端ExistTriggeState的pFighter参数为空";
		GenErr(ExpStr.str());
		return false;
	}
	return pFighter->GetTriggerStateMgrClient()->ExistState(name);
}

template<>
bool CCfgGlobal::ExistDamageChangeState<CFighterDirector>(string& name, const CFighterDirector* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "客户端ExistDamageChangeState的pFighter参数为空";
		GenErr(ExpStr.str());
		return false;
	}
	return pFighter->GetDamageChangeStateMgrClient()->ExistState(name);
}

template<>
bool CCfgGlobal::ExistSpecialState<CFighterDirector>(string& name, const CFighterDirector* pFighter)
{
	if(!pFighter)
	{
		stringstream ExpStr;
		ExpStr << "客户端ExistSpecialState的pFighter参数为空";
		GenErr(ExpStr.str());
		return false;
	}
	return pFighter->GetSpecialStateMgrClient()->ExistState(name);
}

template<>
bool CCfgGlobal::ExistState<CFighterDirector>(string& name, const CFighterDirector* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "客户端ExistState的pFighter参数为空";
		GenErr(ExpStr.str());
		return false;
	}

	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(name);
	if(pCfg)
	{
		switch(pCfg->GetGlobalType())
		{
		case eSGT_MagicState:
			return pFighter->GetMagicStateMgrClient()->ExistState(name);
		case eSGT_TriggerState:
			return pFighter->GetTriggerStateMgrClient()->ExistState(name);
		case eSGT_DamageChangeState:
			return pFighter->GetDamageChangeStateMgrClient()->ExistState(name);
		case eSGT_CumuliTriggerState:
			return pFighter->GetCumuliTriggerStateMgrClient()->ExistState(name);
		case eSGT_SpecialState:
			return pFighter->GetSpecialStateMgrClient()->ExistState(name);
		default:
			{
				stringstream ExpStr;
				ExpStr << "客户端ExistState" << name << "类型" << pCfg->GetGlobalType() << "不正确\n";
				GenErr(ExpStr.str());
				//return false;
			}
		}
	}
	return false;
}

template<>
uint32 CCfgGlobal::StateCount<CFighterDirector>(string& name, const CFighterDirector* pFighter)
{
	if(!pFighter)
	{
		strstream ExpStr;
		ExpStr << "客户端ExistState的pFighter参数为空";
		GenErr(ExpStr.str());
		return 0;
	}

	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(name);
	if(pCfg)
	{
		switch(pCfg->GetGlobalType())
		{
		case eSGT_MagicState:
			return pFighter->GetMagicStateMgrClient()->StateCount(name);
		case eSGT_TriggerState:
			return (uint32)pFighter->GetTriggerStateMgrClient()->ExistState(name);
		case eSGT_DamageChangeState:
			return (uint32)pFighter->GetDamageChangeStateMgrClient()->ExistState(name);
		case eSGT_CumuliTriggerState:
			return (uint32)pFighter->GetCumuliTriggerStateMgrClient()->ExistState(name);
		case eSGT_SpecialState:
			return (uint32)pFighter->GetSpecialStateMgrClient()->ExistState(name);
		default:
			{
				stringstream ExpStr;
				ExpStr << "客户端StateCount" << name << "类型" << pCfg->GetGlobalType() << "不正确\n";
				GenErr(ExpStr.str());
				//return false;
			}
		}
	}
	return 0;
}

template<>
uint32 CCfgGlobal::CurRlserStateCount<CFighterDirector>(string& name, const CFighterDirector* pFrom, const CFighterDirector* pFighter)
{
	//客户端现在不需要
	return 0;
}


template<>
int32 CCfgGlobal::StateLeftTime<CFighterDirector>(string& name, const CFighterDirector* pFrom, const CFighterDirector* pTo)
{
	strstream ExpStr;
	ExpStr << "客户端暂时不支持表达式的StateLeftTime()函数\n";
	GenErr(ExpStr.str());
	return 0;

	//if(!pTo)
	//{
	//	strstream ExpStr;
	//	ExpStr << "客户端ExistState的pTo参数为空";
	//	GenErr(ExpStr.str());
	//	return 0;
	//}

	//CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(name);
	//if(pCfg)
	//{
	//	switch(pCfg->GetGlobalType())
	//	{
	//	case eSGT_MagicState:
	//		return pTo->GetMagicStateMgrClient()->StateLeftTime(name, pFrom);
	//	case eSGT_TriggerState:
	//		return (uint32)pTo->GetTriggerStateMgrClient()->StateLeftTime(name);
	//	case eSGT_DamageChangeState:
	//		return (uint32)pTo->GetDamageChangeStateMgrClient()->StateLeftTime(name);
	//	case eSGT_SpecialState:
	//		return (uint32)pTo->GetSpecialStateMgrClient()->StateLeftTime(name);
	//	default:
	//		{
	//			stringstream ExpStr;
	//			ExpStr << "客户端StateLeftTime" << name << "类型" << pCfg->GetGlobalType() << "不正确\n";
	//			GenErr(ExpStr.str());
	//			//return false;
	//		}
	//	}
	//}
	//return 0;
}

template<>
uint32 CCfgGlobal::TriggerCount<CFighterDirector>(string& name, const CFighterDirector* pFighter)
{
	strstream ExpStr;
	ExpStr << "客户端暂时不支持表达式的TriggerCount()函数\n";
	GenErr(ExpStr.str());
	return 0;
}

template<>
double CCfgGlobal::Distance<CFighterDirector>(const CFighterDirector* pFrom, const CFighterDirector* pTo)
{	
	if(pFrom == NULL || pTo == NULL || pFrom == pTo)
		return 0.0;

	CEntityClient* pEntityFrom = pFrom->GetEntity();
	CEntityClient* pEntityTo = pTo->GetEntity();
	if(pEntityFrom == NULL || pEntityTo == NULL)
		return 0.0;

	return pEntityFrom->GetEntityDistInGrid(pEntityTo);
}

template<>
bool CCfgGlobal::TargetIsNPC<CFighterDirector>(const CFighterDirector* pTo)
{
	return false;
}

bool CCfgGlobal::IsActiveSkill(string& name, const CFighterDirector* pFighter)
{
	return true;
}

