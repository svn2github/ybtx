#include "StdAfx.h"
#include "Brush/TerrainGridBrush.h"
#include "CEventState.h"
#include "MapCommand/CmdSetVertexHeight.h"
#include "MapCommand/CmdSetTerrainHeight.h"
#include "MapCommand/CmdSetWaterHeight.h"
#include "Brush/BaseBrushDraw.h"
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CSceneCommon.h"
#include "CSceneBase.h"
#include "CTerrainOperate.h"

CTerrainGridBrush::CTerrainGridBrush(void)
{
	m_eOpType			= EOP_NONE;
	m_eTemplateType		= EHTT_GAUSSCUT;
	m_fHeightStrength	= 16.0f;
	m_TerrainOp	= new CTerrainOperate();
}

CTerrainGridBrush::~CTerrainGridBrush(void)
{
}

void CTerrainGridBrush::SetTerrainOpType(ETERRAIN_OP_TYPE _type)
{
	m_eOpType=_type;
}

void CTerrainGridBrush::SetTemplateType(const int nType)
{
	m_eTemplateType = (EHEIGHT_TEMPLATE_TYPE)nType;
}

void CTerrainGridBrush::SetHeightStrength(const float f)
{
	m_fHeightStrength = f;
}
float CTerrainGridBrush::GetHeightStrength() const
{
	return m_fHeightStrength;
}

void CTerrainGridBrush::MoveLButtonDownEvent(MSG_MOUSE_INF mouse)
{
	CBaseBrushDraw::brushVertexsPool vertexIndexs;
	m_pBaseBrushDraw->GetBrushVertexIndexs(vertexIndexs);
	if( vertexIndexs.empty() || (m_eOpType == EOP_NONE) || (!m_pContext) )
		return;

	m_TerrainOp->SetLogicDataMgr((CEditLogicDataMgr*)m_pContext->GetDataScene()->GetLogicDataMgr());
	CVector2I ivec;
	ivec.x=(m_vecBrushCenter.x/ eSGridSpan)>(int(m_vecBrushCenter.x/ eSGridSpan))?int((m_vecBrushCenter.x/ eSGridSpan)+1):int(m_vecBrushCenter.x/ eSGridSpan);
	ivec.y=(m_vecBrushCenter.z/ eSGridSpan)>(int(m_vecBrushCenter.z/ eSGridSpan))?int((m_vecBrushCenter.z/ eSGridSpan)+1):int(m_vecBrushCenter.z/ eSGridSpan);

	//CCmdSetWaterHeight* m_pCmd = new CCmdSetWaterHeight(m_pContext);
	CCmdSetTerrainHeight* m_pCmd = new CCmdSetTerrainHeight(m_pContext);
	m_pCmd->BindOperate(m_TerrainOp);
	m_pCmd->SetOpType(m_eOpType);
	m_pCmd->SetStrength(m_fHeightStrength);
	m_pCmd->SetHotVertex(CVector2I(ivec.x, ivec.y));
	m_pCmd->SetBrushSize(m_pBaseBrushDraw->GetBrushOuterWidth(), m_pBaseBrushDraw->GetBrushOuterHeight());
	
	size_t size = vertexIndexs.size();
 	for ( size_t m = 0 ; m < size; ++m )
 	{
		CVector3f vertex = vertexIndexs[m];
		CVector2I ivec = CVector2I(int(vertex.x+eSRegionSpan) / eSGridSpan, int(vertex.z+eSRegionSpan) / eSGridSpan);
 		m_pCmd->AddVertex(ivec);
 	}
	m_pCmd->Execute();
}

bool CTerrainGridBrush::KeyDownEvent( MSG_KEY_INF key )
{
	if ( CEventState::GetInst()->GetState(MGT_MOUSE_LBUTTON) == MGT_MOUSE_LBNDOWN )
	{
		if ( key == MGI_KEY_CtrlKey )
		{
			int test = 0;
		}
	}

	return CBaseBrush::KeyDownEvent(key);
}

void CTerrainGridBrush::KeyUpEvent( MSG_KEY_INF key )
{

}
