#include "stdafx.h"
#include "CCoreObjectPerformer.h"
#include "ICoreObjectPerformerHandler.h"
#include "CoreCommon.h"
#include "COEventClient.h"
#include "CCoreMsgMgr.h"
#include "CRenderObject.h"
#include "CRenderSystemClient.h"

CCoreObjectPerformer::CCoreObjectPerformer(ICoreObjectPerformerHandler* pHandler,const CFPos& PixelPos)
:CCoreObjectClient(pHandler,PixelPos )
,m_pRenderObject(NULL)
,m_fHeightInPixel(0)
,m_bCanBeSelected( true )
,m_bLieOnTheTerrain( true )
{
	m_pRenderObject=CClientRenderScene::GetInst()->CreateRenderObject(NULL);
	if(!m_pRenderObject)
		GenErr("Create Render Object failed.");
	RefreshRenderObjectPos();
}

CCoreObjectPerformer::~CCoreObjectPerformer(void)
{
	SQR_TRY
	{
		CClientRenderScene::GetInst()->DestroyRenderObject(m_pRenderObject);
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

CRenderObject* CCoreObjectPerformer::GetRenderObject()const
{
	return m_pRenderObject;
}

ICoreObjectPerformerHandler* CCoreObjectPerformer::GetHandler()const
{
	ICoreObjectClientHandler* pHandler=CCoreObjectClient::GetHandler();
	return pHandler?pHandler->CastToObjPerHandler():NULL;
}

void CCoreObjectPerformer::OnEndMoveDerived()
{
	CCoreMsgMgr::Inst().PostMsg(new (CCoreMsgMgr::Inst())
		CCOEventMoveEndedClient(this,eMET_Reached));
}

EClientObjectType CCoreObjectPerformer::GetType()const
{
	return eCOT_Performer;
}

ESetPosResult CCoreObjectPerformer::SetPixelPos(const CFPos& PixelPos)
{
	return IntSetPixelPos(PixelPos);
}

ESetPosResult CCoreObjectPerformer::SetGridPos(const CPos& GridPos)
{
	return SetPixelPos( CFPos(GetPixelOfGrid(GridPos.x),GetPixelOfGrid(GridPos.y)) );
}

ESetSpeedResult CCoreObjectPerformer::SetSpeed(float fSpeed)
{
	return IntSetSpeed(fSpeed);
}

EMoveToResult CCoreObjectPerformer::MoveTo(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType,float fReachDist)
{
	return IntMoveTo(PixelPosDes,fSpeed,eFindPathType,eBarrierType,fReachDist,NULL,0);
}

EStopMovingResult CCoreObjectPerformer::StopMoving()
{
	return IntStopMoving();
}

bool CCoreObjectPerformer::IsMoving()const
{
	return IntIsDoingMove();
}

void CCoreObjectPerformer::SetHeight(float fHeightInPixel)
{
	m_fHeightInPixel=fHeightInPixel;
	RefreshRenderObjectPos();
}

float CCoreObjectPerformer::GetHeight()const
{
	return m_fHeightInPixel;
}

void CCoreObjectPerformer::SetCanBeSelected( bool bCan )
{
	m_bCanBeSelected = bCan;
}

bool CCoreObjectPerformer::GetCanBeSelected()
{
	return m_bCanBeSelected;
}

void CCoreObjectPerformer::SetLieOnTheTerrain( bool bLieOn )
{
	m_bLieOnTheTerrain = bLieOn;
}

void CCoreObjectPerformer::IntSetPositionDerived(const CFPos& PixelPos)
{
	ParentObject_t::IntSetPositionDerived( PixelPos );
	RefreshRenderObjectPos();
}

void CCoreObjectPerformer::RefreshRenderObjectPos()
{
	SQR_TRY
	{
		Ast( m_pRenderObject );

		float fRatio = eGridSpanForObj / eGridSpan;
		CFPos pos;
		pos = GetPixelPos();
		//cout << "CCoreObjectClient::RefreshRenderObjectPos " << pos.x / fRatio << " " << pos.y / fRatio << endl;
		Ast( CClientRenderScene::HasInst() );
		CRenderScene* pRenderScene = CClientRenderScene::GetInst();
		if (m_bLieOnTheTerrain)
		{
			pRenderScene->AttachRenderObject(*m_pRenderObject,CVector3f(pos.x / fRatio, m_fHeightInPixel, pos.y / fRatio));
		}
		else
		{
			pRenderScene->AttachRenderObjectAbs(*m_pRenderObject,CVector3f(pos.x / fRatio, m_fHeightInPixel, pos.y / fRatio));
		}
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}