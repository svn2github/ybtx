#include "StdAfx.h"
#include "Brush/TerrainBlockBrush.h"
#include "Brush/BaseBrushDraw.h"
#include "CEditContext.h"
#include "CDataScene.h"
#include "CEditLogicDataMgr.h"
#include "MapCommand/CmdSetTerrainBlock.h"

CTerrainBlockBrush::CTerrainBlockBrush(void)
{
	m_eBlockTypeHot			= EBT_HIGH;
	m_eBlockEditType		= EET_WRITE;
	m_eEditBlockBrushType	= EBT_NORMAL;
}

CTerrainBlockBrush::~CTerrainBlockBrush(void)
{
}


bool CTerrainBlockBrush::KeyDownEvent( MSG_KEY_INF key )
{
	bool bContinue = false;
	switch( key )
	{
	case MGI_KEY_F2:	
		SetTerrainBlock(m_eBlockTypeHot);
		break;

	case MGI_KEY_F3:	
		SetTerrainBlock(EBT_NONE);
		break;

	default:
		bContinue = CBaseBrush::KeyDownEvent(key);
		break;
	}

	return bContinue;
}

void CTerrainBlockBrush::KeyUpEvent( MSG_KEY_INF key )
{
}

void CTerrainBlockBrush::SetTerrainBlock(const EBlockType blocktype)
{
	CBaseBrushDraw::brushGridPool gridIndexs;
	m_pBaseBrushDraw->GetBrushGridIndexs(gridIndexs);
	if( gridIndexs.empty() )
		return;
	
	CCmdSetTerrainBlock *pCmd = new CCmdSetTerrainBlock(m_pContext);
	pCmd->SetBlockType(blocktype);

	size_t size = gridIndexs.size();
	for ( size_t m = 0 ; m < size; ++m )
	{
		pCmd->AddGrid(gridIndexs[m]);
	}

	pCmd->Execute();
}
