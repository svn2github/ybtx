//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "extradefine.h"
#include "Picker.h"
#include "Input.h"
#include "Render.h"
#include "Camera.h"
#include "Engine.h"
#include "BluelyGUI.h"
#include "CTileEditApp.h"
#include "TerrainMesh.h"
#include "../TileEditorEngine/BluelyMath.h"
#include "VisibleSet.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bg
{
namespace sc
{

CPicker * CPicker::one = NULL;

CPicker::CPicker()
{ 
	m_pLastSelected = NULL;
}

void CPicker::SetLastSelected(CTerrainMesh::CObject * p)
{
	m_pLastSelected = p;
}

CPicker::~CPicker() { }


D3DXVECTOR2 CPicker::GetGridsCnt()
{
	POINT ptLeft = {0,768/2};
	D3DXVECTOR3 vLeft = this->GetSpecIntersectedPos(ptLeft);
	POINT ptRight = {1024,768/2};
	D3DXVECTOR3 vRight = this->GetSpecIntersectedPos(ptRight);
	float x = sqrt((vRight.x-vLeft.x)*(vRight.x-vLeft.x)+(vRight.z-vLeft.z)*(vRight.z-vLeft.z))/GRID_SPACE;
	POINT ptUp = {1024/2,0};
	D3DXVECTOR3 vUp = this->GetSpecIntersectedPos(ptUp);
	POINT ptDown = {1024/2,768};
	D3DXVECTOR3 vDown = this->GetSpecIntersectedPos(ptDown);
	float y = sqrt((vUp.x-vDown.x)*(vUp.x-vDown.x)+(vUp.z-vDown.z)*(vUp.z-vDown.z))/GRID_SPACE;
	return D3DXVECTOR2(x,y);
}

D3DXVECTOR3 CPicker::GetSpecIntersectedPos(POINT pt)
{
	LPDIRECT3DDEVICE9 pDevice = rd::CRenderDevice::GetInstance()->GetRawDevice();
	
	D3DXVECTOR3 vec1, vec2;
	D3DXMATRIX matProj, matView, matWorld;
	D3DVIEWPORT9 viewport;
	
	CCamera * pCamera = eg::CEngine::GetInstance()->GetActiveCamera();
	
	matProj = *pCamera->GetProjectionMatrix();
	
	matView = *pCamera->GetViewMatrix();
	
	D3DXMatrixIdentity(&matWorld);
	
	pDevice->GetViewport(&viewport);
	
	vec1 = eg::CEngine::GetInstance()->GetActiveCamera()->GetPosition();
	
	D3DXVec3Unproject(&vec2,&D3DXVECTOR3(static_cast<float>(pt.x), static_cast<float>(pt.y), 0.9999f),
		&viewport, &matProj, &matView, &matWorld);
	
	D3DXVECTOR3 vecEnd;
	D3DXPlaneIntersectLine(&vecEnd, 
		&D3DXPLANE(0.0, 1.0, 0.0, 0.0), &vec1, &vec2);
	
	return vecEnd;
}

//bool CPicker::GetHitPoint(D3DXVECTOR3 & vHitPoint)
//{
//	const DWORD SEARCH_TOLERANCE = 100;
//	
//	LPDIRECT3DDEVICE9 pDevice = rd::CRenderDevice::GetInstance()->GetRawDevice();
//	
//	POINT pt;
//	ip::CInputDevice::GetInstance()->GetMousePosition((int&)pt.x,(int&)pt.y);
//	
//	D3DXVECTOR3 vec;
//	D3DXMATRIX matProj, matView, matWorld;
//	D3DVIEWPORT9 viewport;
//	
//	CCamera * pCamera = eg::CEngine::GetInstance()->GetActiveCamera();
//	
//	matProj = *pCamera->GetProjectionMatrix();
//
//	matView = *pCamera->GetViewMatrix();
//
//	D3DXMatrixIdentity(&matWorld);
//	
//	pDevice->GetViewport(&viewport);
//
//	D3DXVec3Unproject(&vec,&D3DXVECTOR3(static_cast<float>(pt.x), static_cast<float>(pt.y), 0.9999f),
//		&viewport, &matProj, &matView, &matWorld);
//
//	//////////////////////////////////////////////////////////////////////////
//	
//	D3DXVECTOR3 vStart = eg::CEngine::GetInstance()->GetActiveCamera()->GetPosition();
//	
//	D3DXVECTOR3 vEnd =  vec;
//	
//	float tg = (vEnd.z - vStart.z)/(vEnd.x - vStart.x);
//	
//	D3DXVECTOR3 vNew(vStart);
//	
//	vNew.x = vNew.x/GRID_SPACE;
//	vNew.z = vNew.z/GRID_SPACE;
//	
//	DWORD dwGrid;
//	
//	std::vector<int> vecGridIndices;
//	
//	if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
//	{
//		dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//		vecGridIndices.push_back(dwGrid);
//	}
//	
//	if ( vStart.z < vEnd.z )
//	{
//		if ( 0.0f < tg )
//		{
//			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
//			{
//				if ( tg <= 1.0f )
//				{
//					vNew.x += 1.0f;
//					vNew.z += tg;
//				}
//				else
//				{
//					vNew.z += 1.0f;
//					vNew.x += 1/tg;
//				}
//
//				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
//				{
//					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//					vecGridIndices.push_back(dwGrid);
//					
//					// right
//					dwGrid += 1;
//					vecGridIndices.push_back(dwGrid);
//
//					// upright
//					dwGrid += CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//					vecGridIndices.push_back(dwGrid);
//
//					// up
//					dwGrid -= 1;
//					vecGridIndices.push_back(dwGrid);
//				}
//			}
//		}
//		else // tg < 0.0f
//		{
//			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
//			{
//				if ( tg >= -1.0f )
//				{
//					vNew.x -= 1.0f;
//					vNew.z += -tg;
//
//				}
//				else  // < -1.0f
//				{
//					vNew.z += 1.0f;
//					vNew.x -= -1/tg;
//				}
//
//				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
//				{
//					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//					vecGridIndices.push_back(dwGrid);
//
//					// left
//					dwGrid -= 1;
//					vecGridIndices.push_back(dwGrid);
//
//					// upleft
//					dwGrid += CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//					vecGridIndices.push_back(dwGrid);
//
//					// up
//					dwGrid += 1;
//					vecGridIndices.push_back(dwGrid);
//				}
//			}
//		}
//	}
//	else  // inverse direction
//	{
//		if ( 0.0f > tg )
//		{
//			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
//			{
//				if ( tg >= -1.0f )
//				{
//					vNew.x += 1.0f;
//					vNew.z += tg;
//				}
//				else
//				{
//					vNew.z -= 1.0f;
//					vNew.x -= 1/tg;
//				}
//
//				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
//				{
//					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//					vecGridIndices.push_back(dwGrid);
//
//					// left
//					dwGrid += 1;
//					vecGridIndices.push_back(dwGrid);
//
//					// upleft
//					dwGrid -= CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//					vecGridIndices.push_back(dwGrid);
//
//					// up
//					dwGrid -= 1;
//					vecGridIndices.push_back(dwGrid);
//				}
//			}
//		}
//		else // tg > 0.0f
//		{
//			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
//			{
//				if ( tg < 1.0f )
//				{
//					vNew.x -= 1.0f;
//					vNew.z -= tg;
//				}
//				else  // > 1.0f
//				{
//					vNew.z -= 1.0f;jjjjjjjjjjjh
//					vNew.x -= 1/tg;
//				}
//
//				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
//				{
//					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//					vecGridIndices.push_back(dwGrid);
//
//					// right
//					dwGrid -= 1;
//					vecGridIndices.push_back(dwGrid);
//
//					// upright
//					dwGrid -= CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//					vecGridIndices.push_back(dwGrid);
//
//					// up
//					dwGrid += 1;
//					vecGridIndices.push_back(dwGrid);
//				}
//			}
//		}
//	}
//	
//	D3DXVECTOR3 vDir = vEnd - vStart;
//	
//	for ( std::vector<int>::iterator iter = vecGridIndices.begin(); iter != vecGridIndices.end(); iter++ )
//	{	
//		CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter);
//	
//		//////////////////////////////////////////////////////////////////////////
//	
//		D3DXVECTOR3 v0 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[0]).vPosition;
//		D3DXVECTOR3 v1 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[1]).vPosition;
//		D3DXVECTOR3 v2 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[2]).vPosition;
//		D3DXVECTOR3 v3 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[3]).vPosition;
//	
//		float u,v,d;
//	
//		if ( D3DXIntersectTri(&v0,&v1,&v3,&vStart,&vDir,&u,&v,&d) )
//		{
//			vHitPoint = v0 + u*(v1 - v0) + v*(v3 - v0);
//			return true;
//		}
//		else if ( D3DXIntersectTri(&v1,&v2,&v3,&vStart,&vDir,&u,&v,&d) )
//		{
//			vHitPoint = v1 + u*(v2 - v1) + v*(v3 - v1);
//			return true;
//		}
//	}	
//
//	return false;
//}
//
//////////////////////////////////////////////////////////////////////////
	
	
	//float tg = (vec2.z - vec1.z)/(vec2.x - vec1.x);
	//
	//vec1.x = vec1.x/GRID_SPACE;
	//vec1.z = vec1.z/GRID_SPACE;
	//
	//std::vector<DWORD> vecGridIndices;
	//
	//D3DXVECTOR3 vecStart(vec1); 
	//D3DXVECTOR3 vecNew(vec1);
	//DWORD dwGrid;
	//
	//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecStart) )
	//{
	//	dwGrid = floor(vecStart.x) + floor(vecStart.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//	vecGridIndices.push_back(dwGrid);
	//}
	//
	//if ( vPos.z < vec2.z )
	//{
	//	if ( 0.0f < tg )
	//	{
	//		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
	//		{
	//			// up
	//			D3DXVECTOR3 vecUp(vecNew.x,0.0f,vecNew.z+1.0f);

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecUp) )
	//			{
	//				dwGrid = floor(vecUp.x) + floor(vecUp.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				vecGridIndices.push_back(dwGrid);
	//			}
	//			
	//			// right
	//			D3DXVECTOR3 vecRight(vecNew.x+1.0f,0.0f,vecNew.z);
	//			
	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecRight) )
	//			{
	//				dwGrid = floor(vecRight.x) + floor(vecRight.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				vecGridIndices.push_back(dwGrid);
	//			}

	//			//// rightup
	//			//D3DXVECTOR3 vecRightUp(vecNew.x+1.0f,0.0f,vecNew.z+1.0f);

	//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecRightUp) )
	//			//{
	//			//	dwGrid = floor(vecRightUp.x) + floor(vecRightUp.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//			//	vecGridIndices.push_back(dwGrid);
	//			//}

	//			//// rightdown
	//			//D3DXVECTOR3 vecRightDown(vecNew.x+1.0f,0.0f,vecNew.z-1.0f);

	//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecRightDown) )
	//			//{
	//			//	dwGrid = floor(vecRightDown.x) + floor(vecRightDown	.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//			//	vecGridIndices.push_back(dwGrid);
	//			//}

	//			//// down
	//			//D3DXVECTOR3 vecDown(vecNew.x,0.0f,vecNew.z-1.0f);

	//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecDown) )
	//			//{
	//			//	dwGrid = floor(vecDown.x) + floor(vecDown.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//			//	vecGridIndices.push_back(dwGrid);
	//			//}

	//			if ( tg <= 1.0f )
	//			{
	//				vecNew.x += 1.0f;
	//				vecNew.z += tg;
	//			}
	//			else
	//			{
	//				vecNew.z += 1.0f;
	//				vecNew.x += 1/tg;
	//			}
	//			
	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecNew) )
	//			{
	//				dwGrid = floor(vecNew.x) + floor(vecNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				vecGridIndices.push_back(dwGrid);
	//			}
	//		}
	//	}
	//	else // tg < 0.0f
	//	{
	//		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
	//		{
	//			// up
	//			D3DXVECTOR3 vecUp(vecNew.x,0.0f,vecNew.z+1.0f);

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecUp) )
	//			{
	//				dwGrid = floor(vecUp.x) + floor(vecUp.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				vecGridIndices.push_back(dwGrid);
	//			}

	//			// left
	//			D3DXVECTOR3 vecLeft(vecNew.x-1.0f,0.0f,vecNew.z);
	//			
	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecLeft) )
	//			{
	//				dwGrid = floor(vecLeft.x) + floor(vecLeft.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				vecGridIndices.push_back(dwGrid);
	//			}

	//			//// rightup
	//			//D3DXVECTOR3 vecRightUp(vecNew.x+1.0f,0.0f,vecNew.z+1.0f);

	//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecRightUp) )
	//			//{
	//			//	dwGrid = floor(vecRightUp.x) + floor(vecRightUp.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//			//	vecGridIndices.push_back(dwGrid);
	//			//}

	//			//// rightdown
	//			//D3DXVECTOR3 vecRightDown(vecNew.x+1.0f,0.0f,vecNew.z-1.0f);

	//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecRightDown) )
	//			//{
	//			//	dwGrid = floor(vecRightDown.x) + floor(vecRightDown	.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//			//	vecGridIndices.push_back(dwGrid);
	//			//}

	//			//// down
	//			//D3DXVECTOR3 vecDown(vecNew.x,0.0f,vecNew.z-1.0f);

	//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecDown) )
	//			//{
	//			//	dwGrid = floor(vecDown.x) + floor(vecDown.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//			//	vecGridIndices.push_back(dwGrid);
	//			//}

	//			if ( tg >= -1.0f )
	//			{
	//				vecNew.x -= 1.0f;
	//				vecNew.z += -tg;
	//			}
	//			else  // < -1.0f
	//			{
	//				vecNew.z += 1.0f;
	//				vecNew.x -= -1/tg;
	//			}

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecNew) )
	//			{
	//				dwGrid = floor(vecNew.x) + floor(vecNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				vecGridIndices.push_back(dwGrid);
	//			}
	//		}
	//	}
	//}
	//else  // inverse direction
	//{
	//	if ( 0.0f > tg )
	//	{
	//		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
	//		{
	//			// up
	//			D3DXVECTOR3 vecUp(vecNew.x,0.0f,vecNew.z-1.0f);

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecUp) )
	//			{
	//				dwGrid = floor(vecUp.x) + floor(vecUp.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				vecGridIndices.push_back(dwGrid);
	//			}

	//			// right
	//			D3DXVECTOR3 vecRight(vecNew.x+1.0f,0.0f,vecNew.z);

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecRight) )
	//			{
	//				dwGrid = floor(vecRight.x) + floor(vecRight.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				vecGridIndices.push_back(dwGrid);
	//			}

	//			//// rightup
	//			//D3DXVECTOR3 vecRightUp(vecNew.x+1.0f,0.0f,vecNew.z+1.0f);

	//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecRightUp) )
	//			//{
	//			//	dwGrid = floor(vecRightUp.x) + floor(vecRightUp.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//			//	vecGridIndices.push_back(dwGrid);
	//			//}

	//			//// rightdown
	//			//D3DXVECTOR3 vecRightDown(vecNew.x+1.0f,0.0f,vecNew.z-1.0f);

//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecRightDown) )
//			//{
//			//	dwGrid = floor(vecRightDown.x) + floor(vecRightDown	.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//			//	vecGridIndices.push_back(dwGrid);
//			//}

//			//// down
//			//D3DXVECTOR3 vecDown(vecNew.x,0.0f,vecNew.z-1.0f);

//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecDown) )
//			//{
//			//	dwGrid = floor(vecDown.x) + floor(vecDown.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//			//	vecGridIndices.push_back(dwGrid);
//			//}

//			if ( tg >= -1.0f )
//			{
//				vecNew.x += 1.0f;
//				vecNew.z += tg;
//			}
//			else
//			{
//				vecNew.z -= 1.0f;
//				vecNew.x -= 1/tg;
//			}

//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecNew) )
//			{
//				dwGrid = floor(vecNew.x) + floor(vecNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//				vecGridIndices.push_back(dwGrid);
//			}
//		}
//	}
//	else // tg > 0.0f
//	{
//		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
//		{
//			// up
//			D3DXVECTOR3 vecUp(vecNew.x,0.0f,vecNew.z-1.0f);

//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecUp) )
//			{
//				dwGrid = floor(vecUp.x) + floor(vecUp.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//				vecGridIndices.push_back(dwGrid);
//			}

//			// left
//			D3DXVECTOR3 vecLeft(vecNew.x-1.0f,0.0f,vecNew.z);

//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecLeft) )
//			{
//				dwGrid = floor(vecLeft.x) + floor(vecLeft.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//				vecGridIndices.push_back(dwGrid);
//			}

//			//// rightup
//			//D3DXVECTOR3 vecRightUp(vecNew.x+1.0f,0.0f,vecNew.z+1.0f);

//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecRightUp) )
//			//{
//			//	dwGrid = floor(vecRightUp.x) + floor(vecRightUp.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//			//	vecGridIndices.push_back(dwGrid);
//			//}

//			//// rightdown
//			//D3DXVECTOR3 vecRightDown(vecNew.x+1.0f,0.0f,vecNew.z-1.0f);

//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecRightDown) )
//			//{
//			//	dwGrid = floor(vecRightDown.x) + floor(vecRightDown	.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//			//	vecGridIndices.push_back(dwGrid);
//			//}

//			//// down
//			//D3DXVECTOR3 vecDown(vecNew.x,0.0f,vecNew.z-1.0f);

//			//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecDown) )
//			//{
//			//	dwGrid = floor(vecDown.x) + floor(vecDown.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//			//	vecGridIndices.push_back(dwGrid);
//			//}

//			if ( tg < 1.0f )
//			{
//				vecNew.x -= 1.0f;
//				vecNew.z -= tg;
//			}
//			else  // > 1.0f
//			{
//				vecNew.z -= 1.0f;
//				vecNew.x -= 1/tg;
//			}
//
//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vecNew) )
//			{
//				dwGrid = floor(vecNew.x) + floor(vecNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//				vecGridIndices.push_back(dwGrid);
//			}
//		}
//	}
//}

//float fxCamera =  eg::CEngine::GetInstance()->GetActiveCamera()->GetPosition().x;
//float fyCamera =  eg::CEngine::GetInstance()->GetActiveCamera()->GetPosition().y;
//float fzCamera =  eg::CEngine::GetInstance()->GetActiveCamera()->GetPosition().z;
//
//float A = (vecEnd.x - fxCamera)*(vecEnd.x - fxCamera) + (vecEnd.z - fzCamera)*(vecEnd.z - fzCamera);
//float B = fyCamera*fyCamera;
//	
//for ( std::vector<DWORD>::iterator iter = vecGridIndices.begin(); iter != vecGridIndices.end(); iter++ )
//{	
//	CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter);
//	
//	//////////////////////////////////////////////////////////////////////////
//	
//	D3DXVECTOR3 v0 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[0]).vPosition;
//	D3DXVECTOR3 v1 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[1]).vPosition;
//	D3DXVECTOR3 v2 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[2]).vPosition;
//	D3DXVECTOR3 v3 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[3]).vPosition;
//	
//	float u,v,d;
//	
//	if ( D3DXIntersectTri(&v0,&v1,&v3,&vPos,&vDir,&u,&v,&d) )
//	{
//		vHitPoint = v0 + u*(v1 - v0) + v*(v3 - v0);
//		return true;
//	}
//		
//	if ( D3DXIntersectTri(&v1,&v2,&v3,&vPos,&vDir,&u,&v,&d) )
//	{
//		vHitPoint = v1 + u*(v2 - v1) + v*(v3 - v1);
//		return true;
//	}
//}	

//return false;


int CPicker::GetPickPos(CTerrainMesh::CObject ** pp, D3DXVECTOR3 * pvHitPoint)
{
	//const DWORD SEARCH_TOLERANCE = 100;
	//
	//LPDIRECT3DDEVICE9 pDevice = rd::CRenderDevice::GetInstance()->GetRawDevice();
	//
	//POINT pt;
	//ip::CInputDevice::GetInstance()->GetMousePosition((int&)pt.x,(int&)pt.y);

	//D3DXVECTOR3 vStart, vEnd;
	//D3DXMATRIX matProj, matView, matWorld;
	//D3DVIEWPORT9 viewport;

	//CCamera * pCamera = eg::CEngine::GetInstance()->GetActiveCamera();

	//matProj = *pCamera->GetProjectionMatrix();

	//matView = *pCamera->GetViewMatrix();

	//D3DXMatrixIdentity(&matWorld);

	//pDevice->GetViewport(&viewport);

	//vStart = eg::CEngine::GetInstance()->GetActiveCamera()->GetPosition();
	//
	//D3DXVec3Unproject(&vEnd,&D3DXVECTOR3(static_cast<float>(pt.x), static_cast<float>(pt.y), 0.9999f),
	//	&viewport, &matProj, &matView, &matWorld);

	//vStart.x = (int)(vStart.x/GRID_SPACE)*GRID_SPACE + 0.5*GRID_SPACE*vStart.x/fabs(vStart.x);
	//vStart.z = (int)(vStart.z/GRID_SPACE)*GRID_SPACE + 0.5*GRID_SPACE*vStart.z/fabs(vStart.z);
	//
	////////////////////////////////////////////////////////////////////////////
	//

	//float tg = (vEnd.z - vStart.z)/(vEnd.x - vStart.x);
	//
	//D3DXVECTOR3 vNew(vStart);
	//D3DXVECTOR3 vNewAdded;
	//	
	//vNew.x = vNew.x/GRID_SPACE;
	//vNew.z = vNew.z/GRID_SPACE;
	//
	//DWORD dwGrid;

	//if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
	//{
	//	dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//	m_vecGridIndices.push_back(dwGrid);
	//}
	//
	//if ( vStart.z < vEnd.z )
	//{
	//	if ( 0.0f < tg )
	//	{
	//		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
	//		{
	//			if ( tg <= 1.0f )
	//			{
	//				vNewAdded = vNew;
	//				
	//				vNew.x += 1.0f;
	//				vNew.z += tg;
	//				
	//				if ( int(vNew.z) > int(vNewAdded.z) )
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,-1.0f);
	//				}
	//				else
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,1.0f);
	//				}								
	//			}
	//			else
	//			{
	//				vNewAdded = vNew;
	//				
	//				vNew.z += 1.0f;
	//				vNew.x += 1/tg;
	//				
	//				if ( int(vNew.x) > int(vNewAdded.x) )
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(-1.0f,0.0f,0.0f);
	//				}
	//				else
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(1.0f,0.0f,0.0f);
	//				}								
	//			}

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
	//			{
	//				dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				m_vecGridIndices.push_back(dwGrid);
	//			}

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNewAdded) )
	//			{
	//				dwGrid = floor(vNewAdded.x) + floor(vNewAdded.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				m_vecGridIndices.push_back(dwGrid);
	//			}
	//		}
	//	}
	//	else // tg < 0.0f
	//	{
	//		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
	//		{
	//			if ( tg >= -1.0f )
	//			{
	//				vNewAdded = vNew;
	//				
	//				vNew.x -= 1.0f;
	//				vNew.z += -tg;
	//				
	//				if ( int(vNew.z) > int(vNewAdded.z) )
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,-1.0f);
	//				}
	//				else
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,1.0f);
	//				}								
	//			}
	//			else  // < -1.0f
	//			{
	//				vNewAdded = vNew;
	//				
	//				vNew.z += 1.0f;
	//				vNew.x -= -1/tg;

	//				if ( int(vNew.x) < int(vNewAdded.x) )
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(1.0f,0.0f,0.0f);
	//				}
	//				else
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(-1.0f,0.0f,0.0f);
	//				}								
	//			}

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
	//			{
	//				dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				m_vecGridIndices.push_back(dwGrid);
	//			}

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNewAdded) )
	//			{
	//				dwGrid = floor(vNewAdded.x) + floor(vNewAdded.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				m_vecGridIndices.push_back(dwGrid);
	//			}
	//		}
	//	}
	//}
	//else  // inverse direction
	//{
	//	if ( 0.0f > tg )
	//	{
	//		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
	//		{
	//			if ( tg >= -1.0f )
	//			{
	//				vNewAdded = vNew;

	//				vNew.x += 1.0f;
	//				vNew.z += tg;

	//				if ( int(vNew.z) < int(vNewAdded.z) )
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,1.0f);
	//				}
	//				else
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,-1.0f);
	//				}								
	//			}
	//			else
	//			{
	//				vNewAdded = vNew;

	//				vNew.z -= 1.0f;
	//				vNew.x -= 1/tg;

	//				if ( int(vNew.x) > int(vNewAdded.x) )
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(-1.0f,0.0f,0.0f);
	//				}
	//				else
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(1.0f,0.0f,0.0f);
	//				}								
	//			}

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
	//			{
	//				dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				m_vecGridIndices.push_back(dwGrid);
	//			}

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNewAdded) )
	//			{
	//				dwGrid = floor(vNewAdded.x) + floor(vNewAdded.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				m_vecGridIndices.push_back(dwGrid);
	//			}
	//		}
	//	}
	//	else // tg > 0.0f
	//	{
	//		for ( int i = 0; i < SEARCH_TOLERANCE; i++)
	//		{
	//			if ( tg < 1.0f )
	//			{
	//				vNewAdded = vNew;
	//				
	//				vNew.x -= 1.0f;
	//				vNew.z -= tg;

	//				if ( int(vNew.z) < int(vNewAdded.z) )
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,1.0f);
	//				}
	//				else
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,-1.0f);
	//				}								
	//			}
	//			else  // > 1.0f
	//			{
	//				vNewAdded = vNew;

	//				vNew.z -= 1.0f;
	//				vNew.x -= 1/tg;
	//				
	//				if ( int(vNew.x) < int(vNewAdded.x) )
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(1.0f,0.0f,0.0f);
	//				}
	//				else
	//				{
	//					vNewAdded = vNew + D3DXVECTOR3(-1.0f,0.0f,0.0f);
	//				}								
	//			}

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
	//			{
	//				dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				m_vecGridIndices.push_back(dwGrid);
	//			}

	//			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNewAdded) )
	//			{
	//				dwGrid = floor(vNewAdded.x) + floor(vNewAdded.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	//				m_vecGridIndices.push_back(dwGrid);
	//			}
	//		}
	//	}
	//}
	//
	//DWORD dwGridIndexHot = INVALID;
	//
	//*pp = NULL;

	//// adjust back vStart
	//vStart = eg::CEngine::GetInstance()->GetActiveCamera()->GetPosition();
	//D3DXVECTOR3 vDir = vEnd - vStart;

	m_vecGridIndices.clear();

	const DWORD SEARCH_TOLERANCE = 100;

	LPDIRECT3DDEVICE9 pDevice = rd::CRenderDevice::GetInstance()->GetRawDevice();

	POINT pt;
	ip::CInputDevice::GetInstance()->GetMousePosition((int&)pt.x,(int&)pt.y);

	D3DXVECTOR3 vec;
	D3DXMATRIX matProj, matView, matWorld;
	D3DVIEWPORT9 viewport;

	CCamera * pCamera = eg::CEngine::GetInstance()->GetActiveCamera();

	matProj = *pCamera->GetProjectionMatrix();

	matView = *pCamera->GetViewMatrix();

	D3DXMatrixIdentity(&matWorld);

	pDevice->GetViewport(&viewport);
	
	RECT rc;
	HWND hWnd;
	hWnd = eg::CEngine::GetInstance()->GetD3DWindow();
	GetClientRect(hWnd,&rc);

	viewport.Height = rc.bottom;
	viewport.Width = rc.right;

	D3DXVec3Unproject(&vec,&D3DXVECTOR3(static_cast<float>(pt.x), static_cast<float>(pt.y), 0.9999f),
		&viewport, &matProj, &matView, &matWorld);

	//////////////////////////////////////////////////////////////////////////

	D3DXVECTOR3 vStart = eg::CEngine::GetInstance()->GetActiveCamera()->GetPosition();

	D3DXVECTOR3 vEnd =  vec;

	float tg = (vEnd.z - vStart.z)/(vEnd.x - vStart.x);

	D3DXVECTOR3 vNew(vStart);

	vNew.x = vNew.x/GRID_SPACE;
	vNew.z = vNew.z/GRID_SPACE;

	DWORD dwGrid;

	if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
	{
		dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
		m_vecGridIndices.push_back(dwGrid);
	}

	if ( vStart.z < vEnd.z )
	{
		if ( 0.0f < tg )
		{
			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
			{
				if ( tg <= 1.0f )
				{
					vNew.x += 1.0f;
					vNew.z += tg;
				}
				else
				{
					vNew.z += 1.0f;
					vNew.x += 1/tg;
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}


					// right
					dwGrid += 1;
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}


					// upright
					dwGrid += CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}


					// up
					dwGrid -= 1;
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}

				}
			}
		}
		else // tg < 0.0f
		{
			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
			{
				if ( tg >= -1.0f )
				{
					vNew.x -= 1.0f;
					vNew.z += -tg;

				}
				else  // < -1.0f
				{
					vNew.z += 1.0f;
					vNew.x -= -1/tg;
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}

					// left
					dwGrid -= 1;
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}

					// upleft
					dwGrid += CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}

					// up
					dwGrid += 1;
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}
				}
			}
		}
	}
	else  // inverse direction
	{
		if ( 0.0f > tg )
		{
			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
			{
				if ( tg >= -1.0f )
				{
					vNew.x += 1.0f;
					vNew.z += tg;
				}
				else
				{
					vNew.z -= 1.0f;
					vNew.x -= 1/tg;
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();

					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}

					// left
					dwGrid += 1;

					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}

					// upleft
					dwGrid -= CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();

					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}

					// up
					dwGrid -= 1;
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}
				}
			}
		}
		else // tg > 0.0f
		{
			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
			{
				if ( tg < 1.0f )
				{
					vNew.x -= 1.0f;
					vNew.z -= tg;
				}
				else  // > 1.0f
				{
					vNew.z -= 1.0f;
					vNew.x -= 1/tg;
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}

					// right
					dwGrid -= 1;
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}

					// upright
					dwGrid -= CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}

					// up
					dwGrid += 1;
					if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGrid) )
					{
						m_vecGridIndices.push_back(dwGrid);
					}
				}
			}
		}
	}

	D3DXVECTOR3 vDir = vEnd - vStart;

	DWORD dwGridIndexHot = INVALID;

	if ( pp ) 
	{
		*pp = NULL;
	}

	for ( std::vector<int>::iterator iter = m_vecGridIndices.begin(); iter != m_vecGridIndices.end(); iter++ )
	{	
		CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter);

		//////////////////////////////////////////////////////////////////////////

		if ( pp && !grid.vecObjects.empty() && *pp == NULL )
		{
			for ( std::vector< Ptr<CTerrainMesh::CObject> >::iterator iter = grid.vecObjects.begin(); iter != grid.vecObjects.end(); ++iter)
			{
				CTerrainMesh::CObject * p = (*iter).ptr();

				if ( p->GetFace()->Intersected(vDir,vStart) )
				{
					p->GetFace()->SetSelected(true);
					*pp = p;

					if ( m_pLastSelected && m_pLastSelected.ptr() != p )
					{
						m_pLastSelected->GetFace()->SetSelected(false);
					}

					m_pLastSelected = p;

					break;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////

		D3DXVECTOR3 v0 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[0]).vPosition;
		D3DXVECTOR3 v1 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[1]).vPosition;
		D3DXVECTOR3 v2 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[2]).vPosition;
		D3DXVECTOR3 v3 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[3]).vPosition;

		float u,v,d;

		if ( D3DXIntersectTri(&v0,&v1,&v3,&vStart,&vDir,&u,&v,&d) )
		{
			if ( pvHitPoint )
			{
				*pvHitPoint = v0 + u*(v1 - v0) + v*(v3 - v0);
			}

			dwGridIndexHot = *iter;

			if ( pp && m_pLastSelected && *pp == NULL )
			{
				m_pLastSelected->GetFace()->SetSelected(false);
			}

			return dwGridIndexHot;
		}
		else if ( D3DXIntersectTri(&v1,&v2,&v3,&vStart,&vDir,&u,&v,&d) )
		{
			if ( pvHitPoint )
			{
				*pvHitPoint = v1 + u*(v2 - v1) + v*(v3 - v1);
			}

			dwGridIndexHot = *iter;

			if ( pp && m_pLastSelected && *pp == NULL )
			{
				m_pLastSelected->GetFace()->SetSelected(false);
			}

			return dwGridIndexHot;
		}
	}	

	return dwGridIndexHot;
}

float CPicker::GetHeight(float fX, float fZ)
{
	int nGridIndexInX = int(fX/GRID_SPACE);
	int nGridIndexInZ = int(fZ/GRID_SPACE);
	int nGridIndex = nGridIndexInZ * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth() + nGridIndexInX;

	CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(nGridIndex);

	D3DXVECTOR3 v0 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[0]).vPosition;
	D3DXVECTOR3 v1 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[1]).vPosition;
	D3DXVECTOR3 v2 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[2]).vPosition;
	D3DXVECTOR3 v3 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[3]).vPosition;

	float y;

	if ( v1.x - fX > fZ - v1.z )
	{
		y = v0.y + (v1.y - v0.y)*(fX-v0.x)/GRID_SPACE;
		y = y + (v3.y - y)*(fZ - v0.z)/GRID_SPACE; 
	}
	else
	{
		y = v3.y + (v2.y - v3.y)*(fX-v0.x)/GRID_SPACE;
		y = y + (v3.y - y)*(fZ - v0.z)/GRID_SPACE; 
	}

	return y;
}

bool CPicker::GetIntersected(D3DXVECTOR3 & vHitPoint)
{
	const DWORD SEARCH_TOLERANCE = 100;

	D3DXVECTOR3 vStart = eg::CEngine::GetInstance()->GetActiveCamera()->GetPosition();
	D3DXVECTOR3 vEnd =  eg::CEngine::GetInstance()->GetActiveCamera()->GetView();

	float fx = vEnd.x - vStart.x; 

	if ( fabs(fx) < FLT_MIN  )
	{
		fx += 0.0001f;
	}

	float tg = (vEnd.z - vStart.z)/fx;

	D3DXVECTOR3 vNew(vStart);

	vNew.x = vNew.x/GRID_SPACE;
	vNew.z = vNew.z/GRID_SPACE;

	DWORD dwGrid;

	std::vector<int> vecGridIndices;

	if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
	{
		dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
		vecGridIndices.push_back(dwGrid);
	}

	if ( vStart.z < vEnd.z )
	{
		if ( 0.0f < tg )
		{
			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
			{
				if ( tg <= 1.0f )
				{
					vNew.x += 1.0f;
					vNew.z += tg;
				}
				else
				{
					vNew.z += 1.0f;
					vNew.x += 1/tg;
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);

					// right
					dwGrid += 1;
					vecGridIndices.push_back(dwGrid);

					// upright
					dwGrid += CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);

					// up
					dwGrid -= 1;
					vecGridIndices.push_back(dwGrid);
				}
			}
		}
		else // tg < 0.0f
		{
			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
			{
				if ( tg >= -1.0f )
				{
					vNew.x -= 1.0f;
					vNew.z += -tg;

				}
				else  // < -1.0f
				{
					vNew.z += 1.0f;
					vNew.x -= -1/tg;
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);

					// left
					dwGrid -= 1;
					vecGridIndices.push_back(dwGrid);

					// upleft
					dwGrid += CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);

					// up
					dwGrid += 1;
					vecGridIndices.push_back(dwGrid);
				}
			}
		}
	}
	else  // inverse direction
	{
		if ( 0.0f > tg )
		{
			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
			{
				if ( tg >= -1.0f )
				{
					vNew.x += 1.0f;
					vNew.z += tg;
				}
				else
				{
					vNew.z -= 1.0f;
					vNew.x -= 1/tg;
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);

					// left
					dwGrid += 1;
					vecGridIndices.push_back(dwGrid);

					// upleft
					dwGrid -= CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);

					// up
					dwGrid -= 1;
					vecGridIndices.push_back(dwGrid);
				}
			}
		}
		else // tg > 0.0f
		{
			for ( int i = 0; i < SEARCH_TOLERANCE; i++)
			{
				if ( tg < 1.0f )
				{
					vNew.x -= 1.0f;
					vNew.z -= tg;
				}
				else  // > 1.0f
				{
					vNew.z -= 1.0f;
					vNew.x -= 1/tg;
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);

					// right
					dwGrid -= 1;
					vecGridIndices.push_back(dwGrid);

					// upright
					dwGrid -= CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);

					// up
					dwGrid += 1;
					vecGridIndices.push_back(dwGrid);
				}
			}
		}
	}

	D3DXVECTOR3 vDir = vEnd - vStart;

	for ( std::vector<int>::iterator iter = vecGridIndices.begin(); iter != vecGridIndices.end(); iter++ )
	{	
		CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter);

		//////////////////////////////////////////////////////////////////////////

		D3DXVECTOR3 v0 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[0]).vPosition;
		D3DXVECTOR3 v1 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[1]).vPosition;
		D3DXVECTOR3 v2 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[2]).vPosition;
		D3DXVECTOR3 v3 = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[3]).vPosition;

		float u,v,d;

		if ( D3DXIntersectTri(&v0,&v1,&v3,&vStart,&vDir,&u,&v,&d) )
		{
			vHitPoint = v0 + u*(v1 - v0) + v*(v3 - v0);
			return true;
		}
		else if ( D3DXIntersectTri(&v1,&v2,&v3,&vStart,&vDir,&u,&v,&d) )
		{
			vHitPoint = v1 + u*(v2 - v1) + v*(v3 - v1);
			return true;
		}
	}	

	return false;
}

std::vector<int> & CPicker::GetGridIndices()
{
	return m_vecGridIndices;
}

void CPicker::Evaluate(D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd, DWORD dwSearchCnt, std::vector<DWORD> & vecGridIndices)
{	
	float fx = vEnd.x - vStart.x; 

	if ( fabs(fx) < FLT_MIN  )
	{
		fx += 0.0001f;
	}

	float tg = (vEnd.z - vStart.z)/fx;

	D3DXVECTOR3 vNew(vStart);
	D3DXVECTOR3 vNewAdded;

	vNew.x = vNew.x/GRID_SPACE;
	vNew.z = vNew.z/GRID_SPACE;

	DWORD dwGrid;

	if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
	{
		dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
		vecGridIndices.push_back(dwGrid);
	}

	dwSearchCnt--;

	//////////////////////////////////////////////////////////////////////////

	if ( fabs(vEnd.z - vStart.z) < FLT_MIN )
	{
		if ( vStart.x < vEnd.x )
		{
			for ( int i = 0; i < dwSearchCnt; i++)
			{
				vecGridIndices.push_back(++dwGrid);
			}
		}
		else
		{
			for ( int i = 0; i < dwSearchCnt; i++)
			{
				vecGridIndices.push_back(--dwGrid);
			}
		}

		return;
	}

	//////////////////////////////////////////////////////////////////////////

	if ( fabs(vEnd.x - vStart.x) < FLT_MIN )
	{
		if ( vStart.z < vEnd.z )
		{
			for ( int i = 0; i < dwSearchCnt; i++)
			{
				dwGrid += CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
				vecGridIndices.push_back(dwGrid);
			}
		}
		else
		{
			for ( int i = 0; i < dwSearchCnt; i++)
			{
				dwGrid -= CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
				vecGridIndices.push_back(dwGrid);
			}
		}

		return;
	}

	//////////////////////////////////////////////////////////////////////////

	bool fNewAdded = false;

	if ( vStart.z < vEnd.z )
	{
		if ( tg > 0.0f )
		{
			for ( int i = 0; i < dwSearchCnt; i++)
			{
				if ( tg <= 1.0f )
				{
					vNewAdded = vNew;

					vNew.x += 1.0f;
					vNew.z += tg;

					if ( int(vNew.z) > int(vNewAdded.z) )
					{
						vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,-1.0f);
						fNewAdded = true;
					}
					else
					{
						fNewAdded = false;
					}								
				}
				else
				{
					vNewAdded = vNew;

					vNew.z += 1.0f;
					vNew.x += 1/tg;

					if ( int(vNew.x) > int(vNewAdded.x) )
					{
						vNewAdded = vNew + D3DXVECTOR3(-1.0f,0.0f,0.0f);
						fNewAdded = true;
					}
					else
					{
						fNewAdded = false;
					}								
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);
				}

				if ( fNewAdded && CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNewAdded) )
				{
					dwGrid = floor(vNewAdded.x) + floor(vNewAdded.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);
				}
			}
		}
		else // tg <= 0.0f
		{
			for ( int i = 0; i < dwSearchCnt; i++)
			{
				if ( tg >= -1.0f )
				{
					vNewAdded = vNew;

					vNew.x -= 1.0f;
					vNew.z += -tg;

					if ( int(vNew.z) > int(vNewAdded.z) )
					{
						vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,-1.0f);
						fNewAdded = true;
					}
					else
					{
						fNewAdded = false;
					}								
				}
				else  // < -1.0f
				{
					vNewAdded = vNew;

					vNew.z += 1.0f;
					vNew.x -= -1/tg;

					if ( int(vNew.x) < int(vNewAdded.x) )
					{
						vNewAdded = vNew + D3DXVECTOR3(1.0f,0.0f,0.0f);
						fNewAdded = true;
					}
					else
					{
						fNewAdded = false;
					}								
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);
				}

				if ( fNewAdded && CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNewAdded) )
				{
					dwGrid = floor(vNewAdded.x) + floor(vNewAdded.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);
				}
			}
		}
	}
	else  // inverse direction
	{
		if ( 0.0f > tg )
		{
			for ( size_t i = 0; i < dwSearchCnt; i++)
			{
				if ( tg >= -1.0f )
				{
					vNewAdded = vNew;

					vNew.x += 1.0f;
					vNew.z += tg;

					if ( int(vNew.z) < int(vNewAdded.z) )
					{
						vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,1.0f);
						fNewAdded = true;
					}
					else
					{
						fNewAdded = false;
					}								
				}
				else
				{
					vNewAdded = vNew;

					vNew.z -= 1.0f;
					vNew.x -= 1/tg;

					if ( int(vNew.x) > int(vNewAdded.x) )
					{
						vNewAdded = vNew + D3DXVECTOR3(-1.0f,0.0f,0.0f);
						fNewAdded = true;
					}
					else
					{
						fNewAdded = false;
					}								
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);
				}

				if ( fNewAdded && CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNewAdded) )
				{
					dwGrid = floor(vNewAdded.x) + floor(vNewAdded.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);
				}
			}
		}
		else // tg >= 0.0f
		{
			for ( size_t i = 0; i < dwSearchCnt; i++)
			{
				if ( tg < 1.0f )
				{
					vNewAdded = vNew;

					vNew.x -= 1.0f;
					vNew.z -= tg;

					if ( int(vNew.z) < int(vNewAdded.z) )
					{
						vNewAdded = vNew + D3DXVECTOR3(0.0f,0.0f,1.0f);
						fNewAdded = true;
					}
					else
					{
						fNewAdded = false;
					}								
				}
				else  // > 1.0f
				{
					vNewAdded = vNew;

					vNew.z -= 1.0f;
					vNew.x -= 1/tg;

					if ( int(vNew.x) < int(vNewAdded.x) )
					{
						vNewAdded = vNew + D3DXVECTOR3(1.0f,0.0f,0.0f);
						fNewAdded = true;
					}
					else
					{
						fNewAdded = false;
					}								
				}

				if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNew) )
				{
					dwGrid = floor(vNew.x) + floor(vNew.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);
				}

				if ( fNewAdded && CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(vNewAdded) )
				{
					dwGrid = floor(vNewAdded.x) + floor(vNewAdded.z) * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
					vecGridIndices.push_back(dwGrid);
				}
			}
		}
	}
}

}
}