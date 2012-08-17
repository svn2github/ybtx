#include "stdafx.h"
#include "TerrainMesh.h"
#include "VisibleSet.h"
#include "QTNode.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "TerrainMeshGrid.h"

namespace bly
{

CQTNode::CQTNode()
{
	ZeroMemory(m_pChildren,sizeof(CQTNode *) * 4);
	m_pIndices = NULL;
	m_bLeaf = false;
}

CQTNode::~CQTNode()
{
	SAFE_DELETE_ARRAY(m_pIndices);
	
	for ( int i = 0; i < 4; i++ )
	{
		SafeDelete(m_pChildren[i]);
	}
}

void CQTNode::Create(std::vector<int> & vIndices, CTerrainMesh * pMap)
{
	std::vector<int> vCurrentNodeIndices;
	
	int iMapWidth = pMap->GetWidth();
	int iMapHeight= pMap->GetDepth();
	
	m_box.Get(m_vMin,m_vMax);
	
	for ( std::vector<int>::iterator it = vIndices.begin(); it != vIndices.end(); it++ )
	{
		int iGridIndex = *it;
		
		D3DXVECTOR3 pos[4];
		float x = (float)(iGridIndex % iMapWidth);
		float z = (float)(iGridIndex / iMapWidth);
				
		//[Z]     
		//2------3
		//|      |
		//|      |
		//0------1[X]
		
		pos[0] = D3DXVECTOR3(x*GRID_SPACE+GRID_SPACE/2.0f,0,z*GRID_SPACE+GRID_SPACE/2.0f);
		
		// test
		if ( (pos[0].x >= m_vMin.x &&
				pos[0].z  >= m_vMin.z &&
				pos[0].x  <= m_vMax.x &&
				pos[0].z  <= m_vMax.z) )
		{
			vCurrentNodeIndices.push_back(iGridIndex);
		}
	}

	// copy to array
	m_iIndexCnt = vCurrentNodeIndices.size();
	m_pIndices = new int[m_iIndexCnt];
	
	for ( int iIndex = 0; iIndex < m_iIndexCnt; iIndex++ )
	{
		m_pIndices[iIndex] = vCurrentNodeIndices[iIndex];
		
		 SGrid & grid = pMap->GetGrid(vCurrentNodeIndices[iIndex]);
	
		
		m_vecVertexIndices.push_back(grid.dwVertexIndex[0]);
		m_vecVertexIndices.push_back(grid.dwVertexIndex[1]);
		m_vecVertexIndices.push_back(grid.dwVertexIndex[2]);
		m_vecVertexIndices.push_back(grid.dwVertexIndex[3]);
		
		m_vMin.y = min(m_vMin.y, grid.vCenter.y);
		m_vMax.y = max(m_vMax.y, grid.vCenter.y);
		
		if (pMap->GetWater())
		{
			 SGrid & gridWater = pMap->GetWater()->GetGrid(vCurrentNodeIndices[iIndex]);
			m_vMax.y = max(m_vMax.y, gridWater.vCenter.y);
		}
		
		grid.vecQTNodes.push_back(this);
	}
	
	m_box.Set(m_vMin,m_vMax);
	
	if ( vCurrentNodeIndices.size() > MAX_GRID_IN_NODE )
	{
		for ( int n = 0; n < 4; n++ ) 
		{			
			m_pChildren[n] = new CQTNode;
		}
		
		D3DXVECTOR3 vChildDim((m_vMax.x - m_vMin.x) * 0.5f,0.0f,(m_vMax.z - m_vMin.z) * 0.5f);

		//   +-------+
		//   | 3 | 2 |
        //	 |-------|
		//z  | 0 | 1 |
		//|  +-------+
		//|
		//+----x
		
		m_pChildren[0]->GetBox()->Set(m_vMin.x, m_vMin.x + vChildDim.x, FLT_MAX, -FLT_MAX, m_vMin.z, m_vMin.z + vChildDim.z);
		m_pChildren[1]->GetBox()->Set(m_vMin.x + vChildDim.x, m_vMax.x, FLT_MAX, -FLT_MAX, m_vMin.z, m_vMin.z + vChildDim.z);
		m_pChildren[2]->GetBox()->Set(m_vMin.x + vChildDim.x, m_vMax.x, FLT_MAX, -FLT_MAX, m_vMin.z + vChildDim.z, m_vMax.z);
		m_pChildren[3]->GetBox()->Set(m_vMin.x, m_vMin.x + vChildDim.x, FLT_MAX, -FLT_MAX, m_vMin.z + vChildDim.z, m_vMax.z);
				
		for (int n = 0; n < 4; n++ )
			m_pChildren[n]->Create(vCurrentNodeIndices, pMap);
	}
	else // leaf
	{
		m_bLeaf = true;
	}
	
	vCurrentNodeIndices.clear();
}

void CQTNode::TestVisibility(CVisibleSet * pVisibleSet, CFrustum * pFrustum)
{
	if ( !pFrustum->BoxIntersectFrustum(&m_box) ) 
		return;
		
	if ( IsLeaf() )
	{
		int gridIndex = 0;
		for ( int i =0; i < m_iIndexCnt; i++ )
		{
			gridIndex = m_pIndices[i];
			
			pVisibleSet->SetVisible(gridIndex);
		}

		if ( CMapEditApp::GetInstance()->GetOperator()->GetDoesShowLogicHeight() )
		{
			for ( std::vector<int>::iterator iter = m_vecVertexIndices.begin(); iter != m_vecVertexIndices.end(); ++iter )
			{
				pVisibleSet->SetVertexVisible(*iter);
			}
		}

		return;
	}
	
	bool bFlag = true;
		
	for ( int iChild=0; iChild < 4; iChild++ )
	{
		if ( m_pChildren[iChild] && !pFrustum->BoxInFrustum(&m_pChildren[iChild]->m_box) )
		{
			bFlag = false;
			break;
		}
	}
	
	if ( bFlag )
	{
		int gridIndex = 0;

		for ( int i = 0; i < m_iIndexCnt; i++ )
		{
			gridIndex = m_pIndices[i];

			pVisibleSet->SetVisible(gridIndex);
		}

		if ( CMapEditApp::GetInstance()->GetOperator()->GetDoesShowLogicHeight() )
		{
			for ( std::vector<int>::iterator iter = m_vecVertexIndices.begin(); iter != m_vecVertexIndices.end(); ++iter )
			{
				pVisibleSet->SetVertexVisible(*iter);
			}
		}
				
		return;
	}
	
	for ( int iChild = 0; iChild < 4; iChild++ )	
		if ( m_pChildren[iChild] ) m_pChildren[iChild]->TestVisibility(pVisibleSet,pFrustum);
}

bool CQTNode::IsLeaf()
{
	return m_bLeaf;
}

void CQTNode::UpdateBoundingBoxByHeight()
{
	m_vMin.y = FLT_MAX;
	m_vMax.y = FLT_MIN;
	
	for ( int iIndex = 0; iIndex < m_iIndexCnt; iIndex++ )
	{
		 SGrid & grid = CMapEditApp::GetInstance()->GetT()->GetGrid(m_pIndices[iIndex]);

		 SGrid & gridWater = CMapEditApp::GetInstance()->GetWater()->GetGrid(m_pIndices[iIndex]);
		
		m_vMin.y = min(m_vMin.y, grid.vCenter.y);
		m_vMax.y = max(m_vMax.y, grid.vCenter.y);
		m_vMax.y = max(m_vMax.y, gridWater.vCenter.y);
	}
	
	m_box.Set(m_vMin,m_vMax);
}

}