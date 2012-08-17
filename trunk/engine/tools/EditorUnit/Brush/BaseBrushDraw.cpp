#include "StdAfx.h"
#include "Brush/BaseBrushDraw.h"
#include "Brush/BrushGridHepler.h"
#include "CurvePatch/CEditCurveRender.h"
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"
#include "CTexture.h"
#include "CGraphic.h"
#include "BaseHelper.h"
#include "TraceHelper.h"

CBaseBrushDraw::CBaseBrushDraw(void) 
	: m_nOuterWidth(1), m_nOuterHeight(1), m_nInnerWidth(1), m_nInnerHeight(1), m_nBrushUnit(1), m_dwBrushColor(0xffff0000), m_bShowGrid(false),
	  m_pTexture(NULL), m_pEditCurveRender(NULL)
{
	m_pBrushGridHepler = new CBrushGridHepler;

	m_curveSampler.SetDensity(2);

	m_pEditCurveRender = new CEditCurveRender;

	m_pEditCurveRender->SetColor(m_dwBrushColor);
	m_pEditCurveRender->SetSolid(true);
	m_pEditCurveRender->SetSampler(&m_curveSampler);
}

CBaseBrushDraw::~CBaseBrushDraw(void)
{
	SafeRelease(m_pTexture);
	SafeDelete(m_pBrushGridHepler);
	SafeDelete(m_pEditCurveRender);
}

void CBaseBrushDraw::SetEditContext(CEditContext* pContext)
{
	m_pContext = pContext;
}

void CBaseBrushDraw::Init()
{

}

void CBaseBrushDraw::SetBrushColor( const DWORD color )
{
	m_dwBrushColor = color;
	m_pEditCurveRender->SetColor(m_dwBrushColor);
}

void CBaseBrushDraw::SetBrushSmooth(const float smooth)
{
	m_curveSampler.SetSmooth(smooth);
}
	
void CBaseBrushDraw::SetBrushTexture( const string& name )
{
	locale loc = locale::global(locale(""));
	if( FAILED(CGraphic::GetInst()->CreateTexture(L"", name.c_str(), &m_pTexture) ) )
	{
		MessageBox(NULL, name.c_str(), "ÕÒ²»µ½", MB_OK);
	}
	locale::global(loc);
}

void CBaseBrushDraw::GetBrushGridIndexs( brushGridPool& indexs, brushVertexsPool& vertexs )
{
	uint32 uWidth  = m_pContext->GetDataScene()->GetWidthInGrid();
	uint32 uHeight = m_pContext->GetDataScene()->GetHeightInGrid();
	m_pBrushGridHepler->SetSceneSize(uWidth, uHeight);
	m_pBrushGridHepler->GetBrushGrids(m_brushCenter, m_nOuterWidth, m_nOuterHeight, indexs, vertexs);
	size_t size = vertexs.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CVector3f &vertex = vertexs[m];		
// 		int dx0 = int(vertex.x/64.0f);
// 		int dz0 = int(vertex.z/64.0f);
// 		int dx1 = int(vertex.x/64.0f + 0.5f);
// 		int dz1 = int(vertex.z/64.0f + 0.5f);
		//vertex.y = m_pContext->GetEditDataScene()->GetLogicDataMgr()->GetLogicHight(dx0+1, dz0+1);
		vertex.y = m_pContext->GetTerrainHeight(vertex.x+eSRegionSpan, vertex.z+eSRegionSpan);
//		CVector2I gridcoord = CVector2I(int(vertex.x) / eSRegionSpan, int(vertex.z) / eSRegionSpan);

		//if( vertex.y > 0.0f )
		//	DebugOut("x = %d, z = %d, %d, %d, x = %f, z = %f, %f\n", dx0, dz0, dx1, dz1, vertex.x, vertex.z, vertex.y);
	}
}


void CBaseBrushDraw::UpdateControlPoints()
{
	GetBrushGridIndexs(m_vecBrushGridIndices, m_vecBrushVertexs);
	if ( m_lastBrushVertexs == m_vecBrushVertexs )
		return;

	m_lastBrushVertexs = m_vecBrushVertexs;

	int nWidth = m_nOuterWidth  * 2;
	MathVector<CVector3f> temp;
	CVector3f leftbottom, rightbottom, righttop, lefttop;
	leftbottom  = m_vecBrushVertexs[0];
	rightbottom = m_vecBrushVertexs[0+nWidth];
	righttop    = m_vecBrushVertexs[m_vecBrushVertexs.size()-1];
	lefttop     = m_vecBrushVertexs[m_vecBrushVertexs.size()-1-nWidth];

	leftbottom.y += 1.0f;
	rightbottom.y += 1.0f;
	righttop.y += 1.0f;
	leftbottom.y += 1.0f;

	temp.push_back(leftbottom);
	temp.push_back(rightbottom);
	temp.push_back(righttop);
	temp.push_back(lefttop);

	m_curveSampler.SetControlPoint(temp, true);
}

void CBaseBrushDraw::UpdatePointsHeight()
{
	MathVector<CVector3f> &points = m_curveSampler.GetAllPoints();
	size_t size = points.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CVector3f &vertex = points[m];
		vertex.y = m_pContext->GetTerrainHeight(vertex.x+eSRegionSpan, vertex.z+eSRegionSpan) + 1.0f;
		//vertex.y = m_pContext->GetEditDataScene()->GetLogicDataMgr()->GetLogicHight(int(point.x) / eSRegionSpan, int(point.z) / eSRegionSpan) + 1.0f;
		// 		int dx0 = int(vertex.x/eSRegionSpan);
		// 		int dz0 = int(vertex.z/eSRegionSpan);
		// 		int dx1 = int(vertex.x/eSRegionSpan + 0.5f);
		// 		int dz1 = int(vertex.z/eSRegionSpan + 0.5f);
		//vertex.y = m_pContext->GetEditDataScene()->GetLogicDataMgr()->GetLogicHight(dx1, dz0);
	}
}

void CBaseBrushDraw::Draw()
{
	if( m_vecBrushVertexs.empty() || m_vecBrushGridIndices.empty() )
		return;

	if( m_vecBrushVertexs.size() < 4 )
		return;

	UpdatePointsHeight();
	m_pEditCurveRender->DrawCurve();
}

void CBaseBrushDraw::DrawGrid()
{

}