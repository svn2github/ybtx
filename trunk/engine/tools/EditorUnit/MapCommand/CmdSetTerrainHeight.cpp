#include "StdAfx.h"
#include "CmdSetTerrainHeight.h"
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"
#include "CTerrainOperate.h"
#include "CEditTerrainMgr.h"

CCmdSetTerrainHeight::CCmdSetTerrainHeight(CEditContext* pContext)
:CCmdSetVertexHeight(pContext)
{
}

CCmdSetTerrainHeight::~CCmdSetTerrainHeight(void)
{
}

void CCmdSetTerrainHeight::_Execute()
{
	//获取地图宽高点数
	int widthNum,heightNum;
	if(!m_pEditContext)
		return ;
	CEditDataScene* ds=m_pEditContext->GetEditDataScene();
	if(!ds)
		return ;
	widthNum=ds->GetWidthInGrid();
	heightNum=ds->GetHeightInGrid();
	//
	set<URegionID> regions;
	CEditLogicDataMgr* pEditLogicDataMgr = ds->GetLogicDataMgr();
	
	size_t size = m_vecDiffs.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CVector2I vertexcoord = m_vecDiffs[m].vertexCoord;
		switch(m_opType)
		{
		case EOP_SMOOTH:
			{
				m_vecDiffs[m].fHeight=m_pTerrainOp->SmoothVertexHeight(vertexcoord.x, vertexcoord.y,widthNum,heightNum,m_fStrength);
				break;
			}
		case EOP_ALIGN:
			{
				m_vecDiffs[m].fHeight=m_pTerrainOp->AlignVertexHeight(vertexcoord.x, vertexcoord.y,100.0f);
				break;
			}
		case EOP_GAUS_INC:
			{
				m_vecDiffs[m].fHeight=m_pTerrainOp->CulGaussVertexHeight(
					vertexcoord.x, vertexcoord.y, m_hotVertex.x, m_hotVertex.y, m_dwBrushWidth, m_dwBrushHeight, m_fStrength,true);
				 break;
			}
		case EOP_GAUS_DEC:
			{
				m_vecDiffs[m].fHeight=m_pTerrainOp->CulGaussVertexHeight(
					vertexcoord.x, vertexcoord.y, m_hotVertex.x, m_hotVertex.y, m_dwBrushWidth, m_dwBrushHeight, m_fStrength,false);
				break;
			}
		case EOP_LINE_INCX:
			{
				m_vecDiffs[m].fHeight=m_pTerrainOp->CulLineVertexHeightX(
					vertexcoord.x, vertexcoord.y, m_hotVertex.x, m_hotVertex.y, m_dwBrushWidth, m_dwBrushHeight, m_fStrength,true);
				break;
			}
		case EOP_LINE_DECX:
			{
				m_vecDiffs[m].fHeight=m_pTerrainOp->CulLineVertexHeightX(
					vertexcoord.x, vertexcoord.y, m_hotVertex.x, m_hotVertex.y, m_dwBrushWidth, m_dwBrushHeight, m_fStrength,false);
				break;
			}
		case EOP_LINE_INCZ:
			{
				m_vecDiffs[m].fHeight=m_pTerrainOp->CulLineVertexHeightZ(
					vertexcoord.x, vertexcoord.y, m_hotVertex.x, m_hotVertex.y, m_dwBrushWidth, m_dwBrushHeight, m_fStrength,true);
				break;
			}
		case EOP_LINE_DECZ:
			{
				m_vecDiffs[m].fHeight=m_pTerrainOp->CulLineVertexHeightZ(
					vertexcoord.x, vertexcoord.y, m_hotVertex.x, m_hotVertex.y, m_dwBrushWidth, m_dwBrushHeight, m_fStrength,false);
				break;
			}
		case EOP_LINE_INCB:
			{
				m_vecDiffs[m].fHeight=m_pTerrainOp->CulLineVertexHeightB(
					vertexcoord.x, vertexcoord.y, m_hotVertex.x, m_hotVertex.y, m_dwBrushWidth, m_dwBrushHeight, m_fStrength,true);
				break;
			}
		case EOP_LINE_DECB:
			{
				m_vecDiffs[m].fHeight=m_pTerrainOp->CulLineVertexHeightB(
					vertexcoord.x, vertexcoord.y, m_hotVertex.x, m_hotVertex.y, m_dwBrushWidth, m_dwBrushHeight, m_fStrength,false);
				break;
			}
		}
		URegionID regionID = URegionID(vertexcoord.x / eSRegionSpan, vertexcoord.y / eSRegionSpan);
		regions.insert(regionID);
	}
	//法线
	for ( size_t m = 0; m < size; ++m )
	{
		CVector2I vertexcoord = m_vecDiffs[m].vertexCoord;
		m_vecDiffs[m].vertexNormal=m_pTerrainOp->CalculateVertexNormal(
			vertexcoord.x,vertexcoord.y,widthNum-1,heightNum-1);
	}
	m_pEditContext->GetEditDataScene()->GetTerrainMgr()->WriteMapBuffer(regions);
}

void CCmdSetTerrainHeight::_UnExecute()
{
	set<URegionID> regions;
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pEditContext->GetDataScene()->GetLogicDataMgr();
	size_t size = m_vecDiffs.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CVector2I vertexcoord = m_vecDiffs[m].vertexCoord;
		pEditLogicDataMgr->SetVertexPositionHeight(vertexcoord.x, vertexcoord.y,  m_vecDiffs[m].fHeight);
		pEditLogicDataMgr->SetVertexLogicNormal(vertexcoord.x, vertexcoord.y,  m_vecDiffs[m].vertexNormal);
		URegionID regionID = URegionID(vertexcoord.x / eSRegionSpan, vertexcoord.y / eSRegionSpan);
		regions.insert(regionID);
	}
	m_pEditContext->GetEditDataScene()->GetTerrainMgr()->WriteMapBuffer(regions);
}
