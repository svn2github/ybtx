//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "extradefine.h"
#include "TerrainMesh.h"
#include "../TileEditorEngine/Console.h"
#include "Render.h"
#include "VisibleSet.h"
#include "QTNode.h"
#include "Picker.h"
#include "Camera.h"
#include "Engine.h"
#include "BluelyGUI.h"
#include "Operator.h"
#include "CTileEditApp.h"
#include "SharedTexture.h"
#include "../TileEditorEngine/Timer.h"
#include "UserMsg.h"
#include "Face.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

#define MAX_GRID_SIZE 1000000

HWND CTerrainMesh::m_hClassView = NULL;

const D3DXVECTOR3 vNormalLight(0.4f,0.8f,0.4f); 

//////////////////////////////////////////////////////////////////////////

CTerrainMesh::CTerrainMesh(ISceneNode * pParent) : ISceneNode(pParent)
{
	m_dwTextureHandleDefault = INVALID;
	m_dwShaderHandle = INVALID;
	m_pQTRoot = NULL;
	m_pTTRoot = NULL;
	m_fHeight = 4.0f;
	m_fAlpha = 16.0f;
}

//////////////////////////////////////////////////////////////////////////

CTerrainMesh::~CTerrainMesh()
{
	SAFE_DELETE_ARRAY(m_pVertices);
	SAFE_DELETE_ARRAY(m_pGrids);
	SAFE_DELETE(m_pQTRoot);																									
	SAFE_DELETE(m_pTTRoot);																									
	SAFE_DELETE(m_pVisibleSet);
	LogConsole("info : CTerrainMesh::~CTerrainMesh : Terrain node destructed...");
}

//////////////////////////////////////////////////////////////////////////

bool CTerrainMesh::Create(DWORD dwWidth, DWORD dwDepth, DWORD dwTextureHandleDefault)
{
	//CCamera * pCamera = eg::CEngine::GetInstance()->GetActiveCamera();
	//pCamera->SetPosition(D3DXVECTOR3(1000.0f,1000.0f,-1000.0f));
	//pCamera->SetView(D3DXVECTOR3(0.0f,0.0f,0.0f));
	
	m_dwTextureHandleDefault = dwTextureHandleDefault;
	
	m_dwWidthInRegion = dwWidth;
	m_dwDepthInRegion = dwDepth;
	
	m_dwWidth = dwWidth * REGION_SPACE;
	m_dwDepth = dwDepth * REGION_SPACE;
	
	m_dwRegionCnt = m_dwWidthInRegion *	m_dwDepthInRegion; 
	
	m_pRegions = new SRegion[m_dwRegionCnt];
	
	//for ( int i = 0; i < m_dwRegionCnt; i++ )
	//{	
	//	SRegion & reg = m_pRegions[i];
	//	
	//	int nX = i%this->GetWidthInRegion();
	//	int nY = i/this->GetDepthInRegion();
	//	
	//	int nVertexIndex = nX*REGION_SPACE + nY*REGION_SPACE*(this->GetWidth()+1);
	//	
	//	DWORD dwVerterxCntInRegion = (REGION_SPACE+1)*(REGION_SPACE+1);
	//	
	//	for ( int j = 0; j < dwVerterxCntInRegion; j++ )
	//	{
	//		if ( j != 0 && j % (REGION_SPACE+1) == 0 )
	//		{
	//			nVertexIndex += -1 + this->GetWidth()+1 - REGION_SPACE;
	//		}
	//		
	//		reg.vVertexIndices.push_back(nVertexIndex);
	//	
	//		nVertexIndex++;
	//	}
	//	
	//	int nGridIndex = nX*REGION_SPACE + nY*REGION_SPACE*(this->GetWidth());

	//	DWORD dwGridCntInRegion = (REGION_SPACE)*(REGION_SPACE);

	//	for ( int j = 0; j < dwGridCntInRegion; j++ )
	//	{
	//		if ( j != 0 && j % (REGION_SPACE) == 0 )
	//		{
	//			nGridIndex += -1 + this->GetWidth() - REGION_SPACE +1;
	//		}
	//		
	//		reg.vGridIndices.push_back(nGridIndex);

	//		nGridIndex++;
	//	}
	//}
	
	m_dwVertexCnt = (m_dwWidth + 1) * (m_dwDepth + 1);
	
	m_pVertices = new SVertex[m_dwVertexCnt];	
	ZeroMemory(m_pVertices, sizeof(SVertex) * m_dwVertexCnt);
	
	D3DXVECTOR3 vNormal(0.0, 1.0f, 0.0f);
	float f = D3DXVec3Dot(&vNormalLight,&vNormal);
	if ( f < 0.7f ) f = 0.7f;
	DWORD dwAddUp = 128 * f;
	DWORD d = (dwAddUp << 16 | dwAddUp << 8 | dwAddUp);
	
	for ( DWORD dwVertexIndex = 0; dwVertexIndex < m_dwVertexCnt; dwVertexIndex++ )
	{
		float x = (float)(dwVertexIndex % (m_dwWidth + 1)) * GRID_SPACE;
		float z = (float)(dwVertexIndex / (m_dwWidth + 1)) * GRID_SPACE;
		SVertex & vertex = m_pVertices[dwVertexIndex];
		vertex.dwVertexIndex = dwVertexIndex;
		vertex.vPosition = D3DXVECTOR3(x, 0.0f, z);
		vertex.vNormal = vNormal;
	//////	vertex.cColor = 0x006f6f6f + d;
		vertex.cColor = 0x00ffffff;
	}
	
	m_dwGridCnt = m_dwWidth * m_dwDepth;
	
	if ( m_dwGridCnt > MAX_GRID_SIZE )
	{
		LogConsole("error : CTerrainMesh::Create : Too many grids...");
		assert(0);
	}
	
	m_pGrids = new SGrid[m_dwGridCnt];	

	/*
	 * z v3  --------  v2
	 *		|        |
	 *		|	     |	
	 *	v0	|________| v1   ---> x
	 */
	
	for ( DWORD z = 0 ; z < m_dwDepth; z++ )
	{
		for ( DWORD x = 0 ; x < m_dwWidth ; x++ )
		{
			DWORD dwIndex = x + z * m_dwWidth;
	
			DWORD v0, v1, v2, v3;
	
			v0 = z * (m_dwWidth + 1) + x;
			v1 = v0 + 1;
			v2 = v1 + (m_dwWidth + 1);
			v3 = v2 - 1;
	
			SGrid & grid = m_pGrids[dwIndex];
			grid.dwGridIndex = dwIndex;
			grid.nodeInfo.dwGridIndex = dwIndex;
			grid.nodeInfo.dwTextureHandles[0] = dwTextureHandleDefault;
			grid.nodeInfo.dwTextureHandles[1] = INVALID;
			grid.nodeInfo.dwTextureHandles[2] = INVALID;
	
			grid.dwVertexIndex[0] = v0;
			grid.dwVertexIndex[1] = v1;
			grid.dwVertexIndex[2] = v2;
			grid.dwVertexIndex[3] = v3;
	
			int nX = x % 2;
			int nZ = z % 2;
	
			float delta = 0.001f;

			switch ( nX )
			{
			case 0:
				{
					grid.uv[0][0].x = 0.0f + delta;
					grid.uv[0][1].x = .5f;
					grid.uv[0][2].x = .5f;
					grid.uv[0][3].x = 0.0f + delta;

					grid.uv[1][0].x = 0.0f + delta;
					grid.uv[1][1].x = .5f;
					grid.uv[1][2].x = .5f;
					grid.uv[1][3].x = 0.0f + delta;
				}
			break;

			case 1:
				{
					grid.uv[0][0].x = .5f;
					grid.uv[0][1].x = 1.0f - delta;
					grid.uv[0][2].x = 1.0f - delta;
					grid.uv[0][3].x = .5f;

					grid.uv[1][0].x = .5f;
					grid.uv[1][1].x = 1.0f - delta;
					grid.uv[1][2].x = 1.0f - delta;
					grid.uv[1][3].x = .5f;
				}
			break;
			}

			switch ( nZ )
			{
			case 0:
				{
					grid.uv[0][0].y = 1.0f - delta;
					grid.uv[0][1].y = 1.0f - delta;
					grid.uv[0][2].y = .5f;
					grid.uv[0][3].y = .5f;

					grid.uv[1][0].y = 1.0f - delta;
					grid.uv[1][1].y = 1.0f - delta;
					grid.uv[1][2].y = .5f;
					grid.uv[1][3].y = .5f;
				}
			break;

			case 1:
				{
					grid.uv[0][0].y = .5f;
					grid.uv[0][1].y = .5f;
					grid.uv[0][2].y = 0.0f + delta;
					grid.uv[0][3].y = 0.0f + delta;

					grid.uv[1][0].y = .5f;
					grid.uv[1][1].y = .5f;
					grid.uv[1][2].y = 0.0f + delta;
					grid.uv[1][3].y = 0.0f + delta;
				}
			break;
			}

			DWORD dwGridIndex;
			dwGridIndex = m_pVertices[v0].dwUsedCnt;
			m_pVertices[v0].dwGridsBelongto[dwGridIndex] = dwIndex;
			m_pVertices[v0].dwUsedCnt++;
			
			dwGridIndex = m_pVertices[v1].dwUsedCnt;
			m_pVertices[v1].dwGridsBelongto[dwGridIndex] = dwIndex;
			m_pVertices[v1].dwUsedCnt++;
			
			dwGridIndex = m_pVertices[v2].dwUsedCnt;
			m_pVertices[v2].dwGridsBelongto[dwGridIndex] = dwIndex;
			m_pVertices[v2].dwUsedCnt++;
			
			dwGridIndex = m_pVertices[v3].dwUsedCnt;
			m_pVertices[v3].dwGridsBelongto[dwGridIndex] = dwIndex;
			m_pVertices[v3].dwUsedCnt++;

			grid.vCenter.x = (float)(grid.dwVertexIndex[0] % (m_dwWidth + 1)) * GRID_SPACE + GRID_SPACE/2;
			grid.vCenter.y = 0.0f;
			grid.vCenter.z = (float)(grid.dwVertexIndex[0] / (m_dwWidth + 1)) * GRID_SPACE + GRID_SPACE/2;
			grid.byBlockFlag = 0;
		}
	}

	// create shader
	rd::CRenderDevice::Layer3D * pLayer3D = rd::CRenderDevice::GetInstance()->GetLayer3D();
	m_dwShaderHandle = pLayer3D->CreateShader(m_dwTextureHandleDefault,INVALID,rd::EL3DST_SOLID_1_LAYER,rd::EL3DMST_POSNMLCOLTEX0,false,true,true,true,true,true,true,false);

	//////////////////////////////////////////////////////////////////////////
	
	// create light and material
	D3DLIGHT9 DirLight;
	ZeroMemory(&DirLight ,sizeof(DirLight));
	DirLight.Type =D3DLIGHT_DIRECTIONAL;	
	DirLight.Direction =D3DXVECTOR3( 0.3f, -0.1f, 0.3f );
	DirLight.Diffuse.r = 1.0f;
	DirLight.Diffuse.g = 1.0f;
	DirLight.Diffuse.b = 1.0f;
	DirLight.Diffuse.a = 1.0f;
	DirLight.Specular = DirLight.Diffuse; 
	DirLight.Attenuation0 = 1;
	DirLight.Range = 1000;
	DirLight.Falloff = 0;
	DirLight.Attenuation0 = 1;
	DirLight.Attenuation1 = 0;
	DirLight.Attenuation2 = 0;
	
	D3DXVec3Normalize( (D3DXVECTOR3*)&DirLight.Direction, (D3DXVECTOR3*)&DirLight.Direction );
	LPDIRECT3DDEVICE9 pDevice = rd::CRenderDevice::GetInstance()->GetRawDevice();
//	pDevice->SetLight(0,&DirLight);
//	pDevice->LightEnable(0,true);
	
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	pDevice->SetMaterial(&mtrl);
	
	pDevice->SetRenderState(D3DRS_AMBIENT,0xFFFFFFFF);
	
	//////////////////////////////////////////////////////////////////////////
	
	m_pQTRoot = new CQTNode;
	
	std::vector<int> vRootIndices;
	
	for ( int iIndex = 0; iIndex < m_dwGridCnt; iIndex++ )
	{
		vRootIndices.push_back(iIndex);
	}
	
	m_pQTRoot->GetQuadXZ()->Set(0,(float)m_dwWidth*GRID_SPACE,0,(float)m_dwDepth*GRID_SPACE);
	m_pQTRoot->Create(vRootIndices,this);
	m_pVisibleSet = new CVisibleSet;
	m_pVisibleSet->Create(this);
	
	m_pTTRoot = new CTextureNode;
	m_pTTRoot->SetTextureHandle(INVALID);
	
	this->ResetVS();
	this->RebuildRenderTree();
	return true;
}

//////////////////////////////////////////////////////////////////////////

void CTerrainMesh::Render()
{	
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	rd::CRenderDevice::Layer3D * pLayer3d = pRender->GetLayer3D();

	// self specific transform
	pRender->GetRawDevice()->SetTransform(D3DTS_WORLD,&this->GetAbsolute());
	pLayer3d->SetShader(m_dwShaderHandle);
	m_pTTRoot->Render(m_dwShaderHandle, m_pGrids, m_pVertices);
	this->SubRenderObjects();
	ISceneNode::Render();
}

void CTerrainMesh::SubRenderObjects()
{
}

void CTerrainMesh::SetTextureHandleDefault(DWORD dwTextureHandleDefault)
{
	m_dwTextureHandleDefault = dwTextureHandleDefault;

	if ( m_dwShaderHandle != INVALID )
	{
		rd::CRenderDevice::Layer3D::SShader * pShader = rd::CRenderDevice::GetInstance()->GetLayer3D()->GetShader(m_dwShaderHandle);
		pShader->iTexHandle0 = m_dwTextureHandleDefault;
	}
}

CTerrainMesh::SGrid & CTerrainMesh::GetGrid(DWORD dwGridIndex) 
{ 
	return m_pGrids[dwGridIndex];
}

CTerrainMesh::SRegion & CTerrainMesh::GetRegion(int nRegionIndex) 
{ 
	return m_pRegions[nRegionIndex];
}

CTerrainMesh::SVertex & CTerrainMesh::GetVertex(DWORD dwVertexIndex) 
{ 
	return m_pVertices[dwVertexIndex];
}

int CTerrainMesh::GetRegionIndexByGridIndex(int nGridIndex)
{
	int nX = ( nGridIndex % this->GetWidth() ) / REGION_SPACE;
	int nY = ( nGridIndex / this->GetWidth() ) / REGION_SPACE;
	return nX + nY*this->GetWidthInRegion();
}

void CTerrainMesh::Update(ip::CInputDevice::SEvent & event)
{
	CCamera * pCamera = eg::CEngine::GetInstance()->GetActiveCamera();

	if ( pCamera->GetResetVS() )
	{
		this->ResetVS();
		this->RebuildRenderTree();
		CCamera * pCamera = eg::CEngine::GetInstance()->GetActiveCamera();
		pCamera->SetResetVS(false);
	}
	ISceneNode::Update(event);
}

void CTerrainMesh::ResetVS()
{
	m_pVisibleSet->Reset();
	m_pQTRoot->TestVisibility(m_pVisibleSet);
}

void CTerrainMesh::RebuildRenderTree()
{
	DWORD * pVisibleGrids = m_pVisibleSet->GetVisibleGridIndices();

	DWORD cnt = m_pVisibleSet->GetVisibleGridCnt();

	m_pTTRoot->Clear();

	for ( DWORD n = 0; n < cnt; n++ )
	{	
		SGrid & grid = m_pGrids[pVisibleGrids[n]];
       m_pTTRoot->Insert(grid.nodeInfo); 
	}
}

void CTerrainMesh::SetGridTextureHandle(DWORD dwLayer, DWORD dwGridIndex, DWORD dwTextureHandle)
{
	assert( dwLayer < 2 );
	SGrid & grid = this->GetGrid(dwGridIndex);
	grid.nodeInfo.dwTextureHandles[dwLayer] = dwTextureHandle;
}

void CTerrainMesh::SetVertexAlpha(DWORD dwVertexIndex, D3DCOLOR cColor)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	vertex.cColor = cColor;
}

void CTerrainMesh::SetVertexHeight(DWORD dwVertexIndex, float fHeight)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	vertex.vPosition.y = fHeight;
}

void CTerrainMesh::ReCalculateGridsHeight(DWORD dwVertexIndex)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);

	for ( int i = 0; i < vertex.dwUsedCnt; i++ )
	{
		SGrid & grid = this->GetGrid(vertex.dwGridsBelongto[i]);
		
		SVertex & vertex0 = this->GetVertex(grid.dwVertexIndex[0]);
		SVertex & vertex1 = this->GetVertex(grid.dwVertexIndex[1]);
		SVertex & vertex2 = this->GetVertex(grid.dwVertexIndex[2]);
		SVertex & vertex3 = this->GetVertex(grid.dwVertexIndex[3]);
		grid.vCenter.y = (vertex0.vPosition.y + vertex1.vPosition.y + vertex2.vPosition.y + vertex3.vPosition.y)/4.0f;
	}
}

void CTerrainMesh::LightMe(DWORD dwVertexIndex)
{
	// add light effect

	DWORD index;

	DWORD dwWidth = this->GetWidth();
	DWORD dwDepth = this->GetDepth();

	// skip all margin vertex
	if ( ( 0 <= dwVertexIndex && dwVertexIndex < dwWidth + 1 ) || 
		!(dwVertexIndex%(dwWidth+1)) || !((dwVertexIndex+1)%(dwWidth+1)) ||
		( dwDepth * (dwWidth+1) <= dwVertexIndex && dwVertexIndex < (dwDepth+1)*(dwWidth+1) ) )
	{
		return;
	}
	
	// v0
	index = dwVertexIndex;
	CTerrainMesh::SVertex & vertex0 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(index);
	D3DXVECTOR3 v0 = vertex0.vPosition;
	
	//////DWORD d = 0x006f6f6f;
	//////vertex0.cColor &= 0xff000000;
	//////vertex0.cColor |= d;
	
	// v1
	index = dwVertexIndex - (dwWidth+1) - 1;
	CTerrainMesh::SVertex & vertex1 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(index);
	D3DXVECTOR3 v1 = vertex1.vPosition;
	
	// v2
	index++; 
	CTerrainMesh::SVertex & vertex2 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(index);
	D3DXVECTOR3 v2 = vertex2.vPosition;
	
	// v3
	index++; 
	CTerrainMesh::SVertex & vertex3 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(index);
	D3DXVECTOR3 v3 = vertex3.vPosition;
	
	// v4
	index = dwVertexIndex - 1;					 
	CTerrainMesh::SVertex & vertex4 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(index);
	D3DXVECTOR3 v4 = vertex4.vPosition;
	
	// v5
	index = dwVertexIndex + 1;				
	CTerrainMesh::SVertex & vertex5 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(index);
	D3DXVECTOR3 v5 = vertex5.vPosition;

	// v6
	index = dwVertexIndex + (dwWidth+1) - 1;					 
	CTerrainMesh::SVertex & vertex6 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(index);
	D3DXVECTOR3 v6 = vertex6.vPosition;

	// v7
	index++;					 
	CTerrainMesh::SVertex & vertex7 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(index);
	D3DXVECTOR3 v7 = vertex7.vPosition;

	// v8
	index++;					 
	CTerrainMesh::SVertex & vertex8 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(index);
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

//	D3DXVECTOR3 vNormalLight(dwWidth*GRID_SPACE/2,dwWidth*GRID_SPACE,dwWidth*GRID_SPACE/2);
//	D3DXVec3Normalize(&vNormalLight,&vNormalLight);			

	//////float f = D3DXVec3Dot(&vNormalLight,&vertex0.vNormal);
	//////if ( f < 0.7f ) f = 0.7f;
	//////DWORD dwAddUp = 128 * f;
	//////d = (dwAddUp << 16 | dwAddUp << 8 | dwAddUp);
	//////vertex0.cColor += d;
}

DWORD CTerrainMesh::GetVertexIndexHot()
{
	return this->GetGrid(CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwGridIndexHot).dwVertexIndex[0]; 
}

CTerrainMesh::SGrid & CTerrainMesh::GetGridHot()
{
	return this->GetGrid(CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwGridIndexHot);
}

CTerrainMesh::SVertex & CTerrainMesh::GetVertexHot()
{
	SGrid & grid = this->GetGrid(CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwGridIndexHot);
	return this->GetVertex(grid.dwVertexIndex[0]);
}

//////////////////////////////////////////////////////////////////////////

void CTerrainMesh::CutVertexAlpha(DWORD dwVertexIndex)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	vertex.cColor &= 0x00ffffff;
}

void CTerrainMesh::IncLinearVertexAlphaX(DWORD dwVertexIndex)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SVertex & vertexHot = this->GetVertexHot();
	float distance = abs(vertex.vPosition.x - vertexHot.vPosition.x);
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	unsigned char alpha = vertex.cColor >> 24;
	
	if ( static_cast<unsigned char>(alpha + radio*m_fAlpha) < alpha )
	{
		alpha = 0xff;
	}
	else
	{
		alpha += static_cast<unsigned char>(radio*m_fAlpha);
	}
	
	vertex.cColor |= 0xff000000;
	vertex.cColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

void CTerrainMesh::IncLinearVertexAlphaZ(DWORD dwVertexIndex)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SVertex & vertexHot = this->GetVertexHot();
	float distance = abs(vertex.vPosition.z - vertexHot.vPosition.z);
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	unsigned char alpha = vertex.cColor >> 24;

	if ( static_cast<unsigned char>(alpha + radio*m_fAlpha) < alpha )
	{
		alpha = 0xff;
	}
	else
	{
		alpha += static_cast<unsigned char>(radio*m_fAlpha);
	}

	vertex.cColor |= 0xff000000;
	vertex.cColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

void CTerrainMesh::IncLinearVertexAlphaB(DWORD dwVertexIndex, DWORD dwVertexIndexHot)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SVertex & vertexHot = this->GetVertex(dwVertexIndexHot);
	float distanceZ = abs(vertex.vPosition.z - vertexHot.vPosition.z);
	float distanceX = abs(vertex.vPosition.x - vertexHot.vPosition.x);
	float distance = distanceZ > distanceX ? distanceZ : distanceX; 
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	unsigned char alpha = vertex.cColor >> 24;
	
	if ( static_cast<unsigned char>(alpha + radio*m_fAlpha) < alpha )
	{
		alpha = 0xff;
	}
	else
	{
		alpha += static_cast<unsigned char>(radio*m_fAlpha);
	}
	
	vertex.cColor |= 0xff000000;
	vertex.cColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

void CTerrainMesh::DecLinearVertexAlphaX(DWORD dwVertexIndex)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SVertex & vertexHot = this->GetVertexHot();
	float distance = abs(vertex.vPosition.x - vertexHot.vPosition.x);
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	unsigned char alpha = vertex.cColor >> 24;

	if ( static_cast<unsigned char>(alpha - radio*m_fAlpha) > alpha )
	{
		alpha = 0x00;
	}
	else
	{
		alpha -= static_cast<unsigned char>(radio*m_fAlpha);
	}

	vertex.cColor |= 0xff000000;
	vertex.cColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

void CTerrainMesh::DecLinearVertexAlphaZ(DWORD dwVertexIndex)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SVertex & vertexHot = this->GetVertexHot();
	float distance = abs(vertex.vPosition.z - vertexHot.vPosition.z);
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	unsigned char alpha = vertex.cColor >> 24;
	if ( static_cast<unsigned char>(alpha - radio*m_fAlpha) > alpha )
	{
		alpha = 0x00;
	}
	else
	{
		alpha -= static_cast<unsigned char>(radio*m_fAlpha);
	}
	vertex.cColor |= 0xff000000;
	vertex.cColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

void CTerrainMesh::DecLinearVertexAlphaB(DWORD dwVertexIndex)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SVertex & vertexHot = this->GetVertexHot();
	float distanceZ = abs(vertex.vPosition.z - vertexHot.vPosition.z);
	float distanceX = abs(vertex.vPosition.x - vertexHot.vPosition.x);
	float distance = distanceZ > distanceX ? distanceZ : distanceX; 
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	unsigned char alpha = vertex.cColor >> 24;
	if ( static_cast<unsigned char>(alpha - radio*m_fAlpha) > alpha )
	{
		alpha = 0x00;
	}
	else
	{
		alpha -= static_cast<unsigned char>(radio*m_fAlpha);
	}
	vertex.cColor |= 0xff000000;
	vertex.cColor &= (DWORD)alpha << 24 | 0x00ffffff;
}

//////////////////////////////////////////////////////////////////////////

void CTerrainMesh::CutVertexHeight(DWORD dwVertexIndex)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	vertex.vPosition.y = 0.0f;
}

void CTerrainMesh::IncLinearVertexHeightX(DWORD dwVertexIndex, DWORD dwGridIndexHot)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SGrid & grid = this->GetGrid(dwGridIndexHot);
	SVertex & vertexHot = this->GetVertex(grid.dwVertexIndex[0]);
	float distance = abs(vertex.vPosition.x - vertexHot.vPosition.x);
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	vertex.vPosition.y += radio*m_fHeight;
}

void CTerrainMesh::IncLinearVertexHeightZ(DWORD dwVertexIndex, DWORD dwGridIndexHot)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SGrid & grid = this->GetGrid(dwGridIndexHot);
	SVertex & vertexHot = this->GetVertex(grid.dwVertexIndex[0]);
	float distance = abs(vertex.vPosition.z - vertexHot.vPosition.z);
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	vertex.vPosition.y += radio*m_fHeight;
}

void CTerrainMesh::IncLinearVertexHeightB(DWORD dwVertexIndex, DWORD dwGridIndexHot)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SGrid & grid = this->GetGrid(dwGridIndexHot);
	SVertex & vertexHot = this->GetVertex(grid.dwVertexIndex[0]);
	float distanceZ = abs(vertex.vPosition.z - vertexHot.vPosition.z);
	float distanceX = abs(vertex.vPosition.x - vertexHot.vPosition.x);
	float distance = distanceZ > distanceX ? distanceZ : distanceX; 
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	vertex.vPosition.y += radio*m_fHeight;
}

void CTerrainMesh::DecLinearVertexHeightX(DWORD dwVertexIndex, DWORD dwGridIndexHot)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SGrid & grid = this->GetGrid(dwGridIndexHot);
	SVertex & vertexHot = this->GetVertex(grid.dwVertexIndex[0]);
	float distance = abs(vertex.vPosition.x - vertexHot.vPosition.x);
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	vertex.vPosition.y -= radio*m_fHeight;
}

void CTerrainMesh::DecLinearVertexHeightZ(DWORD dwVertexIndex, DWORD dwGridIndexHot)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SGrid & grid = this->GetGrid(dwGridIndexHot);
	SVertex & vertexHot = this->GetVertex(grid.dwVertexIndex[0]);
	float distance = abs(vertex.vPosition.z - vertexHot.vPosition.z);
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	vertex.vPosition.y -= radio*m_fHeight;
}

void CTerrainMesh::DecLinearVertexHeightB(DWORD dwVertexIndex, DWORD dwGridIndexHot)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	SGrid & grid = this->GetGrid(dwGridIndexHot);
	SVertex & vertexHot = this->GetVertex(grid.dwVertexIndex[0]);
	float distanceZ = abs(vertex.vPosition.z - vertexHot.vPosition.z);
	float distanceX = abs(vertex.vPosition.x - vertexHot.vPosition.x);
	float distance = distanceZ > distanceX ? distanceZ : distanceX; 
	float radio = 1.0f - distance/(GRID_SPACE * CTileEditApp::GetInstance()->GetOperator()->GetProcessParams()->dwBrushSize);
	vertex.vPosition.y -= radio*m_fHeight;
}

void CTerrainMesh::IncGaussVertexHeight(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, DWORD dwGridIndexHot)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);
	
	SGrid & grid = this->GetGrid(dwGridIndexHot);
	SVertex & vertexHot = this->GetVertex(grid.dwVertexIndex[0]);

	float distanceZ = vertex.vPosition.z - ( vertexHot.vPosition.z + 0.5f );
	float distanceX = vertex.vPosition.x - ( vertexHot.vPosition.x + 0.5f );
	
	float a,b,k,r;
	a = dwBrushWidth * GRID_SPACE; 
	b = dwBrushDepth * GRID_SPACE;
	
	k = distanceX/distanceZ;
	r = ((a*a*b*b)/(k*k*b*b+a*a))*(1+k*k);
	r = sqrtf((distanceX*distanceX+distanceZ*distanceZ)/r);

	vertex.vPosition.y += m_fHeight*250*expf(-r*r*3)/100;
}

void CTerrainMesh::DecGaussVertexHeight(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, DWORD dwGridIndexHot)
{
	SVertex & vertex = this->GetVertex(dwVertexIndex);

	SGrid & grid = this->GetGrid(dwGridIndexHot);
	SVertex & vertexHot = this->GetVertex(grid.dwVertexIndex[0]);

	float distanceZ = vertex.vPosition.z - ( vertexHot.vPosition.z + 0.5f );
	float distanceX = vertex.vPosition.x - ( vertexHot.vPosition.x + 0.5f );

	float a,b,k,r;
	a = dwBrushWidth * GRID_SPACE; 
	b = dwBrushDepth * GRID_SPACE;

	k = distanceX/distanceZ;
	r = ((a*a*b*b)/(k*k*b*b+a*a))*(1+k*k);
	r = sqrtf((distanceX*distanceX+distanceZ*distanceZ)/r);

	vertex.vPosition.y -= m_fHeight*250*expf(-r*r*3)/100;
}

//void CTerrainMesh::IncRandomVertexHeight(DWORD dwVertexIndex)
//{
//	SVertex & vertex = this->GetVertex(dwVertexIndex);
//	vertex.vPosition.y += ;
//}

//////////////////////////////////////////////////////////////////////////

void CTerrainMesh::SetWireFrame(bool b)
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	rd::CRenderDevice::Layer3D * pLayer3d = pRender->GetLayer3D();
	pLayer3d->GetShader(m_dwShaderHandle)->bWireframe = b;
}

//////////////////////////////////////////////////////////////////////////

bool CTerrainMesh::IsValid(D3DXVECTOR3 v)
{
	return ( 0.0 <= v.x && v.x < this->GetWidth() &&
			 0.0 <= v.z && v.z < this->GetDepth() );
}

bool CTerrainMesh::IsValid(int nGridIndex)
{
	return ( nGridIndex > 0 && nGridIndex < this->GetDepth() * this->GetWidth() ); 
}

//////////////////////////////////////////////////////////////////////////

void CTerrainMesh::SaveAsLocal(std::string strFileName)
{
/*
	FILE * fp = fopen(strFileName.c_str(),"wb");
	
	if ( fp )
	{
		// header
		fwrite("LOCL",4,1,fp);
		
		// version
		DWORD dwVersion = LOCAL_VERSION;
		fwrite(&dwVersion,sizeof(DWORD),1,fp);
		
		// camera
		CCamera * pCamera = eg::CEngine::GetInstance()->GetActiveCamera();
		D3DXVECTOR3 vView = pCamera->GetView();
		fwrite(&vView,sizeof(D3DXVECTOR3),1,fp);

		// width depth
		DWORD dwWidth = this->GetWidthInRegion();
		DWORD dwDepth = this->GetDepthInRegion();
		fwrite(&dwWidth,sizeof(DWORD),1,fp);			
		fwrite(&dwDepth,sizeof(DWORD),1,fp);
		
		// vertices
		fwrite(m_pVertices, sizeof(SVertex) * m_dwVertexCnt,1,fp);
		
		// grids
		for ( int i = 0; i< m_dwGridCnt; i++ )
		{
			SGrid * pGrid = &m_pGrids[i];
			fwrite(&pGrid->dwGridIndex,sizeof(DWORD),1,fp);		
			fwrite(&pGrid->dwVertexIndex,sizeof(pGrid->dwVertexIndex),1,fp);		
			fwrite(&pGrid->uv,sizeof(pGrid->uv),1,fp);	
			fwrite(&pGrid->nodeInfo,sizeof(pGrid->nodeInfo),1,fp);
			fwrite(&pGrid->byFlag,sizeof(pGrid->byFlag),1,fp);
			fwrite(&pGrid->vCenter,sizeof(pGrid->vCenter),1,fp);		
			
			// central  object
			if ( !pGrid->vecCenterObjects.empty() )
			{
				DWORD dw = pGrid->vecCenterObjects.size();
				fwrite(&dw,sizeof(DWORD),1,fp);
				
				for ( std::vector< Ptr<CTerrainMesh::CObject> >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
				{
					Ptr<CObject> p = (*iter);
					const char * pName = p->GetFace()->GetSelfPresentation()->GetName().c_str();
					DWORD dw = strlen(pName);
					fwrite(&dw,sizeof(int),1,fp);
					fwrite(pName,dw,1,fp);
					
					bool bSubUnit = p->GetFace()->GetSelfPresentation()->IsSubUnit();
					fwrite(&bSubUnit,sizeof(bool),1,fp);
					
					// nPhysicalCenterGridIndex
					int nPhysicalCenterGridIndex = p->GetFace()->GetSelfPresentation()->GetPhysicalCenterGridIndex();
					fwrite(&nPhysicalCenterGridIndex,sizeof(int),1,fp);
					
					DWORD dwBlockGridCnt = p->GetFace()->GetSelfPresentation()->GetBlockSize();
					fwrite(&dwBlockGridCnt,sizeof(DWORD),1,fp);
					
					for ( int i = 0; i < dwBlockGridCnt; i++ )
					{
						int nBlockGridIndex = p->GetFace()->GetSelfPresentation()->GetBlockGridIndex(i);
						fwrite(&nBlockGridIndex,sizeof(int),1,fp);
					}
										
					// fLift
					float fLift = p->GetFace()->GetSelfPresentation()->GetLift();
					fwrite(&fLift,sizeof(float),1,fp);
					
					// fRotation
					float fRotation = p->GetFace()->GetSelfPresentation()->GetRotation();
					fwrite(&fRotation,sizeof(float),1,fp);
				}
			}
			else
			{
				DWORD dw = 0;
				fwrite(&dw,sizeof(int),1,fp);
			}
		}

		// texture streams
		const char * p = NULL;
		int nTextureHandle = 0;
		while(true)
		{
			p = CSharedTexture::GetTextureFileNameByTextureHandle(nTextureHandle);
			if ( p == NULL ) break;
			nTextureHandle++;
		}

		fwrite(&nTextureHandle,sizeof(int),1,fp);
		
		for ( int i = 0; i < nTextureHandle; i++ )
		{
			const char * p = CSharedTexture::GetTextureFileNameByTextureHandle(i);
			int n = strlen(p);
			fwrite(&n,sizeof(int),1,fp);
			fwrite(p,n,1,fp);
		}

		// logic objects
//		::SendMessage(m_hClassView,WM_CLASSVIEWSAVEASLOCAL,(WPARAM)fp,0);
		
		fclose(fp);
	}
	else
	{
		MessageBox(NULL,"写入文件错误","错误",MB_OK);
	}

*/
}

//////////////////////////////////////////////////////////////////////////

void CTerrainMesh::LoadAsLocal(std::string strFileName)
{
/*
	FILE * fp = fopen(strFileName.c_str(),"rb");
	
	// header
	char szHeader[4] = "";
	fread(szHeader,4,1,fp);
	if ( *((DWORD*)szHeader) != MAKEFOURCC('L','O','C','L') )
	{
		MessageBox(NULL,"文件类型不符","错误",MB_OK);
		fclose(fp);
		return;
	}
	
	// version
	DWORD dwVersion;
	fread(&dwVersion,sizeof(DWORD),1,fp);
	if ( dwVersion != LOCAL_VERSION )
	{
		MessageBox(NULL,"文件版本不对","错误",MB_OK);
		fclose(fp);
		return;
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	// camera
	
	D3DXVECTOR3 vView;
	fread(&vView,sizeof(D3DXVECTOR3),1,fp);
	CCamera * pCamera = eg::CEngine::GetInstance()->GetActiveCamera();
	pCamera->SetView(vView);
	pCamera->SetFOVY(STDFOV);
	pCamera->SetPitchAndYaw(STDPITCH,STDYAW);
	
	//////////////////////////////////////////////////////////////////////////
	
	// width depth
	DWORD dwWidth;
	DWORD dwDepth;
	fread(&dwWidth,sizeof(DWORD),1,fp);			
	fread(&dwDepth,sizeof(DWORD),1,fp);
	
	// create a new terrain
	DWORD dwTextureHandleDefault = CSharedTexture::GetTexture("texture\\Default.tga")->GetTextureHandle();
	CTerrainMesh * pTerrain = CSceneManager::GetInstance()->AddNodeTerrainMesh(dwWidth,dwDepth,dwTextureHandleDefault);
	CTileEditApp::GetInstance()->SetTerrainMesh(pTerrain);
		
	//////////////////////////////////////////////////////////////////////////
		
	// mesh info
	
	// vertices
	fread(pTerrain->m_pVertices,sizeof(SVertex)*pTerrain->m_dwVertexCnt,1,fp);
	
	// grids
	for ( int i = 0; i< pTerrain->m_dwGridCnt; i++ )
	{
		SGrid * pGrid = &pTerrain->m_pGrids[i];
		
		fread(&pGrid->dwGridIndex,sizeof(int),1,fp);
		fread(&pGrid->dwVertexIndex,sizeof(pGrid->dwVertexIndex),1,fp);		
		fread(&pGrid->uv,sizeof(pGrid->uv),1,fp);	
		fread(&pGrid->nodeInfo,sizeof(pGrid->nodeInfo),1,fp);
		fread(&pGrid->byFlag,sizeof(pGrid->byFlag),1,fp);
		fread(&pGrid->vCenter,sizeof(pGrid->vCenter),1,fp);
		
		// any central Object?
		DWORD dw;
		fread(&dw,sizeof(DWORD),1,fp);
		
		if ( dw != 0 )
		{
			for ( int i = 0; i < dw; i++ )
			{
				DWORD dwFileNameLen;
				fread(&dwFileNameLen,sizeof(DWORD),1,fp);
				std::string strFileName;
				strFileName.resize(dwFileNameLen);
				fread((char*)strFileName.data(),dwFileNameLen,1,fp);
				bool bSubUnit;
				fread(&bSubUnit,sizeof(bool),1,fp);
				
				CModel * pModel = CSceneManager::GetInstance()->AddNodeObject(strFileName, bSubUnit);
				
				Ptr<CObject> pObject = new CObject;           
				CTerrainMesh::CFace * pFace = new CFace;
				pFace->AddSelfPresentation(pModel);
				pObject->SetFace(pFace);
				
				pGrid->vecCenterObjects.push_back(pObject);
				
				// nPhysicalCenterGridIndex
				int nPhysicalCenterGridIndex;
				fread(&nPhysicalCenterGridIndex,sizeof(int),1,fp);
								
				//////////////////////////////////////////////////////////////////////////
							
				DWORD dwBlockGridCnt;
				fread(&dwBlockGridCnt,sizeof(DWORD),1,fp);
				
				for ( int j = 0; j < dwBlockGridCnt; j++ )
				{
					int nBlockGridIndex;
					fread(&nBlockGridIndex,sizeof(int),1,fp);
					SGrid * pGrid2 = &pTerrain->m_pGrids[nBlockGridIndex];
					pGrid2->vecObjects.push_back(pObject);
					pObject->GetFace()->GetSelfPresentation()->AddBlockGridIndex(nBlockGridIndex);
					float f = pObject->GetFace()->GetSelfPresentation()->GetGridLogicHeight(j);
					pGrid2->vecLogicHeights.push_back(make_pair(f,pObject));
				}
				
				// fLift and fRotation
				float fLift,fRotation;
				fread(&fLift,sizeof(float),1,fp);
				fread(&fRotation,sizeof(float),1,fp);
				
				pModel->RestoreLift(fLift);
				pModel->RestoreRotation(fRotation);
				pModel->RestoreOffset();
				pModel->RestoreCenter();
				pObject->SetCenterGridIndex(nPhysicalCenterGridIndex);
				pModel->UpdateBoundingBox();
				pModel->SetEditState(CTileEditorModel::EES_PLACED);
			}
		}
	}
	
	CSharedTexture::Clear();
	
	DWORD dwCnt;
	fread(&dwCnt,sizeof(int),1,fp);
	
	for ( int i = 0; i < dwCnt; i++ )
	{
		DWORD dwFileNameLen;
		fread(&dwFileNameLen,sizeof(DWORD),1,fp);
		std::string strFileName;
		strFileName.resize(dwFileNameLen);
		fread((char*)strFileName.data(),dwFileNameLen,1,fp);
		CSharedTexture::GetTexture(strFileName.c_str());
	}
	
	pTerrain->RebuildRenderTree();
	
	CTileEditApp::GetInstance()->GetOperator()->SetSelectMode(true);
	
	// reload operator
	CTileEditApp::GetInstance()->GetOperator()->InitProcessParams();
	
   	bc::ut::CTimer::GetInstance()->ResumeUpdate();
	bc::ut::CTimer::GetInstance()->Start();

*/
}

//////////////////////////////////////////////////////////////////////////

void CTerrainMesh::SaveAsClient(std::string strFileName)
{

/*
	// map file
	std::string strMapFileName = "map\\" + strFileName + ".amp";
	
	FILE * fp = fopen(strMapFileName.c_str(),"wb");
	
	if ( fp )
	{
		// header
		fwrite("AMAP",4,1,fp);
		
		// version
		int nVersion = CLIENT_VERSION; // CLIENT_VERSION == 0x01 
		fwrite(&nVersion,sizeof(int),1,fp);
		
		// desc
		char s[64] = "测试地图";
		fwrite(s,64,1,fp);

		// width depth in region
		WORD wWidth = this->GetWidthInRegion();
		WORD wDepth = this->GetDepthInRegion();
		fwrite(&wWidth,sizeof(WORD),1,fp);			
		fwrite(&wDepth,sizeof(WORD),1,fp);

		// REGION_SPACE
		WORD w = REGION_SPACE;
		fwrite(&w,sizeof(WORD),1,fp);		

		// GRID_SPACE
		w = GRID_SPACE;
		fwrite(&w,sizeof(WORD),1,fp);			
		
		// Tileset
		fwrite(s,64,1,fp);

		//////////////////////////////////////////////////////////////////////////

		DWORD dwAmbientColor = 0;			
		fwrite(&dwAmbientColor,sizeof(DWORD),1,fp);
		
		DWORD dwWeatherParam = 0;			
		fwrite(&dwWeatherParam,sizeof(DWORD),1,fp);
		
		DWORD dwWindParam = 0;			
		fwrite(&dwWindParam,sizeof(DWORD),1,fp);
		
		//////////////////////////////////////////////////////////////////////////
		
		BYTE b = 0;
		fwrite(&b,sizeof(BYTE),1,fp);
		fwrite(&b,sizeof(BYTE),1,fp);

		//////////////////////////////////////////////////////////////////////////
		
		float f = D3DXToRadian(STDPITCH);
		fwrite(&f,sizeof(float),1,fp);
		
		f = D3DXToRadian(STDYAW);
		fwrite(&f,sizeof(float),1,fp);
		
		f = D3DXToRadian(STDFOV);
		fwrite(&f,sizeof(float),1,fp);
		
		f = STDDISTANCE;
		fwrite(&f,sizeof(float),1,fp);
		
		//////////////////////////////////////////////////////////////////////////
		
		// texture streams
		const char * p = NULL;
		int nTextureHandle = 0;
		
		while(true)
		{
			p = CSharedTexture::GetTextureFileNameByTextureHandle(nTextureHandle);
			if ( p == NULL ) break;
			nTextureHandle++;
		}
		
		fwrite(&nTextureHandle,sizeof(int),1,fp);
		
		for ( int i = 0; i < nTextureHandle; i++ )
		{
			const char * p = CSharedTexture::GetTextureFileNameByTextureHandle(i);
			int n = strlen(p);
			fwrite(&n,sizeof(int),1,fp);
			fwrite(p,n,1,fp);
		}
		
		fclose(fp);
	}
	
	// region file
	char strbuf[MAX_PATH];
	std::string strFilePath = "map\\" + strFileName;
	::CreateDirectory(strFilePath.c_str(),NULL);

	std::string strRgnFileName = strFilePath + "\\" + strFileName;
			
	for ( int i = 0; i < this->GetRegionCount(); ++i )
	{
		int nReg = i;
		CTerrainMesh::SRegion & reg = CTileEditApp::GetInstance()->GetTerrainMesh()->GetRegion(i);
		
		int nX = i%this->GetWidthInRegion();
		int nY = i/this->GetWidthInRegion();
		strcpy(strbuf,strRgnFileName.c_str());
		sprintf(strbuf+strRgnFileName.length(),"_%.3d_%.3d_c.arg",nX,nY);
		
		FILE * fp = fopen(strbuf,"wb");
		
		if ( fp )
		{
			// header
			fwrite("REGN",4,1,fp);
			
			// version
			DWORD dwVersion = CLIENT_VERSION;
			fwrite(&dwVersion,sizeof(DWORD),1,fp);
			
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			
			// chunk count
			DWORD dwDataChunkCnt = 5;			
			fwrite(&dwDataChunkCnt,sizeof(DWORD),1,fp);

			// chunk name
			fwrite("BARR",4,1,fp);

			// chunk offset and size
			long lBarrPt = ftell(fp); 
			DWORD dwPad = 0;
			fwrite(&dwPad,sizeof(DWORD),1,fp); // offset
			fwrite(&dwPad,sizeof(DWORD),1,fp); // size

			// chunk name
			fwrite("TRAP",4,1,fp);

			// chunk offset and size
			long lTrapPt = ftell(fp); 
			fwrite(&dwPad,sizeof(DWORD),1,fp); // offset
			fwrite(&dwPad,sizeof(DWORD),1,fp); // size

			// chunk name
			fwrite("OBJT",4,1,fp);

			// chunk offset and size
			long lObjtPt = ftell(fp); 
			fwrite(&dwPad,sizeof(DWORD),1,fp); // offset
			fwrite(&dwPad,sizeof(DWORD),1,fp); // size

			// chunk name
			fwrite("VERT",4,1,fp);
			
			// chunk offset and size
			long lVertPt = ftell(fp); 
			fwrite(&dwPad,sizeof(DWORD),1,fp); // offset
			fwrite(&dwPad,sizeof(DWORD),1,fp); // size
			
			// chunk name
			fwrite("GRID",4,1,fp);
			
			// chunk offset and size
			long lGridPt = ftell(fp); 
			fwrite(&dwPad,sizeof(DWORD),1,fp); // offset
			fwrite(&dwPad,sizeof(DWORD),1,fp); // size
			
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			
			// barr chunk
			
			// BarrOffset backfill
			long lBarrOffset = ftell(fp);
			fseek(fp,lBarrPt,SEEK_SET);
			fwrite(&lBarrOffset,sizeof(long),1,fp); // size
			fseek(fp,lBarrOffset,SEEK_SET);
			
			// chunk raw data
			DWORD dwGridCntPerRegion = (REGION_SPACE)*(REGION_SPACE);
			
			// cnt
			fwrite(&dwGridCntPerRegion,sizeof(DWORD),1,fp);
			
			for ( int i = 0; i < dwGridCntPerRegion; i++ )
			{
				SGrid * pGrid = &m_pGrids[reg.vGridIndices[i]];
				BYTE byFlag;
				float fHeight;

				if ( pGrid->vecObjects.empty() )
				{
					byFlag = 0;  // Null;
				}
				else
				{
					byFlag = 1; // Normal;
				}

				// @ add build height onto it
				fHeight = pGrid->vCenter.y;  
				fwrite(&byFlag,sizeof(BYTE),1,fp);
				fwrite(&fHeight,sizeof(float),1,fp);
			}
			
			// BarrSize backfill
			long lBarrSize = ftell(fp) - lBarrOffset;
			fseek(fp,lBarrPt+sizeof(long),SEEK_SET);
			fwrite(&lBarrSize,sizeof(long),1,fp); // size
			fseek(fp,lBarrSize+lBarrOffset,SEEK_SET);
			
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			
			// trap chunk
			
			// TrapOffset backfill
			long lTrapOffset = ftell(fp);
			fseek(fp,lTrapPt,SEEK_SET);
			fwrite(&lTrapOffset,sizeof(long),1,fp); // size
			fseek(fp,lTrapOffset,SEEK_SET);

			// chunk raw data
			fwrite(&dwGridCntPerRegion,sizeof(DWORD),1,fp);
			
			for ( int i = 0; i < dwGridCntPerRegion; i++ )
			{
				fwrite(&dwPad,sizeof(DWORD),1,fp);
			}
			
			// TrapSize backfill
			long lTrapSize = ftell(fp) - lTrapOffset;
			fseek(fp,lTrapPt+sizeof(long),SEEK_SET);
			fwrite(&lTrapSize,sizeof(long),1,fp); // size
			fseek(fp,lTrapSize+lTrapOffset,SEEK_SET);
			
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			
			// object chunk
			
			// ObjtOffset backfill
			long lObjtOffset = ftell(fp);
			fseek(fp,lObjtPt,SEEK_SET);
			fwrite(&lObjtOffset,sizeof(long),1,fp); // size
			fseek(fp,lObjtOffset,SEEK_SET);

			// chunk raw data
			::SendMessage(m_hClassView,WM_CLASSVIEWSAVEASCLIENT,(WPARAM)fp,(LPARAM)nReg);
			
			// ObjtSize backfill
			long lObjtSize = ftell(fp) - lObjtOffset;
			fseek(fp,lObjtPt+sizeof(long),SEEK_SET);
			fwrite(&lObjtSize,sizeof(long),1,fp); // size
			fseek(fp,lObjtSize+lObjtOffset,SEEK_SET);
			
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
						
			// vert chunk

			// VertOffset backfill
			long lVertOffset = ftell(fp);
			fseek(fp,lVertPt,SEEK_SET);
			fwrite(&lVertOffset,sizeof(long),1,fp); // size
			fseek(fp,lVertOffset,SEEK_SET);

			// chunk raw data
						
			// vertices cnt
			DWORD dwVertexCntPerRegion = (REGION_SPACE+1)*(REGION_SPACE+1);
			
			fwrite(&dwVertexCntPerRegion,sizeof(DWORD),1,fp);
			
			for ( int i = 0; i < dwVertexCntPerRegion; i++ )
			{
				CTerrainMesh::SVertex vertex = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(reg.vVertexIndices[i]);
				fwrite(&vertex,sizeof(CTerrainMesh::SVertex),1,fp);
			}
			
			// VertSize backfill
			long lVertSize = ftell(fp) - lVertOffset;
			fseek(fp,lVertPt+sizeof(long),SEEK_SET);
			fwrite(&lVertSize,sizeof(long),1,fp); // size
			fseek(fp,lVertSize+lVertOffset,SEEK_SET);
			
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			
			// grid chunk
			
			// GridOffset backfill
			long lGridOffset = ftell(fp);
			fseek(fp,lGridPt,SEEK_SET);
			fwrite(&lGridOffset,sizeof(long),1,fp); // size
			fseek(fp,lGridOffset,SEEK_SET);
			
			// grid cnt
			fwrite(&dwGridCntPerRegion,sizeof(DWORD),1,fp);
			
			for ( int i = 0; i < dwGridCntPerRegion; i++ )
			{
				SGrid * pGrid = &m_pGrids[reg.vGridIndices[i]];
				
				// .1
				// fwrite(&pGrid->dwGridIndex,sizeof(DWORD),1,fp);		
				
				// .1
				// unsigned short wVertexIndex = this->MapVertexIndexToRegion(pGrid->dwVertexIndex[0],nReg);
				// fwrite(&wVertexIndex,sizeof(unsigned short),1,fp);		
				//
				// wVertexIndex = this->MapVertexIndexToRegion(pGrid->dwVertexIndex[1],nReg);
				// fwrite(&wVertexIndex,sizeof(unsigned short),1,fp);		
				//
				// wVertexIndex = this->MapVertexIndexToRegion(pGrid->dwVertexIndex[2],nReg);
				// fwrite(&wVertexIndex,sizeof(unsigned short),1,fp);		
				//
				// wVertexIndex = this->MapVertexIndexToRegion(pGrid->dwVertexIndex[3],nReg);
				// fwrite(&wVertexIndex,sizeof(unsigned short),1,fp);	
				
				fwrite(&pGrid->uv,sizeof(pGrid->uv),1,fp);	
				fwrite(&pGrid->nodeInfo,sizeof(pGrid->nodeInfo),1,fp);
				
				// .1
				// fwrite(&pGrid->byFlag,sizeof(pGrid->byFlag),1,fp);
				
				// .1
				// fwrite(&pGrid->vCenter,sizeof(pGrid->vCenter),1,fp);		
				
				// central  object
				if ( !pGrid->vecCenterObjects.empty() && !pGrid->vecCenterObjects[0]->GetIsServerBrainBound() )
				{
					DWORD dw = pGrid->vecCenterObjects.size();
					fwrite(&dw,sizeof(DWORD),1,fp);
				
					for ( std::vector< Ptr<CTerrainMesh::CObject> >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
					{
						Ptr<CObject> p = (*iter);
						const char * pName = p->GetFace()->GetSelfPresentation()->GetName().c_str();
						DWORD dw = strlen(pName);
						fwrite(&dw,sizeof(int),1,fp);
						fwrite(pName,dw,1,fp);
					
						bool bSubUnit = p->GetFace()->GetSelfPresentation()->IsSubUnit();
						fwrite(&bSubUnit,sizeof(bool),1,fp);
					
						// nPhysicalCenterGridIndex
						int nPhysicalCenterGridIndex = p->GetFace()->GetSelfPresentation()->GetPhysicalCenterGridIndex();
						fwrite(&nPhysicalCenterGridIndex,sizeof(int),1,fp);
					
						DWORD dwBlockGridCnt = p->GetFace()->GetSelfPresentation()->GetBlockSize();
						fwrite(&dwBlockGridCnt,sizeof(DWORD),1,fp);
						
						for ( int i = 0; i < dwBlockGridCnt; i++ )
						{
							int nBlockGridIndex = p->GetFace()->GetSelfPresentation()->GetBlockGridIndex(i);
							fwrite(&nBlockGridIndex,sizeof(int),1,fp);
						}
						
						// fLift
						float fLift = p->GetFace()->GetSelfPresentation()->GetLift();
						fwrite(&fLift,sizeof(float),1,fp);
						
						// fRotation
						float fRotation = p->GetFace()->GetSelfPresentation()->GetRotation();
						fwrite(&fRotation,sizeof(float),1,fp);
					}
				}
				else
				{
					DWORD dw = 0;
					fwrite(&dw,sizeof(int),1,fp);
				}
			}
			
			// GridSize backfill
			long lGridSize = ftell(fp) - lGridOffset;
			fseek(fp,lGridPt+sizeof(long),SEEK_SET);
			fwrite(&lGridSize,sizeof(long),1,fp); // size
			fseek(fp,lGridSize+lGridOffset,SEEK_SET);

			//////////////////////////////////////////////////////////////////////////
						
			fclose(fp);
		}
		else
		{
			MessageBox(NULL,"写入文件错误","错误",MB_OK);
		}
	}

*/
}

void CTerrainMesh::SaveAsServer(std::string strFileName)
{
/*
	// map file
	std::string strMapFileName = "map\\" + strFileName + ".amp";
	
	FILE * fp = fopen(strMapFileName.c_str(),"wb");
	
	if ( fp )
	{
		// header
		fwrite("AMAP",4,1,fp);
		
		// version
		int nVersion = CLIENT_VERSION; // CLIENT_VERSION == 0x01 
		fwrite(&nVersion,sizeof(int),1,fp);
	
		// desc
		char s[64] = "测试地图";
		fwrite(s,64,1,fp);
		
		// width depth in region
		WORD wWidth = this->GetWidthInRegion();
		WORD wDepth = this->GetDepthInRegion();
		fwrite(&wWidth,sizeof(WORD),1,fp);			
		fwrite(&wDepth,sizeof(WORD),1,fp);
		
		// REGION_SPACE
		WORD w = REGION_SPACE;
		fwrite(&w,sizeof(WORD),1,fp);		
		
		// GRID_SPACE
		w = GRID_SPACE;
		fwrite(&w,sizeof(WORD),1,fp);			
		
		// Tileset
		fwrite(s,64,1,fp);
		
		//////////////////////////////////////////////////////////////////////////
		
		DWORD dwAmbientColor = 0;			
		fwrite(&dwAmbientColor,sizeof(DWORD),1,fp);
		
		DWORD dwWeatherParam = 0;			
		fwrite(&dwWeatherParam,sizeof(DWORD),1,fp);
		
		DWORD dwWindParam = 0;			
		fwrite(&dwWindParam,sizeof(DWORD),1,fp);
		
		//////////////////////////////////////////////////////////////////////////
		
		BYTE b = 0;
		fwrite(&b,sizeof(BYTE),1,fp);
		fwrite(&b,sizeof(BYTE),1,fp);
		
		//////////////////////////////////////////////////////////////////////////
		
		float f = D3DXToRadian(STDPITCH);
		fwrite(&f,sizeof(float),1,fp);

		f = D3DXToRadian(STDYAW);
		fwrite(&f,sizeof(float),1,fp);

		f = D3DXToRadian(STDFOV);
		fwrite(&f,sizeof(float),1,fp);

		f = STDDISTANCE;
		fwrite(&f,sizeof(float),1,fp);

		//////////////////////////////////////////////////////////////////////////
		
		fclose(fp);
	}
	
	// region file
	char strbuf[MAX_PATH];
	std::string strFilePath = "map\\" + strFileName;
	::CreateDirectory(strFilePath.c_str(),NULL);

	std::string strRgnFileName = strFilePath + "\\" + strFileName;
	
	for ( int i = 0; i < this->GetRegionCount(); ++i )
	{
		int nReg = i;
		CTerrainMesh::SRegion & reg = CTileEditApp::GetInstance()->GetTerrainMesh()->GetRegion(i);
		
		int nX = i%this->GetWidthInRegion();
		int nY = i/this->GetWidthInRegion();
		strcpy(strbuf,strRgnFileName.c_str());
		sprintf(strbuf+strRgnFileName.length(),"_%.3d_%.3d_s.arg",nX,nY);
		
		FILE * fp = fopen(strbuf,"wb");
		
		if ( fp )
		{
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			
			// header
			fwrite("REGN",4,1,fp);

			// version
			DWORD dwVersion = CLIENT_VERSION;
			fwrite(&dwVersion,sizeof(DWORD),1,fp);
			
			// chunk count
			DWORD dwDataChunkCnt = 3;			
			fwrite(&dwDataChunkCnt,sizeof(DWORD),1,fp);
			
			// chunk name
			fwrite("BARR",4,1,fp);

			// chunk offset and size
			long lBarrPt = ftell(fp); 
			DWORD dwPad = 0;
			fwrite(&dwPad,sizeof(DWORD),1,fp); // offset
			fwrite(&dwPad,sizeof(DWORD),1,fp); // size
		
			// chunk name
			fwrite("TRAP",4,1,fp);

			// chunk offset and size
			long lTrapPt = ftell(fp); 
			fwrite(&dwPad,sizeof(DWORD),1,fp); // offset
			fwrite(&dwPad,sizeof(DWORD),1,fp); // size

			// chunk name
			fwrite("OBJT",4,1,fp);

			// chunk offset and size
			long lObjtPt = ftell(fp); 
			fwrite(&dwPad,sizeof(DWORD),1,fp); // offset
			fwrite(&dwPad,sizeof(DWORD),1,fp); // size

			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			
			// barr chunk

			// BarrOffset backfill
			long lBarrOffset = ftell(fp);
			fseek(fp,lBarrPt,SEEK_SET);
			fwrite(&lBarrOffset,sizeof(long),1,fp); // size
			fseek(fp,lBarrOffset,SEEK_SET);
			
			// chunk raw data
			DWORD dwGridCntPerRegion = (REGION_SPACE)*(REGION_SPACE);
			
			fwrite(&dwGridCntPerRegion,sizeof(DWORD),1,fp);
			
			for ( int i = 0; i < dwGridCntPerRegion; i++ )
			{
				SGrid * pGrid = &m_pGrids[reg.vGridIndices[i]];
				BYTE byFlag;
				float fHeight;
				
				if ( pGrid->vecObjects.empty() )
				{
					byFlag = 0;  // Null;
				}
				else
				{
					byFlag = 1; // Normal;
				}

				// @ add build height onto it
				fHeight = pGrid->vCenter.y;  
				fwrite(&byFlag,sizeof(BYTE),1,fp);
				fwrite(&fHeight,sizeof(float),1,fp);
			}

			// BarrSize backfill
			long lBarrSize = ftell(fp) - lBarrOffset;
			fseek(fp,lBarrPt+sizeof(long),SEEK_SET);
			fwrite(&lBarrSize,sizeof(long),1,fp); // size
			fseek(fp,lBarrSize+lBarrOffset,SEEK_SET);
			
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			
			// trap chunk
			
			// TrapOffset backfill
			long lTrapOffset = ftell(fp);
			fseek(fp,lTrapPt,SEEK_SET);
			fwrite(&lTrapOffset,sizeof(long),1,fp); // size
			fseek(fp,lTrapOffset,SEEK_SET);
			
			// chunk raw data
			fwrite(&dwGridCntPerRegion,sizeof(DWORD),1,fp);
			
			for ( int i = 0; i < dwGridCntPerRegion; i++ )
			{
				fwrite(&dwPad,sizeof(DWORD),1,fp);
			}
			
			// TrapSize backfill
			long lTrapSize = ftell(fp) - lTrapOffset;
			fseek(fp,lTrapPt+sizeof(long),SEEK_SET);
			fwrite(&lTrapSize,sizeof(long),1,fp); // size
			fseek(fp,lTrapSize+lTrapOffset,SEEK_SET);
			
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			
			// object chunk
			
			// ObjtOffset backfill
			long lObjtOffset = ftell(fp);
			fseek(fp,lObjtPt,SEEK_SET);
			fwrite(&lObjtOffset,sizeof(long),1,fp); // size
			fseek(fp,lObjtOffset,SEEK_SET);
			
			// chunk raw data
			::SendMessage(m_hClassView,WM_CLASSVIEWSAVEASSERVER,(WPARAM)fp,(LPARAM)nReg);
						
			// ObjtSize backfill
			long lObjtSize = ftell(fp) - lObjtOffset;
			fseek(fp,lObjtPt+sizeof(long),SEEK_SET);
			fwrite(&lObjtSize,sizeof(long),1,fp); // size
			fseek(fp,lObjtSize+lObjtOffset,SEEK_SET);
			
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			
			fclose(fp);
		}
	}

*/
}

int CTerrainMesh::MapVertexIndexToRegion(int nVertexIndex, int nRegionIndex)
{
	int nRegionY = nRegionIndex/this->GetWidthInRegion() * REGION_SPACE;
	int nRegionX = nRegionIndex%this->GetWidthInRegion() * REGION_SPACE; 
	int nVertexX = nVertexIndex%(this->GetWidth()+1);
	int nVertexY = nVertexIndex/(this->GetWidth()+1);
	int nX = nVertexX - nRegionX;
	int nY = nVertexY - nRegionY;
	return nX + nY * (REGION_SPACE+1);
}

}
}