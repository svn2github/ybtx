#include "stdafx.h"
#include "CMoveMagicServer.h"
#include "CMoveMagicCfgServer.h"
#include "CMagicServer.inl"
#include "CoreCommon.h"
#include "CFighterDictator.h"
#include "CCoreSceneServer.h"
#include "CMagicEffServer.h"
#include "CFilterOpServer.h"
#include "CGenericTarget.h"
#include "CCharacterDictator.h"
#include "CEntityServerManager.h"
#include "CMagicMgrServer.h"
#include "IFighterServerHandler.h"
#include "CSkillInstServer.h"
#include "CActorServer.h"
#include "PatternRef.inl"
#include "CCoreObjectDictator.h"
#include "CTriggerEvent.h"
#include "CAppConfigServer.h"
#include "TSqrAllocator.inl"
#include "CMagicEffDataMgrServer.h"
#include "BaseHelper.h"
#include "CSkillMgrServer.h"

template class TPtBiRef<IMoveManager,CEntityServer>;

//初始化位移魔法

CBaseMoveMagicServer::CBaseMoveMagicServer(CSkillInstServer* pInst,CFighterDictator* pFighter,CGenericTarget* pTarget,const MoveInfo& moveInfo, const string& strName,const CMagicEffServerSharedPtr& pMagicEff,EMoveActionType eMoveActionType, uint32& uMoveMagicMoveID)
	:IMoveManager(pFighter->GetHandler()->GetEntity())
	,m_pInst(pInst)
	,m_pMagicEff(new CMagicEffServerSharedPtr(pMagicEff))
	,m_strMoveMagicName(strName)
	,m_eMoveActionType(eMoveActionType)
{
	m_pTarget = new CGenericTarget(*pTarget);
	CFighterDictator* m_pFighter=GetEntity()->GetFighter();
	m_pInst->AddRef();
	GetEntity()->EnableFollowerDelay(false);
	m_pFighter->CppSetCtrlState(eFCS_ForbitMove,true);
	m_pFighter->CppSetCtrlState(eFCS_ForbitTurnAround,true);
	m_pFighter->Attach(this, eCE_Offline);
	m_pFighter->Attach(this, eCE_MoveSuccess);
	m_pFighter->Attach(this, eCE_MoveEnd);
	m_pFighter->Attach(this, eCE_ChangeMapEnd);
	m_pFighter->Attach(this, eCE_FinishDuel);
	CDir Dir;
	Dir.uDir = moveInfo.uDir;
	GetEntity()->DisableDirectorMoving(true);
	EMoveToResult eResult(eMTR_Success);
	switch (moveInfo.eMoveType)
	{
	case eMT_Move:
		eResult = MoveTo(moveInfo.DesPixelPos, moveInfo.fSpeed, moveInfo.eBarrierType);
		break;
	case eMT_Direction:
		eResult = MoveTo(Dir, moveInfo.fDistance, moveInfo.fSpeed, moveInfo.eBarrierType);
		break;
	case eMT_Random:
		eResult = RandomMoveTo(moveInfo.fDistance, moveInfo.fSpeed, moveInfo.eBarrierType);
		break;
	default :
		break;
	}
	if (eResult != eMTR_Success)
	{
		if (eResult == eMTR_SuperPosition)
		{
			pInst->AddRef();
			if (pMagicEff)
			{	
				CCharacterDictator* pCharacter =  CCharacterDictator::GetCharacterByID(pInst->GetCreatorID());
				CFighterDictator* pFighter =pCharacter?pCharacter->GetFighter():NULL;
				if(pFighter)
					pMagicEff->Do(pInst,pFighter,pTarget);
			}
			pInst->DelRef();
		}
		return;
	}
	uMoveMagicMoveID = m_uMoveID;
	CCharacterDictator* pCharacter = pFighter->GetCharacter();
	if (pCharacter && moveInfo.eAction != eMAT_None)
	{
		pCharacter->SetAndSyncMoveState(eMS_LockMove);
		//临时
		if (moveInfo.eAction == eMAT_HitAway || moveInfo.eAction == eMAT_HitFly)
			pCharacter->SetAndSyncActionState(eAS_Suffer_Back);
		else if (moveInfo.eAction == eMAT_Eddy)
			pCharacter->SetAndSyncActionState(eAS_Sleep);
	}
}

CBaseMoveMagicServer::~CBaseMoveMagicServer()
{
	DetachAll();
	if (GetEntity())
	{
		GetEntity()->EnableFollowerDelay(true);
		CFighterDictator* m_pFighter=GetEntity()->GetFighter();
		m_pInst->DelRef();
		//GetEntity()->StopMoving();
		GetEntity()->DisableDirectorMoving(false);
		m_pFighter->CppSetCtrlState(eFCS_ForbitTurnAround,false);
		m_pFighter->CppSetCtrlState(eFCS_ForbitMove,false);
		CCharacterDictator* pCharacter = m_pFighter->GetCharacter();

		if (pCharacter->GetMoveState() == eMS_LockMove)
		{
			if (pCharacter->IsMoving())
			{
				pCharacter->SetAndSyncMoveState(eMS_Move, true);
			}
			else
			{
				if (pCharacter->CppIsLive())
					pCharacter->SetAndSyncMoveState(eMS_Stop, true);
				else
					pCharacter->SetAndSyncMoveState(eMS_LockStop, true);
			}
		}
	}

	delete m_pTarget;
	SafeDelete(m_pMagicEff);
}

void CBaseMoveMagicServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	CEntityServer* pEntity = GetEntity();
	if (pEntity==NULL)
	{
		return;
	}

	CFighterDictator* pFighter=pEntity->GetFighter();
	if (pSubject == pFighter)
	{
		switch (uEvent)
		{
		case eCE_Offline:
		case eCE_ChangeMapEnd:
		case eCE_FinishDuel:
			delete this;
			break;
		case eCE_MoveEnd:
			if (m_uMoveID == uint32(*reinterpret_cast<uint32*>(pArg)))delete this;
			break;
		case eCE_MoveSuccess:
			{
				if (m_uMoveID == uint32(*reinterpret_cast<uint32*>(pArg)))
				{
					CMagicEffServer* pMagicEff = m_pMagicEff->get();
					CSkillInstServer* pInst = m_pInst;
					pInst->AddRef();
					CGenericTarget pTarget(*m_pTarget);
					EMoveActionType eMoveActionType = m_eMoveActionType;
					delete this;
					if (pMagicEff)
					{	
						CCharacterDictator* pCharacter =  CCharacterDictator::GetCharacterByID(pInst->GetCreatorID());
						CFighterDictator* pFighter =pCharacter?pCharacter->GetFighter():NULL;
						if(pFighter)
						{
							// 跳跃时，eFPT_HypoLine 寻路，如果遇到障碍，直接返回障碍前的目标
							if (eMoveActionType == eMAT_Jump)
							{
								pTarget.SetPos(pCharacter->GetPixelPos());
							}
							pMagicEff->Do(pInst,pFighter,&pTarget);
						}
					}
					pInst->DelRef();
				}
			}
			break;
		}
	}
}

CMoveMagicServer::CMoveMagicServer(CSkillInstServer* pInst, const CMoveMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, CFighterDictator* pTo)
:CMagicServer(pInst,pFrom,pTo->GetEntityID())
,m_pCfg(new CMoveMagicCfgServerSharedPtr(pCfg))
,m_pObjOwner(NULL)
,m_uMoveID(0)
,m_pMagicOwner(NULL)
{
	m_pTo=new CGenericTarget(pTo);
	Do();
}

CMoveMagicServer::CMoveMagicServer(CSkillInstServer* pInst, const CMoveMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom,  const CFPos& pos)
:CMagicServer(pInst,pFrom,0)
,m_pCfg(new CMoveMagicCfgServerSharedPtr(pCfg))
,m_pObjOwner(NULL)
,m_uMoveID(0)
,m_pMagicOwner(NULL)
{
	m_pTo=new CGenericTarget(pos);

	Do();
}

CMoveMagicServer::~CMoveMagicServer()
{
	if(GetCfgSharedPtr()->GetCancelableMagicEff())
	{
		GetCfgSharedPtr()->GetCancelableMagicEff()->Cancel(m_pSkillInst, GetFighter(), m_pCancelEffData);
		delete m_pCancelEffData;
	}
	GetFighter()->UnRegistDistortedTick(this);
	DeleteObj(GetFighter()->GetScene());
	GetFighter()->GetMagicMgr()->SetMoveMagic(NULL);
	if(m_pMagicOwner)
		m_pMagicOwner->GetMagicMgr()->OnCancelMagicSkill();
	delete m_pTo;
	SafeDelete(m_pCfg);
}

void CMoveMagicServer::Do()
{
	m_BeginPos = GetFighter()->GetPixelPos();
	GetFighter()->GetMagicMgr()->SetMoveMagic(this);
	if(GetCfgSharedPtr()->GetCancelableMagicEff())
	{
		CFighterDictator* pFighter = GetFighter();
		uint32 uCheckID = GetCheckID();
		m_pCancelEffData = new CMagicEffDataMgrServer(pFighter);
		GetCfgSharedPtr()->GetCancelableMagicEff()->Do(m_pSkillInst, GetFighter(), GetFighter(), m_pCancelEffData);
		if (!pFighter->GetMagicMgr()->IsValid(uCheckID))
		{
			return;
		}
	}

	GetFighter()->Attach(this, eCE_Offline);
	GetFighter()->Attach(this, eCE_Die);
	GetFighter()->Attach(this, eCE_ChangeMapEnd);
	GetFighter()->Attach(this, eCE_FinishDuel);

	switch (GetCfgSharedPtr()->GetMoveType())
	{
	case eMMT_Move:
	case eMMT_Random:
	case eMMT_Direction:
		{
			GetFighter()->Attach(this, eCE_MoveEnd);
			GetFighter()->Attach(this, eCE_CoreEvent);
			CMagicOpTreeServer::VecEntityID vecEntityID;
			FilterLimit filterLimit;
			MakeFilterLimit<CMoveMagicServer>(filterLimit);
			bool bIsNeedDelete = true;
			CFilterOpServer::FilterOperateObject(m_pSkillInst, vecEntityID, GetCfgSharedPtr()->GetOperateObject(), filterLimit, GetFighter(), m_pTo);
			CMagicOpTreeServer::VecEntityID::iterator it_obj = vecEntityID.begin();
			for (; it_obj != vecEntityID.end(); ++it_obj)
			{
				CCharacterDictator* pCharacter = CEntityServerManager::GetInst()->GetCharacter(*it_obj);
				if (NULL==pCharacter)
				{
					continue;
				}
				CFighterDictator* pTo=pCharacter->GetFighter();
				MoveInfo moveInfo;
				MakeMoveInfo(moveInfo,EMoveType(GetCfgSharedPtr()->GetMoveType()));
				CFighterDictator* pFrom = GetFighter();
				if (pTo==pFrom)
				{
					bIsNeedDelete=false;
				}
				if (!pTo->IsExistMoveMgr()&&(pTo==pFrom&&!pTo->GetCtrlState(eFCS_ForbitMove)||pTo!=pFrom))
				{
					if(!Immune(pFrom, pTo, m_pSkillInst))
					{
						new CBaseMoveMagicServer(m_pSkillInst,pTo,m_pTo,moveInfo,GetCfgSharedPtr()->GetName(),GetCfgSharedPtr()->GetMagicEff(),GetCfgSharedPtr()->GetActionType(), m_uMoveID);
						pTo->GetMagicMgr()->OnCastMagicSkill(GetCfgSharedPtr()->GetId(), eMGT_Move, false);
					}
				}
			}
			if (bIsNeedDelete)
				delete this;
			break;
		}
	case eMMT_AreaMove:
	case eMMT_AreaDirection:
	case eMMT_AreaRandom:
		{
			GetFighter()->Attach(this, eCE_Offline);
			GetFighter()->Attach(this, eCE_Die);
			GetFighter()->Attach(this, eCE_ChangeMapEnd);
			GetFighter()->Attach(this, eCE_FinishDuel);
			GetFighter()->Attach(this, eCE_CoreEvent);
			if (GetCfgSharedPtr()->GetOperateObject()==eOOT_Target)
			{
				Ast(m_pTo->GetFighter());
				m_pObjOwner=m_pTo->GetFighter();
			}
			else
			{
				m_pObjOwner=GetFighter();
			}
			m_pObjOwner->Attach(this, eCE_Offline);
			m_pObjOwner->Attach(this, eCE_Die);
			m_pObjOwner->Attach(this, eCE_MoveEnd);
			m_pObjOwner->Attach(this, eCE_ChangeMapEnd);
			CFPos pixelPos;
			m_pObjOwner->GetPixelPos(pixelPos);
			CCoreObjectDictator* pCoreObj = CreateCoreObj<CMoveMagicServer>(pixelPos);
			if(pCoreObj)
			{
				CCharacterDictator* pChar = m_pObjOwner->GetCharacter();
				if(pChar)
				{
					pCoreObj->BindTo(pChar->GetCoreObjID());
				}	
			}
			float AreaRadius =float(GetCfgSharedPtr()->GetRadius()->GetDblValue(GetFighter(),m_pSkillInst->GetLevelIndex()));
			SetEyeSight(AreaRadius);
			//uint32 uBaseCycTime = CAppConfigServer::Inst()->GetBaseTickCyc();
			//CAppServer::Inst()->RegisterTick(this, uBaseCycTime);
			CMagicOpTreeServer::VecEntityID vecEntityID;
			FilterLimit filterLimit;
			MakeFilterLimit<CMoveMagicServer>(filterLimit);
			CGenericTarget Target(m_pObjOwner);
			CFilterOpServer::FilterOperateObject(m_pSkillInst, vecEntityID, GetCfgSharedPtr()->GetAreaOperateObject(), filterLimit, GetFighter(), &Target);
			MoveInfo moveInfo;
			MakeMoveInfo(moveInfo,EMoveType(GetCfgSharedPtr()->GetMoveType()-3));
			if (!m_pObjOwner->IsExistMoveMgr()&&(m_pObjOwner==GetFighter()&&!m_pObjOwner->GetCtrlState(eFCS_ForbitMove)||m_pObjOwner!=GetFighter()))
			{
				new CBaseMoveMagicServer(m_pSkillInst,m_pObjOwner,&Target,moveInfo,GetCfgSharedPtr()->GetName(),vecEntityID
.empty()?CMagicEffServerSharedPtr():GetCfgSharedPtr()->GetMagicEff(),GetCfgSharedPtr()->GetActionType(), m_uMoveID);
				m_pObjOwner->GetMagicMgr()->OnCastMagicSkill(GetCfgSharedPtr()->GetId(), eMGT_Move, true);
				m_pMagicOwner=m_pObjOwner;
			}
			m_setTarget.insert(m_pObjOwner->GetEntityID());
			break;
		}
	}
}

//void CMoveMagicServer::OnTick()
//{
//	CCoreObjectDictator* pObj = GetCoreObj();
//	if(pObj)
//	{
//		pObj->MoveTo(m_pObjOwner->GetPixelPos(), m_pCfg->GetSpeed(),eFPT_HypoLine,eBT_MidBarrier,0);
//	}
//}


void CMoveMagicServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	if (pSubject == GetFighter())
	{
		switch (uEvent)
		{
		case eCE_Die:
		case eCE_Offline:
		case eCE_FinishDuel:
			DetachAll();
			delete this;
			return;
		case eCE_ChangeMapEnd:
			{
				CCoreSceneServer* pScene = reinterpret_cast<CCoreSceneServer*>(pArg);
				DeleteObj(pScene);
				DetachAll();
				delete this;
				return;
			}
		case eCE_MoveEnd:
			if(m_uMoveID == uint32(*reinterpret_cast<uint32*>(pArg)))
			{
				void* pContext = reinterpret_cast<void*>(this);
				GetFighter()->GetCharacter()->PostCoreEvent(pContext);
			}
			return;
		case eCE_CoreEvent:
			{
				CMoveMagicServer* pMagic = reinterpret_cast<CMoveMagicServer*>(pArg);
				if (pMagic==this)
				{
					delete this;
				}
			}
			return;
		}
	}
	if (pSubject == m_pObjOwner)
	{
		CCoreObjectDictator* pObj = GetCoreObj();
		//uint32 uMoveID = pObj->GetLastMoveID();
		switch (uEvent)
		{
		case eCE_Die:
		case eCE_Offline:
			DetachAll();
			delete this;
			break;
		case eCE_MoveEnd:
			if(m_uMoveID == uint32(*reinterpret_cast<uint32*>(pArg)))
			{
				DetachAll();
				void* pContext = NULL;
				pObj->PostAoiMsgToSelf(pContext);
			}
			break;
		case eCE_ChangeMapEnd:
			{
				CCoreSceneServer* pScene = reinterpret_cast<CCoreSceneServer*>(pArg);
				DeleteObj(pScene);
				DetachAll();
				delete this;
				break;
			}
		case eCE_MoveBegan:
			{
				CFPos Pos = GetFighter()->GetPixelPos();
				pObj->SetPixelPos(Pos);
				break;
			}
		}
	}
}

void CMoveMagicServer::OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{

	CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	if (!pCoreObj || !pCoreObj->GetHandler())
		return;

	if ((size_t)pCoreObj->GetHandler()->GetTag() == (uint32)eCOHT_Character)
	{
		if (m_setTarget.count(uObjGlobalId) != 0)
			return;

		CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
		if (NULL==pCharacter)
		{
			return;
		}
		CFighterDictator* pTo=pCharacter->GetFighter();
		CGenericTarget Target(pTo);
		CMagicOpTreeServer::VecEntityID vecEntityID;
		FilterLimit filterLimit;
		MakeFilterLimit<CMoveMagicServer>(filterLimit);
		CFilterOpServer::FilterOperateObject(m_pSkillInst, vecEntityID, GetCfgSharedPtr()->GetAreaOperateObject(), filterLimit, GetFighter(), &Target);
		CMagicOpTreeServer::VecEntityID::iterator it_obj = vecEntityID.begin();
		for (; it_obj != vecEntityID.end(); ++it_obj)
		{
			CCharacterDictator* pCharacter = CEntityServerManager::GetInst()->GetCharacter(*it_obj);
			if (NULL==pCharacter)
			{
				continue;
			}
			CFighterDictator* pTo=pCharacter->GetFighter();
			MoveInfo moveInfo;
			MakeMoveInfo(moveInfo,EMoveType(GetCfgSharedPtr()->GetMoveType()-3));
			CFighterDictator* pFrom = GetFighter();
			if (!pTo->IsExistMoveMgr()&&(pTo==pFrom&&!pTo->GetCtrlState(eFCS_ForbitMove)||pTo!=pFrom))
			{
				if(!Immune(pFrom, pTo, m_pSkillInst))
				{
					uint32 uMoveID;
					new CBaseMoveMagicServer(m_pSkillInst,pTo,&Target,moveInfo,GetCfgSharedPtr()->GetName(),GetCfgSharedPtr()->GetMagicEff(),GetCfgSharedPtr()->GetActionType(), uMoveID);
				}
			}
			m_setTarget.insert(uObjGlobalId);
		}
	}
}

void CMoveMagicServer::OnAoiMsgFromSelf(const void* pContext)
{
	delete this;
}

void CMoveMagicServer::MakeMoveInfo(MoveInfo& moveInfo, EMoveType MoveType)
{
	moveInfo.fSpeed =GetCfgSharedPtr()->GetSpeed();
	moveInfo.eMoveType=MoveType;
	moveInfo.eBarrierType=GetCfgSharedPtr()->GetBarrierType();
	moveInfo.eAction=GetCfgSharedPtr()->GetActionType();
	if (eMMAL_Offset == GetCfgSharedPtr()->GetMoveArgTypeLimit())
	{
		(float)(GetCfgSharedPtr()->GetMoveArgValue()->GetDblValue(GetFighter()));
	}
	
	switch (MoveType)
	{
	case eMT_Teleport:
	case eMT_Move:
		{
			CFPos Pos(0, 0);
			CDir dir;
			switch(GetCfgSharedPtr()->GetMoveArgType())
			{
			case eMMA_SelfPosition:
				{
					Pos = GetFighter()->GetPixelPos();
				}
				break;
			case eMMA_TargetPosition:
				{
					if (m_pTo && m_pTo->GetFighter())
						Pos = m_pTo->GetFighter()->GetPixelPos();
				}
				break;
			case eMMA_Position:
				{
					if (m_pTo && m_pTo->GetType() == eTT_Position)
						Pos = m_pTo->GetPos();
				}
			    break;
			default:
			    break;
			}

			CVector2f vecDir;
			if (GetCfgSharedPtr()->GetMoveArgType()==eMMA_SelfPosition)
			{

				CFPos TargetDirPos(0, 0);
				if (m_pTo)
				{
					if (m_pTo->GetFighter())
						TargetDirPos = m_pTo->GetFighter()->GetPixelPos();
					else
						TargetDirPos = m_pTo->GetPos();

					vecDir.x = m_BeginPos.x - TargetDirPos.x;
					vecDir.y = m_BeginPos.y - TargetDirPos.y;
					dir.Set(vecDir);
				}
			}
			else
			{
				vecDir.x = Pos.x - m_BeginPos.x;
				vecDir.y = Pos.y - m_BeginPos.y;
				dir.Set(vecDir);
			}
			moveInfo.uDir = dir.uDir;
			if (eMMAL_DistanceLimit==GetCfgSharedPtr()->GetMoveArgTypeLimit())
			{
				CVector2f fDir;
				dir.Get(fDir);
				float MaxDis=float(GetCfgSharedPtr()->GetMoveArgValue()->GetDblValue(GetFighter(),1)*eGridSpanForObj);
				float MinDis=float(GetCfgSharedPtr()->GetMoveArgValue()->GetDblValue(GetFighter(),0)*eGridSpanForObj);
				float distance = vecDir.Mag();
				if (distance > MaxDis)
				{
					moveInfo.DesPixelPos .x = fDir.x * MaxDis+ m_BeginPos.x;
					moveInfo.DesPixelPos .y = fDir.y * MaxDis+ m_BeginPos.y;
				}
				else if(distance < MinDis)
				{
					moveInfo.DesPixelPos .x = fDir.x * MinDis+ m_BeginPos.x;
					moveInfo.DesPixelPos .y = fDir.y * MinDis+ m_BeginPos.y;
				}
				else 
					moveInfo.DesPixelPos = Pos;
			}
			else if (eMMAL_Offset == GetCfgSharedPtr()->GetMoveArgTypeLimit())
			{
				CVector2f fDir;
				dir.Get(fDir);
				float fOffset = (float)(GetCfgSharedPtr()->GetMoveArgValue()->GetDblValue(GetFighter())*eGridSpanForObj);
				moveInfo.DesPixelPos.x = Pos.x - fDir.x * fOffset;
				moveInfo.DesPixelPos.y = Pos.y - fDir.y * fOffset;
			}
			else
			{
				moveInfo.DesPixelPos = Pos;
			}
			if (GetCfgSharedPtr()->GetMoveArgType() == eMMA_SelfPosition)
			{
				CFighterDictator* pFighter =m_pTo->GetFighter();
				if (pFighter->m_ImmuneRate.Get() < 1.0f && pFighter->m_MoveMagicImmuneRate.Get() < 1.0f)
					m_pTo->GetFighter()->GetCharacter()->SetAndSyncActionDir(dir.uDir, true);
			}
			else
			{
				if (GetFighter()->m_ImmuneRate.Get() < 1.0f && GetFighter()->m_MoveMagicImmuneRate.Get() < 1.0f)
					GetFighter()->GetCharacter()->SetAndSyncActionDir(dir.uDir, true);
			}
		}
		break;
	case eMT_Direction:
		switch (GetCfgSharedPtr()->GetMoveArgType())
		{
		case eMMA_Direction:
			{
				CDir dir;
				CVector2f vecDir;
				CFPos posTarget;
				CFPos posSelf=GetFighter()->GetPixelPos();
				if (m_pTo->GetType()==eTT_Fighter)
				{
					posTarget=m_pTo->GetFighter()->GetPixelPos();
				}
				else
				{
					posTarget=m_pTo->GetPos();
				}
				vecDir.x = posTarget.x - posSelf.x;
				vecDir.y = posTarget.y - posSelf.y;
				dir.Set(vecDir);
				moveInfo.uDir=dir.uDir;
				if (GetCfgSharedPtr()->GetMoveArgTypeLimit()==eMMAL_DistanceLimit)
				{
					float MaxDis=float(GetCfgSharedPtr()->GetMoveArgValue()->GetDblValue(GetFighter(),1)*eGridSpanForObj);
					float MinDis=float(GetCfgSharedPtr()->GetMoveArgValue()->GetDblValue(GetFighter(),0)*eGridSpanForObj);
					float distance = vecDir.Mag();
					if (distance > MaxDis)
						moveInfo.fDistance=MaxDis;
					else if(distance < MinDis)
						moveInfo.fDistance=MinDis;
					else 
						moveInfo.fDistance=distance;
				}
				else
				{
					moveInfo.fDistance=float(GetCfgSharedPtr()->GetMoveArgValue()->GetDblValue(GetFighter())*eGridSpanForObj);
				}
				break;
			}
		case eMMA_OppositeDirection:
			{
				CDir dir;
				CVector2f vecDir;
				CFPos posTarget;
				CFPos posSelf=GetFighter()->GetPixelPos();
				if (m_pTo->GetType()==eTT_Fighter)
				{
					posTarget=m_pTo->GetFighter()->GetPixelPos();
				}
				else
				{
					posTarget=m_pTo->GetPos();
				}
				vecDir.x = posSelf.x - posTarget.x;
				vecDir.y = posSelf.y - posTarget.y;
				dir.Set(vecDir);
				moveInfo.uDir=dir.uDir;
				if (GetCfgSharedPtr()->GetMoveArgTypeLimit()==eMMAL_DistanceLimit)
				{
					float MaxDis=float(GetCfgSharedPtr()->GetMoveArgValue()->GetDblValue(GetFighter(),1)*eGridSpanForObj);
					float MinDis=float(GetCfgSharedPtr()->GetMoveArgValue()->GetDblValue(GetFighter(),0)*eGridSpanForObj);
					float distance = vecDir.Mag();
					if (distance > MaxDis)
						moveInfo.fDistance=MaxDis;
					else if(distance < MinDis)
						moveInfo.fDistance=MinDis;
					else 
						moveInfo.fDistance=distance;
				}
				else
				{
					moveInfo.fDistance=float(GetCfgSharedPtr()->GetMoveArgValue()->GetDblValue(GetFighter(),m_pSkillInst->GetLevelIndex())*eGridSpanForObj);
				}
				break;
			}
		default :
			break;
		}
		break;
	case eMT_Random:
		moveInfo.fDistance=float(GetCfgSharedPtr()->GetMoveArgValue()->GetDblValue(GetFighter())*eGridSpanForObj);
		break;
	default:
		break;
	}
}

bool CMoveMagicServer::Immune(CFighterDictator* pFrom, CFighterDictator* pTo, CSkillInstServer* pSkillInst)
{
	if(pTo == pFrom)
	{
		return false;
	}

	if(pTo->m_ImmuneRate.Get() >= 1.0f || pTo->m_MoveMagicImmuneRate.Get() >= 1.0)
	{
		CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
		CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;
		pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,0,eAT_HP,eHR_Immune);
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eHR_Immune, true);
		return true;
	}
	return false;
}

CMoveMagicCfgServerSharedPtr& CMoveMagicServer::GetCfgSharedPtr()const
{
	return *m_pCfg;
}
