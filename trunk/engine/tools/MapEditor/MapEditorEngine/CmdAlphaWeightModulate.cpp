#include "stdafx.h"
#include "Cmd.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "CMapEditApp.h"
#include "CmdAlphaWeightModulate.h"

//////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <atlconv.h>//¼ÓÈëatlsd.lib
inline void DebugOut(const char *format,...)
{

	static char a[50000];
	va_list pz;
	va_start(pz, format);
	vsprintf(a,format,pz);
	USES_CONVERSION;
	LPCWSTR pw = A2CW(a) ;
	OutputDebugString(a);
}


//////////////////////////////////////////////////////////////////////////

namespace bly
{

void CCmdAlphaWeightModulate::Execute()
{
	CCmd::Execute();

	COperator * pOperator = CMapEditApp::GetInstance()->GetOperator();
	CTerrainMesh * pTerrain = CMapEditApp::GetInstance()->GetTerrainMesh();

	CTerrainMesh::SGrid & hotgrid = pTerrain->GetGrid(m_nGridIndexHot);
	int nVertexIndexHot = hotgrid.dwVertexIndex[0];

	int layHot = m_processParams.nLayerHot;

	for ( std::vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		DebugOut("Vertex Index is: %d", iter->dwVertexIndex);

		if ( m_processParams.eWayType == COperator::EWT_CUT )
			pTerrain->CutVertexAlpha(iter->dwVertexIndex, layHot);

		else
			pTerrain->ModulateAlphaWeight(iter->dwVertexIndex, m_processParams.dwBrushSizeInX, m_processParams.dwBrushSizeInZ, nVertexIndexHot, layHot);
	}

	if ( layHot == 1 )
		pTerrain->SetPreWeightGModeulate( pTerrain->GetWeightGModeulate() );
	if ( layHot == 2 )
		pTerrain->SetPreWeightBModeulate( pTerrain->GetWeightBModeulate() );
}

void CCmdAlphaWeightModulate::UnExecute()
{
	CCmd::UnExecute();

	CTerrainMesh * pTerrain = CMapEditApp::GetInstance()->GetTerrainMesh();

	for ( std::vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		pTerrain->SetAlphaWeightAlpha(iter->dwVertexIndex, iter->weightColor);
	}

	DebugOut("UnExecute Test %d, %d",  m_processParams.dwBrushSizeInX,  m_processParams.dwBrushSizeInZ);

	int width = 0, depth = 0;
	width = m_processParams.dwBrushSizeInX;
	depth = m_processParams.dwBrushSizeInZ;

	if ( width != pTerrain->GetWidth() )
		width *= 2;

	if ( depth != pTerrain->GetDepth() )
		depth *= 2;

	DebugOut("FillBlendTexture %d, %d",  width,  depth);
	pTerrain->FillBlendTexture(m_vecDiffs[0].dwVertexIndex , width, depth ) ;
}

void CCmdAlphaWeightModulate::AddVertex(DWORD dwVertexIndex)
{
	SVertex s;
	s.dwVertexIndex = dwVertexIndex;
	if ( find_if(m_vecDiffs.begin(),m_vecDiffs.end(),IsEqual(s)) != m_vecDiffs.end() )
	{
		return;
	}
	CTerrainMesh::SVertex & vertex = CMapEditApp::GetInstance()->GetTerrainMesh()->GetVertex(dwVertexIndex);
	s.weightColor = vertex.weightColor;
	m_vecDiffs.push_back(s);
	m_dwVertexCnt = m_vecDiffs.size();
}

void CCmdAlphaWeightModulate::SetProcessParams(CToolSetTileTexture::SProcessParams processParams)
{
	m_processParams = processParams;
}

void CCmdAlphaWeightModulate::SetGridIndexHot(int n)
{
	m_nGridIndexHot = n;
}

}