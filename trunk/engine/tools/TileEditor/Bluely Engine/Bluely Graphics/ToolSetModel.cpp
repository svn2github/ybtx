//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "extradefine.h"
#include "ToolSetModel.h"
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

CToolSetModel::CToolSetModel()
{
}

CToolSetModel::~CToolSetModel()
{
}	

void CToolSetModel::CreateActiveObject(std::vector<std::string> & veCTileEditorModelFileNames, std::vector< std::vector<sc::CTileEditorModel::SBlockInfo> > & vvBlock, std::vector< std::vector<sc::CTileEditorModel::SLogicHeightInfo> > & vvHeight, bool bSubUnit, CTileEditorModel::ECenter_Type eBlockType, float fLift, float fShiftX, float fShiftZ)
{
	COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();
	CTerrainMesh * pTerrainMesh = CTileEditApp::GetInstance()->GetTerrainMesh();

	//////////////////////////////////////////////////////////////////////////
	
	// delete current if any
	
	if ( pOperator->GetActiveObject() )
	{
		pOperator->SetActiveObject(NULL);
	}
	
	//////////////////////////////////////////////////////////////////////////

	// new born

	Ptr<CTerrainMesh::CObject> pObject = new CTerrainMesh::CObject;
	CFace * pFace = new CFace;
	
	std::vector<sc::CTileEditorModel*> vec;
	
	std::vector<std::string>::const_iterator iter = veCTileEditorModelFileNames.begin();
	
	for ( int i = 0; iter != veCTileEditorModelFileNames.end(); ++iter, ++i )
	{
		sc::CTileEditorModel * p = sc::CSceneManager::GetInstance()->AddNodeModel(*iter, bSubUnit, fLift, fShiftX, fShiftZ);
		vec.push_back(p);

		if ( vvBlock[i].size() != 0 )
		{
			p->SetBlockMask(vvBlock[i]);
			p->SetBlockType(vvBlock[i]);
			p->SetLogicHeight(vvHeight[i]);
		}
		else // new added element
		{
			::SendMessage(m_hResSetView,WM_SETBLOCKSIZE,i,p->GetDepthInGrids()*p->GetWidthInGrids());
			::SendMessage(m_hResSetView,WM_SETHEIGHTSIZE,i,(p->GetDepthInGrids()+1)*(p->GetWidthInGrids()+1));
		}
	}
	
	pFace->AddContainer(vec,bSubUnit,eBlockType,0,0);
	
	pFace->SetModelFileNames(veCTileEditorModelFileNames);

	pObject->SetFace(pFace);
	
	pObject->GetFace()->SetEditState(CTileEditorModel::EES_PLACED);
	
	pOperator->SetActiveObject(pObject);
		
	//////////////////////////////////////////////////////////////////////////
	
	// fix the block

	std::vector<int> vecGridIndices;
	
	for ( int nModelIndex = 0; nModelIndex < pObject->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
	{
		CTileEditorModel * pModel = pObject->GetFace()->GetSelfPresentation(nModelIndex);
	
		int nWidth = pModel->GetWidthInGrids();
		int nDepth = pModel->GetDepthInGrids();

		DWORD dwCnt = nWidth*nDepth;

		int nGridIndex;

		if ( pModel->IsSubUnit() )
		{
			nGridIndex = pModel->GetOriginAdjustedCenterGridIndex(FIX_GRID) - (nWidth-1)/2 - ((nDepth-1)/2)*pTerrainMesh->GetWidth();
		}
		else
		{
			nGridIndex = FIX_GRID - (nWidth-1)/2 - ((nDepth-1)/2)*pTerrainMesh->GetWidth();
		}

		for ( DWORD j = 0; j < dwCnt; j++)
		{
			if ( j )
			{
				if ( !(j % nWidth) ) 
					nGridIndex += pTerrainMesh->GetWidth() - nWidth + 1;
				else
					++nGridIndex;
			}
			
			// check for valid grid
			if ( !pTerrainMesh->IsValid(nGridIndex) )
			{
				continue;
			}

			pModel->AddBlockGridIndex(nGridIndex);
		}

		pModel->UpdateHeightBoxes();
	}
}

void CToolSetModel::AdjustActiveObjectCenterType(CTileEditorModel::ECenter_Type e)
{
	COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();
	CTerrainMesh * pTerrainMesh = CTileEditApp::GetInstance()->GetTerrainMesh();

	if ( pOperator->GetActiveObject() )
	{
		pOperator->GetActiveObject()->GetFace()->SetCenterType(e);

		//////////////////////////////////////////////////////////////////////////
		
		// update block vec
		for ( int nModelIndex = 0; nModelIndex < pOperator->GetActiveObject()->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
		{
			CTileEditorModel * pModel = pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(nModelIndex);
			
			pModel->CleanBlockGridIndices();
			
			int nWidth = pModel->GetWidthInGrids();
			int nDepth = pModel->GetDepthInGrids();
			
			DWORD dwCnt = nWidth*nDepth;
			
			int nGridIndex;
			
			if ( pModel->IsSubUnit() )
			{
				nGridIndex = pModel->GetOriginAdjustedCenterGridIndex(FIX_GRID) - (nWidth-1)/2 - ((nDepth-1)/2)*pTerrainMesh->GetWidth();
			}
			else
			{
				nGridIndex = FIX_GRID - (nWidth-1)/2 - ((nDepth-1)/2)*pTerrainMesh->GetWidth();
			}
			
			for ( DWORD j = 0; j < dwCnt; j++)
			{
				if ( j )
				{
					if ( !(j % nWidth) ) 
						nGridIndex += pTerrainMesh->GetWidth() - nWidth + 1;
					else
						++nGridIndex;
				}

				// check for valid grid
				if ( !pTerrainMesh->IsValid(nGridIndex) )
				{
					continue;
				}

				pModel->AddBlockGridIndex(nGridIndex);
			}
		}
	}
}

void CToolSetModel::OnEvent(ip::CInputDevice::SEvent & event)
{
	COperator * pOperator = CTileEditApp::GetInstance()->GetOperator();
	CTerrainMesh * pTerrainMesh = CTileEditApp::GetInstance()->GetTerrainMesh();
	
	switch ( event.eType )
	{
	case ip::EIET_KB_KEYDOWN :	
		{
			if ( pOperator->GetActiveObject() )
			{
				switch(event.dwData)
				{
				// up
				case DIK_HOME:
					{
						pOperator->GetActiveObject()->GetFace()->UpdateLift(1);
						pOperator->GetActiveObject()->GetFace()->SetPlacedEditState(CTileEditorModel::EESP_LIFT);
						::SendMessage(m_hResSetView,WM_LIFT,(WPARAM)pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(0)->GetLift(),0);
					}
				break;

				// down
				case DIK_END:
					{
						pOperator->GetActiveObject()->GetFace()->UpdateLift(0);
						pOperator->GetActiveObject()->GetFace()->SetPlacedEditState(CTileEditorModel::EESP_LIFT);
						::SendMessage(m_hResSetView,WM_LIFT,(WPARAM)pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(0)->GetLift(),0);
					}
				break;

				// right
				case DIK_RIGHT:
					{
						pOperator->GetActiveObject()->GetFace()->UpdateShiftX(1);
						pOperator->GetActiveObject()->GetFace()->SetPlacedEditState(CTileEditorModel::EESP_SHIFTX);
						::SendMessage(m_hResSetView,WM_SHIFTX,(WPARAM)pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(0)->GetShiftX(),0);
					}
				break;

				// left
				case DIK_LEFT:
					{
						pOperator->GetActiveObject()->GetFace()->UpdateShiftX(0);
						pOperator->GetActiveObject()->GetFace()->SetPlacedEditState(CTileEditorModel::EESP_SHIFTX);
						::SendMessage(m_hResSetView,WM_SHIFTX,(WPARAM)pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(0)->GetShiftX(),0);
					}
				break;

				// front
				case DIK_UP:
					{
						pOperator->GetActiveObject()->GetFace()->UpdateShiftZ(1);
						pOperator->GetActiveObject()->GetFace()->SetPlacedEditState(CTileEditorModel::EESP_SHIFTZ);
						::SendMessage(m_hResSetView,WM_SHIFTZ,(WPARAM)pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(0)->GetShiftZ(),0);
					}
				break;

				// back
				case DIK_DOWN: 
					{
						pOperator->GetActiveObject()->GetFace()->UpdateShiftZ(0);
						pOperator->GetActiveObject()->GetFace()->SetPlacedEditState(CTileEditorModel::EESP_SHIFTZ);
						::SendMessage(m_hResSetView,WM_SHIFTZ,(WPARAM)pOperator->GetActiveObject()->GetFace()->GetSelfPresentation(0)->GetShiftZ(),0);
					}
				break;

				// restore the offset
				case DIK_ESCAPE:
					{
						pOperator->GetActiveObject()->GetFace()->InitPlaced();
					}
				break;
				}
			}

			//////////////////////////////////////////////////////////////////////////
			
			// shrink brush
			if ( event.dwData == DIK_MINUS )
			{
				if ( pOperator->GetBrushSize() > 0 ) pOperator->DecBrushSize();
			}

			// amplify brush
			if ( event.dwData == DIK_EQUALS )
			{
				pOperator->IncBrushSize();
			}

		}
	break;

	case ip::EIET_MS_BTNUP :	
		{
			//////////////////////////////////////////////////////////////////////////
					
			// lock it if any under mouse

			//CTerrainMesh::CObject * p = pOperator->GetObjectHot();

			//if ( p )
			//{
			//	if ( m_pLockedObject.ptr() != NULL )
			//	{
			//		m_pLockedObject->GetFace()->SetLocked(false);
			//	}
			//	
			//	m_pLockedObject = p;
			//	m_pLockedObject->GetFace()->SetLocked(true);
			//}
	
			if ( event.dwData == MK_LBUTTON )
			{
				CTerrainMesh::CObject * p = pOperator->GetActiveObject();
			
				if ( p )
				{
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

						for ( int nModelIndex = 0; nModelIndex < p->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
						{
							CTileEditorModel * pModel = p->GetFace()->GetSelfPresentation(nModelIndex);
						
							for ( int i = 0; i < pModel->GetBlockSize(); i++ )
							{
								if ( dwGridIndex == pModel->GetBlockGridIndex(i) )
								{
									pModel->FlipBlockMaskByIndex(i);
									::SendMessage(m_hResSetView,WM_BLOCKMASK_SETBLOCKMASK,nModelIndex,i);
								}
							}
						}
					}
				}
			}

			//if ( event.dwData == MK_RBUTTON )
			//{
			//	CTerrainMesh::CObject * p = this->GetActiveObject();

			//	if ( p )
			//	{
			//		DWORD dwCnt = 4*pOperator->GetBrushSize()*pOperator->GetBrushSize();

			//		if ( dwCnt == 0 ) dwCnt = 1;

			//		DWORD dwGridIndex = pOperator->GetGridIndexHot() - pOperator->GetBrushSize() - pOperator->GetBrushSize() * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
	
			//		for ( DWORD j = 0; j < dwCnt; j++)
			//		{
			//			if ( j )
			//			{
			//				if ( !(j % (2 * pOperator->GetBrushSize())) ) 
			//					dwGridIndex += CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth() - 2*pOperator->GetBrushSize() + 1;
			//				else
			//					++dwGridIndex;
			//			}
	
			//			if ( dwGridIndex < 0 || dwGridIndex > CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth() * CTileEditApp::GetInstance()->GetTerrainMesh()->GetDepth() )
			//			{
			//				continue;
			//			}

			//			for ( int nModelIndex = 0; nModelIndex < p->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
			//			{
			//				CTileEditorModel * pModel = p->GetFace()->GetSelfPresentation(nModelIndex);

			//				for ( int i = 0; i < pModel->GetBlockSize(); i++ )
			//				{
			//					if ( dwGridIndex == pModel->GetBlockGridIndex(i) )
			//					{
			//						pModel->SetBlockFlagByIndex(i,0);
			//					}
			//				}
			//			}
			//		}
			//	}
			//	
			//	//if ( m_pLockedObject.ptr() != NULL )
			//	//{
			//	//	m_pLockedObject->GetFace()->SetLocked(false);
			//	//	m_pLockedObject = NULL;
			//	//}
			//}
		}
	break;
	}
}

void CToolSetModel::Render()
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

				color[0] = 0xffff00ff;
				color[1] = 0xffff00ff;
				color[2] = 0xffff00ff;
				color[3] = 0xffff00ff;

				uv0[0] = D3DXVECTOR2(0.0f,0.0f);
				uv0[1] = D3DXVECTOR2(0.0f,0.0f);
				uv0[2] = D3DXVECTOR2(0.0f,0.0f);
				uv0[3] = D3DXVECTOR2(0.0f,0.0f);

				pLayer3D->DrawTriangleStrip(2, vertex, NULL, color, uv0,NULL);
			}
		}
	}

	// turn off depth bias
	pDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f) );
	pDevice->SetRenderState( D3DRS_DEPTHBIAS, F2DW(0.0f) );
}

void CToolSetModel::Create(HWND hResSetView)
{
	m_hResSetView = hResSetView;
	
	// create shader
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	rd::CRenderDevice::Layer3D * pLayer3D = pRender->GetLayer3D();
	m_dwShaderHandle = pLayer3D->CreateShader(INVALID,INVALID,rd::EL3DST_SOLID_1_LAYER,rd::EL3DMST_POSCOLTEX0,true,false,true,true,true,true,false,false);
}

}
}