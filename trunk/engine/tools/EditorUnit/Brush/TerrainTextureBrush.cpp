#include "StdAfx.h"
#include "Brush/TerrainTextureBrush.h"
#include "Brush/TerrainTextureData.h"
#include "MapCommand/CmdSetSingleTexture.h"
#include "MapCommand/CmdSetVertexAlpha.h"
#include "Brush/BaseBrushDraw.h"
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"
#include "CEditLogicData.h"
#include "CEditTerrainMgr.h"
#include "CSceneCommon.h"
#include "CSceneBase.h"

CTerrainTextureBrush::CTerrainTextureBrush(void) : m_nHotLayer(-1), m_pTerrainTextureData(NULL)
{
	m_eWayType			= EWT_INC;
	m_eDirType			= EDT_BOTH;
	m_fAlphaStrength	= 16.0f;
}

CTerrainTextureBrush::~CTerrainTextureBrush(void)
{
	m_vecBrushTextureNames.clear();
}

void CTerrainTextureBrush::InitTextureData()
{
	m_pTerrainTextureData = new CTerrainTextureData;
	m_pTerrainTextureData->SetEditContext(m_pContext);
	m_pTerrainTextureData->InitTerrainTexture();
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pContext->GetDataScene()->GetLogicDataMgr();
	pEditLogicDataMgr->SetTextureData(m_pTerrainTextureData);
}

void CTerrainTextureBrush::SetBrushTextureNames( const brushTexPool& names )
{
	m_vecBrushTextureNames = names;
}

void CTerrainTextureBrush::MoveLButtonDownEvent(MSG_MOUSE_INF mouse)
{
	if( m_vecBrushTextureNames.empty() )
		return;

	if( m_nHotLayer < 0 || m_nHotLayer > 2 )
		return;

	CBaseBrushDraw::brushGridPool gridIndexs;
	m_pBaseBrushDraw->GetBrushGridIndexs(gridIndexs);
	if( gridIndexs.empty() )
		return;

	set<URegionID> regions;
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pContext->GetDataScene()->GetLogicDataMgr();

	CCmdSetSingleTexture *pCmd = new CCmdSetSingleTexture(m_pContext);
	pCmd->SetHotLayerTextureName(m_vecBrushTextureNames[0]);
	pCmd->SetHotLayer(m_nHotLayer);

	size_t size = gridIndexs.size();
	for ( size_t m = 0; m < size; ++m )
	{
		pCmd->AddGrid(gridIndexs[m]);
		URegionID regionID = URegionID(gridIndexs[m].x / eSRegionSpan, gridIndexs[m].y / eSRegionSpan);
		regions.insert(regionID);
	}

	pCmd->Execute();

	if( m_nHotLayer == 1 )
		BrushLayerTwoTexture();

	m_pTerrainTextureData->InsertTerrainTexture(m_vecBrushTextureNames);
	m_pContext->GetEditDataScene()->GetTerrainMgr()->WriteMapBuffer(regions);
}

bool CTerrainTextureBrush::KeyDownEvent( MSG_KEY_INF key )
{
	return CBaseBrush::KeyDownEvent(key);
}

void CTerrainTextureBrush::KeyUpEvent( MSG_KEY_INF key )
{

}

void CTerrainTextureBrush::CtrlKeyDownEvent(MSG_KEY_INF key)
{
	switch( key )
	{
	case MGI_KEY_D1:	///ÏÔÊ¾ÎÆÀí
		{
			ShowTexture();
		}
		break;

	default:
		break;
	}
}

void CTerrainTextureBrush::BrushLayerTwoTexture()
{
	CBaseBrushDraw::brushVertexsPool vertexIndexs;
	m_pBaseBrushDraw->GetBrushVertexIndexs(vertexIndexs);
	if( vertexIndexs.empty() )
		return;

 	CCmdSetVertexAlpha *pCmd = new CCmdSetVertexAlpha(m_pContext);
	pCmd->SetEWayType(m_eWayType);
	pCmd->SetTerrainBrush(this);

 	size_t size = vertexIndexs.size();
 	for ( size_t m = 0 ; m < size; ++m )
 	{
 		CVector3f vertex = vertexIndexs[m];
 		CVector2I ivec = CVector2I(int(vertex.x) / eSGridSpan, int(vertex.z) / eSGridSpan);
 
 		pCmd->AddVertex(ivec);
 	}

	pCmd->Execute();
}

void CTerrainTextureBrush::ShowTexture()
{
	if( m_nHotLayer < 0 || m_nHotLayer > 2 )
		return;

	CVector2I centergridcoord = CVector2I(int(m_vecBrushCenter.x) / eSGridSpan, int(m_vecBrushCenter.z) / eSGridSpan);
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pContext->GetDataScene()->GetLogicDataMgr();
	string texname = pEditLogicDataMgr->GetGridTextureName(centergridcoord.x, centergridcoord.y, m_nHotLayer);
	ShellExecute(NULL, "open", texname.c_str(), "", "", SW_SHOW);
}

void CTerrainTextureBrush::IncVertexAlphaB( uint32 uX, uint32 uY )
{
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pContext->GetDataScene()->GetLogicDataMgr();

	CVector3f vertex	= pEditLogicDataMgr->GetVertexPosition(uX, uY);
	DWORD     color		= pEditLogicDataMgr->GetVertexDiffuse(uX, uY);
	CVector3f vertexHot = m_vecBrushCenter;

	float distanceZ = fabs(vertex.z - vertexHot.z);
	float distanceX = fabs(vertex.x - vertexHot.x);
	DWORD dwBrushWidth = m_pBaseBrushDraw->GetBrushOuterWidth();
	DWORD dwBrushDepth = m_pBaseBrushDraw->GetBrushOuterHeight();

	float radio = 0.0f, radio1 = 0.0f, radio2 = 0.0f;
	if ( dwBrushWidth > dwBrushDepth )
	{
		radio1 = fabs((dwBrushWidth*eSGridSpan - distanceX) / (eSGridSpan*dwBrushDepth));
		radio2 = fabs((dwBrushDepth*eSGridSpan - distanceZ) / (eSGridSpan*dwBrushDepth));
		radio = radio1 > radio2 ? radio2 : radio1;
	}
	else
	{
		radio1 = fabs((dwBrushWidth*eSGridSpan - distanceX) / (eSGridSpan*dwBrushWidth));
		radio2 = fabs((dwBrushDepth*eSGridSpan - distanceZ) / (eSGridSpan*dwBrushWidth));
		radio = radio1 > radio2 ? radio2 : radio1;
	}

	byte alpha = byte(color >> 24);
	if ( static_cast<byte>(alpha + radio*m_fAlphaStrength) < alpha )
	{
		alpha = 0xff;
	}
	else
	{
		alpha += static_cast<byte>(radio*m_fAlphaStrength);
	}

	color |= 0xff000000;
	color &= (DWORD)alpha << 24 | 0x00ffffff;

	pEditLogicDataMgr->SetVertexDiffuse(uX, uY, color);
}

void CTerrainTextureBrush::DecVertexAlphaB( uint32 uX, uint32 uY )
{
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pContext->GetDataScene()->GetLogicDataMgr();

	CVector3f vertex	= pEditLogicDataMgr->GetVertexPosition(uX, uY);
	DWORD     color		= pEditLogicDataMgr->GetVertexDiffuse(uX, uY);
	CVector3f vertexHot = m_vecBrushCenter;

	float distanceZ = fabs(vertex.z - vertexHot.z);
	float distanceX = fabs(vertex.x - vertexHot.x);
	DWORD dwBrushWidth = m_pBaseBrushDraw->GetBrushOuterWidth();
	DWORD dwBrushDepth = m_pBaseBrushDraw->GetBrushOuterHeight();

	float radio = 0.0f, radio1 = 0.0f, radio2 = 0.0f;
	radio1 = (dwBrushWidth*eSGridSpan - distanceX) / (eSGridSpan*dwBrushDepth);
	radio2 = (dwBrushDepth*eSGridSpan - distanceZ) / (eSGridSpan*dwBrushDepth);
	radio = radio1 > radio2 ? radio2 : radio1;
	byte alpha = byte(color >> 24);
	if ( static_cast<byte>(alpha - radio*m_fAlphaStrength) > alpha )
	{
		alpha = 0x00;
	}
	else
	{
		alpha -= static_cast<byte>(radio*m_fAlphaStrength);
	}

	color |= 0xff000000;
	color &= (DWORD)alpha << 24 | 0x00ffffff;

	pEditLogicDataMgr->SetVertexDiffuse(uX, uY, color);
}

void CTerrainTextureBrush::CutVertexAlpha( uint32 uX, uint32 uY )
{
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pContext->GetDataScene()->GetLogicDataMgr();
	pEditLogicDataMgr->SetVertexDiffuse(uX, uY, VERTEX_COLOR);
}