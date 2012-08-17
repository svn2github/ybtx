#include "stdafx.h"
#include "TerrainAlgorithm.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "TerrainMeshVertex.h"
#include "TerrainMeshGrid.h"
#include "TerrainMeshRegion.h"
#include "ToolSetTileTexture.h"
#include "ToolSetTileHeight.h"
#include "ToolVertex.h"

namespace sqr
{
	const float VERTEX_HEIGHT_LIMIT = 32767.0f;
	set<int> g_HotRgionUnits;

void CTerrainAlgorithm::CalTerrainNormal()
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

	DWORD dwVertexCnt = pTerrain->GetVertexCount();
	for (DWORD i = 0; i < dwVertexCnt; ++i )
	{
		this->CalculateVertexNormal(i);
	}
}

void CTerrainAlgorithm::CalculateVertexNormal( DWORD dwVertexIndex )
{
	// add light effect
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	DWORD dwWidth = pTerrain->GetWidth();
	DWORD dwDepth = pTerrain->GetDepth();

	// skip all margin vertex
	if ( ( 0 <= dwVertexIndex && dwVertexIndex < dwWidth + 1 ) ||
		!(dwVertexIndex%(dwWidth+1)) || !((dwVertexIndex+1)%(dwWidth+1)) ||
		( dwDepth * (dwWidth+1) <= dwVertexIndex && dwVertexIndex < (dwDepth+1)*(dwWidth+1) ) )
	{
		return;
	}
	// Slope法线生成算法
	const int NEIGHBOR_NUM = 4;	// 左,上,右,下
	int neighborIndex[NEIGHBOR_NUM] =
	{
		dwVertexIndex - 1,
		dwVertexIndex + (dwWidth + 1),
		dwVertexIndex + 1,
		dwVertexIndex - (dwWidth + 1)
	};
	CVector3f neighborVertex[NEIGHBOR_NUM];
	for (int i = 0; i < NEIGHBOR_NUM; i++)
	{
		neighborVertex[i] = CMapEditApp::GetInst()->GetEditingMesh()->GetVertex(neighborIndex[i]).vPosition;
	}
	CVector3f normal(neighborVertex[0].y - neighborVertex[2].y, GRID_SPACE * 2.0f, neighborVertex[3].y - neighborVertex[1].y);
	normal.normalize();

	pTerrain->GetVertex(dwVertexIndex).vNormal = normal;
}

void CTerrainAlgorithm::ReCalculateGridsHeight( DWORD dwVertexIndex )
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

	//每次重新计算高度时将拥有最高高度的网格索引设置为-1，也就是将其无效
	SVertex & vertex = pTerrain->GetVertex(dwVertexIndex);

	for (size_t i = 0; i < vertex.usbUsedCnt; i++ )
	{
		SGrid & grid = pTerrain->GetGrid(vertex.dwGridsBelongto[i]);

		SVertex & vertex0 = pTerrain->GetVertex(grid.dwVertexIndex[0]);
		SVertex & vertex1 = pTerrain->GetVertex(grid.dwVertexIndex[1]);
		SVertex & vertex2 = pTerrain->GetVertex(grid.dwVertexIndex[2]);
		SVertex & vertex3 = pTerrain->GetVertex(grid.dwVertexIndex[3]);

		grid.vCenter.y = (vertex0.vPosition.y + vertex1.vPosition.y + vertex2.vPosition.y + vertex3.vPosition.y) * 0.25f;
	}
}

void CTerrainAlgorithm::GetMapCoordinate( const int nGridIndex, int& nX, int& nZ )
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	if( pTerrain )
	{
		int nTerrainWidth = pTerrain->GetWidth();

		nX = nGridIndex % nTerrainWidth; // 列
		nZ = nGridIndex / nTerrainWidth; // 行
	}
}

CTerrainAlgorithm::CTerrainAlgorithm()
{
}

CTerrainAlgorithm::~CTerrainAlgorithm()
{
}

void CTerrainAlgorithm::AssertHeight(SVertex* pVertex)
{
	if ( pVertex->vPosition.y > VERTEX_HEIGHT_LIMIT )
	{
		pVertex->vPosition.y = VERTEX_HEIGHT_LIMIT;
	}
}

void CTerrainAlgorithm::SetVertexDiffuseRGB(int nVertexIndex, DWORD dwColor)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex & vertex = pTerrainMesh->GetVertex(nVertexIndex);
	vertex.oColor  = (vertex.oColor & 0xff000000) | dwColor;
}

void CTerrainAlgorithm::SetVertexSpecularRGB(int nVertexIndex, DWORD dwColor)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex & vertex = pTerrainMesh->GetVertex(nVertexIndex);
	vertex.sColor  = (vertex.sColor & 0xff000000) | dwColor;
}

void CTerrainAlgorithm::ModulateVertexDiffuseRGB(int nVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, BYTE byRed, BYTE byGreen, BYTE byBlue, int nVertexIndexHot)
{
	CTerrainMesh* pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	float fVertexStrength = CToolVertex::GetInst()->GetVertexStrength();
	SVertex& vertex = pTerrainMesh->GetVertex(nVertexIndex);

	SVertex& vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);

	float distanceZ = vertex.vPosition.z - ( vertexHot.vPosition.z ) + 0.01f;
	float distanceX = vertex.vPosition.x - ( vertexHot.vPosition.x ) + 0.01f;

	float fA,fB;

	fA = dwBrushWidth*GRID_SPACE;
	fB = dwBrushDepth*GRID_SPACE;

	float tg = distanceZ/distanceX;

	float x2 = fA*fA*fB*fB/(fB*fB+fA*fA*tg*tg);

	float y2 = x2*tg*tg;

	float ratio = (distanceX*distanceX+distanceZ*distanceZ)/(x2+y2);

	ratio = sqrtf(ratio);

	if ( ratio > 1.0f ) ratio = 1.0f;

	DWORD dwColorSrc = vertex.oColor;

	BYTE byRedSrc	= BYTE(dwColorSrc >> 16);
	BYTE byGreenSrc = BYTE(dwColorSrc >> 8);
	BYTE byBlueSrc	= BYTE(dwColorSrc);

	//newColor = (1.0f - ratio) * 选择的顶点颜色 + ratio * oldVertexColor
	byRed	= BYTE((1.0f - ratio)*byRed*fVertexStrength + ratio*byRedSrc);
	byGreen = BYTE((1.0f - ratio)*byGreen*fVertexStrength + ratio*byGreenSrc);
	byBlue	= BYTE((1.0f - ratio)*byBlue*fVertexStrength + ratio*byBlueSrc);

	//cColor记录烘焙后的值
	vertex.oColor  = (vertex.oColor & 0xff000000) | byRed << 16 | byGreen << 8 | byBlue;
}

void CTerrainAlgorithm::ModulateVertexSpecularRGB(int nVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, BYTE byRed, BYTE byGreen, BYTE byBlue, int nVertexIndexHot)
{
	CTerrainMesh* pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	float fVertexStrength = CToolVertex::GetInst()->GetVertexStrength();
	SVertex& vertex = pTerrainMesh->GetVertex(nVertexIndex);

	SVertex& vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);

	float distanceZ = vertex.vPosition.z - ( vertexHot.vPosition.z ) + 0.01f;
	float distanceX = vertex.vPosition.x - ( vertexHot.vPosition.x ) + 0.01f;

	float fA,fB;

	fA = dwBrushWidth*GRID_SPACE;
	fB = dwBrushDepth*GRID_SPACE;

	float tg = distanceZ/distanceX;

	float x2 = fA*fA*fB*fB/(fB*fB+fA*fA*tg*tg);

	float y2 = x2*tg*tg;

	float ratio = (distanceX*distanceX+distanceZ*distanceZ)/(x2+y2);

	ratio = sqrtf(ratio);

	if ( ratio > 1.0f ) 
		ratio = 1.0f;

	DWORD dwColorSrc = vertex.sColor;

	BYTE byRedSrc	= BYTE(dwColorSrc >> 16);
	BYTE byGreenSrc = BYTE(dwColorSrc >> 8);
	BYTE byBlueSrc	= BYTE(dwColorSrc);

	//newColor = (1.0f - ratio) * 选择的顶点颜色 + ratio * oldVertexColor
	byRed	= BYTE((1.0f - ratio)*byRed*fVertexStrength + ratio*byRedSrc);
	byGreen = BYTE((1.0f - ratio)*byGreen*fVertexStrength + ratio*byGreenSrc);
	byBlue	= BYTE((1.0f - ratio)*byBlue*fVertexStrength + ratio*byBlueSrc);

	//cColor记录烘焙后的值
	vertex.sColor  = (vertex.sColor & 0xff000000) | byRed << 16 | byGreen << 8 | byBlue;
}

void CTerrainAlgorithm::CutVertexAlpha(DWORD dwVertexIndex)
{
	CTerrainMesh* pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex& vertex = pTerrainMesh->GetVertex(dwVertexIndex);
	vertex.oColor &= 0x00ffffff;
}

void CTerrainAlgorithm::IncVertexAlphaX(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot)
{
	CTerrainMesh* pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex& vertex		= pTerrainMesh->GetVertex(dwVertexIndex);
	SVertex& vertexHot	= pTerrainMesh->GetVertex(nVertexIndexHot);
	float distance		= abs(vertex.vPosition.x - vertexHot.vPosition.x);
	float radio			= 0.0f;

	switch ( eAlphaType )
	{
	case sqr::EAT_LINEAR:
		{
			radio = 1.0f - distance/(GRID_SPACE * dwBrushSize);
			break;
		}

	default:
		break;
	}

	byte  alpha			= byte(vertex.oColor >> 24);
	float fAlphaStrength = CToolSetTileTexture::GetInst()->GetAlphaStrength();

	if ( static_cast<byte>(alpha + radio*fAlphaStrength) < alpha )
	{
		alpha = 0xff;
	}
	else
	{
		alpha += static_cast<byte>(radio*fAlphaStrength);
	}

	vertex.oColor |= 0xff000000;
	vertex.oColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

void CTerrainAlgorithm::IncVertexAlphaZ(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot)
{
	CTerrainMesh* pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex& vertex		= pTerrainMesh->GetVertex(dwVertexIndex);
	SVertex& vertexHot	= pTerrainMesh->GetVertex(nVertexIndexHot);
	float	 distance	= abs(vertex.vPosition.z - vertexHot.vPosition.z);
	float	 radio		= 0.0f;

	switch ( eAlphaType )
	{
	case sqr::EAT_LINEAR:
		{
			radio = 1.0f - distance/(GRID_SPACE * dwBrushSize);
			break;
		}

	default:
		break;
	}

	byte	 alpha		= byte(vertex.oColor >> 24);
	float fAlphaStrength = CToolSetTileTexture::GetInst()->GetAlphaStrength();

	if ( static_cast<byte>(alpha + radio*fAlphaStrength) < alpha )
	{
		alpha = 0xff;
	}
	else
	{
		alpha += static_cast<byte>(radio*fAlphaStrength);
	}

	vertex.oColor |= 0xff000000;
	vertex.oColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

void CTerrainAlgorithm::IncVertexAlphaB(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot)
{
	CTerrainMesh* pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex& vertex = pTerrainMesh->GetVertex(dwVertexIndex);
	SVertex& vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);

	float distanceZ = fabs(vertex.vPosition.z - ( vertexHot.vPosition.z ));
	float distanceX = fabs(vertex.vPosition.x - ( vertexHot.vPosition.x ));
	float distanceY = fabs(vertex.vPosition.y - ( vertexHot.vPosition.y ));

	float radio = 0.0f, radio1 = 0.0f, radio2 = 0.0f;

	switch ( eAlphaType )
	{
	case sqr::EAT_LINEAR:
		{
			if ( dwBrushWidth > dwBrushDepth )
			{
				radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushDepth);
				radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushDepth);
				radio = radio1 > radio2 ? radio2 : radio1;
			}
			else
			{
				radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushWidth);
				radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushWidth);
				radio = radio1 > radio2 ? radio2 : radio1;
			}
			break;
		}

	case sqr::EAT_SPHERE:
		{
			float fDistance = sqrt(distanceX * distanceX + distanceZ * distanceZ + distanceY * distanceY);
			float fMaxDis   = sqrt(dwBrushWidth * GRID_SPACE * dwBrushWidth * GRID_SPACE + dwBrushDepth * GRID_SPACE * dwBrushDepth * GRID_SPACE + distanceY * distanceY);
			radio = 1 - fDistance / fMaxDis;
			if( radio < 0.0f )
				radio = 0.0f;

			break;
		}

	case sqr::EAT_CROSS:
		{
			float fDistance = sqrt(distanceX * distanceX + distanceZ * distanceZ + distanceY * distanceY);
			float fMaxDis   = sqrt(dwBrushWidth * GRID_SPACE * dwBrushWidth * GRID_SPACE + dwBrushDepth * GRID_SPACE * dwBrushDepth * GRID_SPACE + distanceY * distanceY);
			int	  nDistance = int(fDistance + 0.5f);
			int	  nDela     = nDistance % (int)GRID_SPACE;
			if( nDela == 0 )
			{
				radio = 1 - fDistance / fMaxDis;
				if( radio < 0.0f )
					radio = 0.0f;
			}

			break;
		}

	case sqr::EAT_MI:
		{
			float fDistance = sqrt(distanceX * distanceX + distanceZ * distanceZ + distanceY * distanceY);
			float fMaxDis   = sqrt(dwBrushWidth * GRID_SPACE * dwBrushWidth * GRID_SPACE + dwBrushDepth * GRID_SPACE * dwBrushDepth * GRID_SPACE + distanceY * distanceY);
			int   nDistance = int(fDistance + 0.5f);
			int   nDela     = nDistance % (int)GRID_SPACE;
			if( nDela != 0 )
			{
				radio = 1 - fDistance / fMaxDis;
				if( radio < 0.0f )
					radio = 0.0f;
			}

			if( distanceX == 0 && distanceZ == 0 )
				radio = 1.0f;

			break;
		}

	default:
		break;
	}

	byte alpha = byte(vertex.oColor >> 24);
	float AlphaStrength = CToolSetTileTexture::GetInst()->GetAlphaStrength();

	if ( static_cast<byte>(alpha + radio*AlphaStrength) < alpha )
	{
		alpha = 0xff;
	}
	else
	{
		alpha += static_cast<byte>(radio*AlphaStrength);
	}

	vertex.oColor |= 0xff000000;
	vertex.oColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

void CTerrainAlgorithm::DecVertexAlphaX(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot)
{
	CTerrainMesh* pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex& vertex		= pTerrainMesh->GetVertex(dwVertexIndex);
	SVertex& vertexHot	= pTerrainMesh->GetVertex(nVertexIndexHot);
	float	 distance	= abs(vertex.vPosition.x - vertexHot.vPosition.x);
	float	 radio		= 0.0f;

	switch ( eAlphaType )
	{
	case sqr::EAT_LINEAR:
		{
			radio		= 1.0f - distance/(GRID_SPACE * dwBrushSize);
			break;
		}

	default:
		break;
	}

	byte	 alpha		= byte(vertex.oColor >> 24);
	float    AlphaStrength = CToolSetTileTexture::GetInst()->GetAlphaStrength();

	if ( static_cast<byte>(alpha - radio*AlphaStrength) > alpha )
	{
		alpha = 0x00;
	}
	else
	{
		alpha -= static_cast<byte>(radio*AlphaStrength);
	}

	vertex.oColor |= 0xff000000;
	vertex.oColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

void CTerrainAlgorithm::DecVertexAlphaZ(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex & vertex    = pTerrainMesh->GetVertex(dwVertexIndex);
	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);
	float distance      = abs(vertex.vPosition.z - vertexHot.vPosition.z);
	float radio		    = 0.0f;

	switch ( eAlphaType )
	{
	case sqr::EAT_LINEAR:
		{
			radio = 1.0f - distance/(GRID_SPACE * dwBrushSize);
			break;
		}

	default:
		break;
	}

	byte  alpha = byte(vertex.oColor >> 24);
	float AlphaStrength = CToolSetTileTexture::GetInst()->GetAlphaStrength();

	if ( static_cast<byte>(alpha - radio*AlphaStrength) > alpha )
	{
		alpha = 0x00;
	}
	else
	{
		alpha -= static_cast<byte>(radio*AlphaStrength);
	}
	vertex.oColor |= 0xff000000;
	vertex.oColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

void CTerrainAlgorithm::DecVertexAlphaB(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex & vertex = pTerrainMesh->GetVertex(dwVertexIndex);
	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);

	float distanceZ = fabs(vertex.vPosition.z - ( vertexHot.vPosition.z ));
	float distanceX = fabs(vertex.vPosition.x - ( vertexHot.vPosition.x ));
	float distanceY = fabs(vertex.vPosition.y - ( vertexHot.vPosition.y ));

	float radio = 0.0f, radio1 = 0.0f, radio2 = 0.0f;

	switch ( eAlphaType )
	{
	case sqr::EAT_LINEAR:
		{
			if ( dwBrushWidth > dwBrushDepth )
			{
				radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushDepth);
				radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushDepth);
				radio = radio1 > radio2 ? radio2 : radio1;
			}
			else
			{
				radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushWidth);
				radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushWidth);
				radio = radio1 > radio2 ? radio2 : radio1;
			}

			break;
		}

	case sqr::EAT_SPHERE:
		{
			float fDistance = sqrt(distanceX * distanceX + distanceZ * distanceZ + distanceY * distanceY);
			float fMaxDis   = sqrt(dwBrushWidth * GRID_SPACE * dwBrushWidth * GRID_SPACE + dwBrushDepth * GRID_SPACE * dwBrushDepth * GRID_SPACE + distanceY * distanceY);
			radio = 1 - fDistance / fMaxDis;
			if( radio < 0.0f )
				radio = 0.0f;

			break;
		}

	case sqr::EAT_CROSS:
		{
			float fDistance = sqrt(distanceX * distanceX + distanceZ * distanceZ + distanceY * distanceY);
			float fMaxDis   = sqrt(dwBrushWidth * GRID_SPACE * dwBrushWidth * GRID_SPACE + dwBrushDepth * GRID_SPACE * dwBrushDepth * GRID_SPACE + distanceY * distanceY);
			int	  nDistance = int(fDistance + 0.5f);
			int	  nDela     = nDistance % (int)GRID_SPACE;
			if( nDela == 0 )
			{
				radio = 1 - fDistance / fMaxDis;
				if( radio < 0.0f )
					radio = 0.0f;
			}

			break;
		}

	case sqr::EAT_MI:
		{
			break;
		}

	default:
		break;
	}

	byte alpha = byte(vertex.oColor >> 24);
	float AlphaStrength = CToolSetTileTexture::GetInst()->GetAlphaStrength();

	if ( static_cast<byte>(alpha - radio*AlphaStrength) > alpha )
	{
		alpha = 0x00;
	}
	else
	{
		alpha -= static_cast<byte>(radio*AlphaStrength);
	}

	vertex.oColor |= 0xff000000;
	vertex.oColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

void CTerrainAlgorithm::SetVertexAlpha(DWORD dwVertexIndex, D3DCOLOR cColor)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex & vertex = pTerrainMesh->GetVertex(dwVertexIndex);
	vertex.oColor = cColor;
}

void CTerrainAlgorithm::CutVertexHeight(DWORD dwVertexIndex, float fMinY)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex* pVertex = &pTerrainMesh->GetVertex(dwVertexIndex);
	pVertex->vPosition.y = fMinY;
	this->AssertHeight(pVertex);
}

void CTerrainAlgorithm::SmoothVertexHeight(int nVertexIndex)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	//点数
	int num = 0;
	float fTotal = 0.0f;
	//九宫格遍历法
	int nTerrainWidth = pTerrainMesh->GetWidth();
	for (int i = - nTerrainWidth; i <= nTerrainWidth; i += nTerrainWidth)
	{
		for (int j = -1; j <= 1; j++)
		{
			int index = nVertexIndex + i + j;
			if (pTerrainMesh->IsValidVertexIndex(index) && pTerrainMesh->IsPhysicallyNearBasedOnVertex(index, nVertexIndex))
			{
				SVertex& vertex = pTerrainMesh->GetVertex(index);
				fTotal += vertex.vPosition.y;
				++num;
			}
		}
	}
	SVertex* pVertexModify = &pTerrainMesh->GetVertex(nVertexIndex);

	float fHeightStrength = CToolSetTileHeight::GetInst()->GetHeightStrength();
	//根据强度在原值和平均值之间取个插值
	pVertexModify->vPosition.y = pVertexModify->vPosition.y + (fTotal / num - pVertexModify->vPosition.y) * fHeightStrength / 32;

	this->AssertHeight(pVertexModify);
}

void CTerrainAlgorithm::IncLinearVertexHeightX(DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;


	SVertex* pVertex = &pTerrainMesh->GetVertex(dwVertexIndex);
	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);
	float distance = abs(pVertex->vPosition.x - vertexHot.vPosition.x);
	float radio = 1.0f - distance/(GRID_SPACE * dwBrushSize);
	float fHeightStrength = CToolSetTileHeight::GetInst()->GetHeightStrength();

	pVertex->vPosition.y += radio*fHeightStrength;
	this->AssertHeight(pVertex);
}

void CTerrainAlgorithm::IncLinearVertexHeightZ(DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex* pVertex = &pTerrainMesh->GetVertex(dwVertexIndex);
	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);
	float distance = abs(pVertex->vPosition.z - vertexHot.vPosition.z);
	float radio = 1.0f - distance/(GRID_SPACE * dwBrushSize);
	float fHeightStrength = CToolSetTileHeight::GetInst()->GetHeightStrength();

	pVertex->vPosition.y += radio*fHeightStrength;
	this->AssertHeight(pVertex);
}

void CTerrainAlgorithm::IncLinearVertexHeightB(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth,  int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex* pVertex = &pTerrainMesh->GetVertex(dwVertexIndex);
	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);
	float fHeightStrength = CToolSetTileHeight::GetInst()->GetHeightStrength();

	float distanceZ = fabs(pVertex->vPosition.z - ( vertexHot.vPosition.z ));
	float distanceX = fabs(pVertex->vPosition.x - ( vertexHot.vPosition.x ));

	float radio, radio1, radio2;

	if ( dwBrushWidth > dwBrushDepth )
	{
		radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushDepth);
		radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushDepth);
		radio = radio1 > radio2 ? radio2 : radio1;
	}
	else
	{
		radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushWidth);
		radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushWidth);
		radio = radio1 > radio2 ? radio2 : radio1;
	}

	pVertex->vPosition.y += fHeightStrength*radio;

	this->AssertHeight(pVertex);
}

void CTerrainAlgorithm::IncGaussVertexHeight(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth,  int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex* pVertex = &pTerrainMesh->GetVertex(dwVertexIndex);

	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);

	float distanceZ = pVertex->vPosition.z - ( vertexHot.vPosition.z + 0.5f );
	float distanceX = pVertex->vPosition.x - ( vertexHot.vPosition.x + 0.5f );

	float a,b,k,r;
	a = dwBrushWidth * GRID_SPACE;
	b = dwBrushDepth * GRID_SPACE;

	k = distanceX/distanceZ;
	r = ((a*a*b*b)/(k*k*b*b+a*a))*(1+k*k);
	r = sqrtf((distanceX*distanceX+distanceZ*distanceZ)/r);

	float fHeightStrength = CToolSetTileHeight::GetInst()->GetHeightStrength();
	pVertex->vPosition.y += fHeightStrength*250*expf(-r*r*3)/100;

	this->AssertHeight(pVertex);
}

void CTerrainAlgorithm::DecLinearVertexHeightX(DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex & vertex = pTerrainMesh->GetVertex(dwVertexIndex);
	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);
	float distance = abs(vertex.vPosition.x - vertexHot.vPosition.x);
	float radio = 1.0f - distance/(GRID_SPACE * dwBrushSize);
	float fHeightStrength = CToolSetTileHeight::GetInst()->GetHeightStrength();

	vertex.vPosition.y -= radio*fHeightStrength;
}

void CTerrainAlgorithm::DecLinearVertexHeightB(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex & vertex = pTerrainMesh->GetVertex(dwVertexIndex);

	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);

	float distanceZ = fabs(vertex.vPosition.z - ( vertexHot.vPosition.z ));
	float distanceX = fabs(vertex.vPosition.x - ( vertexHot.vPosition.x ));

	float radio, radio1, radio2;

	if ( dwBrushWidth > dwBrushDepth )
	{
		radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushDepth);
		radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushDepth);
		radio = radio1 > radio2 ? radio2 : radio1;
	}
	else
	{
		radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushWidth);
		radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushWidth);
		radio = radio1 > radio2 ? radio2 : radio1;
	}

	float fHeightStrength = CToolSetTileHeight::GetInst()->GetHeightStrength();
	vertex.vPosition.y -= fHeightStrength*radio;
}

void CTerrainAlgorithm::DecLinearVertexHeightZ(DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex & vertex = pTerrainMesh->GetVertex(dwVertexIndex);
	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);
	float distance = abs(vertex.vPosition.z - vertexHot.vPosition.z);
	float radio = 1.0f - distance/(GRID_SPACE * dwBrushSize);
	float fHeightStrength = CToolSetTileHeight::GetInst()->GetHeightStrength();

	vertex.vPosition.y -= radio*fHeightStrength;
}

void CTerrainAlgorithm::DecGaussVertexHeight(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex & vertex = pTerrainMesh->GetVertex(dwVertexIndex);

	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);

	float distanceZ = vertex.vPosition.z - ( vertexHot.vPosition.z + 0.5f );
	float distanceX = vertex.vPosition.x - ( vertexHot.vPosition.x + 0.5f );

	float a,b,k,r;
	a = dwBrushWidth * GRID_SPACE;
	b = dwBrushDepth * GRID_SPACE;

	k = distanceX/distanceZ;
	r = ((a*a*b*b)/(k*k*b*b+a*a))*(1+k*k);
	r = sqrtf((distanceX*distanceX+distanceZ*distanceZ)/r);

	float fHeightStrength = CToolSetTileHeight::GetInst()->GetHeightStrength();
	vertex.vPosition.y -= fHeightStrength*250*expf(-r*r*3)/100;
}

void CTerrainAlgorithm::TemplateIncCutVertexHeightByGauss(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex* pVvertex   = &pTerrainMesh->GetVertex(dwVertexIndex);

	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);

	float distanceZ = pVvertex->vPosition.z - ( vertexHot.vPosition.z );
	float distanceX = pVvertex->vPosition.x - ( vertexHot.vPosition.x );

	distanceX += 0.001f;
	distanceZ += 0.001f;

	float a,b,k,r;
	a = dwBrushWidth * GRID_SPACE;
	b = dwBrushDepth * GRID_SPACE;

	k = distanceX/distanceZ;
	r = ((a*a*b*b)/(k*k*b*b+a*a))*(1+k*k);
	r = sqrtf((distanceX*distanceX+distanceZ*distanceZ)/r);

	int nLiftPower  = CToolSetTileHeight::GetInst()->GetLiftPower();
	float fCutRadio = CToolSetTileHeight::GetInst()->GetCutRadio();

	float f = nLiftPower*GRID_SPACE*250*expf(-r*r*3)/100;

	float fLimit = fCutRadio*nLiftPower*GRID_SPACE;

	if ( f > fLimit )
		pVvertex->vPosition.y += fLimit;
	else
		pVvertex->vPosition.y += f;

	this->AssertHeight(pVvertex);
}

void CTerrainAlgorithm::TemplateIncCutVertexHeightByLinear(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex* pVertex    = &pTerrainMesh->GetVertex(dwVertexIndex);

	SVertex & vertexHot = pTerrainMesh->GetVertex(nVertexIndexHot);

	float distanceZ = fabs(pVertex->vPosition.z - ( vertexHot.vPosition.z ));
	float distanceX = fabs(pVertex->vPosition.x - ( vertexHot.vPosition.x ));

	float radio, radio1, radio2;

	if ( dwBrushWidth > dwBrushDepth )
	{
		radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushDepth);
		radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushDepth);
		radio = radio1 > radio2 ? radio2 : radio1;
	}
	else
	{
		radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushWidth);
		radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushWidth);
		radio = radio1 > radio2 ? radio2 : radio1;
	}

	int nLiftPower  = CToolSetTileHeight::GetInst()->GetLiftPower();
	float fCutRadio = CToolSetTileHeight::GetInst()->GetCutRadio();

	if ( radio > fCutRadio )
		pVertex->vPosition.y += nLiftPower*GRID_SPACE*fCutRadio ;
	else
		pVertex->vPosition.y += nLiftPower*GRID_SPACE*radio;

	this->AssertHeight(pVertex);
}

void CTerrainAlgorithm::SetVertexHeight(DWORD dwVertexIndex, float fHeight)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SVertex* pVertex = &pTerrainMesh->GetVertex(dwVertexIndex);
	pVertex->vPosition.y = fHeight;

	this->AssertHeight(pVertex);
}

void CTerrainAlgorithm::SetTileBlock(DWORD dwGridIndex, int nBlockType)
{
	CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

	SGrid & grid = pTerrainMesh->GetGrid(dwGridIndex);
	grid.nBlockType = nBlockType;
}

CVector2f CTerrainAlgorithm::GetProjectCoord( CVector3f& pos, CVector3f& normal )
{
	if (fabs(normal.x) > fabs(normal.z))
	{
		return CVector2f(pos.z, pos.y);
	}
	else
	{
		return CVector2f(pos.x, pos.y);
	}
}

//计算grid的uv值
void CTerrainAlgorithm::ReCalculateGridsLayerUV(const int nGridIndex, const int nLayer)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SGrid &grid = pTerrainMesh->GetGrid(nGridIndex);
	const int GRID_VERTEX_NUM = 4;
	SVertex vertex[GRID_VERTEX_NUM];
	for (int i = 0; i < GRID_VERTEX_NUM; i++)
	{
		vertex[i] = pTerrainMesh->GetVertex(grid.dwVertexIndex[i]);
	}
	CVector3f dirV01 = vertex[1].vPosition - vertex[0].vPosition;
	CVector3f dirV02 = vertex[2].vPosition - vertex[0].vPosition;
	CVector3f dirV03 = vertex[3].vPosition - vertex[0].vPosition;
	CVector3f normal0, normal1;	//两个三角形的法线
	normal0 = dirV01.Cross(dirV02);
	normal1 = dirV02.Cross(dirV03);
	CVector3f gridNormal = normal0 + normal1;
	float CliffStrength  = CToolSetTileTexture::GetInst()->GetCliffStrength();
	float scale			 = CliffStrength * 0.003125f;
	float mix			 = 999.0f;

	for (int i = 0; i < GRID_VERTEX_NUM; i++)
	{
		CVector2f uv = this->GetProjectCoord(vertex[i].vPosition, gridNormal) * scale;
		if (mix>uv.x)
			mix = uv.x;
		grid.uv[nLayer][i] = uv;

	}

	float temp = mix - fmod(mix,1.0f);
	for (int i = 0; i < GRID_VERTEX_NUM; i++)
		grid.uv[nLayer][i].x =  grid.uv[nLayer][i].x - temp;

	grid.bCliffLayerOne = true;
}

void CTerrainAlgorithm::ReBackGridLayerUV(const int nGridIndex, const int nLayer)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	SGrid &grid = pTerrainMesh->GetGrid(nGridIndex);
	memcpy(grid.uv[nLayer], grid.uvOld[0], sizeof(grid.uv[nLayer]));
	grid.bCliffLayerOne = false;
}

float CTerrainAlgorithm::GetApex()
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return 0.0f;

	float fApex = 0.0f;

	for (int i = 0; i < (pTerrainMesh->GetWidth()+1)*(pTerrainMesh->GetDepth()+1); ++i )
	{
		SVertex & vertex = pTerrainMesh->GetVertex(i);

		fApex = max(fApex,(-vertex.vPosition.x * sqrtf(2.0f) / 2.0f + vertex.vPosition.z * sqrtf(2.0f) / 2.0f + pTerrainMesh->GetWidth() * GRID_SPACE * sqrtf(2.0f) / 2.0f + vertex.vPosition.y * sqrtf(3.0f) )  );
	}

	return fApex;
}

void CTerrainAlgorithm::SetMappedRegion( set<int>& regionunits )
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	if ( regionunits == g_HotRgionUnits )
		return;

	set<DWORD> bufferindexs;
	if ( !g_HotRgionUnits.empty() )
	{
		for ( set<int>::iterator iter = g_HotRgionUnits.begin(); iter != g_HotRgionUnits.end(); ++iter )
		{
			SDivideRegion *pDivideRgn = NULL;
			pDivideRgn = pTerrainMesh->GetDivideRegion(*iter);
			if ( pDivideRgn )
			{
				for ( size_t i = 0; i < pDivideRgn->vGridIndices.size(); ++i )
				{
					SGrid & grid = pTerrainMesh->GetGrid(pDivideRgn->vGridIndices[i]);
					bufferindexs.insert(grid.dwBufferIndex);
					grid.bInHideRegion = false;
				}
			}
		}
	}

	g_HotRgionUnits = regionunits;

	if ( !regionunits.empty() )
	{
		for ( set<int>::iterator iter = regionunits.begin(); iter != regionunits.end(); ++iter )
		{
			SDivideRegion *pDivideRgn = NULL;
			pDivideRgn = pTerrainMesh->GetDivideRegion(*iter);
			if ( pDivideRgn )
			{
				for ( size_t i = 0; i < pDivideRgn->vGridIndices.size(); ++i )
				{
					SGrid & grid = pTerrainMesh->GetGrid(pDivideRgn->vGridIndices[i]);
					grid.bInHideRegion = true;
					bufferindexs.insert(grid.dwBufferIndex);
				}
			}
		}
	}

	if ( !bufferindexs.empty() && !pTerrainMesh->IsWater() )
	{
		//烘焙1 固定0
		pTerrainMesh->ReWriteTerrainBuffer( bufferindexs, 2, 0x00ff0000 );
	}
}

}
