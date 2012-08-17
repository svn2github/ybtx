#include "StdAfx.h"
#include "Brush/RegularGridBrushImp.h"
#include "Brush/BrushGridHepler.h"
#include "CurvePatch/CEditCurveRender.h"
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"
#include "CGraphic.h"

CRegularGridBrushImp::CRegularGridBrushImp(void)
{
	m_curveSampler.SetSmooth(0.0);
}

CRegularGridBrushImp::~CRegularGridBrushImp(void)
{
}

void CRegularGridBrushImp::UpdateControlPoints()
{
	brushGridPool indexs;
	uint32 uWidth  = m_pContext->GetDataScene()->GetWidthInGrid();
	uint32 uHeight = m_pContext->GetDataScene()->GetHeightInGrid();
	m_pBrushGridHepler->SetSceneSize(uWidth, uHeight);
	m_pBrushGridHepler->GetBrushGridIndexsByGrid(m_brushCenter, m_nOuterWidth, m_nOuterHeight, m_nBrushUnit, indexs, m_vecBrushVertexs);
	if( m_vecBrushVertexs.empty() )
		return;

	int   nWidth	= m_nOuterWidth  * 2 / m_nBrushUnit;
	int	  nHeight	= m_nOuterHeight * 2 / m_nBrushUnit;
	MathVector<CVector3f> temp;
	
	CVector3f leftbottom = m_vecBrushVertexs[0];
	temp.push_back(leftbottom);

	CVector3f rightbottom = CVector3f(leftbottom.x + nWidth * eGridSpan, 0.0f, leftbottom.z);
	temp.push_back(rightbottom);

	CVector3f righttop = CVector3f(rightbottom.x, 0.0f, leftbottom.z + nHeight * eGridSpan);
	temp.push_back(righttop);

	CVector3f lefttoptop = CVector3f(leftbottom.x, 0.0f, righttop.z);
	temp.push_back(lefttoptop);

	size_t size = temp.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CVector3f &vertex = temp[m];		
		//vertex.y = m_pContext->GetEditDataScene()->GetLogicDataMgr()->GetLogicHight(dx0, dz0);
		vertex.y = m_pContext->GetTerrainHeight(vertex.x, vertex.z) + 1.0f;
	}

	if( m_vecBrushGridIndices == indexs )
		return;

	m_vecBrushGridIndices = indexs;

	m_curveSampler.SetControlPoint(temp, true);

	UpdatePointsHeight();
}

void CRegularGridBrushImp::UpdatePointsHeight()
{
	MathVector<CVector3f> &points = m_curveSampler.GetAllPoints();
	size_t size = points.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CVector3f &vertex = points[m];
		vertex.y = m_pContext->GetTerrainHeight(vertex.x, vertex.z) + 1.0f;
		//vertex.y = m_pContext->GetEditDataScene()->GetLogicDataMgr()->GetLogicHight(int(point.x) / eSRegionSpan, int(point.z) / eSRegionSpan) + 1.0f;
		// 		int dx0 = int(vertex.x/eSRegionSpan);
		// 		int dz0 = int(vertex.z/eSRegionSpan);
		// 		int dx1 = int(vertex.x/eSRegionSpan + 0.5f);
		// 		int dz1 = int(vertex.z/eSRegionSpan + 0.5f);
		//vertex.y = m_pContext->GetEditDataScene()->GetLogicDataMgr()->GetLogicHight(dx1, dz0);
	}
}

void CRegularGridBrushImp::Draw()
{
	if( m_vecBrushGridIndices.empty() )
		return;
	
	m_pEditCurveRender->DrawCurve();

	if( m_bShowGrid )
		DrawGrid();
}

void CRegularGridBrushImp::DrawGrid()
{
	//////////////////////////////////////////////////////////////////////////
	int nTotalPrim	  = int32(m_vecBrushGridIndices.size());					// 计算面片数目
	int nTotalVertex  = nTotalPrim * 6;						// 计算顶点数目		
	float fUnitSize		= float(eGridSpan);

	// write vertex buffer
	vector<CVector3f> buffers;
	int vertexIndex = 0, bufferIndex = 0;
	DWORD color = 0xff000000;

	for ( int m = 0; m < nTotalPrim; ++m )
	{
		int x = m_vecBrushGridIndices[m].x;
		int z = m_vecBrushGridIndices[m].y;
		float coordx = x * fUnitSize;
		float coordz = z * fUnitSize;
		
		CVector3f vertex0, vertex1, vertex2, vertex3;

		vertex0 = CVector3f(coordx, 0.0f, coordz);
		//vertex0.y = m_pContext->GetTerrainHeight(vertex0.x, vertex0.z) + 1.0f;
		vertex0.y = m_pContext->GetEditDataScene()->GetLogicDataMgr()->GetLogicHight(x, z) + 1.0f;

		vertex1 = CVector3f(coordx, 0.0f, coordz+fUnitSize);
		//vertex1.y = m_pContext->GetTerrainHeight(vertex1.x, vertex1.z) + 1.0f;
		vertex1.y = m_pContext->GetEditDataScene()->GetLogicDataMgr()->GetLogicHight(x, z+1) + 1.0f;

		vertex2 = CVector3f(coordx+fUnitSize, 0.0f, coordz);
		//vertex2.y = m_pContext->GetTerrainHeight(vertex2.x, vertex2.z) + 1.0f;
		vertex2.y = m_pContext->GetEditDataScene()->GetLogicDataMgr()->GetLogicHight(x+1, z) + 1.0f;

		vertex3 = CVector3f(coordx+fUnitSize, 0.0f, coordz+fUnitSize);
		//vertex3.y = m_pContext->GetTerrainHeight(vertex3.x, vertex3.z) + 1.0f;
		vertex3.y = m_pContext->GetEditDataScene()->GetLogicDataMgr()->GetLogicHight(x+1, z+1) + 1.0f;

		buffers.push_back(vertex0);
		buffers.push_back(vertex1);
		buffers.push_back(vertex2);

		buffers.push_back(vertex1);
		buffers.push_back(vertex3);
		buffers.push_back(vertex2);
	}

	// 	CVector3f vertex0 = CVector3f(0.0f, 1.0f, 0.0f);
	// 	CVector3f vertex1 = CVector3f(0.0f, 1.0f, eSRegionSpan);
	// 	CVector3f vertex2 = CVector3f(eSRegionSpan, 1.0f, 0.0f);
	// 	CVector3f vertex3 = CVector3f(eSRegionSpan, 1.0f, eSRegionSpan);
	// 
	// 	buffer[bufferIndex].p = vertex0;
	// 	buffer[bufferIndex++].diffuse = color;
	// 	buffer[bufferIndex].p = vertex1;
	// 	buffer[bufferIndex++].diffuse = color;
	// 	buffer[bufferIndex].p = vertex2;
	// 	buffer[bufferIndex++].diffuse = color;
	// 
	//  	buffer[bufferIndex].p = vertex1;
	//  	buffer[bufferIndex++].diffuse = color;
	//  	buffer[bufferIndex].p = vertex3;
	//  	buffer[bufferIndex++].diffuse = color;
	//  	buffer[bufferIndex].p = vertex2;
	//  	buffer[bufferIndex++].diffuse = color;

	int nBufferNum = (int)buffers.size();
	if( nBufferNum != nTotalVertex )
		return;

	VerColor3D* pVB;
	RenderState* pRS;
	CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, ROS_LIGHTMAT1, NULL, NULL, PT_TRIANGLELIST, 
		nTotalVertex, nTotalPrim*2, VerColor3D::Format, sizeof(VerColor3D), 
		(void**)&pVB, NULL, (void**)&pRS );
	pRS->m_LightEnable = FALSE;
	pRS->m_ZTestEnable = true;
	pRS->m_ZWrite = true;
	pMatrix->Identity();
	pRS->m_FillMode = FILL_WIREFRAME;

	for(int i = 0; i < nTotalVertex; ++i)
	{
		pVB[i].p = buffers[i];
		pVB[i].diffuse = 0xff0000ff;
	}
	//////////////////////////////////////////////////////////////////////////
}