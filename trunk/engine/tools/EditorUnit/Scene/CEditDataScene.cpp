#include "stdafx.h"
#include "CEditDataScene.h"
#include "CEditTerrainMgr.h"
#include "CEditSkyMgr.h"
#include "CEditWaterMgr.h"
#include "CEditLogicDataMgr.h"
#include "CGraphic.h"
#include "CRenderGroup.h"
#include "BaseHelper.h"
#include "CQuadScene.h"
#include "CMetaGridMgr.h"

#include "CEditTerrainNode.h"
#include "CEditLogicData.h"
#include "CVertexDeclaration.h"

#include "TraceHelper.h"

CEditDataScene::CEditDataScene( const string& sName, uint32 Wdith , uint32 Height, uint32 RecycleSize )
: CDataScene(sName.c_str(), new CQuadTreeScene,NULL,Wdith,Height,RecycleSize)
{
}

CEditDataScene::~CEditDataScene()
{
}

void CEditDataScene::InitEditDateScene(CWindowTarget* pTarget)
{
	m_pRenderGroup	= pTarget->GetGroup();
	m_pLogicDataMgr = new CEditLogicDataMgr();
	m_pTerrainMgr	= new CEditTerrainMgr(m_pRenderGroup,(CEditLogicDataMgr*)m_pLogicDataMgr);
	m_pWaterMgr		= new CEditWaterMgr(m_pRenderGroup,(CEditLogicDataMgr*)m_pLogicDataMgr);
	m_pSkyMgr		= new CEditSkyMgr( pTarget);
}

CEditTerrainMgr*	CEditDataScene::GetTerrainMgr(void)
{
	return (CEditTerrainMgr*)m_pTerrainMgr;
}

CEditSkyMgr*		CEditDataScene::GetSkyMgr(void)
{
	return (CEditSkyMgr*)m_pSkyMgr;
}

CEditWaterMgr*		CEditDataScene::GetWaterMgr(void)
{
	return (CEditWaterMgr*)m_pWaterMgr;
}

CEditLogicDataMgr* CEditDataScene::GetLogicDataMgr( void )
{
	return (CEditLogicDataMgr*)m_pLogicDataMgr;
}

void	CEditDataScene::SetWidth( uint16 width )
{
	m_uWidth = width;
	m_uWidthInRegion	= m_uWidth * 4;
	m_uWidthInGrid		= m_uWidthInRegion  * eRegionSpan;
	m_uWidthInPixel		= eSGridSpan * m_uWidthInGrid;
}

void CEditDataScene::SetHeight( uint16 height )
{
	m_uHeight = height;
	m_uHeightInRegion	= m_uHeight * 4;
	m_uHeightInGrid		= m_uHeightInRegion * eRegionSpan;
	m_uHeightInPixel	= eSGridSpan * m_uHeightInGrid;
}