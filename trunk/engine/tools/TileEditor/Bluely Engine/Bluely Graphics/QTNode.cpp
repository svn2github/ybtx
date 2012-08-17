//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "TerrainMesh.h"
#include "VisibleSet.h"
#include "QTNode.h"
#include "Engine.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

CQTNode::CQTNode()
{
	m_pParent = NULL;
	ZeroMemory(m_pChildren,sizeof(CQTNode *) * 4);
	m_pIndices = NULL;
}

CQTNode::~CQTNode()
{
	SAFE_DELETE_ARRAY(m_pIndices);
	for ( int i = 0; i < 4; i++ )
	{
		SAFE_DELETE(m_pChildren[i]);
	}
}

void CQTNode::Create(std::vector<int> & vIndices, CTerrainMesh * pMap)
{
	std::vector<int> vCurrentNodeIndices;

	int iMapWidth = pMap->GetWidth();
	int iMapHeight= pMap->GetDepth();

	D3DXVECTOR3 vMin,vMax;
	
	m_quad.Get(vMin,vMax);

	float fxMin = vMin.x;
	float fxMax = vMax.x;
	float fzMin = vMin.z;
	float fzMax = vMax.z;
	

	for ( std::vector<int>::iterator it = vIndices.begin(); it != vIndices.end(); it++ )
	{
		int iGridIndex = *it;
	//	CTerrainMesh::SGrid * pGrid = pMap->GetGrid(iGridIndex);

		D3DXVECTOR3 pos[4];
		float x = (float)(iGridIndex % iMapWidth);
		float z = (float)(iGridIndex / iMapWidth);
				
		//[Z]     
		//2------3
		//|      |
		//|      |
		//0------1[X]
		
		pos[0] = D3DXVECTOR3(x*GRID_SPACE,0,z*GRID_SPACE);
		pos[1] = D3DXVECTOR3((x+1)*GRID_SPACE,0,z*GRID_SPACE);
		pos[2] = D3DXVECTOR3(x*GRID_SPACE,0,(z+1)*GRID_SPACE);
		pos[3] = D3DXVECTOR3((x+1)*GRID_SPACE,0,(z+1)*GRID_SPACE);

		// test
		if ( 
			(pos[0].x >= fxMin &&
			pos[0].z  >= fzMin &&
			pos[0].x  <= fxMax &&
			pos[0].z  <= fzMax) ||

			(pos[1].x >= fxMin &&
			pos[1].z  >= fzMin &&
			pos[1].x  <=  fxMax &&
			pos[1].z  <=  fzMax) ||

			(pos[2].x >= fxMin &&
			pos[2].z  >= fzMin &&
			pos[2].x  <= fxMax &&
			pos[2].z  <= fzMax) ||

			(pos[3].x >= fxMin &&
			pos[3].z  >= fzMin &&
			pos[3].x  <= fxMax &&
			pos[3].z  <= fzMax)
			)
		{
			vCurrentNodeIndices.push_back(iGridIndex);
		}
	}

	if ( vCurrentNodeIndices.size() > MAX_GRID_IN_NODE )
	{
		for ( int n = 0; n < 4; n++ ) 
		{			
			m_pChildren[n] = new CQTNode;
		}

		float fHalfWidth = (fxMax - fxMin) * 0.5f;
		float fHalfDeepth= (fzMax - fzMin) * 0.5f; 		
		

		//   +-------+
		//   | 3 | 2 |
        //	 |-------|
		//z  | 0 | 1 |
		//|  +-------+
		//|
		//+----x
		
		m_pChildren[0]->GetQuadXZ()->Set(fxMin,fxMin + fHalfWidth,fzMin,fzMin + fHalfDeepth);
		m_pChildren[1]->GetQuadXZ()->Set(fxMin + fHalfWidth,fxMax,fzMin,fzMin + fHalfDeepth);
		m_pChildren[2]->GetQuadXZ()->Set(fxMin + fHalfWidth,fxMax,fzMin + fHalfDeepth,fzMax);
		m_pChildren[3]->GetQuadXZ()->Set(fxMin,fxMin + fHalfWidth,fzMin + fHalfDeepth,fzMax);
		
		for ( int n = 0; n < 4; n++ )
			m_pChildren[n]->Create(vCurrentNodeIndices, pMap);
	}

	// copy to array	
	m_iIndexCnt = vCurrentNodeIndices.size();
	m_pIndices = new int[m_iIndexCnt];
	
	for ( int iIndex = 0; iIndex < m_iIndexCnt; iIndex++ )
	{
		m_pIndices[iIndex] = vCurrentNodeIndices[iIndex];
	}
	
	vCurrentNodeIndices.clear();
}

void CQTNode::TestVisibility(CVisibleSet * pVisibleSet)
{
	rd::CFrustum * pFrustum;

	if ( eg::CEngine::GetInstance()->GetActiveCamera() )
	{
		pFrustum = eg::CEngine::GetInstance()->GetActiveCamera()->GetFrustum();
	}
	else
	{
		return;    			
	}
		
	if ( !pFrustum->QuadXZIntersectFrustum(&m_quad) ) return;
	
	if ( IsLeaf() )
	{	
		for ( int i =0; i < m_iIndexCnt; i++ )
		{
			pVisibleSet->SetVisible(m_pIndices[i]);
		}
		return;
	}
	
	bool bFlag = true;
		
	for ( int iChild=0; iChild < 4; iChild++ )	
	{
		if ( m_pChildren[iChild] && !pFrustum->QuadXZInFrustum(&m_pChildren[iChild]->m_quad) )
		{
			bFlag = false;
			break;
		}
	}
	
	if ( bFlag )
	{
		for ( int i = 0; i < m_iIndexCnt; i++ )
		{
			pVisibleSet->SetVisible(m_pIndices[i]);
		}
		return;
	}
	
	for ( int iChild = 0; iChild < 4; iChild++ )	
		if ( m_pChildren[iChild] ) m_pChildren[iChild]->TestVisibility(pVisibleSet);
}

bool CQTNode::IsLeaf()
{
	if ( !m_pChildren[0] && !m_pChildren[1] && !m_pChildren[2] && !m_pChildren[3] )
		return true;
	else
		return false;
}

}
}