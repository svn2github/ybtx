//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "extradefine.h"
#include "IGraphicModule.h"
#include "Render.h"
#include "CTileEditorModel.h"
#include "Engine.h"
#include "Camera.h"
#include "Timer.h"
#include "TerrainMesh.h"
#include "CTileEditApp.h"
#include "Scene.h"
#include <BluelyMath.h>
#include "IEffect.h"
#include "IEffectGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bly
{

bool IsInside( const D3DXVECTOR2& p, const D3DXVECTOR2& p1, const D3DXVECTOR2& p2, const D3DXVECTOR2& p3, const D3DXVECTOR2& p4 )
{
	D3DXVECTOR2 v1 = p1 - p;
	D3DXVECTOR2 v2 = p2 - p;
	D3DXVECTOR2 v3 = p3 - p;
	D3DXVECTOR2 v4 = p4 - p;
	
	float d1 = v1.y*v2.x - v1.x*v2.y;
	float d2 = v2.y*v3.x - v2.x*v3.y;
	float d3 = v3.y*v4.x - v3.x*v4.y;
	float d4 = v4.y*v1.x - v4.x*v1.y;
	
	if( d1 > 0 && d2 > 0 && d3 > 0 && d4 > 0 || d1 < 0 && d2 < 0 && d3 < 0 && d4 < 0 )
		return true;
	
	return false;
}

CTileEditorModel::CTileEditorModel(ISceneNode * pParent) : ISceneNode(pParent)
{
	m_pModel = NULL;
	m_eEditState = EES_DANGLING;
	m_eEditStatePlaced = EESP_NONE;
	m_eCenterType = m_eCenterTypeOrigin = ECT_NONE;
	D3DXMatrixIdentity(&m_matSelfOff);
	D3DXMatrixIdentity(&m_matWorldOff);
	D3DXMatrixIdentity(&m_mat);
	D3DXMatrixIdentity(&m_matRotY);
	m_bSelected = false;
	m_pWireBox = NULL;
	m_vCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_bLocked = false;
	m_fRotation = 0.0f;
	m_fLift = 0.0f;
	m_fShiftX = 0.0f;
	m_fShiftZ = 0.0f;
	m_nLogicalCenterGridIndex = INVALID;
	m_nPhysicalCenterGridIndex = INVALID;
	m_nRotationCnt = 0;
	m_nHookHandle = INVALID;
	m_bSubUnit = false;
	
	m_pEffect = NULL;
	m_pGroup = NULL;
}

CTileEditorModel::~CTileEditorModel() 
{
	for ( int i = 0; i < (m_nWidthDimInGrids+1)*(m_nDepthDimInGrids+1); ++i )
	{
		m_vecpWireBox[i]->Remove();
	}
}

void CTileEditorModel::SetCenterTypeOrigin(ECenter_Type eCenterType)
{
	m_eCenterType = m_eCenterTypeOrigin = eCenterType;
}

CTileEditorModel::ECenter_Type CTileEditorModel::GetCenterType()
{
	return m_eCenterType;    
}

void CTileEditorModel::SetBlockTypeByIndex(int nIndex, int nBlockType)
{
	m_vecBlockInfo[nIndex].nBlockType = nBlockType;
}

CTileEditorModel::ECenter_Type CTileEditorModel::GetCenterTypeOrigin()
{
	return m_eCenterTypeOrigin;    
}

void CTileEditorModel::Create(string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, int &BlockInfoNum, int &LogicHeightInfoNum)
{
	m_strFileName = strFileName;
	m_bSubUnit = bSubUnit;
	m_fLift = fLift;
	m_fShiftX = fShiftX;
	m_fShiftZ = fShiftZ;
	
	ICoreGraphic * pCoreGraphic = CTileEditApp::GetInstance()->GetOperator()->GetCoreGraphic();
	
	pCoreGraphic->CreateModel("", &m_pModel);
	
	IPieceGroup * pPieceGroup = NULL;
	
	pCoreGraphic->CreatePieceGroup(strFileName.c_str(), &pPieceGroup);

	if(pPieceGroup)
	{
		for ( int i = 0; pPieceGroup && i < pPieceGroup->GetPieceClassNum() ; i++)
		{
			m_pModel->AddMeshPiece(pPieceGroup,pPieceGroup->GetPieceClassName(i),0);
		}
		pPieceGroup->Release();
	}
	
	m_pModel->GetScaleDimension(0,m_nWidthDimInGrids,m_nDepthDimInGrids,GRID_SPACE);
	
	m_vecBlockInfo.assign(m_nWidthDimInGrids*m_nDepthDimInGrids,SBlockInfo());
	m_vecLogicHeightInfo.assign((m_nWidthDimInGrids+1)*(m_nDepthDimInGrids+1),SLogicHeightInfo());

	BlockInfoNum = m_vecBlockInfo.size();
	LogicHeightInfoNum = m_vecLogicHeightInfo.size();
	
	float fHeight = 0.0f;
	float fWidth = GRID_SPACE/4;
	float fDepth = GRID_SPACE/4; 
	
	for ( int i = 0; i < (m_nWidthDimInGrids+1)*(m_nDepthDimInGrids+1); ++i )
	{
		CWireBox * pWireBox = CSceneManager::GetInstance()->AddNodeWireBox(fHeight,fWidth,fDepth,0xffff0000,false,NULL);
		m_vecpWireBox.push_back(pWireBox);
	}
	
	CVector3f vScale = m_pModel->GetScale();
	CVector3f vOffset = m_pModel->GetOff();
	
	m_vScale.x = vScale.x;
	m_vScale.y = vScale.y;
	m_vScale.z = vScale.z;
	
	m_vOffset.x = vOffset.x;
	m_vOffset.y = vOffset.y;
	m_vOffset.z = vOffset.z;
	
	m_fHeight = m_vScale.y;
	m_fWidth = m_vScale.x; 
	m_fDepth = m_vScale.z;
	
	m_vCorners[0] = D3DXVECTOR3(-m_fWidth/2, 0.0f, -m_fDepth/2);
	m_vCorners[1] = m_vCorners[0] + D3DXVECTOR3(0.0f,0.0f,m_fDepth); 
	m_vCorners[2] = m_vCorners[1] + D3DXVECTOR3(m_fWidth,0.0f,0.0f); 
	m_vCorners[3] = m_vCorners[0] + D3DXVECTOR3(m_fWidth,0.0f,0.0f); 
	m_vCorners[4] = m_vCorners[0] + D3DXVECTOR3(0.0f,m_fHeight,0.0f);
	m_vCorners[5] = m_vCorners[1] + D3DXVECTOR3(0.0f,m_fHeight,0.0f); 
	m_vCorners[6] = m_vCorners[2] + D3DXVECTOR3(0.0f,m_fHeight,0.0f); 
	m_vCorners[7] = m_vCorners[3] + D3DXVECTOR3(0.0f,m_fHeight,0.0f);
	
	m_pWireBox = CSceneManager::GetInstance()->AddNodeWireBox(m_fHeight,m_fWidth,m_fDepth,0xff0000ff,false,this);
}

D3DXVECTOR3 const & CTileEditorModel::GetOffset()
{
	return m_vOffset;
}

void CTileEditorModel::SetHeightBoxesVisibility(bool bIsVisible)
{
	for ( int i = 0; i < m_vecpWireBox.size(); ++i )
	{
		m_vecpWireBox[i]->SetVisible(bIsVisible);
	}
}

bool CTileEditorModel::Intersected(D3DXVECTOR3 vDir,D3DXVECTOR3 vPos)
{
	float u,v,d;
	
	if 
	(
			D3DXIntersectTri(&vCorners[0],&vCorners[1],&vCorners[2],&vPos,&vDir,&u,&v,&d) 
		||	D3DXIntersectTri(&vCorners[0],&vCorners[2],&vCorners[3],&vPos,&vDir,&u,&v,&d) 
		||	D3DXIntersectTri(&vCorners[4],&vCorners[5],&vCorners[6],&vPos,&vDir,&u,&v,&d) 
		||	D3DXIntersectTri(&vCorners[4],&vCorners[6],&vCorners[7],&vPos,&vDir,&u,&v,&d) 
		||	D3DXIntersectTri(&vCorners[0],&vCorners[4],&vCorners[7],&vPos,&vDir,&u,&v,&d) 
		||	D3DXIntersectTri(&vCorners[0],&vCorners[7],&vCorners[3],&vPos,&vDir,&u,&v,&d) 
		||	D3DXIntersectTri(&vCorners[1],&vCorners[5],&vCorners[6],&vPos,&vDir,&u,&v,&d) 
		||	D3DXIntersectTri(&vCorners[1],&vCorners[2],&vCorners[6],&vPos,&vDir,&u,&v,&d) 
		||	D3DXIntersectTri(&vCorners[0],&vCorners[1],&vCorners[5],&vPos,&vDir,&u,&v,&d) 
		||	D3DXIntersectTri(&vCorners[0],&vCorners[5],&vCorners[4],&vPos,&vDir,&u,&v,&d) 
		||	D3DXIntersectTri(&vCorners[3],&vCorners[7],&vCorners[6],&vPos,&vDir,&u,&v,&d) 
		||	D3DXIntersectTri(&vCorners[3],&vCorners[2],&vCorners[6],&vPos,&vDir,&u,&v,&d) 
	)
		return true;

	return false;
}

void CTileEditorModel::Render()
{	
	LPDIRECT3DDEVICE9 pDevice = rd::CRenderDevice::GetInstance()->GetRawDevice();
	DWORD delta = bc::ut::CTimer::GetInstance()->GetFrameTime();
	D3DXVECTOR3 vPosCamera = eg::CEngine::GetInstance()->GetActiveCamera()->GetPosition();
	D3DXVECTOR3 vView = eg::CEngine::GetInstance()->GetActiveCamera()->GetView();
	DWORD dwCurrentTime = bc::ut::CTimer::GetSystemTime();

	CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(FIX_GRID);
							
	m_vCenter = grid.vCenter;
	
	switch ( m_eCenterType )
	{
	case ECT_CENTER:
	break;
	
	case ECT_CORNER:
		m_vCenter.x -= GRID_SPACE/2.0f;
		m_vCenter.z -= GRID_SPACE/2.0f;
	break;
		
	case ECT_XMIDWAY:
		m_vCenter.z -= GRID_SPACE/2.0f;
	break;

	case ECT_ZMIDWAY:
		m_vCenter.x -= GRID_SPACE/2.0f;
	break;
	}

	D3DXMatrixTranslation(&m_matWorldOff, m_vCenter.x + m_fShiftX, m_vCenter.y + m_fLift, m_vCenter.z + m_fShiftZ);
	m_mat = m_matSelfOff*m_matRotY*m_matWorldOff;
	
	////////////////////////////////////////////////////////////////////////
	
	//// for wire box
	//
	//if ( this->IsSubUnit() )
	//{
	//	D3DXVECTOR3 v = m_vOffset - m_vOffsetOverall;
	//	D3DXVec3TransformCoord(&v,&v,&m_matRotY);
	//	this->SetRelativePos(D3DXVECTOR3(m_matWorldOff._41 + v.x, m_matWorldOff._42 - m_vScale.y/2 + v.y, m_matWorldOff._43 + v.z));
	//}
	//else
	//{
	//	this->SetRelativePos(D3DXVECTOR3(m_matWorldOff._41, m_matWorldOff._42, m_matWorldOff._43));
	//}
	//
	//this->SetRelativeRotation(D3DXVECTOR3(0.0f,m_fRotation,0.0f));
	//this->UpdateAbsolute();
	
	////////////////////////////////////////////////////////////////////////
	
	ICoreGraphic * pCoreGraphic = CTileEditApp::GetInstance()->GetOperator()->GetCoreGraphic();

	CTileEditApp::GetInstance()->GetOperator()->GetGraphicModule()->ResetGraphicRenderStyle(CTileEditApp::GetInstance()->GetOperator()->GetCoreGraphic());

	ICamera * pCamera;

	pCoreGraphic->GetCamera(&pCamera);

	D3DXVECTOR3 vecPos = eg::CEngine::GetInstance()->GetActiveCamera()->GetPosition();

	D3DXVECTOR3 vecView = eg::CEngine::GetInstance()->GetActiveCamera()->GetView();

	pCamera->SetCamera(CVector3f(vecPos.x,vecPos.y,vecPos.z),CVector3f(vecView.x,vecView.y,vecView.z));

	sc:CCamera * pMyCamera = eg::CEngine::GetInstance()->GetActiveCamera();
	
	pCamera->SetProject(D3DXToRadian(pMyCamera->GetFOVY()), pMyCamera->GetNearPlane(),pMyCamera->GetFarPlane());
	
	m_pModel->Render(*(CMatrix*)(&m_mat),dwCurrentTime,false);
	m_pModel->Render(*(CMatrix*)(&m_mat),dwCurrentTime,true);
	
	//int i = 0;
	
	//for ( std::vector<int>::iterator iter = m_vecBlockGrids.begin(); iter != m_vecBlockGrids.end(); ++iter, ++i )
	//{
	//	m_vecpWireBox[i]->Render();
	//}
	
	if ( CTileEditApp::GetInstance()->GetOperator()->GetSelectMode() )
	{
		if ( this->GetLocked() || this->GetSelected() )
		{
//			m_pWireBox->Render();
		}
		
		//D3DXMATRIX mat;
		//D3DXMatrixIdentity(&mat);
		//pDevice->SetTransform(D3DTS_WORLD,&mat);
		//DWORD color[4] = {0xffffff};
		//rd::CRenderDevice::GetInstance()->GetLayer3D()->DrawLineStrip(3, vCorners, color);
		//rd::CRenderDevice::GetInstance()->GetLayer3D()->DrawLineStrip(3, &vCorners[4], color);
	}
	
	ISceneNode::Render();
}

void CTileEditorModel::InitRotation()
{
	D3DXMatrixIdentity(&m_matRotY);
	m_fRotation = 0.0f;
}

void CTileEditorModel::UpdateRotation()
{
	m_fRotation += 0.1f;
	D3DXMatrixRotationY(&m_matRotY,m_fRotation);
}

void CTileEditorModel::InitLift()
{
	m_fLift = 0.0f;
	this->UpdateBoundingBox();
}

void CTileEditorModel::InitShiftX()
{
	m_fShiftX = 0.0f;
	this->UpdateBoundingBox();
}

void CTileEditorModel::InitShiftZ()
{
	m_fShiftZ = 0.0f;
	this->UpdateBoundingBox();
}

void CTileEditorModel::UpdateLift(int nWay)
{
	if ( nWay == 0 )
		m_fLift -= 4.0f;
	if ( nWay == 1 )
		m_fLift += 4.0f;
		
	this->UpdateBoundingBox();
}

void CTileEditorModel::UpdateShiftX(int nWay)
{
	if ( nWay == 0 )
		m_fShiftX -= 4.0f;
	if ( nWay == 1 )
		m_fShiftX += 4.0f;
	
	this->UpdateBoundingBox();
}

void CTileEditorModel::UpdateShiftZ(int nWay)
{
	if ( nWay == 0 )
		m_fShiftZ -= 4.0f;
	if ( nWay == 1 )
		m_fShiftZ += 4.0f;

	this->UpdateBoundingBox();
}

float CTileEditorModel::GetLift()
{
	return m_fLift;
}

float CTileEditorModel::GetShiftX()
{
	return m_fShiftX;
}

float CTileEditorModel::GetShiftZ()
{
	return m_fShiftZ;
}

float CTileEditorModel::GetRotation()
{
	return m_fRotation;
}

void CTileEditorModel::RestoreLift(float f)
{
	m_fLift = f;
}

void CTileEditorModel::RestoreRotation(float f)
{
	m_fRotation = f;
	D3DXMatrixRotationY(&m_matRotY,m_fRotation);
}

// for picking
void CTileEditorModel::UpdateBoundingBox()
{
	D3DXMATRIX mat;
	D3DXMATRIX matWorldOff;

	D3DXVECTOR3 v = m_vOffset - m_vOffsetOverall;
	D3DXVec3TransformCoord(&v,&v,&m_matRotY);

	
	if ( this->IsSubUnit() ) 
	{
		D3DXMatrixTranslation(&matWorldOff,m_vCenter.x + v.x + m_fShiftX, m_vCenter.y + m_fLift - m_vScale.y/2 + v.y, m_vCenter.z + v.z + m_fShiftZ);
	}
	else
	{
		D3DXMatrixTranslation(&matWorldOff,m_vCenter.x + m_fShiftX, m_vCenter.y + m_fLift,m_vCenter.z + m_fShiftZ);
	}
	
	mat = m_matRotY*matWorldOff;
			
	D3DXVec3TransformCoord(&vCorners[0],&m_vCorners[0],&mat);
	D3DXVec3TransformCoord(&vCorners[1],&m_vCorners[1],&mat);
	D3DXVec3TransformCoord(&vCorners[2],&m_vCorners[2],&mat);
	D3DXVec3TransformCoord(&vCorners[3],&m_vCorners[3],&mat);
	D3DXVec3TransformCoord(&vCorners[4],&m_vCorners[4],&mat);
	D3DXVec3TransformCoord(&vCorners[5],&m_vCorners[5],&mat);
	D3DXVec3TransformCoord(&vCorners[6],&m_vCorners[6],&mat);
	D3DXVec3TransformCoord(&vCorners[7],&m_vCorners[7],&mat);
}

void CTileEditorModel::RestoreOffset()
{
	D3DXMATRIX matRotY;
	D3DXMatrixRotationY(&matRotY,m_fRotation);
	D3DXVec3TransformCoord(&m_vOffset,&m_vOffset,&matRotY);
}

void CTileEditorModel::UpdateRotationDiscrete()
{	
	m_fRotation += D3DXToRadian(90);
	m_nRotationCnt++;
	D3DXMATRIX matRotY;
	D3DXMatrixRotationY(&m_matRotY,m_fRotation);
	D3DXMatrixRotationY(&matRotY,D3DXToRadian(90));
	
	std::vector<int> vecBlockGrids = m_vecBlockGrids;
	m_vecBlockGrids.clear();
	
	if ( !this->IsSubUnit() )
	{		
		for ( std::vector<int>::iterator iter = vecBlockGrids.begin(); iter != vecBlockGrids.end(); iter++ )
		{
			int nX = (*iter) % CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
			int nY = (*iter) / CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	
			int nCenterX = this->GetPhysicalCenterGridIndex() % CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
			int nCenterY = this->GetPhysicalCenterGridIndex() / CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	
			int nNewX = nCenterX + nY - nCenterY;
			int nNewY = nCenterY + (nCenterX - nX);
			m_vecBlockGrids.push_back(nNewY*CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth() + nNewX);
		}
	}
	else
	{
		D3DXVECTOR3 vOldCenter = m_vCenter;
		ECenter_Type eOldBlockType = m_eCenterType;
		
		switch ( m_eCenterType )
		{
		case ECT_CENTER:
		break;
		
		case ECT_CORNER:
		break;
			
		case ECT_XMIDWAY:
			m_eCenterType = ECT_ZMIDWAY;
			m_vCenter.z += GRID_SPACE/2.0f;
			m_vCenter.x -= GRID_SPACE/2.0f;
		break;
			
		case ECT_ZMIDWAY:
			m_eCenterType = ECT_XMIDWAY;
			m_vCenter.x += GRID_SPACE/2.0f;
			m_vCenter.z -= GRID_SPACE/2.0f;
		break;
		}
		
		D3DXMatrixTranslation(&m_matWorldOff, m_vCenter.x + m_fShiftX, m_vCenter.y + m_fLift, m_vCenter.z + m_fShiftZ);
		
		//////////////////////////////////////////////////////////////////////////
		
		for ( std::vector<int>::iterator iter = vecBlockGrids.begin(); iter != vecBlockGrids.end(); iter++ )
		{
			CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter);
			D3DXVECTOR3 vCenter = grid.vCenter - vOldCenter;
			D3DXVec3TransformCoord(&vCenter,&vCenter,&matRotY);
			
			vCenter = m_vCenter + vCenter;
		
			int nX = vCenter.x / GRID_SPACE;
			int nY = vCenter.z / GRID_SPACE;
			int nNewGridIndex = nX + nY * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
			m_vecBlockGrids.push_back(nNewGridIndex);
		}
	}	
	
	int nWidth = this->GetWidthInGrids();
	int nDepth = this->GetDepthInGrids();

	DWORD dwCnt = nWidth*nDepth;
	
	m_nLogicalCenterGridIndex = m_vecBlockGrids[dwCnt/2];
		
	this->UpdateBoundingBox();
}

int CTileEditorModel::GetPhysicalCenterGridIndex()
{
	return m_nPhysicalCenterGridIndex;
}

int CTileEditorModel::GetLogicalCenterGridIndex()
{	
	return m_nLogicalCenterGridIndex;
}

void CTileEditorModel::SetOffsetOverall(D3DXVECTOR3 vec)
{
	m_vOffsetOverall = vec;
	
	if ( false == m_bSubUnit )
	{
		D3DXMatrixTranslation(&m_matSelfOff, -m_vOffset.x, -m_vOffset.y+m_vScale.y/2.0f, -m_vOffset.z);
	}
	else
	{
		D3DXMatrixTranslation(&m_matSelfOff, -m_vOffsetOverall.x, -m_vOffsetOverall.y, -m_vOffsetOverall.z);
	}
}

void CTileEditorModel::SetCenterGridIndex(int n)
{
	// for place redo...restore the worldoff
	
	if ( n != INVALID )
	{
		if ( !this->IsSubUnit() )
		{
			CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(n);
			D3DXMatrixTranslation(&m_matWorldOff, grid.vCenter.x + m_fShiftX, grid.vCenter.y + m_fLift, grid.vCenter.z + m_fShiftZ);
			m_vCenter = grid.vCenter;
		}
		else
		{
			CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(n);
			
			m_vCenter = grid.vCenter;
			
			switch ( m_eCenterType )
			{
			case ECT_CENTER:
				break;
			
			case ECT_CORNER:
				m_vCenter.x -= GRID_SPACE/2.0f;
				m_vCenter.z -= GRID_SPACE/2.0f;
				break;
			
			case ECT_XMIDWAY:
				m_vCenter.z -= GRID_SPACE/2.0f;
				break;
			
			case ECT_ZMIDWAY:
				m_vCenter.x -= GRID_SPACE/2.0f;
				break;
			}
			
			D3DXMatrixTranslation(&m_matWorldOff, m_vCenter.x + m_fShiftX, m_vCenter.y + m_fLift, m_vCenter.z + m_fShiftZ);
		}
		
		m_nPhysicalCenterGridIndex = n;
		
		int nWidth = this->GetWidthInGrids();
		int nDepth = this->GetDepthInGrids();
		
		DWORD dwCnt = nWidth*nDepth;
		
		m_nLogicalCenterGridIndex = m_vecBlockGrids[dwCnt/2];
	}
	else
	{
		m_vCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
		D3DXMatrixIdentity(&m_matWorldOff);
		m_nPhysicalCenterGridIndex = m_nLogicalCenterGridIndex = INVALID;
	}
}

void CTileEditorModel::RestoreCenter(D3DXVECTOR3 & v)
{
	m_vCenter = v;
	D3DXMatrixTranslation(&m_matWorldOff,m_vCenter.x, m_vCenter.y + m_fLift,m_vCenter.z);
	m_mat = m_matSelfOff*m_matRotY*m_matWorldOff;
}

int CTileEditorModel::GetOriginAdjustedCenterGridIndex(int n)
{
	CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(n);
	int nDeltaX = 0;
	int nDeltaZ = 0;

	switch ( m_eCenterTypeOrigin )
	{
	case ECT_CENTER:
		{
			nDeltaX = (int)( (m_vOffset.x - m_vOffsetOverall.x)/GRID_SPACE + 0.5f*(m_vOffset.x - m_vOffsetOverall.x)/abs(m_vOffset.x - m_vOffsetOverall.x) );
			nDeltaZ = (int)( (m_vOffset.z - m_vOffsetOverall.z)/GRID_SPACE + 0.5f*(m_vOffset.z - m_vOffsetOverall.z)/abs(m_vOffset.z - m_vOffsetOverall.z) );
		}
	break;

	case ECT_CORNER:
		{
			nDeltaX = (int)( (m_vOffset.x - m_vOffsetOverall.x)/GRID_SPACE + 1.0f*(m_vOffset.x - m_vOffsetOverall.x)/abs(m_vOffset.x - m_vOffsetOverall.x) );
			if ( nDeltaX > 0 ) --nDeltaX;
			nDeltaZ = (int)( (m_vOffset.z - m_vOffsetOverall.z)/GRID_SPACE + 1.0f*(m_vOffset.z - m_vOffsetOverall.z)/abs(m_vOffset.z - m_vOffsetOverall.z) );
			if ( nDeltaZ > 0 ) --nDeltaZ;
		}
	break;

	case ECT_XMIDWAY:
		{
			nDeltaX = (int)( (m_vOffset.x - m_vOffsetOverall.x)/GRID_SPACE + 0.5f*(m_vOffset.x - m_vOffsetOverall.x)/abs(m_vOffset.x - m_vOffsetOverall.x) );
			nDeltaZ = (int)( (m_vOffset.z - m_vOffsetOverall.z)/GRID_SPACE + 1.0f*(m_vOffset.z - m_vOffsetOverall.z)/abs(m_vOffset.z - m_vOffsetOverall.z) );
			if ( nDeltaZ > 0 ) --nDeltaZ;
		}
	break;

	case ECT_ZMIDWAY:
		{
			nDeltaX = (int)( (m_vOffset.x - m_vOffsetOverall.x)/GRID_SPACE + 1.0f*(m_vOffset.x - m_vOffsetOverall.x)/abs(m_vOffset.x - m_vOffsetOverall.x) );
			if ( nDeltaX > 0 ) --nDeltaX;
			nDeltaZ = (int)( (m_vOffset.z - m_vOffsetOverall.z)/GRID_SPACE + 0.5f*(m_vOffset.z - m_vOffsetOverall.z)/abs(m_vOffset.z - m_vOffsetOverall.z) );
		}
	break;
	}

	return n + nDeltaX + nDeltaZ*CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
}

//		
//	D3DXVECTOR3 vCenter(m_vCenter.x,m_vCenter.y,m_vCenter.z);
//	D3DXMATRIX matRotCenter;
//	D3DXMatrixRotationY(&matRotCenter,m_fRotate);
//	
//	
//	for ( std::vector<int>::iterator iter = vecVertice.begin(); iter != vecVertice.end(); iter++ )
//	{
//		CTerrainMesh::SVertex & vertex = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(*iter);
//		D3DXVECTOR3 pos = vertex.vPosition;
//	
//		D3DXVECTOR3 dir = pos - vCenter;
//
//       D3DXVec3TransformCoord(&dir,&dir,&matRotCenter);
//
//		pos = vCenter + dir;
//		
//		int n = (int)pos.x/GRID_SPACE + (int)pos.z/GRID_SPACE * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//		
//		CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(n);
//		
//		int nCnt = 0;
//		
//		D3DXVECTOR3 v[4];
//		
//		v[0] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[0]).vPosition;
//		v[1] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[1]).vPosition;
//		v[2] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[2]).vPosition;
//		v[3] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[3]).vPosition;
//		
//		//if ( bc::ut::InsidePolygon(v[0],vCorners) ) nCnt++;
//		//if ( bc::ut::InsidePolygon(v[1],vCorners) ) nCnt++;
//		//if ( bc::ut::InsidePolygon(v[2],vCorners) ) nCnt++;
//		//if ( bc::ut::InsidePolygon(v[3],vCorners) ) nCnt++;
//		
//		if ( IsInside(
//			D3DXVECTOR2(v[0].x,v[0].z),
//			D3DXVECTOR2(vCorners[0].x,vCorners[0].z),
//			D3DXVECTOR2(vCorners[1].x,vCorners[1].z),
//			D3DXVECTOR2(vCorners[2].x,vCorners[2].z),
//			D3DXVECTOR2(vCorners[3].x,vCorners[3].z)) ) nCnt++;
//
//		if ( IsInside(
//			D3DXVECTOR2(v[1].x,v[1].z),
//			D3DXVECTOR2(vCorners[0].x,vCorners[0].z),
//			D3DXVECTOR2(vCorners[1].x,vCorners[1].z),
//			D3DXVECTOR2(vCorners[2].x,vCorners[2].z),
//			D3DXVECTOR2(vCorners[3].x,vCorners[3].z)) ) nCnt++;
//
//		if ( IsInside(
//			D3DXVECTOR2(v[2].x,v[2].z),
//			D3DXVECTOR2(vCorners[0].x,vCorners[0].z),
//			D3DXVECTOR2(vCorners[1].x,vCorners[1].z),
//			D3DXVECTOR2(vCorners[2].x,vCorners[2].z),
//			D3DXVECTOR2(vCorners[3].x,vCorners[3].z)) ) nCnt++;
//
//		if ( IsInside(
//			D3DXVECTOR2(v[3].x,v[3].z),
//			D3DXVECTOR2(vCorners[0].x,vCorners[0].z),
//			D3DXVECTOR2(vCorners[1].x,vCorners[1].z),
//			D3DXVECTOR2(vCorners[2].x,vCorners[2].z),
//			D3DXVECTOR2(vCorners[3].x,vCorners[3].z)) ) nCnt++;
//
//		if ( nCnt < 2 ) continue;
//		
//		if ( find(m_vecDynamicBlockGrids.begin(),m_vecDynamicBlockGrids.end(),n) != m_vecDynamicBlockGrids.end() )
//		{
//			continue; 
//		}
//		else
//		{
//			m_vecDynamicBlockGrids.push_back(n);
//		}
//	}
//}
//
//void CTileEditorModel::UpdateRotation()
//{
//	m_fRotate += 0.1f;
//	D3DXMatrixRotationY(&m_matRotY,m_fRotate);
//
//	//	D3DXVECTOR3 vCorners[4];
//	vCorners[0] = m_vCorners[0];
//	vCorners[1] = m_vCorners[1];
//	vCorners[2] = m_vCorners[2];
//	vCorners[3] = m_vCorners[3];
//
//	D3DXVec3TransformCoord(&vCorners[0],&vCorners[0],&D3DXMATRIX(m_matRotY*m_matWorldOff));
//	D3DXVec3TransformCoord(&vCorners[1],&vCorners[1],&D3DXMATRIX(m_matRotY*m_matWorldOff));
//	D3DXVec3TransformCoord(&vCorners[2],&vCorners[2],&D3DXMATRIX(m_matRotY*m_matWorldOff));
//	D3DXVec3TransformCoord(&vCorners[3],&vCorners[3],&D3DXMATRIX(m_matRotY*m_matWorldOff));
//
//	std::vector<int> vecVertice;
//
//	for ( std::vector<int>::iterator iter = m_vecBlockGrids.begin(); iter != m_vecBlockGrids.end(); iter++ )
//	{
//		CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter);
//
//		for ( int i = 0; i< 4; i++)
//		{
//			int n = grid.dwVertexIndex[i];
//			if ( find(vecVertice.begin(),vecVertice.end(),n) != vecVertice.end() )
//			{
//				continue; 
//			}
//			else
//			{
//				vecVertice.push_back(n);
//			}
//
//		}	
//
//		// up
//		CTerrainMesh::SGrid & gridUp = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter+CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth());
//		for ( int i = 0; i< 4; i++)
//		{
//			int n = gridUp.dwVertexIndex[i];
//			if ( find(vecVertice.begin(),vecVertice.end(),n) != vecVertice.end() )
//			{
//				continue; 
//			}
//			else
//			{
//				vecVertice.push_back(n);
//			}
//		}
//
//		// down
//		CTerrainMesh::SGrid & gridDown = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter-CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth());
//		for ( int i = 0; i< 4; i++)
//		{
//			int n = gridDown.dwVertexIndex[i];
//			if ( find(vecVertice.begin(),vecVertice.end(),n) != vecVertice.end() )
//			{
//				continue; 
//			}
//			else
//			{
//				vecVertice.push_back(n);
//			}
//		}
//
//		// left
//		CTerrainMesh::SGrid & gridLeft = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter-1);
//		for ( int i = 0; i< 4; i++)
//		{
//			int n = gridLeft.dwVertexIndex[i];
//			if ( find(vecVertice.begin(),vecVertice.end(),n) != vecVertice.end() )
//			{
//				continue; 
//			}
//			else
//			{
//				vecVertice.push_back(n);
//			}
//		}
//
//		// right
//		CTerrainMesh::SGrid & gridRight = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter+1);
//		for ( int i = 0; i< 4; i++)
//		{
//			int n = gridRight.dwVertexIndex[i];
//			if ( find(vecVertice.begin(),vecVertice.end(),n) != vecVertice.end() )
//			{
//				continue; 
//			}
//			else
//			{
//				vecVertice.push_back(n);
//			}
//		}
//
//		// rightup
//		CTerrainMesh::SGrid & gridRightUp = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter+CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth()+1);
//		for ( int i = 0; i< 4; i++)
//		{
//			int n = gridRightUp.dwVertexIndex[i];
//			if ( find(vecVertice.begin(),vecVertice.end(),n) != vecVertice.end() )
//			{
//				continue; 
//			}
//			else
//			{
//				vecVertice.push_back(n);
//			}
//		}
//
//		// leftup
//		CTerrainMesh::SGrid & gridLeftUp = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter+CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth()-1);
//		for ( int i = 0; i< 4; i++)
//		{
//			int n = gridLeftUp.dwVertexIndex[i];
//			if ( find(vecVertice.begin(),vecVertice.end(),n) != vecVertice.end() )
//			{
//				continue; 
//			}
//			else
//			{
//				vecVertice.push_back(n);
//			}
//		}
//
//		// rightdown
//		CTerrainMesh::SGrid & gridRightDown = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter-CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth()+1);
//		for ( int i = 0; i< 4; i++)
//		{
//			int n = gridRightDown.dwVertexIndex[i];
//			if ( find(vecVertice.begin(),vecVertice.end(),n) != vecVertice.end() )
//			{
//				continue; 
//			}
//			else
//			{
//				vecVertice.push_back(n);
//			}
//		}
//
//		// leftdown
//		CTerrainMesh::SGrid & gridLeftDown = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter-CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth()-1);
//		for ( int i = 0; i< 4; i++)
//		{
//			int n = gridLeftDown.dwVertexIndex[i];
//			if ( find(vecVertice.begin(),vecVertice.end(),n) != vecVertice.end() )
//			{
//				continue; 
//			}
//			else
//			{
//				vecVertice.push_back(n);
//			}
//		}
//	}
//
//	D3DXVECTOR3 vCenter(m_vCenter.x,m_vCenter.y,m_vCenter.z);
//	D3DXMATRIX matRotCenter;
//	D3DXMatrixRotationY(&matRotCenter,m_fRotate);
//
//	m_vecDynamicBlockGrids.clear();
//
//	for ( std::vector<int>::iterator iter = vecVertice.begin(); iter != vecVertice.end(); iter++ )
//	{
//		CTerrainMesh::SVertex & vertex = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(*iter);
//		D3DXVECTOR3 pos = vertex.vPosition;
//
//		D3DXVECTOR3 dir = pos - vCenter;
//
//		D3DXVec3TransformCoord(&dir,&dir,&matRotCenter);
//
//		pos = vCenter + dir;
//
//		int n = (int)pos.x/GRID_SPACE + (int)pos.z/GRID_SPACE * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
//
//		CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(n);
//
//		int nCnt = 0;
//
//		D3DXVECTOR3 v[4];
//
//		v[0] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[0]).vPosition;
//		v[1] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[1]).vPosition;
//		v[2] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[2]).vPosition;
//		v[3] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(grid.dwVertexIndex[3]).vPosition;
//
//		//if ( bc::ut::InsidePolygon(v[0],vCorners) ) nCnt++;
//		//if ( bc::ut::InsidePolygon(v[1],vCorners) ) nCnt++;
//		//if ( bc::ut::InsidePolygon(v[2],vCorners) ) nCnt++;
//		//if ( bc::ut::InsidePolygon(v[3],vCorners) ) nCnt++;
//
//		if ( IsInside(
//			D3DXVECTOR2(v[0].x,v[0].z),
//			D3DXVECTOR2(vCorners[0].x,vCorners[0].z),
//			D3DXVECTOR2(vCorners[1].x,vCorners[1].z),
//			D3DXVECTOR2(vCorners[2].x,vCorners[2].z),
//			D3DXVECTOR2(vCorners[3].x,vCorners[3].z)) ) nCnt++;
//
//		if ( IsInside(
//			D3DXVECTOR2(v[1].x,v[1].z),
//			D3DXVECTOR2(vCorners[0].x,vCorners[0].z),
//			D3DXVECTOR2(vCorners[1].x,vCorners[1].z),
//			D3DXVECTOR2(vCorners[2].x,vCorners[2].z),
//			D3DXVECTOR2(vCorners[3].x,vCorners[3].z)) ) nCnt++;
//
//		if ( IsInside(
//			D3DXVECTOR2(v[2].x,v[2].z),
//			D3DXVECTOR2(vCorners[0].x,vCorners[0].z),
//			D3DXVECTOR2(vCorners[1].x,vCorners[1].z),
//			D3DXVECTOR2(vCorners[2].x,vCorners[2].z),
//			D3DXVECTOR2(vCorners[3].x,vCorners[3].z)) ) nCnt++;
//
//		if ( IsInside(
//			D3DXVECTOR2(v[3].x,v[3].z),
//			D3DXVECTOR2(vCorners[0].x,vCorners[0].z),
//			D3DXVECTOR2(vCorners[1].x,vCorners[1].z),
//			D3DXVECTOR2(vCorners[2].x,vCorners[2].z),
//			D3DXVECTOR2(vCorners[3].x,vCorners[3].z)) ) nCnt++;
//
//		if ( nCnt < 2 ) continue;
//
//		if ( find(m_vecDynamicBlockGrids.begin(),m_vecDynamicBlockGrids.end(),n) != m_vecDynamicBlockGrids.end() )
//		{
//			continue; 
//		}
//		else
//		{
//			m_vecDynamicBlockGrids.push_back(n);
//		}
//	}
//}

void CTileEditorModel::SetBlockType(std::vector<CTileEditorModel::SBlockInfo> & vecBlock) 
{
	for ( int i = 0; i < m_vecBlockInfo.size(); ++i )
	{
		m_vecBlockInfo[i].nBlockType = vecBlock[i].nBlockType;
	}
}

//void CTileEditorModel::SetBlockLogicHeight(std::vector<CTileEditorModel::SBlockInfo> & vecBlock) 
//{
//	for ( int i = 0; i < vecBlock.size(); ++i )
//	{
//		m_vecBlockInfo[i].fLogicHeight = vecBlock[i].fLogicHeight;
//		m_vecpWireBox[i]->SetHeight(vecBlock[i].fLogicHeight);
//	}
//}

void CTileEditorModel::SetLogicHeight(std::vector<CTileEditorModel::SLogicHeightInfo> & vecHeight) 
{
	for ( int i = 0; i < m_vecLogicHeightInfo.size(); ++i )
	{
		m_vecLogicHeightInfo[i].fLogicHeight = vecHeight[i].fLogicHeight;
		m_vecpWireBox[i]->SetHeight(vecHeight[i].fLogicHeight);
	}
}

void CTileEditorModel::SetBlockMask(std::vector<CTileEditorModel::SBlockInfo> & vecBlock) 
{
	for ( int i = 0; i < m_vecBlockInfo.size(); ++i )
	{
		m_vecBlockInfo[i].nBlockMask = vecBlock[i].nBlockMask;
	}
}

void CTileEditorModel::UpdateHeightBoxes()
{
	for ( std::vector<int>::iterator iter = m_vecBlockGrids.begin(); iter != m_vecBlockGrids.end(); ++iter )
	{
		CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter);
		
		for ( int i = 0; i < 4; ++i )
		{
			if ( find(m_vecBlockVertexIndices.begin(),m_vecBlockVertexIndices.end(),grid.dwVertexIndex[i]) == m_vecBlockVertexIndices.end() )
			{
				m_vecBlockVertexIndices.push_back(grid.dwVertexIndex[i]);
			}
		}
	}

	for ( int i = 0; i < m_vecBlockVertexIndices.size(); ++i )
	{
		D3DXVECTOR3 vCenter = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(m_vecBlockVertexIndices[i]).vPosition;
		m_vecpWireBox[i]->SetRelativePos(vCenter);
		m_vecpWireBox[i]->UpdateAbsolute();
	}
}

void CTileEditorModel::IncreaseBoxHeightByIndex(int n)
{
	m_vecpWireBox[n]->IncreaseHeight();
	m_vecLogicHeightInfo[n].fLogicHeight = m_vecpWireBox[n]->GetHeight();
}

void CTileEditorModel::DecreaseBoxHeightByIndex(int n)
{
	m_vecpWireBox[n]->DecreaseHeight();
	m_vecLogicHeightInfo[n].fLogicHeight = m_vecpWireBox[n]->GetHeight();
}

void CTileEditorModel::ClearBoxHeightByIndex(int n)
{
	m_vecpWireBox[n]->SetHeight(0.0f);
	m_vecLogicHeightInfo[n].fLogicHeight = 0.0f;
}

float CTileEditorModel::GetBoxHeightByIndex(int n)
{
	return 	m_vecLogicHeightInfo[n].fLogicHeight;
}

void CTileEditorModel::SetAnimationByAraFileName(std::string strAraFileName)
{
	IAnimationGroup * pAnimationGroup;

	CTileEditApp::GetInstance()->GetOperator()->GetCoreGraphic()->CreateAnimationGroup(strAraFileName.c_str(), &pAnimationGroup);

	m_pModel->SetAnimateGroup(pAnimationGroup);

	pAnimationGroup->Release();
}

int CTileEditorModel::GetBlockTypeByIndex(int i) 
{	
	return m_vecBlockInfo[i].nBlockType; 
}

void CTileEditorModel::LinkEffect(IEffect * pEffect, IEffectGroup * pGroup)
{
	m_pGroup = pGroup;
	m_pEffect = pEffect;	
	m_pModel->AddLinkObj(pEffect,"test",LT_CENTER, m_pModel->GetAnimateGroup() != NULL);
}

void CTileEditorModel::UnlinkEffect()
{
	m_pModel->DeleteLinkObj("test",LT_CENTER);
}

}