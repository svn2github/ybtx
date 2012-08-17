#include "StdAfx.h"
#include "BlockViewCtrl/BlockViewCtrlImp.h"
#include "Brush/TerrainBlockBrush.h"
#include "CEditContext.h"

DEF_UI_IMP(CBlockViewCtrlImp);
CBlockViewCtrlImp::CBlockViewCtrlImp(void* param)
{
	m_pTerrainBlockBrush = NULL;

	InitBlockBrush();
}

CBlockViewCtrlImp::~CBlockViewCtrlImp(void)
{
	if( m_pOwnContext )
		m_pOwnContext->UnRegisterDelegate(m_pTerrainBlockBrush);

	SafeDelete(m_pTerrainBlockBrush);
}

void CBlockViewCtrlImp::InitBlockBrush()
{
	m_pTerrainBlockBrush = new CTerrainBlockBrush;
}

void CBlockViewCtrlImp::Update()
{
	CCtrlBase::Update();
}

bool CBlockViewCtrlImp::_ProcMsg( const CMsg& msg )
{
	return false;
}

void CBlockViewCtrlImp::SetContext(CEditContext* pContext)
{
	m_pOwnContext = pContext;
	if(m_pOwnContext)
	{
		m_pOwnContext->RegisterDelegate(m_pTerrainBlockBrush);
		m_pTerrainBlockBrush->SetBaseBrushType(BRUSH_REGULAR);
		m_pTerrainBlockBrush->SetBrushIsShowGrid(true);
		m_pTerrainBlockBrush->SetBrushUnit(2);
	}
}

void CBlockViewCtrlImp::SetBlockType( int nBlockType )
{
	m_pTerrainBlockBrush->SetBlockType(nBlockType);
}

int CBlockViewCtrlImp::GetBlockType() const
{
	return m_pTerrainBlockBrush->GetBlockType();
}

void CBlockViewCtrlImp::SetEditType( int nEditType )
{
	m_pTerrainBlockBrush->SetEditType(nEditType);
}

int CBlockViewCtrlImp::GetEditType() const
{
	return m_pTerrainBlockBrush->GetEditType();
}

void CBlockViewCtrlImp::SetEditBrushType( int nEditBrushType )
{
	m_pTerrainBlockBrush->SetEditBrushType(nEditBrushType);
}

int CBlockViewCtrlImp::GetEditBrushType() const
{
	return m_pTerrainBlockBrush->GetEditBrushType();
}

void CBlockViewCtrlImp::ActiveBrush()
{
	if( m_pTerrainBlockBrush )
		m_pTerrainBlockBrush->Active();
}