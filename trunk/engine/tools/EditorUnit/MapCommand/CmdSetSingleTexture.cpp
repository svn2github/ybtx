#include "stdafx.h"
#include "MapCommand/CmdSetSingleTexture.h"
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"
#include "CEditTerrainMgr.h"

CCmdSetSingleTexture::CCmdSetSingleTexture( CEditContext* pContext ) 
: m_nHotLayer(-1)
, m_pEditContext(pContext)
{

}

void CCmdSetSingleTexture::_Execute()
{
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pEditContext->GetDataScene()->GetLogicDataMgr();
	size_t size = m_vecDiffs.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CVector2I gridcoord = m_vecDiffs[m].gridCoord;
		pEditLogicDataMgr->SetGridTextureName(gridcoord.x, gridcoord.y, m_nHotLayer, m_strHotTextureName);
	}
}

void CCmdSetSingleTexture::_UnExecute()
{
	set<URegionID> regions;
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pEditContext->GetDataScene()->GetLogicDataMgr();
	size_t size = m_vecDiffs.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CVector2I gridcoord = m_vecDiffs[m].gridCoord;
		string texname = m_vecDiffs[m].gridTextures[m_nHotLayer];
		pEditLogicDataMgr->SetGridTextureName(gridcoord.x, gridcoord.y, m_nHotLayer, texname);

		URegionID regionID = URegionID(gridcoord.x / eSRegionSpan, gridcoord.y / eSRegionSpan);
		regions.insert(regionID);
	}

	m_pEditContext->GetEditDataScene()->GetTerrainMgr()->WriteMapBuffer(regions);
}

void CCmdSetSingleTexture::AddGrid(const CVector2I& gridcoord)
{
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pEditContext->GetDataScene()->GetLogicDataMgr();
	string texname = pEditLogicDataMgr->GetGridTextureName(gridcoord.x, gridcoord.y, m_nHotLayer);

	SDiff s;
	s.gridCoord  = gridcoord;
	s.gridTextures[m_nHotLayer] = texname;

	m_vecDiffs.push_back(s);
}