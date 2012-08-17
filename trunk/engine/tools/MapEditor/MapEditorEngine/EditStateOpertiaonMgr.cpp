#include "stdafx.h"
#include "EditStateOpertiaonMgr.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "Operator.inl"
#include "ToolSetTileTexture.h"
#include "CCameraManager.h"
#include "CMapEditCamera.inl"
#include "CRecordCamera.inl"
#include "CGameGacCamera.h"
#include "ToolRegion.h"
#include "ToolSetTileHeight.h"
#include "ToolSetLogicHeight.h"
#include "ToolRecord.h"
#include "ToolMainPlayer.h"
#include "ToolVertex.h"
#include "ToolSetTileBlock.h"
#include "ToolSetModel.h"
#include "ToolHideTileRegionInView.h"
#include "CWindowTarget.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "IApplication.h"
#include "CEventState.h"

#include "CMapEffect.h"

namespace sqr
{

	CEditStateOpertiaonMgr::CEditStateOpertiaonMgr()
	{
		m_eEditState			= EES_NONE;
		m_dwHotGridIndex		= -1;
		m_bLogicShowLine		= false;
		m_bIsCalObjectWorOffset = true;

		m_bHideLayerOneTex		= false;
		m_bHideLayerTwoTex		= false;
		m_bHideVertexColor		= false;

		this->CreateSingle();
	}

	CEditStateOpertiaonMgr::~CEditStateOpertiaonMgr()
	{
		this->DestroySingle();
	}

	void CEditStateOpertiaonMgr::OnEvent( const CMsg& msg )
	{
		CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrainMesh == NULL )
			return;

		DWORD dwGridIndexHot = 0;

		switch(msg.msgSource) 
		{
		case MGS_KEYMSG:
			switch(msg.msgType.key_type)
			{
			case MGT_KEY_DOWN:
				if ( ( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN ) )
				{
					switch(msg.msgInfo.key_info)
					{
						this->SmallKeyBoardEvent(msg.msgInfo.key_info);

						///game view
					case MGI_KEY_G:
						{
							this->ShowGameView();
							return;
						}

						///map view
					case MGI_KEY_M:
						{
							this->ShowMapEditorView();
							return;
						}

					case MGI_KEY_F:
						{
							if( m_eEditState == EES_SET_TILE_TEXTURE )
							{
								if ( ( CEventState::GetInst()->GetState(MGI_KEY_Menu)== MGT_KEY_DOWN ) )
									CToolSetTileTexture::GetInst()->BrushLayerOneTexture();
							}

							return;
						}

					default:
						break;
					}
				}
				else if ( ( CEventState::GetInst()->GetState(MGI_KEY_ShiftKey) == MGT_KEY_DOWN ) )
				{
					switch(msg.msgInfo.key_info)
					{
					case MGI_KEY_D1:
						CToolSetModel::GetInst()->GetObjectIntersectedPosHeight();
						return;

						///顶视视角
					case MGI_KEY_G:
						this->ShowTopView();
						break;

					default:
						break;
					}
				}
				else
				{
					this->SingleKeyDownEvent(msg.msgInfo.key_info);
				}			
			}
		}

		// pass to tools
		this->ToolStateEvent(msg);
	}

	void CEditStateOpertiaonMgr::ShowGameView()
	{
		CMapEditCamera* camera = CCameraManager::GetInst()->GetMapEditCamera();

		CVector3f vPos = camera->GetNewPosition();
		CVector3f vTarget = camera->GetNewTarget();
		static bool bFirst = true;

		CGameGacCamera *pGameGacViewCamera = CCameraManager::GetInst()->GetGameGacCamera();
		if( bFirst )
		{
			vTarget = camera->GetNewTarget();
			bFirst = false;
		}
		else
			vTarget = pGameGacViewCamera->GetTarget();

		pGameGacViewCamera->SetCameraTarget(vTarget);
		pGameGacViewCamera->BindGameCamera(IApplication::GetInst()->GetCurTime());

		CCameraManager::GetInst()->SetActiveCamera(pGameGacViewCamera);

		// 切换到游戏视角
		m_eDefViewState   = YBTX_GAME;
		STDYAW			  = YBTX_STDYAW;
		STDPITCH		  = YBTX_STDPITCH;
		STDDISTANCE		  = YBTX_MAX_DISTANCE;

		this->m_bGameView = true;
	}

	void CEditStateOpertiaonMgr::ShowMapEditorView()
	{
		CMapEditCamera* mcamera = CCameraManager::GetInst()->GetMapEditCamera();
		float	   fFov    = mcamera->GetFovY();

		mcamera->Bind(CMainWindowTarget::GetInst()->GetCamera());
		mcamera->BeginCtrl(IApplication::GetInst()->GetCurTime());
		this->m_bGameView = false;

		CCameraBase* camera = CCameraManager::GetInst()->GetActiveCamera();
		CVector3f  vPos   = mcamera->GetNewPosition();
		CVector3f  vView  = mcamera->GetNewTarget();

		STDYAW		= DT1_STDYAW;
		STDPITCH	= DT1_STDPITCH;
		STDDISTANCE	= DT1_STDDISTANCE;
		// 还原之前的参数
		camera->SetFovY(fFov);
		camera->SetPosition(vPos);
		camera->SetTarget(vView);
	}

	void CEditStateOpertiaonMgr::ShowOrthogonalView()
	{
		ShowMapEditorView();
		CMapEditCamera* camera = CCameraManager::GetInst()->GetMapEditCamera();

		if ( camera->GetIsOrthogonal() )
		{
			camera->SetIsOrthogonal(false);
			camera->SetFovY(STDFOV);
			camera->SetPitchAndYaw(STDPITCH,STDYAW);
		}
		else
		{
			CVector3f vecPos  = camera->GetNewPosition();
			CVector3f vecView = camera->GetNewTarget();

			CIRect viewPort;
			CMainWindowTarget::GetInst()->GetRect(viewPort);
			float fPatchWidth  = float(viewPort.Width());
			float fPatchHeight = float(viewPort.Height());
			camera->SetPatchWidth(fPatchWidth);
			camera->SetPatchHeight(fPatchHeight);

			camera->SetIsOrthogonal(true);
			CVector3f v = vecPos - vecView;
			v.normalize();
			vecPos = vecView + CVector3f(v.x, STDLOGICDISTANCE, v.z);
		}
	}

	void CEditStateOpertiaonMgr::ShowTopView()
	{
		static bool bTopView = true;
		static CVector3f pos;
		static CVector3f view;

		CMapEditCamera  *pCamera	= CCameraManager::GetInst()->GetMapEditCamera();
		if ( bTopView )
		{
			CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
			if( pTerrainMesh == NULL )
				return;

			int nHotGridIndex = CMapEditApp::GetInst()->GetOperator()->GetGridIndexHot();
			if( pTerrainMesh->IsValid(nHotGridIndex) )
			{
				pos	 = pCamera->GetNewPosition();
				view = pCamera->GetNewTarget();
				SGrid grid = pTerrainMesh->GetGrid(nHotGridIndex);

				pCamera->SetIsOrthogonal(true);

				pCamera->SwitchToTopViewMapStandard(grid.vCenter,89.9999f);
				CCameraManager::GetInst()->Update();
			}
		}
		else
		{
			pCamera->SetIsOrthogonal(false);
			pCamera->SetNewPosition(pos);
			pCamera->SetNewTarget(view);
		}

		bTopView = !bTopView;
	}

	void CEditStateOpertiaonMgr::ShowMainPlayer()
	{
		static EEDIT_STATE e;

		if ( m_eEditState == EES_RUN )
		{
			this->SetEditState(e);
		}
		else
		{
			e = m_eEditState;
			this->SetEditState(EES_RUN);
		}
	}

	void CEditStateOpertiaonMgr::DrawStateBrush()
	{
		// pass to tools
		switch ( m_eEditState )
		{
		case EES_SET_TILE_FACE:
		case EES_SET_TILE_EFFECT:
			{
				CToolSetModel::GetInst()->Render();
			}
			break;

		case EES_SET_TILE_TEXTURE:
			{
				CToolSetTileTexture::GetInst()->Render();
			}
			break;

		case EES_SET_TILE_HEIGHT:
			{
				CToolSetTileHeight::GetInst()->Render();
			}
			break;

		case EES_SET_LOGIC_HEIGHT:
			{
				CToolSetLogicHeight::GetInst()->Render();
			}
			break;

		case EES_REGION:
			{
				CToolRegion::GetInst()->Render();
			}
			break;

		case EES_VERTEX:
			{
				CToolVertex::GetInst()->Render();
			}
			break;

		case EES_SET_TILE_BLOCK:
			CToolSetTileBlock::GetInst()->Render();
			break;

		case EES_SET_TILE_REGIOON:
			CToolHideTileRegionInView::GetInst()->Render();
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::AmplifyBrushX()
	{
		switch ( m_eEditState ) 
		{
			///纹理面板
		case EES_SET_TILE_TEXTURE:
			{
				CToolSetTileTexture::GetInst()->IncBrushSizeInX();
			}
			break;

			///网格面板
		case EES_SET_TILE_HEIGHT:
			{
				CToolSetTileHeight::GetInst()->IncBrushSizeInX();
			}
			break;

		case EES_SET_LOGIC_HEIGHT:
			{
				CToolSetLogicHeight::GetInst()->IncVertexBrushSizeInX();
			}
			break;

			///顶点面板
		case EES_VERTEX:
			{
				CToolVertex::GetInst()->IncBrushSizeInX();
			}
			break;

			///区域面板
		case EES_REGION:
			{
				CToolRegion::GetInst()->IncBrushSizeInX();
			}
			break;

			///阻挡面板
		case EES_SET_TILE_BLOCK:
			{
				CToolSetTileBlock::GetInst()->IncBrushSizeInX();
			}
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::ShrinkBrushX()
	{
		switch ( m_eEditState ) 
		{
			///纹理面板
		case EES_SET_TILE_TEXTURE:
			{
				if ( CToolSetTileTexture::GetInst()->GetBrushSizeInX() > 1 ) CToolSetTileTexture::GetInst()->DecBrushSizeInX();
			}
			break;

			///网格面板
		case EES_SET_TILE_HEIGHT:
			{
				if ( CToolSetTileHeight::GetInst()->GetBrushSizeInX() > 1 ) CToolSetTileHeight::GetInst()->DecBrushSizeInX();
			}
			break;

		case EES_SET_LOGIC_HEIGHT:
			{
				CToolSetLogicHeight::GetInst()->DecVertexBrushSizeInX();
			}
			break;

			///顶点面板
		case EES_VERTEX:
			{
				CToolVertex::GetInst()->DecBrushSizeInX();
			}
			break;

			///区域面板
		case EES_REGION:
			{
				CToolRegion::GetInst()->DecBrushSizeInX();
			}
			break;

			///阻挡面板
		case EES_SET_TILE_BLOCK:
			{
				CToolSetTileBlock::GetInst()->DecBrushSizeInX();
			}
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::AmplifyBrushZ()
	{
		switch ( m_eEditState ) 
		{
			///纹理面板
		case EES_SET_TILE_TEXTURE:
			{
				CToolSetTileTexture::GetInst()->IncBrushSizeInZ();
			}
			break;

			///网格面板
		case EES_SET_TILE_HEIGHT:
			{
				CToolSetTileHeight::GetInst()->IncBrushSizeInZ();
			}
			break;

		case EES_SET_LOGIC_HEIGHT:
			{
				CToolSetLogicHeight::GetInst()->IncVertexBrushSizeInZ();
			}
			break;

			///顶点面板
		case EES_VERTEX:
			{
				CToolVertex::GetInst()->IncBrushSizeInZ();
			}
			break;

			///区域面板
		case EES_REGION:
			{
				CToolRegion::GetInst()->IncBrushSizeInZ();
			}
			break;

			///阻挡面板
		case EES_SET_TILE_BLOCK:
			{
				CToolSetTileBlock::GetInst()->IncBrushSizeInZ();
			}
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::ShrinkBrushZ()
	{
		switch ( m_eEditState ) 
		{
			///纹理面板
		case EES_SET_TILE_TEXTURE:
			{
				if ( CToolSetTileTexture::GetInst()->GetBrushSizeInZ() > 1 ) CToolSetTileTexture::GetInst()->DecBrushSizeInZ();
			}
			break;

			///网格面板
		case EES_SET_TILE_HEIGHT:
			{
				if ( CToolSetTileHeight::GetInst()->GetBrushSizeInZ() > 1 ) 
					CToolSetTileHeight::GetInst()->DecBrushSizeInZ();
			}
			break;

		case EES_SET_LOGIC_HEIGHT:
			{
				if ( CToolSetLogicHeight::GetInst()->GetVertexBrushSizeInZ() > 1 ) 
					CToolSetLogicHeight::GetInst()->DecVertexBrushSizeInZ();
			}
			break;

			///顶点面板
		case EES_VERTEX:
			{
				CToolVertex::GetInst()->DecBrushSizeInZ();
			}
			break;

			///区域面板
		case EES_REGION:
			{
				CToolRegion::GetInst()->DecBrushSizeInZ();
			}
			break;

			///阻挡面板
		case EES_SET_TILE_BLOCK:
			{
				CToolSetTileBlock::GetInst()->DecBrushSizeInZ();
			}
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::HideLayerOneTexture()
	{
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain->IsWater() )
			return;

		m_bHideLayerOneTex = !m_bHideLayerOneTex;

		pTerrain->ReWriteTerrainBuffer( pTerrain->m_TerrainBufferIndexs, 3 );
	}

	void CEditStateOpertiaonMgr::HideLayerTwoTexture()
	{
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain->IsWater() )
			return;

		m_bHideLayerTwoTex = !m_bHideLayerTwoTex;

		pTerrain->ReWriteTerrainBuffer( pTerrain->m_TerrainBufferIndexs, 3 );
	}

	void CEditStateOpertiaonMgr::HideVertexColor()
	{
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		m_bHideVertexColor = !m_bHideVertexColor;

		pTerrain->ReWriteTerrainBuffer( pTerrain->m_TerrainBufferIndexs, 2 );
	}

	void CEditStateOpertiaonMgr::DeleteBlock()
	{
		CToolSetTileBlock::GetInst()->DeleteBlock();
	}

	void CEditStateOpertiaonMgr::PlusEvent()
	{
		switch ( m_eEditState ) 
		{
		case EES_SET_TILE_TEXTURE:
		case EES_SET_TILE_HEIGHT:
		case EES_SET_LOGIC_HEIGHT:
		case EES_VERTEX:
		case EES_REGION:
		case EES_SET_TILE_BLOCK:
			{
				this->AmplifyBrushX();
				this->AmplifyBrushZ();
				break;
			}

		case EES_SET_TILE_FACE:
		case EES_SET_TILE_LOGIN:
			CToolSetModel::GetInst()->ChangeObjectAttention(0);
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::SubtractionEvent()
	{
		switch ( m_eEditState ) 
		{
		case EES_SET_TILE_TEXTURE:
		case EES_SET_TILE_HEIGHT:
		case EES_SET_LOGIC_HEIGHT:
		case EES_VERTEX:
		case EES_REGION:
		case EES_SET_TILE_BLOCK:
			{
				this->ShrinkBrushX();
				this->ShrinkBrushZ();
				break;
			}


		case EES_SET_TILE_FACE:
		case EES_SET_TILE_LOGIN:
			CToolSetModel::GetInst()->ChangeObjectAttention(1);
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::F1Event()
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		if( pTerrain == NULL )
			return;

		if( pTerrain->IsWater() )
		{
			MessageBox(NULL, "水面层状态下不要进行F1操作", "提示", MB_OK);
			return;
		}

		switch ( m_eEditState ) 
		{
		case EES_SET_TILE_TEXTURE:
			this->HideLayerOneTexture();
			break;

		case EES_VERTEX:
			this->HideVertexColor();
			break;

		case EES_REGION:
			CToolRegion::GetInst()->SetHideGrid(true);
			break;

		case EES_SET_TILE_BLOCK:
			CToolSetTileBlock::GetInst()->SetExactGridIndex();
			//CCmdMgr::GetInst()->UnDoToKeyPoint();
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::F2Event()
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		if( pTerrain == NULL )
			return;

		if( pTerrain->IsWater() )
		{
			MessageBox(NULL, "水面层状态下不要进行F2操作", "提示", MB_OK);
			return;
		}

		switch ( m_eEditState ) 
		{
		case EES_SET_TILE_TEXTURE:
			this->HideLayerTwoTexture();
			break;

		case EES_REGION:
			CToolRegion::GetInst()->SetHideGrid(false);
			break;

		case EES_SET_TILE_BLOCK:
			CToolSetTileBlock::GetInst()->SetTerrainBlock();
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::F4Event()
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		if( pTerrain == NULL )
			return;

		if( pTerrain->IsWater() )
		{
			MessageBox(NULL, "水面层状态下不要进行F4操作", "提示", MB_OK);
			return;
		}

		switch ( m_eEditState ) 
		{
		case EES_SET_TILE_TEXTURE:
			CToolSetTileTexture::GetInst()->DeleteTexture();
			break;

		case EES_SET_TILE_REGIOON:
			CToolHideTileRegionInView::GetInst()->OperatorDeleteTileRegion();
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::F8Event()
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		if( pTerrain == NULL )
			return;

		if( pTerrain->IsWater() )
		{
			MessageBox(NULL, "水面层状态下不要进行F8操作", "提示", MB_OK);
			return;
		}


		CSceneEnvMgr::GetInst()->StopBackMusic();
	}

	void CEditStateOpertiaonMgr::DeleteEvent()
	{
		switch ( m_eEditState ) 
		{
		case EES_SET_TILE_FACE:
		case EES_SET_TILE_EFFECT:
		case EES_AUDIO:
		case EES_SET_TILE_LOGIN:
			CToolSetModel::GetInst()->OnDelete();
			break;

		case EES_VERTEX:
			CToolVertex::GetInst()->DeleteVertexColor();
			break;

		case EES_SET_TILE_BLOCK:
			CToolSetTileBlock::GetInst()->DeleteBlock();
			break;

		case EES_SET_LOGIC_HEIGHT:
			CToolSetLogicHeight::GetInst()->EraseLogicHeight();
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::PageUpEvent()
	{
		switch ( m_eEditState ) 
		{
		case EES_SET_TILE_FACE:
		case EES_SET_TILE_EFFECT:
		case EES_AUDIO:
		case EES_SET_TILE_LOGIN:
			CToolSetModel::GetInst()->MoveSelectObject(0);
			break;

		case EES_SET_LOGIC_HEIGHT:
			CToolSetLogicHeight::GetInst()->QuickAddLogicHeight();
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::PageDownEvent()
	{
		switch ( m_eEditState ) 
		{
		case EES_SET_TILE_FACE:
		case EES_SET_TILE_EFFECT:
		case EES_AUDIO:
		case EES_SET_TILE_LOGIN:
			CToolSetModel::GetInst()->MoveSelectObject(1);
			break;

		case EES_SET_LOGIC_HEIGHT:
			CToolSetLogicHeight::GetInst()->QucikDeduceLogicHeight();
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::HomeEvent()
	{
		switch ( m_eEditState ) 
		{
		case EES_SET_TILE_FACE:
		case EES_SET_TILE_EFFECT:
		case EES_AUDIO:
		case EES_SET_TILE_LOGIN:
			CToolSetModel::GetInst()->MoveSelectObject(2);
			break;

		case EES_SET_LOGIC_HEIGHT:
			CToolSetLogicHeight::GetInst()->AddLogicHeight();
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::EndEvent()
	{
		switch ( m_eEditState ) 
		{
		case EES_SET_TILE_FACE:
		case EES_SET_TILE_EFFECT:
		case EES_AUDIO:
		case EES_SET_TILE_LOGIN:
			CToolSetModel::GetInst()->MoveSelectObject(3);
			break;

		case EES_SET_LOGIC_HEIGHT:
			CToolSetLogicHeight::GetInst()->DeduceLogicHeight();
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::SetEditState( EEDIT_STATE e )
	{
		m_eEditState = e;

		if ( m_eEditState == EES_RUN )
		{
			CToolMainPlayer::GetInst()->Restart();
			return;
		}

		if ( m_eEditState == EES_RECORD )
		{
			//CCameraManager::GetInst()->GetRecordCamera()->Bind(CMainWindowTarget::GetInst()->GetCamera());
			//CCameraManager::GetInst()->GetRecordCamera()->BeginCtrl(CEditToolApp::GetInst()->GetCurTime());
		}
		else
		{
			if( !this->m_bGameView )
			{
				/*CCameraManager::GetInst()->GetMapEditCamera()->Bind(CMainWindowTarget::GetInst()->GetCamera());
				CCameraManager::GetInst()->GetMapEditCamera()->BeginCtrl(CEditToolApp::GetInst()->GetCurTime());*/
			}
		}

		if ( m_eEditState == EES_SET_TILE_BLOCK )
		{
			CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
			if(pTerrain)
				pTerrain->ReWriteTerrainBuffer( pTerrain->m_TerrainBufferIndexs, 9 );
		}
	}

	void CEditStateOpertiaonMgr::Initialize(bool bFirst, HWND hhMainFrame, HWND hParamsView, HWND hTextureView, HWND hVertexView, HWND hMeshView, HWND hResSetView, HWND hLoginSetView, HWND hRegionView, HWND hTileRegionView)
	{
		m_bGameView		= false;
		m_eDefViewState = CEditStateOpertiaonMgr::YBTX;

		CToolSetTileTexture::GetInst()->InitProcessParams(hTextureView);
		CToolVertex::GetInst()->Intialize(hVertexView);
		CToolSetTileHeight::GetInst()->InitProcessParams(hMeshView);
		CToolRegion::GetInst()->Initialize(hRegionView);
		CToolSetModel::GetInst()->Initialize(hResSetView, hLoginSetView);
		CToolHideTileRegionInView::GetInst()->Initialize(hTileRegionView);

		if(bFirst)
		{
			CSceneEnvMgr::GetInst()->InitiaRgnDefault();
		}
	}

	void CEditStateOpertiaonMgr::CreateSingle()
	{
		CToolSetTileTexture::Create();
		CToolSetTileBlock::Create();
		CToolRecord::Create();
		CToolVertex::Create();
		CToolSetTileHeight::Create();
		CToolRegion::Create();
		CToolSetModel::Create();
		CToolMainPlayer::Create();
		CSceneEnvMgr::Create();
		CToolHideTileRegionInView::Create();
		CToolSetLogicHeight::Create();

		CMapEffect::Create();

		CSceneEnvMgr::GetInst()->Initialize();
	}

	void CEditStateOpertiaonMgr::DestroySingle()
	{
		CToolSetTileTexture::Destroy();
		CToolSetTileBlock::Destroy();
		CToolRecord::Destroy();
		CToolVertex::Destroy();
		CToolSetTileHeight::Destroy();
		CToolRegion::Destroy();
		CToolSetModel::Destroy();
		CToolMainPlayer::Destroy();
		CToolHideTileRegionInView::Destroy();
		CToolSetLogicHeight::Destroy();

		CSceneEnvMgr::Destroy();
	}

	void CEditStateOpertiaonMgr::UpdateFPSLimited()
	{
		if ( m_eEditState == EES_RECORD )
			CToolRecord::GetInst()->UpdateFPSLimited();

		if( m_bGameView )
		{
			CCameraManager::GetInst()->GetGameGacCamera()->_DoCameraCtrl(IApplication::GetInst()->GetCurTime());
		}

		if( m_eEditState == EES_RUN )
			CToolMainPlayer::GetInst()->UpdateFPSLimited();

		CSceneEnvMgr::GetInst()->UpdateSkyBoxFPSLimited();
	}

	void CEditStateOpertiaonMgr::Update()
	{
		CSceneEnvMgr::GetInst()->Update();
		//this->UpdateFPSLimited();
	}

	void CEditStateOpertiaonMgr::RenderMainPlayer()
	{
		if ( m_eEditState == EES_RUN )
			CToolMainPlayer::GetInst()->Render();
		else
			CToolMainPlayer::GetInst()->UnRender();
	}

	bool CEditStateOpertiaonMgr::GetDoesShowLogicHeight()
	{
		return 	m_eEditState == EES_SET_LOGIC_HEIGHT;
	}

	CVector3f CEditStateOpertiaonMgr::GetCurPlayerPosition() const
	{
		return CToolMainPlayer::GetInst()->GetCurPlayerPosition();
	}

	bool CEditStateOpertiaonMgr::GetDoesShowBlock()
	{
		return 	(m_eEditState == EES_SET_TILE_BLOCK) || CToolSetTileBlock::GetInst()->GetDoesShowBlock();
	}

	void CEditStateOpertiaonMgr::SetMeshLogicHeight( const float fHeight )
	{
		CToolSetLogicHeight::GetInst()->SetMeshLogicHeight(fHeight);
	}

	float CEditStateOpertiaonMgr::GetMeshLogicHeight()
	{
		return CToolSetLogicHeight::GetInst()->GetMeshLogicHeight();
	}

	void CEditStateOpertiaonMgr::SmallKeyBoardEvent( MSG_KEY_INF msg )
	{
		switch (msg)
		{
		case MGI_KEY_NumPad1:
			{
				CCameraManager::GetInst()->GetMapEditCamera()->IncreaseTheFOV();
			}
			break;

		case MGI_KEY_NumPad3:
			{
				CCameraManager::GetInst()->GetMapEditCamera()->DecreaseTheFOV();
			}
			break;

		case MGI_KEY_NumPad8:
			{
				CCameraManager::GetInst()->GetMapEditCamera()->AdjustViewY(true);
			}
			break;

		case MGI_KEY_NumPad2:
			{
				CCameraManager::GetInst()->GetMapEditCamera()->AdjustViewY(false);
			}
			break;

		case MGI_KEY_NumPad7:
			{
				CCameraManager::GetInst()->GetMapEditCamera()->IncreaseTheView();
			}
			break;

		case MGI_KEY_NumPad9:
			{
				CCameraManager::GetInst()->GetMapEditCamera()->DecreaseTheView();
			}
			break;

		case MGI_KEY_NumPad4:
			{
				CCameraManager::GetInst()->GetMapEditCamera()->AdjustBothY(true);
			}
			break;

		case MGI_KEY_NumPad6:
			{
				CCameraManager::GetInst()->GetMapEditCamera()->AdjustBothY(false);
			}
			break;

		default :
			break;
		}
	}

	void CEditStateOpertiaonMgr::ToolStateEvent( const CMsg& msg )
	{
		switch ( m_eEditState ) 
		{
		case EES_SET_TILE_TEXTURE:
			CToolSetTileTexture::GetInst()->OnEvent(msg);
			break;

		case EES_SET_LOGIC_HEIGHT:
			CToolSetLogicHeight::GetInst()->OnEvent(msg);
			break;

		case EES_SET_TILE_HEIGHT:
			CToolSetTileHeight::GetInst()->OnEvent(msg);
			break;

		case EES_SET_TILE_FACE:
		case EES_SET_TILE_EFFECT:
		case EES_AUDIO:
		case EES_SET_TILE_LOGIN:
			CToolSetModel::GetInst()->OnEvent(msg);
			break;

		case EES_RUN:
			CToolMainPlayer::GetInst()->OnEvent(msg);
			break;

		case EES_VERTEX:
			CToolVertex::GetInst()->OnEvent(msg);
			break;

		case EES_SET_TILE_BLOCK:
			CToolSetTileBlock::GetInst()->OnEvent(msg);
			break;

		case EES_REGION:
			CToolRegion::GetInst()->OnEvent(msg);		
			break;

		case EES_RECORD:
			CToolRecord::GetInst()->OnEvent(msg);
			break;

		case EES_SET_TILE_REGIOON:
			CToolHideTileRegionInView::GetInst()->OnEvent(msg);
			break;

		default:
			break;
		}
	}

	void CEditStateOpertiaonMgr::SingleKeyDownEvent( MSG_KEY_INF key )
	{
		CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		switch(key)
		{
			// 隐藏显示阻挡开关
		case MGI_KEY_B:
			{
				this->SwtichShowBlock();
			}
			break;

			// 垂直视角
		case MGI_KEY_O:
			this->ShowOrthogonalView();
			break;

			// 隐藏显示player开关
		case MGI_KEY_P:
			this->ShowMainPlayer();
			return;

			// 隐藏显示场景点光源影响，阻挡和逻辑高
		case MGI_KEY_D1:
			{
				if( m_eEditState == EES_SET_TILE_FACE || m_eEditState == EES_SET_TILE_EFFECT )
				{
					pTerrainMesh->m_bHideAdhereGridsRender = !pTerrainMesh->m_bHideAdhereGridsRender;
					break;
				}

			}
			break;

			///+， PlusEvent操作
		case MGI_KEY_Equals:
			{
				this->PlusEvent();
				return;
			}

			///-， SubtractionEvent操作
		case MGI_KEY_Minus:
			{
				this->SubtractionEvent();
				return;
			}

			/// enlarge brush x 
		case MGI_KEY_D9:
			{
				this->AmplifyBrushX();
				return;
			}

			/// small   brush x 
		case MGI_KEY_D0:
			{
				this->ShrinkBrushX();
				break;
			}

			/// enlarge brush z |
		case MGI_KEY_Back:
			{
				this->AmplifyBrushZ();
				return;
			}

			/// small   brush z 
		case MGI_KEY_Insert:
			{
				this->ShrinkBrushZ();
				return;
			}

			/// home
		case MGI_KEY_Home:
			{
				this->HomeEvent();
				return;
			}

			/// end
		case MGI_KEY_End:
			{
				this->EndEvent();
				return;
			}

			/// pageup
		case MGI_KEY_Prior:
			{	
				this->PageUpEvent();
				return;
			}

			/// pagedown
		case MGI_KEY_Next:
			{
				this->PageDownEvent();
				return;
			}

			/// F1Event
		case MGI_KEY_F1:
			{
				this->F1Event();
				return;
			}

			/// F2Event
		case MGI_KEY_F2:
			{
				this->F2Event();
				return;
			}

			/// F4Event
		case MGI_KEY_F4:
			{
				this->F4Event();
				return;
			}

			/// F8Event
		case MGI_KEY_F8:
			{
				this->F8Event();
				return;
			}

			/// DeleteEvent
		case MGI_KEY_Delete:
			{
				this->DeleteEvent();
				return;
			}

		default:
			break;
		}
	}
}

void CEditStateOpertiaonMgr::SwtichShowBlock()
{
	CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

	bool bShowBlock = !CToolSetTileBlock::GetInst()->GetDoesShowBlock();
	if(bShowBlock)
		pTerrainMesh->ReWriteTerrainBuffer( pTerrainMesh->m_TerrainBufferIndexs, 9 );
	else
		pTerrainMesh->ReWriteTerrainBuffer( pTerrainMesh->m_TerrainBufferIndexs, 2 );

	CToolSetTileBlock::GetInst()->SetDoesShowBlock(bShowBlock);
}
