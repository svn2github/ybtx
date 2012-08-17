#include "stdafx.h"
#include "CMoveServerManager.h"
#include "CCoreObjectMediator.h"
#include "CFighterDictator.h"
#include "CEntityServer.h"
#include "CQuickRand.h"
#include "CoreCommon.h"
#include "CCoreSceneServer.h"
#include "CMetaSceneServer.h"
#include "CPixelPath.h"
#include "CCharacterDictator.h"

IMoveManager::IMoveManager(CEntityServer* pEntity)
:m_uMoveID(0)
{
	m_Entity.SetHolder(this);
	pEntity ? m_Entity.Attach(pEntity->m_RefsByMoveMgr) : m_Entity.Detach();
}


EMoveToResult IMoveManager::MoveTo(const CFPos& PixelPosDes,float fPixelSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType)
{
	Ast(GetEntity());
	Ast(GetEntity()->GetCoreObj());
	CPos CurPos, DesPos;
	GetEntity()->GetGridPos(CurPos);
	PixelPos2GridPos(DesPos,PixelPosDes);
	CFPos BeginPos = GetEntity()->GetPixelPos();
	if (CurPos == DesPos)
	{
		delete this;
		return eMTR_SuperPosition;
	}
	CCoreObjectDictator* pObj = GetEntity()->GetCoreObj();
	EMoveToResult ret = pObj->MoveTo(PixelPosDes,fPixelSpeed,eFindPathType,eBarrierType,0.0f);
	if(ret != eMTR_Success)
	{
		delete this;
		return ret;
	}
	m_uMoveID = GetEntity()->GetLastMoveID();	

	return eMTR_Success;
}

EMoveToResult IMoveManager::MoveTo(const CFPos& PixelPosDes,float fPixelSpeed,EBarrierType eBarrierType)
{
	return IMoveManager::MoveTo(PixelPosDes,fPixelSpeed,eFPT_HypoLine,eBarrierType);
}

EMoveToResult IMoveManager::MoveTo(const CDir& Dir,float fPixelDistance,float fPixelSpeed,EBarrierType eBarrierType)
{
	Ast(GetEntity());
	Ast(GetEntity()->GetCoreObj());
	CFPos PixelPosDes;
	CFPos posSelf=GetEntity()->GetCoreObj()->GetPixelPos();
	CVector2f fDir;
	Dir.Get(fDir);
	PixelPosDes.x = fDir.x * fPixelDistance+ posSelf.x;
	PixelPosDes.y = fDir.y * fPixelDistance+ posSelf.y;

	return MoveTo(PixelPosDes,fPixelSpeed,eBarrierType);
}

EMoveToResult IMoveManager::RandomMoveTo(float fDistance, float fPixelSpeed, EBarrierType eBarrierType)
{
	Ast(GetEntity());
	Ast(GetEntity()->GetCoreObj());
	CFPos PixelPosDes;
	CFPos PixelPosSelf = GetEntity()->GetCoreObj()->GetPixelPos();
	PixelPosDes.x = PixelPosSelf.x + CQuickRand::Rand1(-fDistance , fDistance );
	PixelPosDes.y = PixelPosSelf.y + CQuickRand::Rand1(-fDistance , fDistance );
	CMetaSceneServer* pMeta = GetEntity()->GetCoreObj()->GetScene()->GetMetaScene();
	float fPixelWidth = float(pMeta->GetWidthInPixel());
	float fPixelHeight = float(pMeta->GetHeightInPixel());
	PixelPosDes.x = max(1.0f, PixelPosDes.x);
	PixelPosDes.x = min(PixelPosDes.x, fPixelWidth);
	PixelPosDes.y = max(1.0f, PixelPosDes.y);
	PixelPosDes.y = min(PixelPosDes.y, fPixelHeight);
	
	if (GetEntity()->GetFighter() && GetEntity()->GetFighter()->GetCharacter())
	{
		GetEntity()->GetFighter()->GetCharacter()->SetAndSyncActionDirByPos(PixelPosDes, true);
	}
	return MoveTo(PixelPosDes, fPixelSpeed, eBarrierType);
}

CMoveServerManager::CMoveServerManager(CEntityServer* pEntity)
:IMoveManager(pEntity)
{
	Ast(pEntity);
	Ast(pEntity->GetFighter());
	pEntity->GetFighter()->Attach(this, eCE_MoveEnd);
	GetEntity()->DisableDirectorMoving(true);
	GetEntity()->m_bMoveManagerHaveOwner=false;
}

CMoveServerManager::~CMoveServerManager()
{
	DetachAll();
	if (GetEntity())
	{
		GetEntity()->StopMoving();
		GetEntity()->DisableDirectorMoving(false);
	}
}

void CMoveServerManager::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity);

	if(pSubject == pEntity->GetFighter())
	{
		switch(uEvent)
		{
		case eCE_MoveEnd:
			if(m_uMoveID == uint32(*reinterpret_cast<uint32*>(pArg)))
			{
				DetachAll();
				delete this;
			}
		}
	}
}

EMoveToResult CMoveServerManager::MoveTo(const CFPos& PixelPosDes,float fPixelSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType)
{
	return IMoveManager::MoveTo(PixelPosDes,fPixelSpeed,eFindPathType,eBarrierType);
}
