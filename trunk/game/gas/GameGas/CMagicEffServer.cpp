#include "stdafx.h"
#include "CMagicEffServer.h"
#include "LoadSkillCommon.h"
#include "CCfgCalc.inl"
#include "CTxtTableFile.h"
#include "CGenericTarget.h"
#include "CFighterDictator.h"
#include "MagicOpPoss_Function.h"
#include "MagicOps_ChangeValue.h"
#include "MagicOps_Damage.h"
#include "MagicOps_Function.h"
#include "MagicConds_TestValue.inl"
#include "MagicConds_Function.h"
#include "CFilterOpServer.h"
#include "CMagicStateInstServer.h"
#include "CMagicStateCfg.h"
#include "CTempVarServer.h"
#include "CSkillInstServer.h"
#include "CCharacterDictator.h"
#include "CSkillMgrServer.h"
#include "CEntityServerManager.h"
#include "ErrLogHelper.h"
#include "CBaseStateServer.h"
#include "BaseHelper.h"
#include "CCoreSceneServer.h"
#include "CIntObjServer.h"
#include "TSqrAllocator.inl"
#include "TStringRef.inl"

template class TStringRefee<CMagicEffServer, CMagicEffServerSharedPtr>;

uint32 CMagicEffServer::ms_uExecNum = 0;

vector<CMagicOpTreeServer::VecResult*, TConfigAllocator<CMagicOpTreeServer::VecResult*> >	CMagicOpTreeServer::ms_vecResult;
vector<CMagicOpTreeServer::VecEntityID*, TConfigAllocator<CMagicOpTreeServer::VecEntityID*> >	CMagicOpTreeServer::ms_vecEntityID;
uint32 CMagicOpTreeServer::ms_uExecNum = 0;



void CMagicOpTreeServer::DeleteAll()
{
	uint32 uCount = ms_vecResult.size();
	for(size_t i = 0; i < uCount; ++i)
	{
		ClearVector(*ms_vecResult[i]);
		SafeDelete(ms_vecResult[i]);
	}
	ms_vecResult.clear();
	ClearVector(ms_vecEntityID);
}

CMagicOpTreeServer::VecEntityID& CMagicOpTreeServer::GetEntityIdVec()
{
	if (ms_vecEntityID.empty())
	{
		return *(new VecEntityID);
	}
	VecEntityID* uEntityID = ms_vecEntityID.back();
	ms_vecEntityID.pop_back();
	return *uEntityID;
}

void CMagicOpTreeServer::ReleaseEntityIdVec(VecEntityID& vecEntityID)
{
	vecEntityID.clear();
	ms_vecEntityID.push_back(&vecEntityID);
}

CMagicOpTreeServer::VecResult& CMagicOpTreeServer::GetVecResult()
{
	if(ms_vecResult.empty())
	{
		return *(new VecResult);
	}
	VecResult* vecResult = ms_vecResult.back();
	ms_vecResult.pop_back();
	return *vecResult;
}

void CMagicOpTreeServer::ReleaseVecResult(VecResult& vecResult)
{
	//SafeDelCtn1(vecResult);
	uint32 uCount = vecResult.size();
	for(size_t i = 0; i < uCount; ++i)
	{
		vecResult[i]->uMagicOpResult = 0;
		vecResult[i]->iMagicOpValue = 0;
	}
	ms_vecResult.push_back(&vecResult);
}


void CMagicEffServer::Do(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CGenericTarget* pTo, CMagicEffDataMgrServer* pCancelDataMgr)
{
	CGenericTarget Target(*pTo);
	pSkillInst->AddRef();
	CMagicOpTreeServer::VecResult& vecResult = CMagicOpTreeServer::GetVecResult();
	
	for(size_t i = vecResult.size(); i < m_uCount; ++i)
	{
		vecResult.push_back(new MagicOpValue);
	}
	
	CreateStoredObjArray(pSkillInst, pCancelDataMgr);

	CMagicOpTreeServer::VecEntityID& vecEntityID = CMagicOpTreeServer::GetEntityIdVec();

	SQR_TRY
	{
		m_pRoot->Do(pSkillInst, pFrom, &Target, vecResult,vecEntityID, pCancelDataMgr);
	}
	SQR_CATCH(exp)	//为了保障外部调用在调用魔法效果并发生异常的情况下的正常逻辑，需要在此加上容错
	{
		string strErrMsg(exp.ErrorMsg());
		ostringstream strm;		
		strm << " CMagicEffServer::Do m_pRoot->Do " << GetName() << " skillname " << pSkillInst->GetSkillName()<<endl;

		//如果是Npc则打印Npc的名字用于查bug
		if(pFrom && pFrom->GetCharacter() && !pFrom->CastToFighterMediator())
		{
			strm<<"ValidityCoefficient("<<pFrom->m_ValidityCoefficient.Get(pFrom)<<")";
			strm<<"NatureSmashValue("<<pFrom->m_NatureSmashValue.Get(pFrom)<<")";
			strm<<"DestructionSmashValue("<<pFrom->m_DestructionSmashValue.Get(pFrom)<<")";
			strm<<"EvilSmashValue("<<pFrom->m_EvilSmashValue.Get(pFrom)<<")";
			strm<<"DefenceSmashValue("<<pFrom->m_DefenceSmashValue.Get(pFrom)<<")";
			strm<<"self npc name :"<<pFrom->GetCharacter()->GetNpcName()<<endl;
		}
		CFighterDictator* pToFighter = pTo->GetFighter();
		if (pToFighter && pToFighter->GetCharacter() && !pToFighter->CastToFighterMediator())
		{
			strm<<"NatureResistanceValue("<<pToFighter->m_NatureResistanceValue.Get(pToFighter)<<")";
			strm<<"DestructionResistanceValue("<<pToFighter->m_DestructionResistanceValue.Get(pToFighter)<<")";
			strm<<"EvilResistanceValue("<<pToFighter->m_EvilResistanceValue.Get(pToFighter)<<")";
			strm<<"m_Defence("<<pToFighter->m_Defence.Get(pToFighter)<<")";
			strm<<"target npc name :"<<pTo->GetFighter()->GetCharacter()->GetNpcName()<<endl;
		}

		exp.AppendMsg(strm.str().c_str());

		if(pFrom)
		{
			CConnServer* pConn = pFrom->GetConnection();
			if(pConn)
				LogExp(exp, pConn);
		}
		LogExp(exp);
	}
	SQR_TRY_END;

	CMagicOpTreeServer::ReleaseEntityIdVec(vecEntityID);

	SQR_TRY
	{
		if(pSkillInst->GetType() == eIT_MagicStateInst)
		{
			if(!pSkillInst->GetIsCalc())
			{
				//暂时定为即使pFrom死亡也要执行撤销
				m_pRoot->CancelTempMOP(pSkillInst, pFrom, &Target, vecResult, pCancelDataMgr);
			}
		}
		else
		{
			//暂时定为即使pFrom死亡也要执行撤销
			m_pRoot->CancelTempMOP(pSkillInst, pFrom, &Target, vecResult, pCancelDataMgr);
		}
	}
	SQR_CATCH(exp)	//为了保障外部调用在调用魔法效果并发生异常的情况下的正常逻辑，需要在此加上容错
	{
		string strErrMsg(exp.ErrorMsg());
		ostringstream strm;		
		strm << " CMagicEffServer::Do m_pRoot->CancelTempMOP " << GetName() << " skillname " << pSkillInst->GetSkillName() << endl;
		exp.AppendMsg(strm.str().c_str());

		if(pFrom)
		{
			CConnServer* pConn = pFrom->GetConnection();
			if(pConn)
				LogExp(exp, pConn);
		}
		LogExp(exp);
	}
	SQR_TRY_END;

	CMagicOpTreeServer::ReleaseVecResult(vecResult);
	pSkillInst->DelRef();

#if defined _FightSkillCounter
	++ms_uExecNum;
#endif
}

void CMagicEffServer::Do(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CFighterDictator* pTo, CMagicEffDataMgrServer* pCancelDataMgr)
{
	CGenericTarget Target(pTo);
	Do(pSkillInst, pFrom, &Target, pCancelDataMgr);
}

EDoSkillResult CMagicEffServer::DoMagicCond(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CGenericTarget* pTo)
{
	CMagicOpTreeServer::VecEntityID& vecEntityID = CMagicOpTreeServer::GetEntityIdVec();
	EDoSkillResult eDoSkillRet = m_pRoot->DoMagicCond(pSkillInst,pFrom,pTo,vecEntityID);
	CMagicOpTreeServer::ReleaseEntityIdVec(vecEntityID);
	return eDoSkillRet;
}

void CMagicEffServer::Cancel(CSkillInstServer* pSkillInst, CFighterDictator* pOwner, CMagicEffDataMgrServer* pCancelDataMgr)
{
	SQR_TRY
	{
		m_pRoot->Cancel(pSkillInst, pOwner, pCancelDataMgr);
	}
	SQR_CATCH(exp)
	{
		if(pOwner)
		{
			CConnServer* pConn = pOwner->GetConnection();
			if(pConn)
			{
				LogExp(exp, pConn);
			}
			else
			{
				LogExp(exp);
			}
		}
	}
	SQR_TRY_END;
}

uint32 CMagicEffServer::GetMagicEffExecNum()
{
	return ms_uExecNum;
}

bool CMagicOpTreeServer::CanDo(const VecResult& vecResult)
{
	MapCond::iterator it = m_mapCond.begin();
	for(; it != m_mapCond.end(); ++it)
	{
		int32 iNodeNum = it->first;
		int32 iRet = it->second;

		if(vecResult[iNodeNum-1]->uMagicOpResult != uint32(iRet))
		{
			return false;
		}
	}
	return true;
}

uint32 CMagicOpTreeServer::FilterTargetAndExec(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CGenericTarget* pTo,VecResult& vecResult,VecEntityID& vecEntityID,CMagicEffDataMgrServer* pCancelDataMgr, uint32& eHurtResult)
{
	FilterLimit filterLimit;
	MakeFilterLimit(filterLimit, pSkillInst, this, pFrom);
	CFilterOpServer::FilterByPipe(pSkillInst, vecEntityID, m_vecFilterPipe, filterLimit, pFrom, pTo);

	VecEntityID::iterator it_obj = vecEntityID.begin();
	uint32 uObjCount = 0;
	uint32 uCount = vecEntityID.size();
	bool bAnyoneIsSuccess = false;	//该操作只要对一个目标的执行结果为成功则整个操作的返回值为成功，目前该规则只适用于魔法条件，魔法操作的返回值还是等于作用于最后一个目标的返回值
	for(; it_obj != vecEntityID.end(); ++it_obj)
	{
		--uCount;
		if (uCount == 0)
			pSkillInst->SetBeLastEffect(true);
		CEntityServer* pCharacter = CEntityServerManager::GetInst()->GetEntityByID(*it_obj);			
		if (!pCharacter ||
			(!pCharacter->GetFighter()->CppIsAlive() && !pSkillInst->GetTargetAliveCheckIsIgnored() &&
			 !m_pMagicOp->CanExecuteWithTargetIsDead()))
			/*|| (pTo->CppIsAlive() && pSkillInst->m_bTargetAliveCheckIsIgnored))*/	//!(*it_obj)的判断是因为目标类型为自己时(*it_obj)值为pFrom，若施法者下线，则由于pFrom为NULL导致(*it_obj)为NULL
			//之所以把上面这行屏蔽掉是因为it_obj有可能为自身，而这个技能又是对尸体施放的，这种情况下会造成那些筛选目标为“自身”的魔法操作不会被执行
		{
			continue;
		}
		CFighterDictator* pTarget=pCharacter->GetFighter();
		if(!pTarget)	//没有fighter的entity...
		{
			ostringstream strm;
			EGameEntityType type = pCharacter->GetGameEntityType();
			switch(type)
			{
			case eGET_CharacterDictator:
				strm << class_cast<CCharacterDictator*>(pCharacter)->m_sNpcName;
				break;
			case eGET_IntObject:
				strm << class_cast<CIntObjServer*>(pCharacter)->GetObjName();
				break;
			default:
				break;
			}

			strm << pCharacter->GetGameEntityType() << pCharacter->GetObjValidState() << endl;
			GenErr("惊现没有fighter的entity... entity_type,ObjValidState = ", strm.str());
		}
		uObjCount++;

		if (m_pMagicOp->GetType() == eMOPCT_MagicCondition)
		{		
			uint32 eResult;
			CMagicCondServer* pMagicCondOp = class_cast<CMagicCondServer*>(m_pMagicOp);

			if (pMagicCondOp->GetMagicCondType() == eMCT_Function)
			{
				CFunctionMagicCondServer* pMagicCondOp = class_cast<CFunctionMagicCondServer*>(m_pMagicOp);
				eResult = pMagicCondOp->Test(pSkillInst, m_MagicOpArg, pFrom, pTarget);
			}
			else
			{
				CValueMagicCondServer* pMagicCondOp = class_cast<CValueMagicCondServer*>(m_pMagicOp);
				eResult = pMagicCondOp->Test(pSkillInst, m_MagicOpArg, pFrom, pTarget);
			}
			
			if (eResult != eDSR_Success)
			{
				if(!bAnyoneIsSuccess)	//如果已经有一个目标的执行结果为成功则不再需要更新eHurtResult变量
				{
					eHurtResult = eHR_Fail;
				}
			}
			else
			{
				bAnyoneIsSuccess = true;
				eHurtResult = eHR_Success;
			}
		}
		else if(m_pMagicOp->GetType() == eMOPCT_MagicOP)
		{
			CMagicOpServer * pMagicOp = class_cast<CMagicOpServer*>(m_pMagicOp);

			if (pMagicOp->GetMagicOpType()==eMOT_Function)
			{
				CFunctionMagicOpServer * pMagicOp = class_cast<CFunctionMagicOpServer*>(m_pMagicOp);
				eHurtResult = DoMagicOp(pMagicOp, pSkillInst, m_MagicOpArg, pFrom, pTarget);
			}
			else if(pMagicOp->GetMagicOpType() == eMOT_PosFunction)
			{
				CPosMagicOpServer * pMagicOp = class_cast<CPosMagicOpServer*>(m_pMagicOp);
				CFPos pos;
				pTarget->GetPixelPos(pos);
				eHurtResult = DoMagicOp(pMagicOp, pSkillInst, m_MagicOpArg, pFrom, pos);
			}
			else
			{
				CValueMagicOpServer* pMagicOp = class_cast<CValueMagicOpServer*>(m_pMagicOp);

				if (pFrom)
				{
					eHurtResult = DoMagicOp(pMagicOp, pSkillInst, m_MagicOpArg, pFrom, pTarget, &(pFrom->GetTempVarMgr()->m_iMopCalcValue), it_obj - vecEntityID.begin(), pCancelDataMgr);
					vecResult[m_uIndex-1]->iMagicOpValue = pFrom->GetTempVarMgr()->m_iMopCalcValue;
				}
				else
				{
					eHurtResult = DoMagicOp(pMagicOp, pSkillInst, m_MagicOpArg, pFrom, pTarget, &(vecResult[m_uIndex-1]->iMagicOpValue), it_obj - vecEntityID.begin(), pCancelDataMgr);
				}
			}

			if(!(pSkillInst->GetType() == eIT_MagicStateInst && pSkillInst->GetIsCalc()))
			{
				if (m_strFX != "" || m_eFxType == eFT_Suffer)
				{
					bool bPlayFx = true;
					vector<MagicEffFilter*>::iterator it = m_vecFilterPipe.begin();
					if ((*it)->m_eObjectFilter == eOF_Position)
						bPlayFx = false;
					pTarget->GetSkillMgr()->OnDoMagicOp(m_uId, true, pFrom?pFrom->GetEntityID():0, pTarget?pTarget->GetEntityID():0, CFPos(0,0), eHurtResult, bPlayFx);
				}
			}
		}
	}

	return uObjCount;
}

void CMagicOpTreeServer::ExecWithNoFilterTarget(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CMagicEffDataMgrServer* pCancelDataMgr, uint32& eHurtResult)
{
	eHurtResult = eHR_Fail;

	if(m_pMagicOp->GetType()==eMOPCT_MagicOP && class_cast<CMagicOpServer*>(m_pMagicOp)->GetMagicOpType()==eMOT_Value &&
	   pSkillInst->GetType() == eIT_MagicStateInst && pSkillInst->GetIsCalc())
	{
		//魔法状态调用再次筛选目标为范围选择的魔法操作时，若有效目标集合为空，则必须至少执行一次DoMagicOp()进入calc以保证之后可能出现的的exec有计算结果可用
		//另外死亡时不能执行魔法状态改变值效果的计算，如果有需要死亡时安装的魔法状态但又不执行状态已有的任何改变值效果，则这里需要排除因为死亡导致uObjCount=0的情况，
		//但就需求来说不可能有这么变态的要求，所以这个异常检测加回
		//另外魔法状态之外的状态不需要计算值，因此也不会进这个分支，但仍需要注意撤销与安装的效果不一致的情况
		CValueMagicOpServer* pMagicOp = class_cast<CValueMagicOpServer*>(m_pMagicOp);
		if(pMagicOp->CalcNeedTarget(m_MagicOpArg))
		{
			stringstream sExp;
			sExp << ((class_cast<CSkillMagicStateInstServer*>(pSkillInst->GetLocalInst()))->GetCfg()->GetName()) << endl;
			GenErr("安装状态时调用目标再次筛选结果为空的改变值魔法操作计算部分不允许目标为空", sExp.str());
		}
		else
		{
			eHurtResult = DoMagicOp(pMagicOp, pSkillInst, m_MagicOpArg, pFrom, NULL, &(pFrom->GetTempVarMgr()->m_iMopCalcValue), 0, pCancelDataMgr);
		}
	}
}

uint32 CMagicOpTreeServer::Do(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CGenericTarget* pTo,
	VecResult& vecResult, VecEntityID& vecEntityID, CMagicEffDataMgrServer* pCancelDataMgr)
{
	pSkillInst->SetValueScale(0);
	uint32 eHurtResult = eHR_Success;
	CEntityServer* pEntity = CEntityServerManager::GetEntityByID(pSkillInst->GetCreatorID());
	CFighterDictator* pFighterCreator = pEntity?pEntity->GetFighter():NULL;

	pSkillInst->SetBeLastEffect(false);
	pSkillInst->SetEffectHPChangeValue(0);
	//如果有死亡时安装复活后需要执行间隔和最终效果的状态，则需要将StateMopNeedForceCalc与整个条件做或运算，并在之后所有判断角色死亡的地方加上跟StateMopNeedForceCalc的或运算
	if( (StateMopNeedForceCalc(pSkillInst) || CanDo(vecResult)) && (pFrom == NULL || pFrom->CppIsAlive() || m_pMagicOp->CanExecuteWithAttackerIsDead()) )
	{
		vector<MagicEffFilter*>::iterator it = m_vecFilterPipe.begin();
		if ((*it)->m_eObjectFilter == eOF_Position && m_vecFilterPipe.size() == 1)
		{
			if (pFrom)
			{
				CFPos pos=pTo->GetPos();
				if (pFighterCreator && (m_strFX != "" || m_eFxType == eFT_Suffer))
					pFighterCreator->GetSkillMgr()->OnDoMagicOp(m_uId, false, pFighterCreator->GetEntityID(), 0, pos);
				CPosMagicOpServer* pMagicOp = class_cast<CPosMagicOpServer*>(m_pMagicOp);
				eHurtResult = DoMagicOp(pMagicOp, pSkillInst, m_MagicOpArg, pFrom, pTo->GetPos());
			}
		}
		else
		{
			if ((*it)->m_eObjectFilter == eOF_Position)
			{
				CFPos pos;
				if (eTT_Position==pTo->GetType())
				{
					pos=pTo->GetPos();
				}
				else
				{
					GenErr("目标类型错误");
				}
				CFighterDictator* pFighter=pFighterCreator?pFighterCreator:pFrom;
				if (pFighter && (m_strFX != "" || m_eFxType == eFT_Suffer))
				{
					pFighter->GetSkillMgr()->OnDoMagicOp(m_uId, false, pFighterCreator->GetEntityID(), 0, pos);
				}
			}

			if(FilterTargetAndExec(pSkillInst, pFrom, pTo, vecResult, vecEntityID, pCancelDataMgr, eHurtResult) == 0)
			{
				ExecWithNoFilterTarget(pSkillInst, pFrom, pCancelDataMgr, eHurtResult);
			}
		}
		vecResult[m_uIndex-1]->uMagicOpResult = eHurtResult;
	}
	else
	{
		vecResult[m_uIndex-1]->uMagicOpResult = eHR_Fail;
	}

	ListChild::iterator it_list = m_listChild.begin();
	ListChild::iterator it_End= m_listChild.end();
	
	for(; it_list != it_End; ++it_list)
	{
		class_cast<CMagicOpTreeServer*>(*it_list)->Do(pSkillInst, pFrom, pTo, vecResult, vecEntityID,pCancelDataMgr);
	}

#if defined _FightSkillCounter
	++ms_uExecNum;
#endif
	return eHurtResult;
}

EDoSkillResult CMagicOpTreeServer::DoMagicCond(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CGenericTarget* pTo,VecEntityID& vecEntityID)
{
	if (!m_pMagicOp->GetType() == eMOPCT_MagicCondition|| !m_mapCond.empty())
		return eDSR_Success;

	PreDoMagicCond(pSkillInst);

	CMagicCondServer* pMagicCond = class_cast<CMagicCondServer*>(m_pMagicOp);
	EDoSkillResult eResult = eDSR_Fail;

	FilterLimit filterLimit;
	MakeFilterLimit(filterLimit, pSkillInst, this, pFrom);
	CFilterOpServer::FilterByPipe(pSkillInst, vecEntityID, m_vecFilterPipe, filterLimit, pFrom, pTo);

	VecEntityID::iterator it_obj = vecEntityID.begin();
	for(; it_obj != vecEntityID.end(); ++it_obj)
	{
		CCharacterDictator* pCharacter = CEntityServerManager::GetInst()->GetCharacter(*it_obj);
		if (NULL==pCharacter)
		{
			continue;
		}
		CFighterDictator* pTo=pCharacter->GetFighter();
		if (pMagicCond->GetMagicCondType() == eMCT_Value)
		{
			CValueMagicCondServer* pMagicCond = class_cast<CValueMagicCondServer*>(m_pMagicOp);
			eResult = EDoSkillResult(pMagicCond->Test(pSkillInst, m_MagicOpArg, pFrom, pTo));
		}
		else
		{
			CFunctionMagicCondServer* pMagicCond = class_cast<CFunctionMagicCondServer*>(m_pMagicOp);
			eResult = EDoSkillResult(pMagicCond->Test(pSkillInst, m_MagicOpArg, pFrom, pTo));
		}

		//if (eResult != eDSR_Success)
			//return eResult;
		if(eResult == eDSR_Success)
		{
			break;
		}
	}

	ListChild::iterator it_list = m_listChild.begin();
	for(; it_list != m_listChild.end(); ++it_list)
	{
		EDoSkillResult eChildResult = class_cast<CMagicOpTreeServer*>(*it_list)->DoMagicCond(pSkillInst, pFrom, pTo,vecEntityID);
		if (eChildResult!= eDSR_Success)
			return eChildResult;
	}

	return eResult;
}


void CMagicOpTreeServer::CancelTempMOP(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CGenericTarget* pTo,VecResult& vecResult, CMagicEffDataMgrServer* pCancelDataMgr)
{
	//pCancelDataMgr参数在这个函数里暂时不用
	if (m_strMOPType=="临时"&&m_pMagicOp->GetType()==eMOPCT_MagicOP)
	{
		if(CanDo(vecResult))
		{
			CMagicOpServer * pMagicOp = class_cast<CMagicOpServer*>(m_pMagicOp);	
			CEntityServer* pEntity = CEntityServerManager::GetEntityByID(pSkillInst->GetCreatorID());
			CFighterDictator* pFighterCreator = pEntity?pEntity->GetFighter():NULL;
			CFighterDictator*	pTarget=0;
			vector<MagicEffFilter*>::iterator it = m_vecFilterPipe.begin();
			if ((*it)->m_eObjectFilter==eOF_Target && pTo->GetType()==eTT_Fighter)
			{
				pTarget=pTo->GetFighter();
			}
			else
			{
				pTarget=pFrom;
			}
			
			if(pTarget != NULL&&pMagicOp->Cancelable())
			{
				if ((int32)pMagicOp->GetMagicOpType()==(int32)eMCT_Function)
				{
					CFunctionCancelableMagicOp * pMagicOp = class_cast<CFunctionCancelableMagicOp*>(m_pMagicOp);
					pMagicOp->Cancel(pSkillInst, m_MagicOpArg, pFighterCreator, pTarget);
				} 
				else
				{
					CValueCancelableMagicOp* pMagicOp = class_cast<CValueCancelableMagicOp*>(m_pMagicOp);
					pMagicOp->CancelExec(pSkillInst, pFighterCreator, pTarget,
						CValueData(vecResult[m_uIndex-1]->iMagicOpValue));
				}	
			}
		}
	}
	ListChild::iterator it_list = m_listChild.begin();
	for(; it_list != m_listChild.end(); ++it_list)
	{
		class_cast<CMagicOpTreeServer*>(*it_list)->CancelTempMOP(pSkillInst,pFrom,pTo,vecResult, pCancelDataMgr);
	}
}

void CMagicOpTreeServer::Cancel(CSkillInstServer* pSkillInst, CFighterDictator* pOwner, CMagicEffDataMgrServer* pCancelDataMgr)
{
	if(pOwner && (pOwner->CppIsAlive() || pSkillInst->GetTargetAliveCheckIsIgnored() || m_pMagicOp->CanExecuteWithTargetIsDead()))
	{
		CGenericTarget Target(pOwner);
		VecEntityID& vecEntityID = CMagicOpTreeServer::GetEntityIdVec();
		FilterLimit filterLimit;
		MakeFilterLimit(filterLimit, pSkillInst, this, pOwner);
		CFilterOpServer::FilterByPipe(pSkillInst, vecEntityID, m_vecFilterPipe, filterLimit, pOwner, &Target);

		if (vecEntityID.empty())
		{
			CMagicOpTreeServer::ReleaseEntityIdVec(vecEntityID);
			return;
		}
		VecEntityID::iterator it_obj = vecEntityID.begin();
		CCharacterDictator* pCharacter = CEntityServerManager::GetInst()->GetCharacter(*it_obj);
		if (NULL==pCharacter)
		{
			CMagicOpTreeServer::ReleaseEntityIdVec(vecEntityID);
			return;
		}
		CFighterDictator* pTo=pCharacter->GetFighter();
		if (m_pMagicOp->GetType()==eMOPCT_MagicOP)
		{
			CMagicOpServer * pMagicOp = class_cast<CMagicOpServer*>(m_pMagicOp);
			if (pMagicOp->Cancelable())
			{
				//CEntityServer* pEntity = CEntityServerManager::GetEntityByID(pSkillInst->GetCreatorID());
				//CFighterDictator* pFighterCreator = pEntity?pEntity->GetFighter():NULL;
				CancelMagicOp(m_pMagicOp, pSkillInst, m_MagicOpArg, pTo, pCancelDataMgr);
			}
		}
		CMagicOpTreeServer::ReleaseEntityIdVec(vecEntityID);
		ListChild::iterator it_list = m_listChild.begin();
		for(; it_list != m_listChild.end(); ++it_list)
		{
			//uint32 uID = pSkillInst->GetID();
			class_cast<CMagicOpTreeServer*>(*it_list)->Cancel(pSkillInst,pOwner, pCancelDataMgr);
			//if(!CSkillInstServer::ExistSkillInst(uID))
			//	return;
		}
	}
}

//魔法条件的预判
void CMagicOpTreeServer::PreDoMagicCond(CSkillInstServer* pSkillInst)
{
	//if(pSkillInst->GetType() == eIT_MagicStateInst)
	//{
	//	GenErr("魔法状态调用的魔法效果不能含有魔法条件\n");
	//}
	
	//状态效果不能含有魔法条件的限制解禁，但现在只允许条件的结果为安装触发器状态、安装伤害变更状态
	//以后需要在StateInst保存魔法操作树执行流程，并在撤销时按这个流程撤销各魔法操作
}

//魔法操作种类的预判
bool CMagicOpTreeServer::PreDoMagicOp(CSkillInstServer* pSkillInst)
{
	if(pSkillInst->GetType() == eIT_MagicStateInst)
	{
		stringstream str;
		str << "魔法状态不能调用临时魔法操作\n";
		GenErr(str.str());
		//return false;
	}
	return true;
}


void CMagicEffServer::CreateStoredObjArray(CSkillInstServer* pSkillInst, CMagicEffDataMgrServer* pCancelDataMgr)
{
	if(pSkillInst->GetType() == eIT_MagicStateInst)
	{
		if(pSkillInst->GetIsCalc())
		{
			Ast(pCancelDataMgr);
			pCancelDataMgr->CreateArrayMopData(m_uCount);
		}
	}
	else if(pCancelDataMgr)
	{
		pCancelDataMgr->CreateArrayMopData(m_uCount);
	}
}

//功能型魔法操作调用
uint32 CMagicOpTreeServer::DoMagicOp(CFunctionMagicOpServer* pMagicOp, CSkillInstServer* pSkillInst, const CCfgArg* Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if(pFrom == NULL || pFrom->CppIsAlive() || m_pMagicOp->CanExecuteWithAttackerIsDead())
	{
		if(pSkillInst->GetType() == eIT_MagicStateInst)
		{
			if(pSkillInst->GetIsCalc())
			{
				return eHR_Fail;
			}
			//pSkillInst->ChangeToSkillInst();
			uint32 uRet = pMagicOp->Do(pSkillInst, Arg, pFrom, pTo);
			//pSkillInst->RevertInstType();
			return uRet; 
		}
		else
		{
			return pMagicOp->Do(pSkillInst, Arg, pFrom, pTo);
		}
	}
	else
	{
		return eHR_Fail;
	}
}

//改变型魔法操作调用
uint32 CMagicOpTreeServer::DoMagicOp(CValueMagicOpServer* pMagicOp, CSkillInstServer* pSkillInst,
	const CCfgArg* Arg, CFighterDictator* pFrom, CFighterDictator* pTo, int32* iMopCalcValue,
	size_t iTargetIndex, CMagicEffDataMgrServer* pCancelDataMgr)
{
	CValueData TempValue;
	if(pFrom == NULL || pFrom->CppIsAlive() || m_pMagicOp->CanExecuteWithAttackerIsDead())
	{
		EInstType eInstType = pSkillInst->GetType();

		if(eInstType == eIT_MagicStateInst)				//由魔法状态调用而来
		{
			if(pSkillInst->GetIsCalc())									//由魔法状态的计算调用
			{
				if(pMagicOp->GetMagicOpType() == eMOT_Value)						//魔法操作是改变型
				{
					if(iTargetIndex == 0)										//新的魔法操作行（即目标为第一个人）
					{
						uint32 uMOPRetValue = 0;

						uint32 iExtraValue = 0;	// 这个值传给魔法操作暂时只是为了避免传空指针，并没有其他用途
						uMOPRetValue = pMagicOp->MagicOpCalc(pSkillInst, Arg, pFrom, pTo, &TempValue, &iExtraValue);
						if (iMopCalcValue)
						{
							*iMopCalcValue=TempValue.GetInt32();
						}		
						Ast(pCancelDataMgr && "状态调用改变值DoMagicOp计算部分的pCancelDataMgr不能为空");
						pCancelDataMgr->SetArrayMopDataForMagicState(m_uIndex, CMagicOpStoredObj(TempValue, uMOPRetValue));//这里似乎有点问题，创建一个局部的CMagicOpStoredObj对象 
						return uMOPRetValue;
					}
				}
				else	//这个条件分支本来已经由魔法效果调用本函数处判断，但若以后统一魔法效果调用本函数处的判断分支，则这块代码就必须起作用
				{
					//魔法状态的计算部分调用改变型以外（目前即功能型和地点型）的魔法操作则直接返回
					return eHR_Fail;
				}
			}
			else														//由魔法状态的执行调用
			{
				CSkillLocalInstServer* pLocalInst = pSkillInst->GetLocalInst();
				if(pLocalInst && pLocalInst->GetContainerExist())
				{
					if(pMagicOp->GetMagicOpType() == eMOT_Value)
					{
						//CMagicEffDataMgrServer* pDataMgr = pStateInst->GetCurEffDataMgr();

						Ast(pCancelDataMgr && "状态调用改变值DoMagicOp计算部分的pCancelDataMgr不能为空");
						const CMagicOpStoredObj& StoredObj = pCancelDataMgr->GetArrayMopDataForMagicState(m_uIndex, pSkillInst);
						if(!StoredObj.m_bValid)
						{
							stringstream str;
							str << "[" << class_cast<CSkillMagicStateInstServer*>(pSkillInst->GetLocalInst())->GetCfg()->GetName() << "]第[" <<
								GetLineNo() << "]行[" << m_strMagicOpName << "]\n";
							GenErr("状态改变值型魔法操作未经过计算", str.str());
						}
						//pSkillInst->ChangeToSkillInst();
						pMagicOp->MagicOpExec(pSkillInst, pFrom, pTo, StoredObj.m_MOPArg, (EHurtResult)StoredObj.m_uMOPRet);
						*iMopCalcValue = StoredObj.m_MOPArg.GetInt32();
						//pSkillInst->RevertInstType();
						//注意这里的返回值约定为MagicOpExec的需要的第二个数值参数，否则还需要在StoredObj里保存这个返回值
						return (uint32)StoredObj.m_uMOPRet;//此时魔法状态有可能已被删除，但它的Inst有可能还未被删除，所以StoredObj有可能还存在
					}
					else	//这个条件分支本来已经由魔法效果调用本函数处判断，但若以后统一魔法效果调用本函数处的判断分支，则这块代码就必须起作用
					{
						uint32 uRet;
						//魔法状态的执行部分调用改变型以外（目前即功能型和地点型）的魔法操作则与技能直接调用该魔法操作的规则一致
						//pSkillInst->ChangeToSkillInst();
						uRet = pMagicOp->Do(pSkillInst, Arg, pFrom, pTo, iMopCalcValue);
						//pSkillInst->RevertInstType();
						return uRet;
					}
				}
			}
		}
		else if(eInstType != eIT_None)				//由技能直接调用而来
		{
			if(pCancelDataMgr)		//如果是调用可撤销效果（状态除外）
			{
				uint32 uMOPRetValue = 0;

				uint32 iExtraValue = 0;	// 这个值传给魔法操作暂时只是为了避免传空指针，并没有其他用途
				uMOPRetValue = pMagicOp->MagicOpCalc(pSkillInst, Arg, pFrom, pTo, &TempValue, &iExtraValue);
				if (iMopCalcValue)
				{
					*iMopCalcValue=TempValue.GetInt32();
				}		
				pCancelDataMgr->SetArrayMopDataForOther(m_uIndex, CMagicOpStoredObj(TempValue, uMOPRetValue));
				
				const CMagicOpStoredObj& StoredObj = pCancelDataMgr->GetArrayMopDataForOther(m_uIndex, pSkillInst);
				pMagicOp->MagicOpExec(pSkillInst, pFrom, pTo, StoredObj.m_MOPArg, (EHurtResult)StoredObj.m_uMOPRet);
				//*iMopCalcValue = StoredObj.m_MOPArg.GetInt32();
			}
			else
			{
				return pMagicOp->Do(pSkillInst, Arg, pFrom, pTo,iMopCalcValue);
			}
		}
	}
	return eHR_Fail;
}

//地点型魔法操作调用
uint32 CMagicOpTreeServer::DoMagicOp(CPosMagicOpServer* pMagicOp, CSkillInstServer* pSkillInst, const CCfgArg* Arg, CFighterDictator* pFrom, const CFPos& pTo)
{
	if(pFrom)
	{
		CCoreSceneServer* pScene = pFrom->GetScene();
		if(!pScene->IsPixelValid(pTo))
			return eHR_Fail;
	}

	if(pFrom == NULL || pFrom->CppIsAlive() || m_pMagicOp->CanExecuteWithAttackerIsDead())
	{
		//if(pSkillInst->GetType() == eIT_MagicStateInst)
		//{
		//	GenErr("魔法状态不能调用目标为地点的魔法操作\n");
		//	return eHR_Fail;
		//}
		//else
		if(pSkillInst->GetType() == eIT_MagicStateInst)
		{
			if(pSkillInst->GetIsCalc())
			{
				return eHR_Fail;
			}
			//pSkillInst->ChangeToSkillInst();
			uint32 uRet = pMagicOp->Do(pSkillInst, Arg, pFrom, pTo);
			//pSkillInst->RevertInstType();
			return uRet;
		}
		else
		{
			return pMagicOp->Do(pSkillInst, Arg, pFrom, pTo);
		}
	}
	else
	{
		return eHR_Fail;
	}
}

//可撤销魔法效果的撤销
void CMagicOpTreeServer::CancelMagicOp(CBaseMagicOpServer* pBaseMagicOp,
	CSkillInstServer* pSkillInst, const CCfgArg* Arg,
	CFighterDictator* pTo, CMagicEffDataMgrServer* pCancelDataMgr)
{
	CMagicOpServer* pMagicOp = class_cast<CMagicOpServer*>(pBaseMagicOp);
	if (m_strMOPType=="临时")
	{
		//临时操作只能由CancelTempMOP()撤销，否则变成重复撤销
		return;
	}

	//CEntityServer* pEntity = CEntityServerManager::GetEntityByID(pSkillInst->GetCreatorID());
	//CFighterDictator* pFrom = pEntity?pEntity->GetFighter():NULL;

	CFighterDictator* pFrom = NULL;
	if(pCancelDataMgr)
	{
		CEntityServer* pEntityFrom = CEntityServerManager::GetEntityByID(pCancelDataMgr->GetFromID());
		if(pEntityFrom)
			pFrom = pEntityFrom->GetFighter();
	}
	switch(pMagicOp->GetMagicOpType())
	{
	case eMOT_Function:
		{
			CFunctionCancelableMagicOp* pFCMagicOp = class_cast<CFunctionCancelableMagicOp*>(pMagicOp);
			if(pSkillInst->GetType() == eIT_MagicStateInst)				//由魔法状态调用而来
			{
				//pSkillInst->ChangeToSkillInst();
				pFCMagicOp->Cancel(pSkillInst, Arg, pFrom, pTo);
				//pSkillInst->RevertInstType();
			}
			else
			{
				pFCMagicOp->Cancel(pSkillInst, Arg, pFrom, pTo);
			}
		}
		return;
	case eMOT_Value:
		{
			if(pSkillInst->GetType() == eIT_MagicStateInst)				//由魔法状态调用而来
			{
				//CMagicStateCascadeDataMgr *pStateInst = class_cast<CMagicStateCascadeDataMgr*>(pSkillInst);

				//CMagicEffDataMgrServer* pDataMgr = pStateInst->GetCurEffDataMgr();

				Ast(pCancelDataMgr && "状态调用改变值CancelMagicOp的pCancelDataMgr不能为空");
				const CMagicOpStoredObj& StoredObj = pCancelDataMgr->GetArrayMopData(m_uIndex);
				//以下异常抛得没有意义；因为条件不满足的分支魔法操作计算值默认是0，撤销时会全部执行到
				//if(!StoredObj.m_bValid)
				//{
				//	stringstream str;
				//	str << "状态[" << pStateInst->GetCfg()->GetName() << "]撤销的第[" <<
				//		GetLineNo() << "]行改变值型魔法操作[" << m_strMagicOpName << "]未经过计算\n";
				//	GenErr(str.str());
				//}

				CValueCancelableMagicOp* pVCMagicOp = class_cast<CValueCancelableMagicOp*>(pMagicOp);
				pVCMagicOp->CancelExec(pSkillInst, pFrom, pTo, StoredObj.m_MOPArg);
			}
			else //由技能直接调用而来
			{
				if(pCancelDataMgr)
				{
					const CMagicOpStoredObj& StoredObj = pCancelDataMgr->GetArrayMopData(m_uIndex);

					CValueCancelableMagicOp* pVCMagicOp = class_cast<CValueCancelableMagicOp*>(pMagicOp);
					pVCMagicOp->CancelExec(pSkillInst, pFrom, pTo, StoredObj.m_MOPArg);
				}
				else
				{
					CValueCancelableMagicOp* pVCMagicOp = class_cast<CValueCancelableMagicOp*>(pMagicOp);
					pVCMagicOp->Cancel(pSkillInst, Arg, pFrom, pTo);
				}
			}
		}
		return;
	default:
		{
			GenErr("功能型和改变型以外的魔法操作不能调用Cancel函数\n");
			return;
		}
	}
}


uint32 CMagicOpTreeServer::GetMagicOpExecNum()
{
	return ms_uExecNum;
}

void CMagicOpTreeServer::MakeFilterLimit(FilterLimit& filterLimit, CSkillInstServer* pInst, CMagicOpTreeServer* pMagicOpTree, CFighterDictator* pFrom)
{
	filterLimit.bIsValueMagicOp=false;
	if(pMagicOpTree->m_pMagicOp->GetType()==eMOPCT_MagicOP)
	{
		CMagicOpServer * pMagicOp = class_cast<CMagicOpServer*>(pMagicOpTree->m_pMagicOp);
		if((int32)pMagicOp->GetMagicOpType()==(int32)eMCT_Value)
		{
			filterLimit.bIsValueMagicOp=true;
		}
	}
	filterLimit.bTargetAliveCheckIsIgnored=pInst->GetTargetAliveCheckIsIgnored() || pMagicOpTree->m_pMagicOp->CanExecuteWithTargetIsDead();
}

bool CMagicOpTreeServer::StateMopNeedForceCalc(CSkillInstServer* pSkillInst)
{
	//魔法状态的间隔和最终效果的改变值魔法操作以及之前的目标筛选操作必须在计算的时候执行一次计算
	if(pSkillInst->GetType() == eIT_MagicStateInst)
	{
		CSkillMagicStateInstServer* pStateInst = class_cast<CSkillMagicStateInstServer*>(pSkillInst->GetLocalInst());
		if(pStateInst->GetIsCalc() && pStateInst->GetMagicStateEffType() != eMSET_Cancellable)
		{
			switch(m_pMagicOp->GetType())
			{
			case eMOPCT_MagicOP:
				{
					CMagicOpServer* pMagicOp = class_cast<CMagicOpServer*>(m_pMagicOp);
					if(pMagicOp->GetMagicOpType() == eMOT_Value)
					{
						return true;
					}
				}
			default:
				break;
			}
		}
	}
	return false;
}

