//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "extradefine.h"
#include "IEffectMgrModule.h"
#include "IEffectManager.h"
#include "IEffect.h"
#include "IGraphicModule.h"
#include "ICoreGraphic.h"
#include "Operator.h"
#include "TerrainMesh.h"
#include "Render.h"
#include "Picker.h"
#include "./SharedTexture.h"
#include "BluelyGUI.h"
#include "engine.h"
#include "CTileEditApp.h"
#include "Cmd.h"
#include "BluelyGUI.h"
#include "Camera.h"
#include <FPSCounter.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bg
{
namespace sc
{

IGraphicModule * COperator::ms_pGraphicModule;
IEffectMgrModule * COperator::ms_pEffectMgrModule;
//IAudioManagerModule * COperator::ms_pAudioMgrModule;

char * editstate[] = {"地表高度调整","地表纹理编辑","地表景物放置","逻辑对象设置","地编参数设定","状态为空"};

char * heighttype[] = {"线性","高斯"};

char * alphatype[] = {"线性"};

char * axistype[] = {"沿X轴","沿Z轴","双向"};

char * waytype[] = {"递增","衰减","清零"};

COperator::COperator(ISceneNode * pParent) : ISceneNode(pParent)
{
	//m_eEditState = EES_NONE;
	m_pCoreGraphic = NULL;
	m_pEffectManager = NULL;
	m_pEffect = NULL;
}

COperator::~COperator() { }

void COperator::Create(HWND hResSetView)
{
//	BindModule( "TileEditor.dll" );

//	BindModule( "GraphicDebug.dll" );
	
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
	
	// BindModule( "EffectMgr.dll" );
	std::string strDll;
	strDll = "Effect.dll";
	
	ms_pGraphicModule = ::GetIGraphicModule();
	m_pCoreGraphic = ms_pGraphicModule->CreateGraphic((HANDLE)pDevice,&pRender->m_deviceInfo.d3dpp);
	
	ms_pEffectMgrModule = ::GetIEffectMgrModule();
	m_pEffectManager = ms_pEffectMgrModule->CreateEffectMgr(m_pCoreGraphic,NULL,NULL);
	
	m_pEffectManager->AddClassDesc(strDll.c_str());
	
	this->GetGraphicModule()->ResetGraphicRenderStyle(this->GetCoreGraphic());
	
	// save gui data
	m_hResSetView = hResSetView;
	
	m_toolSetModel.Create(m_hResSetView);
	
	m_toolSetBlockType.Create(m_hResSetView);
	
	m_toolSetBlockLogicHeight.Create(m_hResSetView);
	
	// create shader
	rd::CRenderDevice::Layer3D * pLayer3D = pRender->GetLayer3D();
	m_dwShaderHandle = pLayer3D->CreateShader(INVALID,INVALID,rd::EL3DST_SOLID_1_LAYER,rd::EL3DMST_POSCOLTEX0,true,false,true,true,true,true,false,false);
	
	//// create gui
	//DWORD dwWorldWidth = pRender->GetWindowWidth();
	//DWORD dwWorldHeight = pRender->GetWindowHeight();
	//
	//RECT rc = {0, 0, dwWorldWidth, dwWorldHeight};
	//ui::CWnd * pWnd = new ui::CWnd;
	//pWnd->Create("MapEditor Powered by Bluely Engine",&rc,BWS_VISIBLE,0);
	//
	//eg::CEngine::GetInstance()->SetWnd(pWnd);
	//
	//// check boxes
	//// box 1
	//rc.left = 10;
	//rc.right = 50;
	//rc.top = dwWorldHeight - 50;
	//rc.bottom = dwWorldHeight - 10;
	//m_pCheckBoxTextureLayerOne = new ui::CCheckBox;
	//m_pCheckBoxTextureLayerOne->Create("check1",&rc,BWS_VISIBLE,pWnd);
	//
	//// box 2
	//rc.left = 55;
	//rc.right = 95;
	//rc.top = dwWorldHeight - 50;
	//rc.bottom = dwWorldHeight - 10;
	//m_pCheckBoxTextureLayerTwo = new ui::CCheckBox;
	//m_pCheckBoxTextureLayerTwo->Create("check2",&rc,BWS_VISIBLE,pWnd);
	//
	//DWORD dwTextureHandleLayerOne = CSharedTexture::GetTexture("texture\\DefaultLayerOne.tga")->GetTextureHandle();
	//DWORD dwTextureHandleLayerTwo = CSharedTexture::GetTexture("texture\\DefaultLayerTwo.tga")->GetTextureHandle();
	//
	//m_pCheckBoxTextureLayerOne->SetTextureHandle(dwTextureHandleLayerOne);
	//m_pCheckBoxTextureLayerTwo->SetTextureHandle(dwTextureHandleLayerTwo);
}

void COperator::ResetCheckBoxTextureLayerOne(int nTextureHandle)
{
	m_pCheckBoxTextureLayerOne->SetTextureHandle(nTextureHandle);
}

void COperator::ResetCheckBoxTextureLayerTwo(int nTextureHandle)
{
	m_pCheckBoxTextureLayerTwo->SetTextureHandle(nTextureHandle);
}

void COperator::Update(ip::CInputDevice::SEvent & event)
{
	ISceneNode::Update(event);
}

void COperator::Render()
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
	rd::CRenderDevice::Layer3D * pLayer3D = pRender->GetLayer3D();
	

	//if ( m_eEditState != EES_SET_BLOCK_LOGIC_HEIGHT )
	//{
	// self specific transform
	pRender->GetRawDevice()->SetTransform(D3DTS_WORLD,&this->GetAbsolute());
	
	pLayer3D->SetShader(m_dwShaderHandle);
	
	static D3DXVECTOR3 vertex[4];
	static D3DXVECTOR2 uv0[4];
	static DWORD color[4];

	// Use depth bias
	pDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f) );
	pDevice->SetRenderState( D3DRS_DEPTHBIAS, F2DW(-.012f) );
	
	// draw brush
	if ( this->GetDrawBrush() )
	{
		if ( this->GetGridIndexHot() != INVALID  )
		{
			DWORD brushSize = this->GetBrushSize();

			//DWORD dwCnt = 4*this->GetBrushSize()*this->GetBrushSize();
			DWORD dwCnt = 4 * brushSize * brushSize;

			if ( dwCnt == 0 ) 
				dwCnt = 1;
			
			DWORD terrianWidth = CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
			//DWORD dwGridIndex = this->GetGridIndexHot() - this->GetBrushSize() - this->GetBrushSize() * CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth();
			DWORD dwGridIndex = this->GetGridIndexHot() - brushSize - brushSize * terrianWidth;

			for ( DWORD j = 0; j < dwCnt; j++)
			{
				if ( j )
				{
					if ( !(j % (2 * this->GetBrushSize())) ) 
						dwGridIndex += CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth() - 2*this->GetBrushSize() + 1;
					else
						++dwGridIndex;
				}
				
				if ( dwGridIndex < 0 || dwGridIndex > CTileEditApp::GetInstance()->GetTerrainMesh()->GetWidth() * CTileEditApp::GetInstance()->GetTerrainMesh()->GetDepth() )
				{
					continue;
				}
				


				CTerrainMesh::SGrid & s = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(dwGridIndex);
				
				vertex[0] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[0]).vPosition;
				
				vertex[1] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[3]).vPosition;
				
				vertex[2] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[1]).vPosition;
				
				vertex[3] = CTileEditApp::GetInstance()->GetTerrainMesh()->GetVertex(s.dwVertexIndex[2]).vPosition;

				//////////////////////////////////////////////////////////////////////////
				

				color[0] = 0xff0000ff;
				color[1] = 0xff0000ff;
				color[2] = 0xff0000ff;
				color[3] = 0xff0000ff;
				
				uv0[0] = D3DXVECTOR2(0.0f,0.0f);
				uv0[1] = D3DXVECTOR2(0.0f,0.0f);
				uv0[2] = D3DXVECTOR2(0.0f,0.0f);
				uv0[3] = D3DXVECTOR2(0.0f,0.0f);
				
				pLayer3D->DrawTriangleStrip(2, vertex, NULL, color, uv0,NULL);
			}
		}
		
	//}

	// turn off depth bias
	pDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f) );
	pDevice->SetRenderState( D3DRS_DEPTHBIAS, F2DW(0.0f) );
	}
	
	// warning  object must be drawn after tools ( tools needs identity matrix !)
	
	// pass to tools
	//switch ( m_eEditState ) 
	//{
	/*case EES_SET_BLOCK_TYPE:
		{
			m_toolSetBlockType.Render();
		}
	break;*/
	
	/*case EES_SET_BLOCK_MASK:
		{
			m_toolSetModel.Render();
		}
	break;*/

	//case EES_SET_BLOCK_LOGIC_HEIGHT:
	//	{
	//		m_toolSetBlockLogicHeight.Render();
	//	}
	//break;
	//}
	
	// render the model
	if ( m_pObject )
		m_pObject->GetFace()->Render();
	
	ISceneNode::Render();
}

void COperator::OnEvent(ip::CInputDevice::SEvent & event)
{
	switch ( event.eType )
	{
	case ip::EIET_MS_MOVE:
		{
			CTerrainMesh::CObject * p = NULL;
			D3DXVECTOR3 v;
			
			DWORD dwGridIndexHot = CPicker::GetInstance()->GetPickPos(&p,&v);

			m_pHitVec = v;
			
			if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(dwGridIndexHot) )
			{
				this->SetGridIndexHot(dwGridIndexHot);

				CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(dwGridIndexHot);
				
				if ( int(v.x) % int(GRID_SPACE) <= GRID_SPACE/2 && int(v.z) % int(GRID_SPACE) <= GRID_SPACE/2 )
				{
					this->SetVertexIndexHot(grid.dwVertexIndex[0]);
				}
				else if ( int(v.x) % int(GRID_SPACE) <= GRID_SPACE/2 && int(v.z) % int(GRID_SPACE) >= GRID_SPACE/2 )
				{
					this->SetVertexIndexHot(grid.dwVertexIndex[3]);
				}
				else if ( int(v.x) % int(GRID_SPACE) >= GRID_SPACE/2 && int(v.z) % int(GRID_SPACE) <= GRID_SPACE/2 )
				{
					this->SetVertexIndexHot(grid.dwVertexIndex[1]);
				}
				else if ( int(v.x) % int(GRID_SPACE) >= GRID_SPACE/2 && int(v.z) % int(GRID_SPACE) >= GRID_SPACE/2 )
				{
					this->SetVertexIndexHot(grid.dwVertexIndex[2]);
				}
			}
			
			::SetFocus(eg::CEngine::GetInstance()->GetD3DWindow());
		}
	break;

	case ip::EIET_KB_KEYDOWN :
		{
			switch(event.dwData)
			{
			// shrink brush
			case DIK_MINUS:
				{
					if ( this->GetBrushSize() > 0 ) this->DecBrushSize();
				}
			break;

			// amplify brush
			case DIK_EQUALS:
			{
				this->IncBrushSize();
			}
			break;
			
			/*case DIK_SPACE:
				{
					this->SetEditState( (COperator::EEDIT_STATE) ((this->GetEditState() + 1) % this->GetEditStateCount()) ) ;
					::SendMessage(m_hResSetView,WM_EDITMODECHANGE,m_eEditState,0);
				}
			break;*/

			case DIK_L:
				{
					static bool b = true;
					b = !b;
					CTileEditApp::GetInstance()->GetTerrainMesh()->SetWireFrame(b);
				}
			break;
			}
		}
	break;
	}
	
	//// pass to tools
	//switch ( m_eEditState ) 
	//{
	//case EES_SET_BLOCK_TYPE:
	//	{
	//		m_toolSetBlockType.OnEvent(event);
	//	}
	//break;
	//
	///*case EES_SET_BLOCK_MASK:
	//	{
	//		m_toolSetModel.OnEvent(event);
	//	}
	//break;*/
	//
	////case EES_SET_BLOCK_LOGIC_HEIGHT:
	////	{
	////		m_toolSetBlockLogicHeight.OnEvent(event);
	////	}
	////break;
	//}
}

void COperator::InitProcessParams()
{
	m_processParams.dwGridIndexHot = 0;
	m_processParams.dwTextureHandleHot = INVALID;
	m_processParams.bBrush = true;
	m_processParams.dwBrushSize = 0;
	m_processParams.dwLayerHot = INVALID;
	m_processParams.eSetAlphaType = ESAT_LINEAR;
	m_processParams.eSetHeightType = ESHT_GAUSS;
	m_processParams.eSetWayType = ESWT_INC;
	m_processParams.eSetAxisType = EAT_BOTH;
	m_processParams.eBrushType = EBT_NONE;
	m_processParams.bSelectMode = false;
	m_processParams.pObjectHot = 0;
}

void COperator::MoveTo(CTerrainMesh::CObject * p)
{
	int nGridIndex = p->GetCenterGridIndex();
	if ( CTileEditApp::GetInstance()->GetTerrainMesh()->IsValid(nGridIndex) )
	{
		CTerrainMesh::SGrid & grid = CTileEditApp::GetInstance()->GetTerrainMesh()->GetGrid(nGridIndex);
		CCamera * pCamera = eg::CEngine::GetInstance()->GetActiveCamera();
		pCamera->MoveToRTSByView(grid.vCenter);
	}
}

void COperator::SetBlockType(int n)
{
	m_toolSetBlockType.SetBlockType(n);
}

void COperator::SetTransType(CFace::ETrans_Type e)
{
	if ( m_pObject )
	{
		m_pObject->GetFace()->SetTransType(e);
	}
}

ICoreGraphic * COperator::GetCoreGraphic()
{
	return m_pCoreGraphic;
}

IGraphicModule * COperator::GetGraphicModule()
{
	return COperator::ms_pGraphicModule;
}

void COperator::ToolSetModel_LinkEffect(std::string str, void * p)
{
	if ( m_pEffect )
	{
		if ( m_pObject ) m_pObject->GetFace()->UnlinkEffect();
		m_pEffect->Release();
	}
	
	EffectCreateInfo Ninfo	= {NULL,NULL,NULL} ;

	m_pEffectManager->CreateEffect( (IEffectGroup*)p,str.c_str(), &m_pEffect);
	
	if ( m_pEffect )
	{
		if ( m_pObject ) m_pObject->GetFace()->LinkEffect(m_pEffect,(IEffectGroup*)p);
	}
}

void COperator::GetFXNames(std::string & strFXFileName, std::map< std::string, vector< std::string > > & mapFX, void ** p)
{
	m_pEffectManager->CreateEffectGroup(strFXFileName.c_str(), (IEffectGroup**)p);
	
	for ( size_t i = 0; i < ((IEffectGroup*)(*p))->GetEffectNum(); ++i )
	{
		std::string strFXFullPathName = ((IEffectGroup*)(*p))->GetEffectName(i);
		std::string strFXGroup = strFXFullPathName.substr(0,strFXFullPathName.find_first_of('\\'));
		std::string strFXInstance = strFXFullPathName.substr(strFXFullPathName.find_first_of('\\')+1);
		mapFX[strFXGroup].push_back(strFXInstance);
	}
}

}
}