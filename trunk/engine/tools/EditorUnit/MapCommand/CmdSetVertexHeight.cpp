#include "stdafx.h"
#include "MapCommand/CmdSetVertexHeight.h"
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"
#include "CTerrainOperate.h"
#include "CEditTerrainMgr.h"

CCmdSetVertexHeight::CCmdSetVertexHeight( CEditContext* pContext ) 
: m_dwBrushWidth(1)
, m_dwBrushHeight(1)
, m_pEditContext(pContext)
{
	m_fStrength=16.0f;
}

CCmdSetVertexHeight::~CCmdSetVertexHeight()
{
	m_vecDiffs.clear();
}

void CCmdSetVertexHeight::SetContext(CEditContext* pContext)
{
	m_pEditContext=pContext;
}

bool CCmdSetVertexHeight::IsSetContext()
{
	if(m_pEditContext)
		return true;
	return false;
}

void CCmdSetVertexHeight::SetOpType(ETERRAIN_OP_TYPE opType)
{
	m_opType=opType;
}

void CCmdSetVertexHeight::BindOperate(CTerrainOperate* Op)
{
	m_pTerrainOp=Op;
}

void CCmdSetVertexHeight::_Execute()
{
}

void CCmdSetVertexHeight::_UnExecute()
{	
}

void CCmdSetVertexHeight::AddVertex(const CVector2I& vertexcoord)
{
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pEditContext->GetDataScene()->GetLogicDataMgr();
	float fHeight = pEditLogicDataMgr->GetVertexPosition(vertexcoord.x, vertexcoord.y).y;

	SCmdVertex s;
	s.vertexCoord  = vertexcoord;
	s.fHeight	   = fHeight;

	m_vecDiffs.push_back(s);
}
void CCmdSetVertexHeight::ClearVertex()
{
	m_vecDiffs.clear();
}

void CCmdSetVertexHeight::SetHotVertex( const CVector2I& vertexcoord )
{
	m_hotVertex = vertexcoord;
}

void CCmdSetVertexHeight::SetBrushSize( const DWORD dwWidth, const DWORD dwHeight )
{
	m_dwBrushWidth  = dwWidth;
	m_dwBrushHeight = dwHeight;
}

void CCmdSetVertexHeight::SetStrength( const float f )
{
	m_fStrength = f;
}
