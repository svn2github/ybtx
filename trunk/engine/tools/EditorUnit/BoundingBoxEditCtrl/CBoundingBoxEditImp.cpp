#include "stdafx.h"
#include "BoundingBoxEditCtrl/CBoundingBoxEditImp.h"
#include "BoundingBoxEditCtrl/CBoundingBoxDelegate.h"
#include "CEditContext.h"
#include "BaseHelper.h"
#include "CEditModel.h"
#include "CEditRenderStyle.h"
#include "CEditPieceGroup.h"
#include "CRenderDebugFun.h"

DEF_UI_IMP(CBoundingBoxEditImp);

#define MODEL ((CEditModel*)m_pOwnContext->GetActiveObj()) 

CBoundingBoxEditImp::CBoundingBoxEditImp( void* param )
:m_pEditPiece(NULL)
,m_pModel(NULL)
,m_pDelegate(NULL)
{
	m_bShowBigBox = m_bShowSmallBox = false;
}

CBoundingBoxEditImp::~CBoundingBoxEditImp( void )
{
	if (m_pDelegate)
	{
		delete m_pDelegate;
		m_pDelegate = NULL;
	}
}

void CBoundingBoxEditImp::SetContext( CEditContext* pContext )
{
	CCtrlBaseObserver::SetContext(pContext);

	if (m_pDelegate)
		return;
	m_pDelegate = new CBoundingBoxDelegate(this);
	m_pOwnContext->RegisterDelegate(m_pDelegate);
	m_pOwnContext->ActiveDelegate(m_pDelegate);
}

void CBoundingBoxEditImp::ActiveNode( CRenderNode* pNode )
{
	if (!MODEL)
		return;

	if (m_pEditPiece != MODEL->GetSelectPiece())
	{
		m_pEditPiece = MODEL->GetSelectPiece();
		CCtrlBase::Update();
	}
}

void sqr_tools::CBoundingBoxEditImp::SetModel( CEditModel* model )
{
	m_pModel = model;
}

void CBoundingBoxEditImp::SetShowBigBox( bool value )
{
	m_bShowBigBox = value;
}

void CBoundingBoxEditImp::SetShowSmallBox( bool value )
{
	m_bShowSmallBox = value;
}

void CBoundingBoxEditImp::SetPieceDirty(bool dirty)
{
	m_pEditPiece->m_bMeshDirty = dirty;
}

void CBoundingBoxEditImp::RefreshMesh(CEditMeshWriter* pMesh, float xDelta, float yDelta, float zDelta, float xSize, float ySize, float zSize)
{
	if ( pMesh )
	{
		CVector3f off(xDelta, yDelta, zDelta);
		pMesh->SetOff(off);
		CVector3f scale(xSize, ySize, zSize);
		pMesh->SetScale(scale);

		scale = scale*0.5f;
		scale.x = CMath::Abs(scale.x);
		scale.y = CMath::Abs(scale.y);
		scale.z = CMath::Abs(scale.z);

		m_pEditPiece->SetAABoxExtends( off - scale , off + scale );
		SetPieceDirty(true);
	}
}

void CBoundingBoxEditImp::GetPieceBoxInfo( float& xDelta, float& yDelta, float& zDelta, float& xSize, float& ySize, float& zSize )
{
	if (!m_pEditPiece)
	{
		xDelta = yDelta = zDelta = xSize = ySize = zSize = -1;
		return;
	}

	xDelta = m_pEditPiece->GetMesh()->GetOff().x;
	yDelta = m_pEditPiece->GetMesh()->GetOff().y;
	zDelta = m_pEditPiece->GetMesh()->GetOff().z;
	xSize = m_pEditPiece->GetMesh()->GetScale().x;
	ySize = m_pEditPiece->GetMesh()->GetScale().y;
	zSize = m_pEditPiece->GetMesh()->GetScale().z;
}

void CBoundingBoxEditImp::SetPieceBoxInfo( float xDelta, float yDelta, float zDelta, float xSize, float ySize, float zSize )
{
	if (!m_pEditPiece)
		return;

	RefreshMesh(m_pEditPiece->GetMeshWriter(), xDelta, yDelta, zDelta, xSize, ySize, zSize);

	m_pEditPiece->GetMesh()->GetOff().x = xDelta;
	m_pEditPiece->GetMesh()->GetOff().y = yDelta;
	m_pEditPiece->GetMesh()->GetOff().z = zDelta;
	m_pEditPiece->GetMesh()->GetScale().x = xSize;
	m_pEditPiece->GetMesh()->GetScale().y = ySize;
	m_pEditPiece->GetMesh()->GetScale().z = zSize;
}

void CBoundingBoxEditImp::Render( void )
{
	if (MODEL)
	{
		if (m_bShowBigBox)
		{
			RenderBigBoxDebug(MODEL);
		}

		if (m_bShowSmallBox && !MODEL->GetSketetalFrame())
		{
			RenderSmallBoxDebug(MODEL, m_pEditPiece?&m_pEditPiece->GetAABox():NULL);
		}
	}
}

