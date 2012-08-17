#include "stdafx.h"
#include "MapCommand/CmdSetTerrainBlock.h"
#include "CEditContext.h"
#include "CDataScene.h"
#include "CEditLogicDataMgr.h"


CCmdSetTerrainBlock::CCmdSetTerrainBlock( CEditContext* pContext ) 
: m_nBlockType(0)
, m_pEditContext(pContext)
{

}

void CCmdSetTerrainBlock::_Execute()
{
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pEditContext->GetDataScene()->GetLogicDataMgr();
	size_t size = m_vecDiffs.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CVector2I gridcoord = m_vecDiffs[m].gridCoord;
		pEditLogicDataMgr->SetBlockType(gridcoord.x, gridcoord.y, m_nBlockType);
	}
}

void CCmdSetTerrainBlock::_UnExecute()
{
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pEditContext->GetDataScene()->GetLogicDataMgr();
	size_t size = m_vecDiffs.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CVector2I gridcoord = m_vecDiffs[m].gridCoord;
		pEditLogicDataMgr->SetBlockType(gridcoord.x, gridcoord.y, m_vecDiffs[m].nBlockType);
	}
}

void CCmdSetTerrainBlock::AddGrid(const CVector2I& gridcoord)
{
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pEditContext->GetDataScene()->GetLogicDataMgr();
	int blocktype = pEditLogicDataMgr->GetBlockType(gridcoord.x, gridcoord.y);

	SDiff s;
	s.gridCoord  = gridcoord;
	s.nBlockType = blocktype;

	m_vecDiffs.push_back(s);
}