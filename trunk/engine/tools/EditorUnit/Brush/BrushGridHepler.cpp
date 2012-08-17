#include "StdAfx.h"
#include "Brush/BrushGridHepler.h"
#include "CSceneCommon.h"

CBrushGridHepler::CBrushGridHepler(void) : m_uWidthInGrid(0), m_uHeightInGrid(0)
{
}

CBrushGridHepler::~CBrushGridHepler(void)
{
}

void CBrushGridHepler::GetBrushGrids( const CVector3f center, const int nBrushWidth, const int nBrushHeight, vector<CVector2I>& grids, vector<CVector3f>& vertexs )
{
	grids.clear();
	vertexs.clear();

	int   nWidth		= nBrushWidth  * 2;
	int	  nHeight		= nBrushHeight * 2;
	float fUnitSize		= float(eGridSpan);

	int nLimit = eSRegionSpan;
	CVector2I centergridcoord = CVector2I(int(center.x) / eGridSpan, int(center.z) / eGridSpan);

	for( int i = -nBrushHeight; i < nBrushHeight; ++i )
	{
		for( int j = -nBrushWidth; j < nBrushWidth; ++j )
		{
			CVector2I gridcoord = CVector2I(centergridcoord.x + j, centergridcoord.y + i);
			if( gridcoord.x < 0 || gridcoord.x > m_uWidthInGrid )
				continue;
			if( gridcoord.y < 0 || gridcoord.y > m_uHeightInGrid )
				continue;

			grids.push_back(gridcoord);

		}
	}

	for( int i = -nBrushHeight; i < nBrushHeight+1; ++i )
	{
		float startZ = center.z + i * fUnitSize;
		for( int j = -nBrushWidth; j < nBrushWidth+1; ++j )
		{
			CVector3f vertex = center;
			vertex.x = center.x + j * fUnitSize;
			vertex.z = startZ;

			vertexs.push_back(vertex);
		}
	}
}

void CBrushGridHepler::GetBrushGridIndexsByGrid( const CVector3f center, const int nBrushWidth, const int nBrushHeight, const int unit, vector<CVector2I>& grids, vector<CVector3f>& vertexs )
{
	grids.clear();
	vertexs.clear();

	int   nWidth		= nBrushWidth  * 2 / unit;
	int	  nHeight		= nBrushHeight * 2 / unit;
	float fUnitSize		= float(eGridSpan);

	int nLimit = eSRegionSpan;
	CVector2I centergridcoord = CVector2I(int(center.x) / eSGridSpan, int(center.z) / eSGridSpan);
	if( centergridcoord.x < 0 || centergridcoord.y < 0 || centergridcoord.x > eSRegionSpan || centergridcoord.y > eSRegionSpan )
		return;

	for ( int i = 0; i < nHeight; ++i )
	{
		for ( int j = 0; j < nWidth; ++j )
		{
			CVector2I gridcoord = CVector2I(centergridcoord.x + j, centergridcoord.y + i);
			if( gridcoord.x < 0 || gridcoord.x > m_uWidthInGrid )
				continue;
			if( gridcoord.y < 0 || gridcoord.y > m_uHeightInGrid )
				continue;

			grids.push_back(gridcoord);
		}
	}

	CVector3f leftbottom = CVector3f(centergridcoord.x * fUnitSize, 0.0f, centergridcoord.y * fUnitSize);
	for ( int i = 0; i <= nHeight; ++i )
	{
		for( int j = 0; j <= nWidth; ++j )
		{
			CVector3f vertex = CVector3f(leftbottom.x + j * fUnitSize, 0.0f, leftbottom.z + i * fUnitSize);
			vertexs.push_back(vertex);
		}
	}
//	vertexs.push_back(leftbottom);
	
// 	CVector3f rightbottom = CVector3f(leftbottom.x + nWidth * fUnitSize, 0.0f, leftbottom.z);
// 	vertexs.push_back(rightbottom);
// 
// 	CVector3f righttop = CVector3f(rightbottom.x, 0.0f, leftbottom.z + nHeight * fUnitSize);
// 	vertexs.push_back(righttop);
// 
// 	CVector3f lefttoptop = CVector3f(leftbottom.x, 0.0f, righttop.z);
// 	vertexs.push_back(lefttoptop);
}