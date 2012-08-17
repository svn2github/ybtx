//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "extradefine.h"
#include "ToolSetBlockLogicHeight.h"
#include "CTileEditApp.h"
#include "TerrainMesh.h"
#include "picker.h"
#include "Scene.h"
#include "CTileEditorModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bg
{
namespace sc
{

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

CToolSetBlockLogicHeight::CToolSetBlockLogicHeight()
{
}

CToolSetBlockLogicHeight::~CToolSetBlockLogicHeight()
{
}	

void CToolSetBlockLogicHeight::OnEvent(ip::CInputDevice::SEvent & event)
{
	COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();
	CTerrainMesh * pTerrainMesh = CTileEditApp::GetInstance()->GetTerrainMesh();

	switch ( event.eType )
	{
	case ip::EIET_KB_KEYDOWN :	
		{
			// shrink brush
			if ( event.dwData == DIK_MINUS )
			{
				if ( this->GetBrushSizeInZ() > 1 ) this->DecBrushSizeInZ();
				if ( this->GetBrushSizeInX() > 1 ) this->DecBrushSizeInX();
			}

			if ( event.dwData == DIK_BACKSLASH )
			{
				if ( this->GetBrushSizeInZ() > 1 ) this->DecBrushSizeInZ();
			}

			if ( event.dwData == DIK_9 )
			{
				if ( this->GetBrushSizeInX() > 1 ) this->DecBrushSizeInX();
			}

			// amplify brush
			if ( event.dwData == DIK_EQUALS )
			{
				this->IncBrushSizeInX();
				this->IncBrushSizeInZ();
			}

			if ( event.dwData == DIK_BACKSPACE )
			{
				this->IncBrushSizeInZ();
			}

			if ( event.dwData == DIK_0 )
			{
				this->IncBrushSizeInX();
			}

			if ( pOperator->GetActiveObject() )
			{
				switch(event.dwData)
				{
					//////////////////////////////////////////////////////////////////////////


					//////////////////////////////////////////////////////////////////////////
					
				case DIK_UP:
					{
						for ( std::vector<DWORD>::iterator iter = m_vecActiveVertexIndices.begin(); iter != m_vecActiveVertexIndices.end(); ++iter )
						{
							COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();

							DWORD dwVertexIndex = *iter;
							
							if ( pOperator->GetActiveObject() )
							{
								for ( int nModelIndex = 0; nModelIndex < pOperator->GetActiveObject()->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
								{
									CTileEditorModel * pModel = pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(nModelIndex);
									
									for ( int i = 0; i < pModel->GetBlockVertexSize(); i++ )
									{
										if ( dwVertexIndex == pModel->GetBlockVertexIndex(i) )
										{
											pModel->IncreaseBoxHeightByIndex(i);
											SBlockLogicHeightExInfo info;
											info.nModelIndex = nModelIndex;
											info.nVertexIndexIndex = i;
											info.fLogicHeight = pModel->GetBoxHeightByIndex(i);
											::SendMessage(m_hResSetView,WM_BLOCKLOGICHEIGHT_SETBLOCKLOGICHEIGHT,(WPARAM)&info,0);
											break;
										}
									}
								}
							}
						}
					}
				break;

				case DIK_DOWN:
					{
						for ( std::vector<DWORD>::iterator iter = m_vecActiveVertexIndices.begin(); iter != m_vecActiveVertexIndices.end(); ++iter )
						{
							COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();

							DWORD dwVertexIndex = *iter;

							if ( pOperator->GetActiveObject() )
							{
								for ( int nModelIndex = 0; nModelIndex < pOperator->GetActiveObject()->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
								{
									CTileEditorModel * pModel = pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(nModelIndex);

									for ( int i = 0; i < pModel->GetBlockVertexSize(); i++ )
									{
										if ( dwVertexIndex == pModel->GetBlockVertexIndex(i) )
										{
											pModel->DecreaseBoxHeightByIndex(i);
											SBlockLogicHeightExInfo info;
											info.nModelIndex = nModelIndex;
											info.nVertexIndexIndex = i;
											info.fLogicHeight = pModel->GetBoxHeightByIndex(i);
											::SendMessage(m_hResSetView,WM_BLOCKLOGICHEIGHT_SETBLOCKLOGICHEIGHT,(WPARAM)&info,0);
											break;
										}
									}
								}
							}
						}
					}
				break;

				case DIK_ESCAPE:
					{
						for ( std::vector<DWORD>::iterator iter = m_vecActiveVertexIndices.begin(); iter != m_vecActiveVertexIndices.end(); ++iter )
						{
							COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();

							DWORD dwVertexIndex = *iter;

							if ( pOperator->GetActiveObject() )
							{
								for ( int nModelIndex = 0; nModelIndex < pOperator->GetActiveObject()->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
								{
									CTileEditorModel * pModel = pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(nModelIndex);

									for ( int i = 0; i < pModel->GetBlockVertexSize(); i++ )
									{
										if ( dwVertexIndex == pModel->GetBlockVertexIndex(i) )
										{
											pModel->ClearBoxHeightByIndex(i);
											SBlockLogicHeightExInfo info;
											info.nModelIndex = nModelIndex;
											info.nVertexIndexIndex = i;
											info.fLogicHeight = pModel->GetBoxHeightByIndex(i);
											::SendMessage(m_hResSetView,WM_BLOCKLOGICHEIGHT_SETBLOCKLOGICHEIGHT,(WPARAM)&info,0);
											break;
										}
									}
								}
							}
						}
					}
				break;
				}
			}

		}
	break;

	case ip::EIET_MS_BTNUP:
		{
			if ( event.dwData == MK_LBUTTON )
			{
				m_vecActiveVertexIndices.clear();

				std::vector<DWORD> & vecActiveVertexIndices = this->GetBrushVertexIndices();

				for ( std::vector<DWORD>::iterator iter = vecActiveVertexIndices.begin(); iter != vecActiveVertexIndices.end(); ++iter )
				{
					DWORD dwVertexIndex = *iter;
					
					COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();
					
					if ( pOperator->GetActiveObject() )
					{
						for ( int nModelIndex = 0; nModelIndex < pOperator->GetActiveObject()->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
						{
							CTileEditorModel * pModel = pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(nModelIndex);
				
							for ( int i = 0; i < pModel->GetBlockVertexSize(); i++ )
							{
								if ( dwVertexIndex == pModel->GetBlockVertexIndex(i) )
								{
									m_vecActiveVertexIndices.push_back(dwVertexIndex);
								}
							}
						}
					}
				}
			}
		}
	break;

	}
}

void CToolSetBlockLogicHeight::Render()
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
	rd::CRenderDevice::Layer3D * pLayer3D = pRender->GetLayer3D();
	
	pLayer3D->SetShader(m_dwShaderHandle);
	
	static D3DXVECTOR3 vertex[4];
	static D3DXVECTOR2 uv0[4];
	static DWORD color[4];
	
	// Use depth bias
	pDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f) );
	pDevice->SetRenderState( D3DRS_DEPTHBIAS, F2DW(-.01f) );
	
	COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();
	CTerrainMesh::CObject * p = pOperator->GetActiveObject();
	
	// in terms of design we'd show height boxes here but for convenience i put the drawing operating in Operator.cpp
	
	std::vector<DWORD> & vecActiveVertexIndices = this->GetBrushVertexIndices();

	// draw locked vertices
	for ( std::vector<DWORD>::iterator iter = vecActiveVertexIndices.begin(); iter != vecActiveVertexIndices.end(); ++iter )
	{
		CTerrainMesh::SVertex & s = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(*iter);
		
		vertex[0] = s.vPosition + D3DXVECTOR3(-GRID_SPACE/2,0.0f,-GRID_SPACE/2);
		vertex[1] = s.vPosition + D3DXVECTOR3(-GRID_SPACE/2, 0.0f, GRID_SPACE/2);
		vertex[2] = s.vPosition + D3DXVECTOR3(GRID_SPACE/2, 0.0f, -GRID_SPACE/2);
		vertex[3] = s.vPosition + D3DXVECTOR3(GRID_SPACE/2, 0.0f, GRID_SPACE/2);
	
		color[0] = 0xffffff00;
		color[1] = 0xffffff00;
		color[2] = 0xffffff00;
		color[3] = 0xffffff00;

		uv0[0] = D3DXVECTOR2(0.0f,0.0f);
		uv0[1] = D3DXVECTOR2(0.0f,0.0f);
		uv0[2] = D3DXVECTOR2(0.0f,0.0f);
		uv0[3] = D3DXVECTOR2(0.0f,0.0f);

		pLayer3D->DrawTriangleStrip(2, vertex, NULL, color, uv0,NULL);
	}

	// turn off depth bias
	pDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f) );
	pDevice->SetRenderState( D3DRS_DEPTHBIAS, F2DW(0.0f) );
}

void CToolSetBlockLogicHeight::Create(HWND hResSetView)
{
	m_hResSetView = hResSetView;

	// create shader
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	rd::CRenderDevice::Layer3D * pLayer3D = pRender->GetLayer3D();
	m_dwShaderHandle = pLayer3D->CreateShader(INVALID,INVALID,rd::EL3DST_SOLID_1_LAYER,rd::EL3DMST_POSCOLTEX0,true,false,true,true,true,true,false,false);
}

DWORD CToolSetBlockLogicHeight::GetBrushSizeInX() 
{ 
	return m_processParams.dwBrushSizeInX; 
}

void CToolSetBlockLogicHeight::SetBrushSizeInX(DWORD dw) 
{ 
	m_processParams.dwBrushSizeInX = dw;	
}

void CToolSetBlockLogicHeight::IncBrushSizeInX() 
{ 
	m_processParams.dwBrushSizeInX++; 
}

void CToolSetBlockLogicHeight::DecBrushSizeInX() 
{ 
	m_processParams.dwBrushSizeInX--;
}

DWORD CToolSetBlockLogicHeight::GetBrushSizeInZ() 
{ 
	return m_processParams.dwBrushSizeInZ; 
}

void CToolSetBlockLogicHeight::SetBrushSizeInZ(DWORD dw) 
{ 
	m_processParams.dwBrushSizeInZ = dw;	
}

void CToolSetBlockLogicHeight::IncBrushSizeInZ() 
{ 
	m_processParams.dwBrushSizeInZ++; 
}

void CToolSetBlockLogicHeight::DecBrushSizeInZ() 
{ 
	 m_processParams.dwBrushSizeInZ--;
}

std::vector<DWORD> & CToolSetBlockLogicHeight::GetBrushVertexIndices()
{
	CTerrainMesh * pTerrain = CTileEditApp::GetInstance()->GetTerrainMesh();
	COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();
	
	static std::vector<DWORD> vecBrushVertexIndices;
	
	vecBrushVertexIndices.clear();

	if ( pTerrain->IsValid(pOperator->GetGridIndexHot()) )
	{
		DWORD dwCnt = this->GetBrushSizeInX() * this->GetBrushSizeInZ();
		
		int nVertexIndex = pOperator->GetVertexIndexHot() - this->GetBrushSizeInX()/2 - (this->GetBrushSizeInZ()/2) * (pTerrain->GetWidth()+1);
		
		for ( DWORD j = 0; j < dwCnt; j++)
		{
			if ( j )
			{
				if ( !(j % this->GetBrushSizeInX()) ) 
					nVertexIndex += pTerrain->GetWidth() + 1 - this->GetBrushSizeInX() + 1;
				else
					++nVertexIndex;
			}

			vecBrushVertexIndices.push_back(nVertexIndex);
		}
	}

	return vecBrushVertexIndices;
}



}
}