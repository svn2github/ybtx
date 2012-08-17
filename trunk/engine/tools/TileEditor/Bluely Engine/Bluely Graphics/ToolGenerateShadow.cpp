//////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "ToolGenerateShadow.h"
#include "XFile.h"
#include "TerrainMesh.h"
#include "CmdMgr.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

CToolGenerateShadow::CToolGenerateShadow()
{

}

CToolGenerateShadow::~CToolGenerateShadow()
{

}	

void CToolGenerateShadow::OnEvent(ip::CInputDevice::SEvent & event)
{
	COperator * pOperator = CXFile::GetInstance()->GetOperator();
	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();
	
	const DWORD SEARCH_TOLERANCE = 	pTerrainMesh->GetWidth();
	
	switch ( event.eType )
	{
	case ip::EIET_KB_KEYDOWN :
		{
			if ( event.dwData == DIK_G )
			{
				
				DWORD dwWidth = CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
				DWORD dwDepth = CXFile::GetInstance()->GetTerrainMesh()->GetDepth();

				DWORD dwVertexCnt = CXFile::GetInstance()->GetTerrainMesh()->GetVertexCount();

				for ( DWORD dwVertexIndex = 0; dwVertexIndex < dwVertexCnt; dwVertexIndex++ )
				{
					CTerrainMesh::SVertex & vertex = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(dwVertexIndex);
					DWORD d = 0x006f6f6f;
					vertex.cColor &= 0xff000000;
					vertex.cColor |= d;
				}
				
				// for all the vertices...calculate normal
				
				for ( DWORD dwVertexIndex = 0; dwVertexIndex < dwVertexCnt; dwVertexIndex++ )
				{
					DWORD index;
					
					// skip all margin vertex
					if ( ( 0 <= dwVertexIndex && dwVertexIndex < dwWidth + 1 ) || 
						!(dwVertexIndex%(dwWidth+1)) || !((dwVertexIndex+1)%(dwWidth+1)) ||
						( dwDepth * (dwWidth+1) <= dwVertexIndex && dwVertexIndex < (dwDepth+1)*(dwWidth+1) ) )
						continue;
					
					// v0
					index = dwVertexIndex;
					CTerrainMesh::SVertex & vertex0 = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(index);
					D3DXVECTOR3 v0 = vertex0.vPosition;
					
					// v1
					index = dwVertexIndex - (dwWidth+1) - 1;
					CTerrainMesh::SVertex & vertex1 = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(index);
					D3DXVECTOR3 v1 = vertex1.vPosition;

					// v2
					index++; 
					CTerrainMesh::SVertex & vertex2 = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(index);
					D3DXVECTOR3 v2 = vertex2.vPosition;
				
					// v3
					index++; 
					CTerrainMesh::SVertex & vertex3 = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(index);
					D3DXVECTOR3 v3 = vertex3.vPosition;

					// v4
					index = dwVertexIndex - 1;					 
					CTerrainMesh::SVertex & vertex4 = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(index);
					D3DXVECTOR3 v4 = vertex4.vPosition;

					// v5
					index = dwVertexIndex + 1;				
					CTerrainMesh::SVertex & vertex5 = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(index);
					D3DXVECTOR3 v5 = vertex5.vPosition;

					// v6
					index = dwVertexIndex + (dwWidth+1) - 1;					 
					CTerrainMesh::SVertex & vertex6 = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(index);
					D3DXVECTOR3 v6 = vertex6.vPosition;

					// v7
					index++;					 
					CTerrainMesh::SVertex & vertex7 = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(index);
					D3DXVECTOR3 v7 = vertex7.vPosition;

					// v8
					index++;					 
					CTerrainMesh::SVertex & vertex8 = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(index);
					D3DXVECTOR3 v8 = vertex8.vPosition;

                  D3DXVECTOR3 n1;
					D3DXVec3Cross(&n1,&D3DXVECTOR3(v2-v0),&D3DXVECTOR3(v4-v0));
					D3DXVec3Normalize(&n1,&n1);

					D3DXVECTOR3 n2;
					D3DXVec3Cross(&n2,&D3DXVECTOR3(v4-v0),&D3DXVECTOR3(v6-v0));
					D3DXVec3Normalize(&n2,&n2);

					D3DXVECTOR3 n3;
					D3DXVec3Cross(&n3,&D3DXVECTOR3(v6-v0),&D3DXVECTOR3(v7-v0));
					D3DXVec3Normalize(&n3,&n3);

					D3DXVECTOR3 n4;
					D3DXVec3Cross(&n4,&D3DXVECTOR3(v7-v0),&D3DXVECTOR3(v5-v0));
					D3DXVec3Normalize(&n4,&n4);

					D3DXVECTOR3 n5;
					D3DXVec3Cross(&n5,&D3DXVECTOR3(v5-v0),&D3DXVECTOR3(v3-v0));
					D3DXVec3Normalize(&n5,&n5);

					D3DXVECTOR3 n6;
					D3DXVec3Cross(&n6,&D3DXVECTOR3(v3-v0),&D3DXVECTOR3(v2-v0));
					D3DXVec3Normalize(&n6,&n6);

					D3DXVECTOR3 n = n1 + n2 + n3 + n4 + n5 + n6;
					D3DXVec3Normalize(&n,&n);

					vertex0.vNormal = n;
				}	

				// for all the vertices... calculate shadow
				
				for ( DWORD dwVertexIndex = 0; dwVertexIndex < dwVertexCnt; dwVertexIndex++ )
				{
					CTerrainMesh::SVertex & vertex = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(dwVertexIndex);
					D3DXVECTOR3 vVertexPos(vertex.vPosition);
					D3DXVECTOR3 vLight(dwWidth*GRID_SPACE/2,dwWidth*GRID_SPACE,dwWidth*GRID_SPACE/2);


//////////////////////////////////////////////////////////////////////////

					D3DXVECTOR3 vDir(vVertexPos - vLight);
					D3DXVECTOR3 vPos(vLight); 

					////////////////////////////////////////////////////////////////////////////////

					//////float tg = (vVertexPos.z - vLight.z)/(vVertexPos.x - vLight.x);

					//////vLight.x = vLight.x/GRID_SPACE;
					//////vLight.z = vLight.z/GRID_SPACE;

					//////std::vector<DWORD> vecGridIndices;

					//////D3DXVECTOR3 vecStart(vLight); 
					//////D3DXVECTOR3 vecNew(vLight);
					//////DWORD dwGrid;

					//////if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecStart) )
					//////{
					//////	dwGrid = floor(vecStart.x) + floor(vecStart.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////	vecGridIndices.push_back(dwGrid);
					//////}

					//////if ( vPos.z < vVertexPos.z )
					//////{
					//////	if ( 0.0f < tg )
					//////	{
					//////		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
					//////		{
					//////			// up
					//////			D3DXVECTOR3 vecUp(vecNew.x,0.0f,vecNew.z+1.0f);

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecUp) )
					//////			{
					//////				dwGrid = floor(vecUp.x) + floor(vecUp.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}

					//////			// right
					//////			D3DXVECTOR3 vecRight(vecNew.x+1.0f,0.0f,vecNew.z);

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecRight) )
					//////			{
					//////				dwGrid = floor(vecRight.x) + floor(vecRight.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}

					//////			//// rightup
					//////			//D3DXVECTOR3 vecRightUp(vecNew.x+1.0f,0.0f,vecNew.z+1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecRightUp) )
					//////			//{
					//////			//	dwGrid = floor(vecRightUp.x) + floor(vecRightUp.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			//// rightdown
					//////			//D3DXVECTOR3 vecRightDown(vecNew.x+1.0f,0.0f,vecNew.z-1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecRightDown) )
					//////			//{
					//////			//	dwGrid = floor(vecRightDown.x) + floor(vecRightDown	.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			//// down
					//////			//D3DXVECTOR3 vecDown(vecNew.x,0.0f,vecNew.z-1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecDown) )
					//////			//{
					//////			//	dwGrid = floor(vecDown.x) + floor(vecDown.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			if ( tg <= 1.0f )
					//////			{
					//////				vecNew.x += 1.0f;
					//////				vecNew.z += tg;
					//////			}
					//////			else
					//////			{
					//////				vecNew.z += 1.0f;
					//////				vecNew.x += 1/tg;
					//////			}

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecNew) )
					//////			{
					//////				dwGrid = floor(vecNew.x) + floor(vecNew.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}
					//////		}
					//////	}
					//////	else // tg < 0.0f
					//////	{
					//////		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
					//////		{
					//////			// up
					//////			D3DXVECTOR3 vecUp(vecNew.x,0.0f,vecNew.z+1.0f);

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecUp) )
					//////			{
					//////				dwGrid = floor(vecUp.x) + floor(vecUp.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}

					//////			// left
					//////			D3DXVECTOR3 vecLeft(vecNew.x-1.0f,0.0f,vecNew.z);

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecLeft) )
					//////			{
					//////				dwGrid = floor(vecLeft.x) + floor(vecLeft.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}

					//////			//// rightup
					//////			//D3DXVECTOR3 vecRightUp(vecNew.x+1.0f,0.0f,vecNew.z+1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecRightUp) )
					//////			//{
					//////			//	dwGrid = floor(vecRightUp.x) + floor(vecRightUp.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			//// rightdown
					//////			//D3DXVECTOR3 vecRightDown(vecNew.x+1.0f,0.0f,vecNew.z-1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecRightDown) )
					//////			//{
					//////			//	dwGrid = floor(vecRightDown.x) + floor(vecRightDown	.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			//// down
					//////			//D3DXVECTOR3 vecDown(vecNew.x,0.0f,vecNew.z-1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecDown) )
					//////			//{
					//////			//	dwGrid = floor(vecDown.x) + floor(vecDown.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			if ( tg >= -1.0f )
					//////			{
					//////				vecNew.x -= 1.0f;
					//////				vecNew.z += -tg;
					//////			}
					//////			else  // < -1.0f
					//////			{
					//////				vecNew.z += 1.0f;
					//////				vecNew.x -= -1/tg;
					//////			}

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecNew) )
					//////			{
					//////				dwGrid = floor(vecNew.x) + floor(vecNew.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}
					//////		}
					//////	}
					//////}
					//////else  // inverse direction
					//////{
					//////	if ( 0.0f > tg )
					//////	{
					//////		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
					//////		{
					//////			// up
					//////			D3DXVECTOR3 vecUp(vecNew.x,0.0f,vecNew.z-1.0f);

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecUp) )
					//////			{
					//////				dwGrid = floor(vecUp.x) + floor(vecUp.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}

					//////			// right
					//////			D3DXVECTOR3 vecRight(vecNew.x+1.0f,0.0f,vecNew.z);

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecRight) )
					//////			{
					//////				dwGrid = floor(vecRight.x) + floor(vecRight.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}

					//////			//// rightup
					//////			//D3DXVECTOR3 vecRightUp(vecNew.x+1.0f,0.0f,vecNew.z+1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecRightUp) )
					//////			//{
					//////			//	dwGrid = floor(vecRightUp.x) + floor(vecRightUp.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			//// rightdown
					//////			//D3DXVECTOR3 vecRightDown(vecNew.x+1.0f,0.0f,vecNew.z-1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecRightDown) )
					//////			//{
					//////			//	dwGrid = floor(vecRightDown.x) + floor(vecRightDown	.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			//// down
					//////			//D3DXVECTOR3 vecDown(vecNew.x,0.0f,vecNew.z-1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecDown) )
					//////			//{
					//////			//	dwGrid = floor(vecDown.x) + floor(vecDown.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			if ( tg >= -1.0f )
					//////			{
					//////				vecNew.x += 1.0f;
					//////				vecNew.z += tg;
					//////			}
					//////			else
					//////			{
					//////				vecNew.z -= 1.0f;
					//////				vecNew.x -= 1/tg;
					//////			}

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecNew) )
					//////			{
					//////				dwGrid = floor(vecNew.x) + floor(vecNew.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}
					//////		}
					//////	}
					//////	else // tg > 0.0f
					//////	{
					//////		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
					//////		{
					//////			// up
					//////			D3DXVECTOR3 vecUp(vecNew.x,0.0f,vecNew.z-1.0f);

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecUp) )
					//////			{
					//////				dwGrid = floor(vecUp.x) + floor(vecUp.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}

					//////			// left
					//////			D3DXVECTOR3 vecLeft(vecNew.x-1.0f,0.0f,vecNew.z);

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecLeft) )
					//////			{
					//////				dwGrid = floor(vecLeft.x) + floor(vecLeft.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}

					//////			//// rightup
					//////			//D3DXVECTOR3 vecRightUp(vecNew.x+1.0f,0.0f,vecNew.z+1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecRightUp) )
					//////			//{
					//////			//	dwGrid = floor(vecRightUp.x) + floor(vecRightUp.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			//// rightdown
					//////			//D3DXVECTOR3 vecRightDown(vecNew.x+1.0f,0.0f,vecNew.z-1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecRightDown) )
					//////			//{
					//////			//	dwGrid = floor(vecRightDown.x) + floor(vecRightDown	.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			//// down
					//////			//D3DXVECTOR3 vecDown(vecNew.x,0.0f,vecNew.z-1.0f);

					//////			//if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecDown) )
					//////			//{
					//////			//	dwGrid = floor(vecDown.x) + floor(vecDown.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////			//	vecGridIndices.push_back(dwGrid);
					//////			//}

					//////			if ( tg < 1.0f )
					//////			{
					//////				vecNew.x -= 1.0f;
					//////				vecNew.z -= tg;
					//////			}
					//////			else  // > 1.0f
					//////			{
					//////				vecNew.z -= 1.0f;
					//////				vecNew.x -= 1/tg;
					//////			}

					//////			if ( CXFile::GetInstance()->GetTerrainMesh()->IsValid(vecNew) )
					//////			{
					//////				dwGrid = floor(vecNew.x) + floor(vecNew.z) * sc::CXFile::GetInstance()->GetTerrainMesh()->GetWidth();
					//////				vecGridIndices.push_back(dwGrid);
					//////			}
					//////		}
					//////	}
					//////}

					//////DWORD dwGridIndexHot = INVALID;

					//////for ( std::vector<DWORD>::iterator iter = vecGridIndices.begin(); iter != vecGridIndices.end(); iter++ )
					//////{	
					//////	sc::CTerrainMesh::SGrid & grid = sc::CXFile::GetInstance()->GetTerrainMesh()->GetGrid(*iter);

					//////	D3DXVECTOR3 v0 = sc::CXFile::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[0]).vPosition;
					//////	D3DXVECTOR3 v1 = sc::CXFile::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[1]).vPosition;
					//////	D3DXVECTOR3 v2 = sc::CXFile::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[2]).vPosition;
					//////	D3DXVECTOR3 v3 = sc::CXFile::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[3]).vPosition;

					//////	float u,v,d;

					//////	if ( D3DXIntersectTri(&v0,&v1,&v3,&vPos,&vDir,&u,&v,&d) || D3DXIntersectTri(&v1,&v2,&v3,&vPos,&vDir,&u,&v,&d) )
					//////	{
					//////		dwGridIndexHot = *iter;
					//////		break;
					//////	}
					//////}

					//if ( dwGridIndexHot != INVALID && dwGridIndexHot != vertex.dwGridsBelongto[0] && dwGridIndexHot != vertex.dwGridsBelongto[1] && dwGridIndexHot != vertex.dwGridsBelongto[2] && dwGridIndexHot != vertex.dwGridsBelongto[3] ) 
					//{
					//	D3DXVECTOR3 vNormalLight = vDir;
					//	D3DXVec3Normalize(&vNormalLight,&vNormalLight);
					//	float f = 1 - D3DXVec3Dot(&vNormalLight,&vertex.vNormal);
					//	if ( f < 0.5f ) f = 0.5f;
					//	DWORD dwAddUp = 32 * f;
					//	DWORD d = (dwAddUp << 16 | dwAddUp << 8 | dwAddUp);
					//	vertex.cColor += d;
					//}
					//else // if ( dwGridIndexHot != INVALID )
					//{
						D3DXVECTOR3 vNormalLight = vLight;
						D3DXVec3Normalize(&vNormalLight,&vNormalLight);			
						float f = D3DXVec3Dot(&vNormalLight,&vertex.vNormal);
						if ( f < 0.7f ) f = 0.7f;
						DWORD dwAddUp = 128 * f;
						DWORD d = (dwAddUp << 16 | dwAddUp << 8 | dwAddUp);
						vertex.cColor += d;
					//}

					//if ( dwGridIndexHot != INVALID && dwGridIndexHot != vertex.dwGridsBelongto[0] && dwGridIndexHot != vertex.dwGridsBelongto[1] && dwGridIndexHot != vertex.dwGridsBelongto[2] && dwGridIndexHot != vertex.dwGridsBelongto[3] ) 
					//{
					//	DWORD r,g,b;
					//	
					//	r = ((vertex.cColor & 0x00ff0000) >> 16)*0.5;                          
					//	g = ((vertex.cColor & 0x0000ff00) >> 8)*0.5;                          
					//	b = (vertex.cColor & 0x000000ff)*0.5;                         
					//	vertex.cColor &= 0xff000000; 
					//	vertex.cColor |= (r << 16 | g << 8 | b);
 					//	}
					//else // if ( dwGridIndexHot != INVALID )
					//{
					//	D3DXVECTOR3 vNormalLight = vLight;
					//	D3DXVec3Normalize(&vNormalLight,&vNormalLight);			
					//	DWORD r,g,b;
					//	r = ((vertex.cColor & 0x00ff0000) >> 16)*D3DXVec3Dot(&vNormalLight,&vertex.vNormal);                          
					//	g = ((vertex.cColor & 0x0000ff00) >> 8)*D3DXVec3Dot(&vNormalLight,&vertex.vNormal);                          
					//	b = (vertex.cColor & 0x000000ff)*D3DXVec3Dot(&vNormalLight,&vertex.vNormal);                         
					//	vertex.cColor &= 0xff000000; 
					//	vertex.cColor |= (r << 16 | g << 8 | b);
					//}

				}
			}
		}
		break;
	}
}

void CToolGenerateShadow::Render()
{

}

}
}