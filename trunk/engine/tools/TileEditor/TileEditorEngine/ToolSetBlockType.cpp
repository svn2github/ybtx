//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "extradefine.h"
#include "ToolSetBlockType.h"
#include "CTileEditApp.h"
#include "TerrainMesh.h"
#include "picker.h"
#include "Scene.h"
#include "CTileEditorModel.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

CToolSetBlockType::CToolSetBlockType()
{
}

CToolSetBlockType::~CToolSetBlockType()
{
}	

void CToolSetBlockType::OnEvent(ip::CInputDevice::SEvent & event)
{
	COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();
	CTerrainMesh * pTerrainMesh = CTileEditApp::GetInstance()->GetTerrainMesh();

	switch ( event.eType )
	{
	case ip::EIET_KB_KEYDOWN :	
		{
			if ( pOperator->GetActiveObject() )
			{
			}

		}
	break;

	case ip::EIET_MS_BTNUP:
		{
			if ( event.dwData == MK_LBUTTON )
			{
				DWORD dwBlockType = 0;

				m_vecActiveGrids.clear();

				DWORD dwCnt = 4*pOperator->GetBrushSize()*pOperator->GetBrushSize();

				if ( dwCnt == 0 ) dwCnt = 1;

				DWORD dwGridIndex = pOperator->GetGridIndexHot() - pOperator->GetBrushSize() - pOperator->GetBrushSize() * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();

				for ( DWORD j = 0; j < dwCnt; j++)
				{
					if ( j )
					{
						if ( !(j % (2 * pOperator->GetBrushSize())) ) 
							dwGridIndex += CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth() - 2*pOperator->GetBrushSize() + 1;
						else
							++dwGridIndex;
					}

					if ( dwGridIndex < 0 || dwGridIndex > CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth() * CTileEditApp::GetInstance()->GetTerrainMesh()->GetDepth() )
					{
						continue;
					}

					//////////////////////////////////////////////////////////////////////////

					COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();

					if ( pOperator->GetActiveObject() )
					{
						for ( int nModelIndex = 0; nModelIndex < pOperator->GetActiveObject()->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
						{
							CTileEditorModel * pModel = pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(nModelIndex);

							for ( int i = 0; i < pModel->GetBlockSize(); i++ )
							{
								if ( dwGridIndex == pModel->GetBlockGridIndex(i) )
								{
									if ( pModel->GetBlockMaskByIndex(i) == 1 )
									{
										m_vecActiveGrids.push_back(dwGridIndex);
										DWORD dw = pModel->GetBlockTypeByIndex(i);
										if ( dwBlockType != 0 && dwBlockType != dw )
											dwBlockType = 0xff;
										else
											dwBlockType = dw;
									}
								}
							}
						}
					}
				}

				//////////////////////////////////////////////////////////////////////////
				
				// sync nBlockType with mfc
				::SendMessage(m_hResSetView,WM_BLOCKTYPE_LOCKGRIDS,dwBlockType,0);
			}
			
			if ( event.dwData == MK_RBUTTON )
			{
				m_vecActiveGrids.clear();
			}
		}
	break;

	}
}

void CToolSetBlockType::Render()
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
	rd::CRenderDevice::Layer3D * pLayer3D = pRender->GetLayer3D();
	
	//// self specific transform
	//pRender->GetRawDevice()->SetTransform(D3DTS_WORLD,&this->GetAbsolute());
	
	pLayer3D->SetShader(m_dwShaderHandle);
	
	static D3DXVECTOR3 vertex[4];
	static D3DXVECTOR2 uv0[4];
	static DWORD color[4];

	// Use depth bias
	pDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f) );
	pDevice->SetRenderState( D3DRS_DEPTHBIAS, F2DW(-.01f) );

	COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();
	CTerrainMesh::CObject * p = pOperator->GetActiveObject();
	
	// show the block
	if ( p )
	{
		for ( int nModelIndex = 0; nModelIndex < p->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
		{
			CTileEditorModel * pModel = p->GetFace()->GetSelfPresentation(nModelIndex);

			for ( int i = 0; i < pModel->GetBlockSize(); i++ )
			{
				if ( pModel->GetBlockMaskByIndex(i) == 0 )
				{
					continue;
				}

				CTerrainMesh::SGrid & s = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(pModel->GetBlockGridIndex(i));
				
				vertex[0] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[0]).vPosition;
				vertex[1] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[3]).vPosition;
				vertex[2] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[1]).vPosition;
				vertex[3] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[2]).vPosition;
				
				if ( pModel->GetBlockTypeByIndex(i) == 1 )
				{
					color[0] = 0xff000000;
					color[1] = 0xff000000;
					color[2] = 0xff000000;
					color[3] = 0xff000000;
				}
				else if ( pModel->GetBlockTypeByIndex(i) == 2 ) 
				{
					color[0] = 0xff00ff00;
					color[1] = 0xff00ff00;
					color[2] = 0xff00ff00;
					color[3] = 0xff00ff00;
				}
				else if ( pModel->GetBlockTypeByIndex(i) == 3 ) 
				{
					color[0] = 0xffff0000;
					color[1] = 0xffff0000;
					color[2] = 0xffff0000;
					color[3] = 0xffff0000;
				}
				
				uv0[0] = D3DXVECTOR2(0.0f,0.0f);
				uv0[1] = D3DXVECTOR2(0.0f,0.0f);
				uv0[2] = D3DXVECTOR2(0.0f,0.0f);
				uv0[3] = D3DXVECTOR2(0.0f,0.0f);
				
				pLayer3D->DrawTriangleStrip(2, vertex, NULL, color, uv0,NULL);
			}
		}
	}


	// draw locked grids
	for ( std::vector<int>::iterator iter = m_vecActiveGrids.begin(); iter != m_vecActiveGrids.end(); ++iter )
	{
		CTerrainMesh::SGrid & s = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(*iter);

		vertex[0] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[0]).vPosition;
		vertex[1] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[3]).vPosition;
		vertex[2] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[1]).vPosition;
		vertex[3] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[2]).vPosition;

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

void CToolSetBlockType::Create(HWND hResSetView)
{
	m_hResSetView = hResSetView;

	// create shader
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	rd::CRenderDevice::Layer3D * pLayer3D = pRender->GetLayer3D();
	m_dwShaderHandle = pLayer3D->CreateShader(INVALID,INVALID,rd::EL3DST_SOLID_1_LAYER,rd::EL3DMST_POSCOLTEX0,true,false,true,true,true,true,false,false);
}

void CToolSetBlockType::SetBlockType(int nBlockType)
{
	for ( std::vector<int>::iterator iter = m_vecActiveGrids.begin(); iter != m_vecActiveGrids.end(); ++iter )
	{
		COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();

		if ( pOperator->GetActiveObject() )
		{
			for ( int nModelIndex = 0; nModelIndex < pOperator->GetActiveObject()->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
			{
				CTileEditorModel * pModel = pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(nModelIndex);

				for ( int i = 0; i < pModel->GetBlockSize(); i++ )
				{
					if ( (*iter) == pModel->GetBlockGridIndex(i) )
					{
						pModel->SetBlockTypeByIndex(i,nBlockType);
						SBlockTypeExInfo info;
						info.nModelIndex = nModelIndex;
						info.nGridIndexIndex = i;
						info.nBlockType = nBlockType;
						::SendMessage(m_hResSetView,WM_BLOCKTYPE_SETBLOCKTYPE,(WPARAM)&info,0);
						break;
					}
				}
			}
		}
	}
}

}
}