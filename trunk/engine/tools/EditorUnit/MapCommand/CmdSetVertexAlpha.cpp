#include "stdafx.h"
#include "CmdSetVertexAlpha.h"
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"
#include "Brush/TerrainTextureBrush.h"
#include "CEditTerrainMgr.h"

CCmdSetVertexAlpha::CCmdSetVertexAlpha( CEditContext* pContext ) : m_pEditContext(pContext)
{	
	m_pTerrainTextureBrush = NULL;
}

void CCmdSetVertexAlpha::_Execute()
{
 	size_t size = m_vecDiffs.size();
 	for ( size_t m = 0; m < size; ++m )
 	{
 		CVector2I gridcoord = m_vecDiffs[m].vertexCoord;

		switch (m_eWayType)
		{
		case EWT_INC:
			m_pTerrainTextureBrush->IncVertexAlphaB(gridcoord.x, gridcoord.y);
			break;

		case EWT_DEC:
			m_pTerrainTextureBrush->DecVertexAlphaB(gridcoord.x, gridcoord.y);
			break;

		case EWT_CUT:
			m_pTerrainTextureBrush->CutVertexAlpha(gridcoord.x, gridcoord.y);
			break;

		default:
			break;
		}
 	}
}

void CCmdSetVertexAlpha::_UnExecute()
{
 	set<URegionID> regions;
 	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pEditContext->GetDataScene()->GetLogicDataMgr();
 	size_t size = m_vecDiffs.size();
 	for ( size_t m = 0; m < size; ++m )
 	{
 		CVector2I gridcoord = m_vecDiffs[m].vertexCoord;
 		DWORD oColor = m_vecDiffs[m].oColor;
		pEditLogicDataMgr->SetVertexDiffuse(gridcoord.x, gridcoord.y, oColor);
 
 		URegionID regionID = URegionID(gridcoord.x / eSRegionSpan, gridcoord.y / eSRegionSpan);
 		regions.insert(regionID);
 	}
 
 	m_pEditContext->GetEditDataScene()->GetTerrainMgr()->WriteMapBuffer(regions);
}

void CCmdSetVertexAlpha::AddVertex(const CVector2I& vertexcoord)
{
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pEditContext->GetDataScene()->GetLogicDataMgr();
	DWORD oColor = pEditLogicDataMgr->GetVertexDiffuse(vertexcoord.x, vertexcoord.y);

	SVertexAlpha s;
	s.vertexCoord  = vertexcoord;
	s.oColor	   = oColor;

	m_vecDiffs.push_back(s);
}


void CCmdSetVertexAlpha::SetTerrainBrush( CTerrainTextureBrush* pBrush )
{
	m_pTerrainTextureBrush = pBrush;
}