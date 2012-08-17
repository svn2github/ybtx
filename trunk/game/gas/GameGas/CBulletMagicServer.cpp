#include "stdafx.h"
#include "CBulletMagicServer.h"
#include "CBulletMagicCfgServer.h"
#include "CMagicServer.inl"
#include "CFighterMediator.h"
#include "CAllStateMgr.h"
#include "CMagicEffServer.h"
#include "CoreCommon.h"
#include "CGenericTarget.h"
#include "CSpecialStateMgr.h"
#include "CAoiDimMgr.h"
#include "CCoreObjectDictator.h"
#include "CSkillInstServer.h"
#include "CEntityServer.h"
#include "BaseHelper.h"
#include "CCoreSceneServer.h"
#include "StringHelper.h"
#include "CCharacterDictator.h"
#include "CMath.h"
#include "CVector2.h"
#include "CDir.h"
#include "CCoreObjectDictator.h"

//初始化子弹魔法
uint32 CBulletMagicServer::GetCheckTimes()
{
	return 1;
}

CBulletMagicServer::CBulletMagicServer(CSkillInstServer* pInst, const CBulletMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, CFighterDictator* pTo)
:CMagicServer(pInst,pFrom,pTo->GetEntityID())
,m_pCfg(new CBulletMagicCfgServerSharedPtr(pCfg))
,m_bIsDumbBullet(false)
,m_bIsPos(false)
{
	m_pSkillInst->SetIsArea(false);
	m_pTo=new CGenericTarget(pTo);
	pTo->Attach(this, eCE_Offline);
	pTo->Attach(this, eCE_Die);
	pTo->Attach(this, eCE_ChangeMapEnd);
	pTo->Attach(this, eCE_Transport);
	//pTo->Attach(this, eCE_MoveBegan);
	CFPos posFrom;
	CFighterMediator* pMedFighter = GetFighter()->CastToFighterMediator();
	if(pMedFighter)
	{
		posFrom = pMedFighter->GetFighterPos();
	}
	else
	{
		posFrom = GetFighter()->GetPixelPos();
	}
	CreatBullet(posFrom);
}

CBulletMagicServer::CBulletMagicServer(CSkillInstServer* pInst, const CBulletMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, const CFPos& pos)
:CMagicServer(pInst,pFrom,0)
,m_pCfg(new CBulletMagicCfgServerSharedPtr(pCfg))
,m_pScene(pFrom->GetScene())
,m_bIsDumbBullet(false)
,m_bIsPos(true)
{
	m_pSkillInst->SetIsArea(false);
	m_pTo=new CGenericTarget(pos);
	CFPos posFrom;
	CFighterMediator* pMedFighter = GetFighter()->CastToFighterMediator();
	if(pMedFighter)
	{
		posFrom = pMedFighter->GetFighterPos();
	}
	else
	{
		posFrom = GetFighter()->GetPixelPos();
	}
	CreatBullet(posFrom);
}
CBulletMagicServer::~CBulletMagicServer()
{	
	CCoreObjectDictator* pObj = GetCoreObj();
	if (pObj)
	{
		pObj->SetHandler(NULL);
		m_pScene->DestroyObject(pObj);
	}
	delete m_pTo;
	SafeDelete(m_pCfg);
}

void CBulletMagicServer::CreatBullet(const CFPos& posFrom)
{
	m_BeginPos = posFrom;
	GetFighter()->Attach(this, eCE_Offline);
	GetFighter()->Attach(this, eCE_ChangeMapEnd);
	GetFighter()->Attach(this, eCE_FinishDuel);
	CFPos PixelPos, Pos;
	if (m_pTo->GetType()==eTT_Fighter)
	{
		Pos=m_pTo->GetFighter()->GetPixelPos();
	}
	else
	{
		Pos=m_pTo->GetPos();
	}
	CPos DesPos,CurPos;
	PixelPos2GridPos(DesPos,Pos);
	GetFighter()->GetPixelPos(PixelPos);
	uint32 uDir = GetFighter()->GetCharacter()->GetActionDir();
	PixelPos.SetX(PixelPos.x + 32*sin(float(uDir*PI*2/256)));
	PixelPos.SetY(PixelPos.y + 32*cos(float(uDir*PI*2/256)));
	GetFighter()->GetGridPos(CurPos);

	if (CurPos == DesPos)
	{
		OnTraceEnded(eTET_Traced);
		return;
	}
	m_eEyeSightType=eAIET_BlindObj;
	CCoreObjectDictator* pObj = CreateCoreObj<CBulletMagicServer>(PixelPos);
	m_pSkillInst->SetMagicBeginPos(PixelPos);

	CSyncVariantServerHolder* pViewRootVariant = GetCoreObj()->GetViewVariantHolder(eCTI_Static);

	CVector2f vec2f(PixelPos.x-Pos.x,PixelPos.y-Pos.y);
	CDir Dir;
	Dir.Set(vec2f);
	pViewRootVariant->SetNumber<uint8>("Attribute/Direction", Dir.uDir);

	EBulletTrackType eBulletTrackType = GetCfgSharedPtr()->GetBulletTrackType();
	EMoveToResult eMoveResult = eMTR_Success;

	switch (eBulletTrackType)
	{
	case eBTT_Line:
	case eBTT_Parabola:
		{
			if(m_pTo->GetType()==eTT_Fighter)
			{
				CEntityServer* pTargetEntity = m_pTo->GetFighter()->GetEntity();
				CCoreObjectServer* pTargetCoreObj = pTargetEntity->GetCoreObj();

				pObj->Trace(pTargetCoreObj, GetCfgSharedPtr()->GetSpeed(),eFPT_HypoLine,eBT_MidBarrier,0);
			}
			else
			{
				eMoveResult = pObj->MoveTo(Pos, GetCfgSharedPtr()->GetSpeed(),eFPT_HypoLine,eBT_MidBarrier,0);
			}
		}
		break;
	default:
		break;
	}
	
	if (eMTR_SuperPosition == eMoveResult)
	{
		OnTraceEnded(eTET_Failed);
		return;
	}
}

void CBulletMagicServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	if(m_pTo && pSubject == m_pTo->GetFighter())
	{
		switch(uEvent)
		{
		case eCE_Die:
		case eCE_Offline:
		case eCE_ChangeMapEnd:
		case eCE_Transport:
			BecomeDumbBullet();
			break;
		default:
			break;
		}
	}
	else if(pSubject == GetFighter())
	{
		switch(uEvent)
		{
		case eCE_Offline:
		case eCE_ChangeMapEnd:
		case eCE_FinishDuel:
			//BecomeDumbBullet();
			DestoryBullet();
			break;
		default:
			break;
		}
	}
}

void CBulletMagicServer::DoEffect()
{
	//PtCOR可能有bug,暂时只能在这加判断进行拦截pFrom为NULL的情况
	if(GetFighter() == NULL)
	{
		delete this;
		return;
	}

	CPos bulletPos;
	CCoreObjectDictator* pObj = GetCoreObj();
	if (pObj)
	{
		pObj->GetGridPos(bulletPos);
	}
	else
	{
		GetFighter()->GetGridPos(bulletPos);
	}

	//产生魔法效果
	if (m_bIsPos)
	{
		GetCfgSharedPtr()->GetMagicEff()->Do(m_pSkillInst, GetFighter(), m_pTo);
	}
	else
	{
		if (m_pTo==NULL||m_pTo->GetFighter()==NULL)
		{
			delete this;
			return;
		}
		CFighterDictator* pTo = m_pTo->GetFighter();
		CAllStateMgrServer* pStateMgr=pTo->GetAllStateMgr();
		Ast(pStateMgr);
		CSpecialStateMgrServer* pSpecialStateaMgr = pStateMgr->GetSpecialStateMgrServer();
		if (pSpecialStateaMgr && pSpecialStateaMgr->ExistStateByType(eSST_Reflect))
		{
			pSpecialStateaMgr->ClearStateByType(eSST_Reflect);
			CFPos pos= pTo->GetPixelPos();
			CFighterMediator* pMedFighter = pTo->CastToFighterMediator();
			if(pMedFighter)
				pMedFighter->SetFighterPos(pos);
			new CBulletMagicServer(m_pSkillInst, GetCfgSharedPtr(), pTo, GetFighter());
		} 
		else
		{
			DetachAll();
			GetCfgSharedPtr()->GetMagicEff()->Do(m_pSkillInst, GetFighter(), pTo);
		}
	}
	delete this;
}

void CBulletMagicServer::OnMovePathChanged() 
{
	if (m_bIsDumbBullet)
		return;
	CFPos PixelPos, Pos;
	if (m_pTo->GetType()==eTT_Fighter)
	{
		Pos=m_pTo->GetFighter()->GetPixelPos();
	}
	else
	{
		Pos=m_pTo->GetPos();
	}
	CPos DesPos,CurPos;
	PixelPos2GridPos(DesPos,Pos);
	GetCoreObj()->GetPixelPos(PixelPos);
	GetCoreObj()->GetGridPos(CurPos);
	if (DesPos!=CurPos)
	{
		CSyncVariantServerHolder* pViewRootVariant = GetCoreObj()->GetViewVariantHolder(eCTI_Static);
		CVector2f vec2f(PixelPos.x-Pos.x,PixelPos.y-Pos.y);
		CDir Dir;
		Dir.Set(vec2f);
		pViewRootVariant->SetNumber<uint8>("Attribute/Direction", Dir.uDir);
	}
}

void CBulletMagicServer::OnTraceEnded(ETraceEndedType eTEType)
{
	if(m_bIsDumbBullet||eTEType!=eTET_Traced)
	{
		delete this;
		return;
	}

	DoEffect();
}

void CBulletMagicServer::OnMoveEnded(EMoveEndedType eMoveEndedType, uint32 uMoveID)
{
	if(m_bIsDumbBullet)
	{
		delete this;
		return;
	}

	if(m_pTo->GetType()==eTT_Position)
	{
		if(eMoveEndedType!=eMET_Reached)
		{
			delete this;
			return;
		}

		DoEffect();
	}
}

void CBulletMagicServer::BecomeDumbBullet()
{
	if(m_bIsDumbBullet)
	{
		return;
	}
	m_bIsDumbBullet = true;
	SafeDelete(m_pTo);
	//SetFighter(NULL);
	this->DetachAll();
}

void CBulletMagicServer::DestoryBullet()
{
	BecomeDumbBullet();
	delete this;
	return;
}

void CBulletMagicServer::OnDestroy()
{
	DestoryBullet();
}

const CFPos& CBulletMagicServer::GetBeginPos()const
{
	return m_BeginPos;
}

CBulletMagicCfgServerSharedPtr& CBulletMagicServer::GetCfgSharedPtr()const
{
	return *m_pCfg;
}
