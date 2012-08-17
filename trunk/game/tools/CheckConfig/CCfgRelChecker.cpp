#include "stdafx.h"
#include "CCfgRelChecker.h"
#include "CCfgMagicEffCheck.h"
#include "CCfgColChecker.inl"
#include "CCfgMagicOp.h"
#include "CCfgAllMagicCheck.h"
#include "CCfgAllStateCheck.h"
#include "CCfgSkillCheck.h"
#include "CCfgNormalAttackCheck.h"
#include "CCfgProcessCheck.h"

CCfgRelChecker::MapPingPongObjPersistent CCfgRelChecker::ms_mapPingPongObjPersistent;

bool CCfgRelChecker::ResetMap()
{
	ms_mapPingPongObjPersistent["安装或清除魔法状态"]	= eOPT_Inexistence;
	ms_mapPingPongObjPersistent["安装或清除触发器状态"]	= eOPT_Inexistence;
	ms_mapPingPongObjPersistent["安装或清除姿态"]		= eOPT_Inexistence;
	return true;
}

bool CCfgRelChecker::CheckRelationAvail()
{
	bool bRet = true;

	//魔法效果关系约束（非读表依赖）
	CfgChk::SetTabName("魔法效果");
	for(CCfgMagicEffCheck::MapMagicEff::iterator itr =  CCfgMagicEffCheck::ms_mapMagicEff.begin(); 
		itr != CCfgMagicEffCheck::ms_mapMagicEff.end(); ++itr)
	{
		CfgChk::g_sMagicEffName = itr->second->GetMagicEffName();
		ResetMap();
		itr->second->CheckMagicOpRelationAvail(CCfgMagicOp::CheckPingPongObjPersistent);

		//检测含特殊撤销操作的效果必须为状态可撤销效果
		itr->second->CheckMustCancelEffOfState();
	}

	//技能表关系约束
	CfgChk::SetTabName("技能");
	for(CCfgSkillCheck::MapSkill::iterator itr = CCfgSkillCheck::ms_mapSkill.begin();
		itr != CCfgSkillCheck::ms_mapSkill.end(); ++itr)
	{
		if((*itr).second && (!(*itr).second->CheckRelationAvail())) bRet = false;
	}

	//普攻表关系约束
	CfgChk::SetTabName("普攻");
	for(CCfgNormalAttackCheck::MapNormalAttack::iterator itr = CCfgNormalAttackCheck::ms_mapNormalAttack.begin();
		itr != CCfgNormalAttackCheck::ms_mapNormalAttack.end(); ++itr)
	{
		if((*itr).second && (!(*itr).second->CheckRelationAvail())) bRet = false;
	}

	//流程表关系约束
	CfgChk::SetTabName("流程");
	for(CCfgProcessCheck::MapProcess::iterator itr = CCfgProcessCheck::ms_mapProcess.begin();
		itr != CCfgProcessCheck::ms_mapProcess.end(); ++itr)
	{
		if((*itr).second && (!(*itr).second->CheckRelationAvail())) bRet = false;
	}

	//状态表关系约束
	CfgChk::SetTabName("状态");
	for(CCfgAllStateCheck::MapState::iterator itr = CCfgAllStateCheck::ms_mapState.begin();
		itr != CCfgAllStateCheck::ms_mapState.end(); ++itr)
	{
		if(!(*itr).second->CheckRelationAvail()) bRet = false;
	}

	//姿态技能表关系约束
	CfgChk::SetTabName("姿态技能");
	for(CCfgAureMagicCheck::MapAureMagic::iterator itr = CCfgAureMagicCheck::ms_mapAureMagic.begin();
		itr != CCfgAureMagicCheck::ms_mapAureMagic.end(); ++itr)
	{
		if(!(*itr).second->CheckRelationAvail()) bRet = false;
	}

	//位置魔法表关系约束
	CfgChk::SetTabName("位置魔法");
	for(CCfgPositionMagicCheck::MapPositionMagic::iterator itr = CCfgPositionMagicCheck::ms_mapPositionMagic.begin();
		itr != CCfgPositionMagicCheck::ms_mapPositionMagic.end(); ++itr)
	{
		if(!(*itr).second->CheckRelationAvail()) bRet = false;
	}

	//位移魔法表关系约束
	CfgChk::SetTabName("位移魔法");
	for (CCfgMoveMagicCheck::MapMoveMagic::iterator itr = CCfgMoveMagicCheck::ms_mapMoveMagic.begin();
		itr != CCfgMoveMagicCheck::ms_mapMoveMagic.end(); ++itr)
	{
		if (!(*itr).second->CheckRelationAvail()) bRet = false;
	}

	//图腾魔法表关系约束
	CfgChk::SetTabName("图腾魔法");
	for(CCfgTotemMagicCheck::MapTotemMagic::iterator itr = CCfgTotemMagicCheck::ms_mapTotemMagic.begin();
		itr != CCfgTotemMagicCheck::ms_mapTotemMagic.end(); ++itr)
	{
		if(!(*itr).second->CheckRelationAvail()) bRet = false;
	}

	CfgChk::SetTabNull();
	return bRet;
}

bool CCfgSkillCheck::CheckRelationAvail()
{
	if(GetMagicEff())
	{
		CfgChk::g_sSrcTabLineName = GetSkillName();
		CfgChk::g_sMagicEffName = GetMagicEff()->GetMagicEffName();
		return GetMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckForbidCancellableValueChangedMop);
	}
	return true;
}

bool CCfgNormalAttackCheck::CheckRelationAvail()
{
	if(GetMagicEff())
	{
		CfgChk::g_sSrcTabLineName = GetName();
		CfgChk::g_sMagicEffName = GetMagicEff()->GetMagicEffName();
		return GetMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckForbidCancellableValueChangedMop);
	}
	return true;
}

bool CCfgProcessCheck::CheckRelationAvail()
{
	if(GetSelfCancelEff())
	{
		CfgChk::g_sMagicEffName = GetSelfCancelEff()->GetMagicEffName();
		if (!GetSelfCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckNoSingleFilterSelf2Eff)) return false;
		if (!GetSelfCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCancelableMopSetupAureMagicNotSave)) return false;
		if (!GetSelfCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCancelableMopSetupStateNotSave)) return false;
	}
	if (GetObjCancelEff())
	{
		CfgChk::g_sMagicEffName = GetObjCancelEff()->GetMagicEffName();
		if (!GetObjCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckNoSingleFilterObj2Eff))	return false;
		if (!GetObjCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCancelableMopSetupAureMagicNotSave)) return false;
		if (!GetObjCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCancelableMopSetupStateNotSave)) return false;
	}

	return true;
}

bool CCfgAllStateCheck::CheckRelationAvail()
{
	if (GetType() == eBST_State)
	{
		if((GetCascadeType() == "集中" || GetCascadeType() == "共享") && GetCascadeMax() != "1")
		{
			if(GetCancelEff() && !GetCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckFilterObj2CentralMS)) return false;
			if(GetDotEff() && !GetDotEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckFilterObj2CentralMS)) return false;
			if(GetFinalEff() && !GetFinalEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckFilterObj2CentralMS)) return false;
		}
		if(GetCancelEff())
		{
			if(!GetCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckNoSingleFilterObj2Eff)) return false;
			if(!GetCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckFilterObj2CalcNeedTargetMOPInMS)) return false;
			CfgChk::g_sMagicEffName = GetCancelEff()->GetMagicEffName();
			if(GetPersistent())
			{
				CfgChk::g_sSrcTabLineName = GetName();
				if(!GetCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckNonPersistentObj)) return false;
			}
			if (!GetCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCancelableMopSetupAureMagicNotSave)) return false;
		}
		if(GetDotEff())
		{
			CfgChk::g_sMagicEffName = GetDotEff()->GetMagicEffName();
			if(!GetDotEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckFilterObj2CalcNeedTargetMOPInMS)) return false;
		}
		if(GetFinalEff())
		{
			CfgChk::g_sMagicEffName = GetFinalEff()->GetMagicEffName();
			if(!GetFinalEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckFilterObj2CalcNeedTargetMOPInMS)) return false;
		}
		return true;
	}
	else if (GetType() == eBST_Trigger || GetType() == eBST_Damage)
	{
		if(GetCancelEff())
		{
			if(!GetCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCascade2CancelableMagicEff)) return false;
			if(GetNeedSaveToDB() && !GetCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckMOPType2CancelableMagicEff)) return false;

			if(!GetCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckNoSingleFilterObj2Eff)) return false;
			if(!GetCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCancellableMopArgIsNotConst)) return false;

			CfgChk::g_sMagicEffName = GetCancelEff()->GetMagicEffName();
			if(GetPersistent())
			{
				CfgChk::g_sSrcTabLineName = GetName();
				GetCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckNonPersistentObj);
			}
			if (!GetCancelEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCancelableMopSetupAureMagicNotSave)) return false;
		}
		return true;
	}
	else
	{
		return true;
	}
}

bool CCfgAureMagicCheck::CheckRelationAvail()
{
	if(GetMagicEff())
	{
		CfgChk::g_sMagicEffName = GetMagicEff()->GetMagicEffName();
		if(GetPersistent())
		{
			CfgChk::g_sSrcTabLineName = GetName();
			GetMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckNonPersistentObj);
		}
		if(!GetMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckNoSingleFilterObj2Eff)) return false;
		if(!GetMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCancelableMopSetupStateNotSave)) return false;
	}
	return true;
}

bool CCfgPositionMagicCheck::CheckRelationAvail()
{
	if(GetTouchMagicEff())
	{
		CfgChk::g_sMagicEffName = GetTouchMagicEff()->GetMagicEffName();
		if(!GetTouchMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckNoSingleFilterObj2Eff)) return false;
		if(!GetTouchMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCancelableMopSetupStateNotSave)) return false;
		if(!GetTouchMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckPosMagicSetupStateType)) return false;
		if (IsMutex())
		{
			if(!GetTouchMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckMutexPosMagicSetupStateType)) return false;
		}
	}

	return true;
}

bool CCfgMoveMagicCheck::CheckRelationAvail()
{
	if(GetCancelMagicEff())
	{
		CfgChk::g_sMagicEffName = GetCancelMagicEff()->GetMagicEffName();
		if (!GetCancelMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCancelableMopSetupAureMagicNotSave)) return false;
		if (!GetCancelMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckCancelableMopSetupStateNotSave)) return false;
	}

	return true;
}

bool CCfgTotemMagicCheck::CheckRelationAvail()
{
	if(GetTouchMagicEff())
	{
		if(!GetTouchMagicEff()->CheckMagicOpRelationAvail(CCfgMagicOp::CheckNoSingleFilterObj2Eff)) return false;
	}

	return true;
}

bool CCfgMagicEffCheck::CheckMagicOpRelationAvail(MOP_MATCH_CHECKER pFunChecker)
{
	VecChild::iterator itrChild = m_vecChild.begin();
	for (; itrChild != m_vecChild.end(); ++ itrChild)
	{
		if(!(*itrChild)->CheckMagicOpRelationAvail(pFunChecker))
			return false;
	}
	return true;
}

bool CCfgMagicEffCheck::CheckMustCancelEffOfState()
{
	CBaseMagicOp* pBaseMagicOp = NULL;
	VecChild::iterator itrChild = m_vecChild.begin();
	for (; itrChild != m_vecChild.end(); ++ itrChild)
	{
		CBaseMagicOp* pBaseMagicOp = (*itrChild)->GetBaseMagicOp();
		if (pBaseMagicOp->CheckIsCancelEffOfState())
		{
			if (!CCfgAllStateCheck::IsCancelEffOfState(this))
			{
				stringstream str;
				str << "\n配置表[魔法效果]第" << ((*itrChild)->GetID() + 1) << "行违反约束：魔法效果[" << CfgChk::g_sMagicEffName << 
					"]因使用类似禁止普攻、技能操作，所以必须是魔法状态的可撤销效果\n";
				CfgChk::GenExpInfo(str.str());
				return false;
			}
		}
	}
	return true;
}

bool CCfgMagicOp::CheckMagicOpRelationAvail(MOP_MATCH_CHECKER pFunChecker)
{
	if(!pFunChecker(this))
		return false;
	return true;
}

bool CCfgMagicOp::CheckFilterObj2CentralMS(CCfgMagicOp* pMagicOp)
{
	vector<MagicEffFilter*>::iterator it = pMagicOp->m_vecFilterPipe.begin();
	if((*it)->m_eObjectFilter == eOF_Self)
	{
		stringstream str;
		str << "配置表[魔法效果]第" << (pMagicOp->m_nID + 1) << "行违反魔法状态关系操作约束：叠加类型为集中的魔法状态不能有魔法操作筛选目标为自身相关[" << (*it)->m_eObjectFilter << "]的魔法效果\n";
		CfgChk::GenExpInfo(str.str());
		return false;
	}

	if(pMagicOp->m_bIsCondition)
	{
		stringstream str;
		str << "配置表[魔法效果]第" << (pMagicOp->m_nID + 1) << "行违反魔法状态关系操作约束：叠加类型为集中的魔法状态不能有魔法条件\n";
		CfgChk::GenExpInfo(str.str());
		return false;
	}

	if(pMagicOp->m_strMOPType == "临时")
	{
		stringstream str;
		str << "配置表[魔法效果]第" << (pMagicOp->m_nID + 1) << "行违反魔法状态关系操作约束：叠加类型为集中的魔法状态不能有临时魔法操作\n";
		CfgChk::GenExpInfo(str.str());
		return false;
	}

	return true;
}

bool CCfgMagicOp::CheckCascade2CancelableMagicEff(CCfgMagicOp* pMagicOp)
{
	if(pMagicOp->m_strMagicOpName == "安装魔法状态")
	{
		CCfgAllStateCheck::MapState::iterator itr = CCfgAllStateCheck::ms_mapState.find(pMagicOp->m_MagicOpArg->GetString());
		if ((*itr).second->GetCascadeType() != "唯一")
		{
			stringstream str;
			str << "配置表[魔法效果]第" << (pMagicOp->m_nID + 1) << "行违反魔法状态关系操作约束：可撤销效果里安装的魔法状态的叠加类型必须为唯一\n";
			CfgChk::GenExpInfo(str.str());
			return false;
		}
	}
	return true;
}

bool CCfgMagicOp::CheckMOPType2CancelableMagicEff(CCfgMagicOp* pMagicOp)
{
	if(pMagicOp->m_pMagicOp && pMagicOp->m_pMagicOp->GetBaseType() == eMOPBT_MagicOP && 
		class_cast<CMagicOp*>(pMagicOp->m_pMagicOp)->GetMagicOpType() != eMOT_Function
		&& pMagicOp->m_strMOPType != "临时")
	{
		stringstream str;
		str << "配置表[魔法效果]第" << (pMagicOp->m_nID + 1) << "行违反魔法状态关系操作约束：需要存储到数据库的触发器和伤害变更状态的可撤销效果里的非条件非临时魔法操作必须为功能型\n";
		CfgChk::GenExpInfo(str.str());
		return false;
	}
	return true;
}


bool CCfgMagicOp::CheckFilterObj2CancelableMagicEff(CCfgMagicOp* pMagicOp)
{
	//vector<MagicEffFilter*>::iterator it = pMagicOp->m_vecFilterPipe.begin();
	//if ((*it)->m_eObjectFilter == eOF_Self)
	//{
	//	stringstream str;
	//	str << "配置表[魔法效果]第" << (pMagicOp->m_nID + 1) << "行违反状态关系操作约束：状态不能有魔法操作筛选目标为自身相关[" << (*it)->m_eObjectFilter << "]的魔法效果\n";
	//	CfgChk::GenExpInfo(str.str());
	//	return false;
	//}
	return true;
}

bool CCfgMagicOp::CheckNoSingleFilterSelf2Eff(CCfgMagicOp* pCfgMagicOp)
{
	vector<MagicEffFilter*>::iterator it = pCfgMagicOp->m_vecFilterPipe.begin();
	CBaseMagicOp* pMagicOp = pCfgMagicOp->m_pMagicOp;
	if(pMagicOp && pMagicOp->GetBaseType() == eMOPBT_MagicOP && 
		/*class_cast<CMagicOp*>(pMagicOp)->GetMagicOpType() == eMOT_Value && */
		class_cast<CMagicOp*>(pMagicOp)->Cancelable() &&
		(*it)->m_eObjectFilter != eOF_Self)
	{
		stringstream str;
		str << "配置表[魔法效果]第" << (pCfgMagicOp->m_nID + 1) << "行违反[" << CfgChk::g_sTabName << "]关系操作约束：可撤销效果里不能有再次筛选目标为[" << (*it)->m_eObjectFilter << "]的非临时可撤销魔法操作\n";
		CfgChk::GenExpInfo(str.str());
		return false;
	}

	return true;
}

//临时调试用
bool CCfgMagicOp::CheckNoSingleFilterObj2Eff(CCfgMagicOp* pCfgMagicOp)
{
	vector<MagicEffFilter*>::iterator it = pCfgMagicOp->m_vecFilterPipe.begin();
	CBaseMagicOp* pMagicOp = pCfgMagicOp->m_pMagicOp;
	if(pMagicOp && pMagicOp->GetBaseType() == eMOPBT_MagicOP && 
		/*class_cast<CMagicOp*>(pMagicOp)->GetMagicOpType() == eMOT_Value && */
		class_cast<CMagicOp*>(pMagicOp)->Cancelable() &&
		pCfgMagicOp->m_strMOPType != "临时")
	{
		if ((*it)->m_eObjectFilter != eOF_Target)
		{
			stringstream str;
			str << "配置表[魔法效果]第" << (pCfgMagicOp->m_nID + 1) << "行违反[" << CfgChk::g_sTabName << "]关系操作约束：可撤销效果里不能有再次筛选目标为[" << (*it)->m_eObjectFilter << "]的非临时可撤销魔法操作\n";
			CfgChk::GenExpInfo(str.str());
			return false;
		}
		if (pCfgMagicOp->m_vecFilterPipe.size() > 1)
		{
			stringstream str;
			str << "配置表[魔法效果]第" << (pCfgMagicOp->m_nID + 1) << "行违反[" << CfgChk::g_sTabName << "]关系操作约束：可撤销效果里不能有筛选管道'|'的非临时可撤销魔法操作\n";
			CfgChk::GenExpInfo(str.str());
			return false;
		}
	}

	return true;
}

bool CCfgMagicOp::CheckNoSingleAndSelfFilterObj2Eff(CCfgMagicOp* pMagicOp)
{
	//vector<MagicEffFilter*>::iterator it = pMagicOp->m_vecFilterPipe.begin();
	//CBaseMagicOp* pMagicOp = pMagicOp->m_pMagicOp;
	//if(pMagicOp && pMagicOp->GetBaseType() == eMOPBT_MagicOP && 
	//	/*class_cast<CMagicOp*>(pMagicOp)->GetMagicOpType() == eMOT_Value && */
	//	class_cast<CMagicOp*>(pMagicOp)->Cancelable() &&
	//	(*it)->m_eObjectFilter != eOF_Target && 
	//	(*it)->m_eObjectFilter != eOF_Self && 
	//	pMagicOp->m_strMOPType != "临时")
	//{
	//	stringstream str;
	//	str << "配置表[魔法效果]第" << (pMagicOp->m_nID + 1) << "行违反[" << CfgChk::g_sTabName << "]关系操作约束：可撤销效果里不能有再次筛选目标为[" << (*it)->m_eObjectFilter << "]的非临时可撤销魔法操作\n";
	//	CfgChk::GenExpInfo(str.str());
	//	return false;
	//}

	return true;
}

bool CCfgMagicOp::CheckFilterObj2CalcNeedTargetMOPInMS(CCfgMagicOp* pCfgMagicOp)
{
	vector<MagicEffFilter*>::iterator it = pCfgMagicOp->m_vecFilterPipe.begin();
	for(;it != pCfgMagicOp->m_vecFilterPipe.end(); ++it)
	{
		if ((*it)->m_eObjectFilter != eOF_Target)
		{
			CBaseMagicOp* pMagicOp = pCfgMagicOp->m_pMagicOp;
			if(pMagicOp->GetBaseType() == eMOPBT_MagicOP
				&& (class_cast<CMagicOp*>(pMagicOp))->GetMagicOpType() == eMOT_Value
				&& (class_cast<CValueMagicOp*>(pMagicOp))->CalcNeedTarget(pCfgMagicOp->m_MagicOpArg))
			{
				stringstream str;
				str << "配置表[魔法效果]第" << (pCfgMagicOp->m_nID + 1)
					<< "行[" << CfgChk::g_sMagicEffName << "]违反魔法状态关系操作约束：魔法状态的效果不能有魔法操作筛选目标为["
					<< (*it)->m_eObjectFilter << "]的计算部分需要目标的魔法操作["
					<< pCfgMagicOp->m_strMagicOpName << "]或需要目标的魔法操作参数["
					<< pCfgMagicOp->m_MagicOpArg->GetTestString() << "]\n";
				CfgChk::GenExpInfo(str.str());
				return false;
			}
		}
	}
	return true;
}

bool CCfgMagicOp::CheckNoCond(CCfgMagicOp* pMagicOp)
{
	//if(pMagicOp->m_pMagicOp->GetBaseType() == eMOPBT_MagicCondition)
	//{
	//	stringstream str;
	//	str << "配置表[魔法效果]第" << (pMagicOp->m_nID + 1)
	//		<< "行违反魔法状态关系操作约束：魔法状态的效果不能有魔法条件["
	//		<< pMagicOp->m_strMagicOpName << "]\n";
	//		CfgChk::GenExpInfo(str.str());
	//		return false;
	//}
	return true;
}

bool CCfgMagicOp::CheckHasNoCond(CCfgMagicOp* pMagicOp)
{
	//return pMagicOp->m_pMagicOp->GetBaseType() != eMOPBT_MagicCondition;
	return true;
}

bool CCfgMagicOp::CheckHasNoValueChangedOp(CCfgMagicOp* pMagicOp)
{
	//if(pMagicOp->m_pMagicOp->GetBaseType() == eMOPBT_MagicOP)
	//{
	//	CMagicOp* pMagicOp = static_cast<CMagicOp*>(pMagicOp->m_pMagicOp);
	//	if(pMagicOp->GetMagicOpType() == eMOT_Value)
	//	{
	//		return false;
	//	}
	//}
	return true;
}

bool CCfgMagicOp::CheckCancellableMopArgIsNotConst(CCfgMagicOp* pCfgMagicOp)
{
	switch(pCfgMagicOp->m_pMagicOp->GetBaseType())
	{
	case eMOPBT_MagicOP:
		{
			CMagicOp* pMagicOp = static_cast<CMagicOp*>(pCfgMagicOp->m_pMagicOp);
			if(pMagicOp->Cancelable() && !pCfgMagicOp->m_MagicOpArg->IsConstValue())
			{
				stringstream str;
				str << "配置表[" << CfgChk::g_sTabName << "]的可撤销效果第[" << pCfgMagicOp->m_nID << "]行的可撤销操作[" << pCfgMagicOp->m_strMagicOpName << 
					"魔法操作参数[" << pCfgMagicOp->m_MagicOpArg->GetTestString() << "]不能为可变值\n";
				CfgChk::GenExpInfo(str.str());
				return false;
			}
		}
		break;
	default:
		break;
	}
	return true;
}

bool CCfgMagicOp::CheckCanExecuteWithTargetIsDead(CCfgMagicOp* pMagicOp)
{
	//现在角色死亡时的状态能执行效果，这个限制可以去了，但要小心死亡后不能安装目标死亡时执行无效的魔法操作，比如改变控制值
	//switch(pMagicOp->m_pMagicOp->GetBaseType())
	//{
	//case eMOPBT_MagicOP:
	//	if(!pMagicOp->m_pMagicOp->CanExecuteWithTargetIsDead())
	//	{
	//		stringstream str;
	//		str << "时间不是无限的永久状态[" << CfgChk::g_sTabName << "]的效果第[" << pMagicOp->m_nID << "]行的操作[" << pMagicOp->m_strMagicOpName << 
	//			"]不能为目标死亡时不能执行的魔法操作\n";
	//		CfgChk::GenExpInfo(str.str());
	//		return false;
	//	}
	//case eMOPBT_MagicCondition:
	//	return true;
	//}
	return true;
}

bool CCfgMagicOp::CheckTempMopForState(CCfgMagicOp* pMagicOp)
{
	//if(pMagicOp->m_strMOPType == "临时")
	//{
	//	stringstream str;
	//	str << "状态[" << CfgChk::g_sTabName << "]的效果（除触发效果外）第[" << pMagicOp->m_uId << "]行不能有临时操作[" << pMagicOp->m_strMagicOpName <<"]\n";
	//	CfgChk::GenExpInfo(str.str());
	//	return false;
	//}
	return false;
}

bool CCfgMagicEffCheck::CheckNpcSkillFilterObj2MagicEff(const string sRuleName, string sSkillName, ESRTargetType eSRTargetType)
{
	VecChild::iterator itrChild = m_vecChild.begin();
	for (; itrChild != m_vecChild.end(); ++ itrChild)
	{
		if(!(*itrChild)->CheckNpcSkillFilterObj2MagicEff(sRuleName, sSkillName, eSRTargetType))
			return false;
	}
	return true;
}

bool CCfgMagicOp::CheckNpcSkillFilterObj2MagicEff(const string sRuleName, string sSkillName, ESRTargetType eSRTargetType)
{
	bool bRet = true;
	vector<MagicEffFilter*>::iterator it = m_vecFilterPipe.begin();
	switch(eSRTargetType)
	{
	case eSRTT_P_EnemyPostion:
	case eSRTT_P_SelfPosition:
	case eSRTT_P_RandomEnemyPostion:
	case eSRTT_P_SelfDirection:
	case eSRTT_P_SelfReverseDirection:
	case eSRTT_P_RandomFriendPos:
	case eSRTT_P_MaxHpEnemyPos:
	case eSRTT_P_MinHpEnemyPos:
	case eSRTT_P_MaxMpEnemyPos:
	case eSRTT_P_MinMpEnemyPos:
	case eSRTT_P_RandomDirection:
	case eSRTT_P_NearestEnemyPos:
	case eSRTT_P_FarthestEnemyPos:
	case eSRTT_P_NearestFriendPos:
	case eSRTT_P_FarthestFriendPos:
	case eSRTT_P_RandomUnEnmityPos:
	case eSRTT_P_RandomUnServantPos:
	case eSRTT_P_RamdomUnEnmityUnServantPos:
	case eSRTT_P_MaxHpFriendPos:
	case eSRTT_P_MinHpFriendPos:
	case eSRTT_P_MaxMpFriendPos:
	case eSRTT_P_MinMpFriendPos:
	case eSRTT_P_AroundPos:
	case eSRTT_P_RandomPos:
		{
			// 之所以"自身"也包括在内，是因为地点流程魔法的条件判断
			if(!((*it)->m_eObjectFilter == eOF_Position || (*it)->m_eObjectFilter == eOF_Self || (*it)->m_eObjectFilter == eOF_FilterResult))
			{
				cout<<endl;
				cout<<"Npc技能规则表中规则【"<<sRuleName<<"】释放的技能【"<<sSkillName<<"】执行的魔法操作【"
					<<m_strMagicOpName<<"】是针对目标的！但是技能规则表中填的是地点！请策划修正！"<<endl;
				return false;
			}
		}
		break;
	default:
		{
			if((*it)->m_eObjectFilter == eOF_Position)
			{
				cout<<endl;
				cout<<"Npc技能规则表中规则【"<<sRuleName<<"】释放的技能【"<<sSkillName<<"】执行的魔法操作【"
					<<m_strMagicOpName<<"】是针对地点的！但是技能规则表中填的是目标！请策划修正！"<<endl;
				return false;	
			}
		}
		break;
	}

	return bRet;
}

bool CCfgMagicOp::CheckForbidCancellableValueChangedMop(CCfgMagicOp* pCfgMagicOp)
{
	CBaseMagicOp* pBaseMagicOp = pCfgMagicOp->m_pMagicOp;
	if(pBaseMagicOp->GetBaseType() == eMOPBT_MagicOP && pCfgMagicOp->m_strMOPType != "临时")
	{
		CMagicOp* pMagicOp = class_cast<CMagicOp*>(pBaseMagicOp);
		if(pMagicOp->GetMagicOpType() == eMOT_Value && pMagicOp->Cancelable())
		{
			stringstream str;
			str << "配置表[魔法效果]第" << (pCfgMagicOp->m_nID + 1) << "行违反数值检测关系约束：" <<
				CfgChk::g_sTabName << "[" << CfgChk::g_sSrcTabLineName << "]的直接魔法效果[" << CfgChk::g_sMagicEffName << 
				"]不能有改变值的非临时可撤销操作[" << pCfgMagicOp->m_strMagicOpName << "]\n";
			CfgChk::GenExpInfo(str.str());
			return false;
		}
	}
	return true;
}

bool CCfgMagicOp::CheckNonPersistentObj(CCfgMagicOp* pCfgMagicOp)
{
	CBaseMagicOp* pBaseMagicOp = pCfgMagicOp->m_pMagicOp;
	if(pBaseMagicOp->GetBaseType() == eMOPBT_MagicOP && pCfgMagicOp->m_strMOPType != "临时")
	{
		CMagicOp* pMagicOp = class_cast<CMagicOp*>(pBaseMagicOp);
		if(pMagicOp->GetMagicOpType() == eMOT_Function && pMagicOp->Cancelable())
		{
			CFunctionCancelableMagicOp* pFunMop = class_cast<CFunctionCancelableMagicOp*>(pMagicOp);
			if(pFunMop->IsSetupNestedObj())
			{
				string sMagicOpName = pCfgMagicOp->m_strMagicOpName;
				if(sMagicOpName == "安装姿态")
				{	
					string strAureName = pCfgMagicOp->m_MagicOpArg->GetTestString();
					CCfgAureMagicCheck::MapAureMagic::iterator itr = CCfgAureMagicCheck::ms_mapAureMagic.find(strAureName);
					if(!(*itr).second->GetPersistent())
					{
						stringstream str;
						str << "配置表[魔法效果]第" << (pCfgMagicOp->m_nID + 1) << "行违反永久对象关系约束：永久的" <<
							CfgChk::g_sTabName << "[" << CfgChk::g_sSrcTabLineName << "]的直接魔法效果[" << CfgChk::g_sMagicEffName << 
							"]不能安装非永久的姿态魔法（光环）[" <<  pCfgMagicOp->m_MagicOpArg->GetTestString() << "]\n";
						CfgChk::GenExpInfo(str.str());
						return false;						
					}
				}
				else/* if(sMagicOpName == "依次安装魔法状态")*/
				{
					uint32 l = 1;
					string sStateName;
					if(pCfgMagicOp->m_MagicOpArg->GetStrType() == eST_MutliString)
					{
						l = pCfgMagicOp->m_MagicOpArg->GetStringCount();
					}
					for(uint32 i = 0; i < l; ++i)
					{
						if(i == 0 && l == 1)
						{
							sStateName = pCfgMagicOp->m_MagicOpArg->GetString();
						}
						else
						{
							sStateName = pCfgMagicOp->m_MagicOpArg->GetString(i);
						}
						if(atoi(sStateName.c_str()) > 0)
							continue;
						CCfgAllStateCheck::MapState::iterator itr = CCfgAllStateCheck::ms_mapState.find(sStateName);
						if(!(*itr).second->GetPersistent())
						{
							stringstream str;
							str << "配置表[魔法效果]第" << (pCfgMagicOp->m_nID + 1) << "行违反永久对象关系约束：永久的" <<
								CfgChk::g_sTabName << "[" << CfgChk::g_sSrcTabLineName << "]的直接魔法效果[" << CfgChk::g_sMagicEffName << 
								"]不能安装非永久状态[" <<  sStateName << "]\n";
							CfgChk::GenExpInfo(str.str());
							return false;						
						}
					}
					
				}
			}
		}
	}
	return true;
}

bool CCfgMagicOp::CheckPingPongObjPersistent(CCfgMagicOp* pCfgMagicOp)
{
	CBaseMagicOp* pBaseMagicOp = pCfgMagicOp->m_pMagicOp;
	if(pBaseMagicOp->GetBaseType() == eMOPBT_MagicOP && pCfgMagicOp->m_strMOPType != "临时")
	{
		CMagicOp* pMagicOp = class_cast<CMagicOp*>(pBaseMagicOp);
		if(pMagicOp->GetMagicOpType() == eMOT_Function)
		{
			CCfgRelChecker::MapPingPongObjPersistent::iterator itr = CCfgRelChecker::ms_mapPingPongObjPersistent.find(pCfgMagicOp->m_strMagicOpName);
			if(itr != CCfgRelChecker::ms_mapPingPongObjPersistent.end())
			{
				bool bPersistent;
				if(pCfgMagicOp->m_strMagicOpName == "安装或清除姿态")
				{
					string strAureName = pCfgMagicOp->m_MagicOpArg->GetString();
					CCfgAureMagicCheck::MapAureMagic::iterator itr = CCfgAureMagicCheck::ms_mapAureMagic.find(strAureName);
					bPersistent = (*itr).second->GetPersistent();
				}
				else
				{
					string strStateName = pCfgMagicOp->m_MagicOpArg->GetString();
					CCfgAllStateCheck::MapState::iterator itr = CCfgAllStateCheck::ms_mapState.find(strStateName);
					bPersistent = (*itr).second->GetPersistent();
				}
				switch(itr->second)
				{
				case CCfgRelChecker::eOPT_Inexistence:
					itr->second = bPersistent ? CCfgRelChecker::eOPT_Persistent : CCfgRelChecker::eOPT_Inpersistent;
					break;
				case CCfgRelChecker::eOPT_Persistent:
					if(!bPersistent)
					{
						stringstream str;
						str << "配置表[魔法效果]第" << (pCfgMagicOp->m_nID + 1) << "行违反安装或撤销操作的一致性约束：魔法效果[" << 
							CfgChk::g_sMagicEffName << "]有两个以上的魔法操作[" <<  pCfgMagicOp->m_strMagicOpName << "]且安装的对象永久性不一致\n";
						CfgChk::GenExpInfo(str.str());
						return false;						
					}
					break;
				case CCfgRelChecker::eOPT_Inpersistent:
					if(bPersistent)
					{
						stringstream str;
						str << "配置表[魔法效果]第" << (pCfgMagicOp->m_nID + 1) << "行违反安装或撤销操作的一致性约束：魔法效果[" << 
							CfgChk::g_sMagicEffName << "]有两个以上的魔法操作[" <<  pCfgMagicOp->m_strMagicOpName << "]且安装的对象永久性不一致\n";
						CfgChk::GenExpInfo(str.str());
						return false;						
					}
					break;
				default:
					{
						stringstream str;
						str << "配置表[魔法效果]第" << (pCfgMagicOp->m_nID + 1) << "行魔法效果[" << CfgChk::g_sMagicEffName << 
							"]违反安装或撤销操作的一致性约束：ms_mapPingPongObjPersistent的second取到EObjPersistentType以外的值[" << 
							itr->second << "]\n";
						CfgChk::GenExpInfo(str.str());
						return false;						
					}
				}
			}
		}
	}
	return true;
}

bool CCfgMagicOp::CheckCancelableMopSetupAureMagicNotSave(CCfgMagicOp* pMagicOp)
{
	if(pMagicOp->m_strMagicOpName == "安装姿态" || pMagicOp->m_strMagicOpName == "安装或清除姿态")
	{
		string strAureName = pMagicOp->m_MagicOpArg->GetTestString();
		CCfgAureMagicCheck::MapAureMagic::iterator itr = CCfgAureMagicCheck::ms_mapAureMagic.find(strAureName);
		if((*itr).second->GetNeedSaveToDB())
		{
			stringstream str;
			str << "\n配置表[魔法效果]第" << (pMagicOp->m_nID + 1) << "行违反约束：可撤销效果[" << CfgChk::g_sMagicEffName << 
				"]安装的姿态(或光环)[" <<  pMagicOp->m_MagicOpArg->GetTestString() << "]必须是下线强制不存的\n";
			CfgChk::GenExpInfo(str.str());
			return false;						
		}
	}
	return true;
}

bool CCfgMagicOp::CheckCancelableMopSetupStateNotSave(CCfgMagicOp* pMagicOp)
{
	if( pMagicOp->m_strMagicOpName == "安装魔法状态" || pMagicOp->m_strMagicOpName == "被动安装魔法状态" ||
		pMagicOp->m_strMagicOpName == "无视免疫安装魔法状态" || pMagicOp->m_strMagicOpName == "安装触发器状态" ||
		pMagicOp->m_strMagicOpName == "安装伤害变更状态" || pMagicOp->m_strMagicOpName == "安装积累触发状态" ||
		pMagicOp->m_strMagicOpName == "安装特殊状态" || pMagicOp->m_strMagicOpName == "按最高技能等级安装魔法状态")
	{
		string strStateName = pMagicOp->m_MagicOpArg->GetTestString();
		CCfgAllStateCheck::MapState::iterator itr = CCfgAllStateCheck::ms_mapState.find(strStateName);
		if ((*itr).second->GetNeedSaveToDB())
		{
			stringstream str;
			str << "\n配置表[魔法效果]第" << (pMagicOp->m_nID + 1) << "行违反约束：非状态的可撤销效果[" << CfgChk::g_sMagicEffName << 
				"]安装的状态[" <<  pMagicOp->m_MagicOpArg->GetTestString() << "]必须是下线强制不存的\n";
			CfgChk::GenExpInfo(str.str());
			return false;
		}
	}
	return true;
}

bool CCfgMagicOp::CheckPosMagicSetupStateType(CCfgMagicOp* pMagicOp)
{
	if( pMagicOp->m_strMagicOpName == "安装魔法状态" || pMagicOp->m_strMagicOpName == "被动安装魔法状态" ||
		pMagicOp->m_strMagicOpName == "无视免疫安装魔法状态" || pMagicOp->m_strMagicOpName == "按最高技能等级安装魔法状态")
	{
		string strStateName = pMagicOp->m_MagicOpArg->GetTestString();
		CCfgAllStateCheck::MapState::iterator itr = CCfgAllStateCheck::ms_mapState.find(strStateName);
		if ((*itr).second->GetCascadeType() == "唯一")
		{
			stringstream str;
			str << "【警告】配置表[魔法效果]第" << (pMagicOp->m_nID + 1) << "行违反约束：地点魔法的触发效果[" << CfgChk::g_sMagicEffName << 
				"]安装的状态[" <<  pMagicOp->m_MagicOpArg->GetTestString() << "]群体叠加类型为唯一\n";
			CfgLogErr(str.str());
			//只警告不返回false
			return true;
		}
	}
	return true;
}

bool CCfgMagicOp::CheckMutexPosMagicSetupStateType(CCfgMagicOp* pMagicOp)
{
	if( pMagicOp->m_strMagicOpName == "安装魔法状态" || pMagicOp->m_strMagicOpName == "被动安装魔法状态" ||
		pMagicOp->m_strMagicOpName == "无视免疫安装魔法状态" || pMagicOp->m_strMagicOpName == "按最高技能等级安装魔法状态")
	{
		string strStateName = pMagicOp->m_MagicOpArg->GetTestString();
		CCfgAllStateCheck::MapState::iterator itr = CCfgAllStateCheck::ms_mapState.find(strStateName);
		if ((*itr).second->GetCascadeType() == "集中")
		{
			stringstream str;
			str << "【警告】配置表[魔法效果]第" << (pMagicOp->m_nID + 1) << "行违反约束：互斥地点魔法的触发效果[" << CfgChk::g_sMagicEffName << 
				"]安装的状态[" <<  pMagicOp->m_MagicOpArg->GetTestString() << "]群体叠加类型为集中\n";
			CfgChk::GenExpInfo(str.str());
			return false;
		}
	}
	return true;
}
