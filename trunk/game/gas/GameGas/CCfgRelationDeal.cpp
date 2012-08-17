#include "stdafx.h"
#include "CCfgRelationDeal.h"
#include "CCfgColChecker.h"
#include "CMagicEffServer.h"
#include "CMagicCondServer.h"
#include "CFunctionMagicCondServer.h"
#include "CBaseStateCfgServer.h"
#include "CCfgArg.h"

CCfgRelationDeal::SetMagicOpTreeServer CCfgRelationDeal::m_setMopTreeForSyncStateInArg;
CCfgRelationDeal::SetMagicOpTreeServer CCfgRelationDeal::m_setMopTreeForSyncStateInArgCalc;
CCfgRelationDeal::MapPingPongObjPersistent CCfgRelationDeal::m_mapPingPongObjPersistent;

bool CCfgRelationDeal::ResetMap()
{
	m_mapPingPongObjPersistent["安装或清除魔法状态"] = eOPT_Inexistence;
	m_mapPingPongObjPersistent["安装或清除触发器状态"] = eOPT_Inexistence;
	m_mapPingPongObjPersistent["安装或清除姿态"] = eOPT_Inexistence;
	return true;
}

bool CCfgRelationDeal::RelationDeal()
{
	bool bRet = true;
	//魔法效果关系约束（读表依赖）
	CfgChk::SetTabName("魔法效果");
	for(SetMagicOpTreeServer::iterator itr = m_setMopTreeForSyncStateInArg.begin();
		itr != m_setMopTreeForSyncStateInArg.end(); ++itr)
	{
		SetStateNeedSyncInArg((*itr));
	}
	m_setMopTreeForSyncStateInArg.clear();

	for(SetMagicOpTreeServer::iterator itr = m_setMopTreeForSyncStateInArgCalc.begin();
		itr != m_setMopTreeForSyncStateInArgCalc.end(); ++itr)
	{
		SetStateNeedSyncInArgCalc((*itr));
	}
	m_setMopTreeForSyncStateInArgCalc.clear();
	CfgChk::SetTabNull();
	return bRet;
}

void CCfgRelationDeal::AddMopTreeForSync(CMagicOpTreeServer* pMagicOpTree)
{
	if(pMagicOpTree->m_pMagicOp->GetType() == eMOPCT_MagicCondition)
	{
		CMagicCondServer* pMagicCond = class_cast<CMagicCondServer*>(pMagicOpTree->m_pMagicOp);
		switch(pMagicCond->GetMagicCondType())
		{
		case eMCT_Function:
			{
				CFunctionMagicCondServer* pFunMagicCond = class_cast<CFunctionMagicCondServer*>(pMagicCond);
				if(pFunMagicCond->GetStateNeedSync())
				{
					if(pFunMagicCond->GetStateNeedSync())
					{
						m_setMopTreeForSyncStateInArg.insert(pMagicOpTree);
					}
				}
			}
			break;
		case eMCT_Value:
			{
				m_setMopTreeForSyncStateInArgCalc.insert(pMagicOpTree);
			}
			break;
		}
	}
}

void CCfgRelationDeal::SetStateNeedSyncInArg(CMagicOpTreeServer* pMopTree)
{
	string sStateName;
	CCfgCalc pMagicOpArg(pMopTree->m_strMagicOpArg);
	switch(pMagicOpArg.GetStrType())
	{
	case eST_String:
		sStateName = pMagicOpArg.GetString();
		break;
	case eST_MutliString:
		sStateName = pMagicOpArg.GetString(0);
		break;
	default:
		return;
	}
	CBaseStateCfg* pBaseState = CBaseStateCfg::GetByGlobalName(sStateName);
	if(!pBaseState)
		return;
	CBaseStateCfgServer* pState = class_cast<CBaseStateCfgServer*>(pBaseState);
	if(pState)
	{
		pState->SetNeedSync();
	}
}

void CCfgRelationDeal::SetStateNeedSyncInArgCalc(CMagicOpTreeServer* pMopTree)
{
	CMagicOpCalcArg* pCalcArg = static_cast<CMagicOpCalcArg*>(pMopTree->m_MagicOpArg);
	if (!pCalcArg)
	{
		return;
	}
	CCfgCalc* pOpArg = pCalcArg->m_pArg;
	uint32 uExprCount = pOpArg->m_vecExprs.size();
	for(uint32 indexExpr = 0; indexExpr < uExprCount; ++indexExpr)
	{
		//CExpression& expr = pOpArg->m_vecExprs[indexExpr];
		CExpression::VecExprUnit& vecExpr = pOpArg->m_vecExprs[indexExpr]->m_vecExpr;
		uint32 uUnitCount = vecExpr.size();
		for(uint32 indexUnit = 0; indexUnit < uUnitCount; ++indexUnit)
		{
			CExprUnit unit = vecExpr[indexUnit];
			if(unit.m_eType == eUT_String)
			{
				CBaseStateCfg* pBaseState = CBaseStateCfg::GetByGlobalName(*unit.m_Value.pStr);
				if(!pBaseState)
					return;
				CBaseStateCfgServer* pState = class_cast<CBaseStateCfgServer*>(pBaseState);
				if(pState)
				{
					pState->SetNeedSync();
				}
			}
		}
	}
}
