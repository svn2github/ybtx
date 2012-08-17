#include "stdafx.h"
#include "Engine.h"
#include "Render.h"
#include "MapGUIManager.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "Operator.inl"
#include "CMapEditCamera.inl"
#include "TerrainAlgorithm.h"
#include "CGraphic.h"
#include "CWindowTarget.h"
#include "TerrainAdhereRenderer.h"
#include "CEditTerrainMgr.h"
#include "IoSystem.h"
#include "BaseHelper.h"
#include "EngineImpl.h"
#include "CCameraManager.h"
#include "Console.h"
#include "StringHelper.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "SceneAudioMgr.h"
#include "EditStateOpertiaonMgr.h"
#include "CXmlConfig.inl"
#include "CSceneConfig.h"
#include "CEditContext.h"
#include "CEditTreeScene.h"
#include "CEditDataScene.h"
#include "EditorExp.h"

CEngine::CEngine()
: m_pImpl(new CEngineImpl)
{
}

CEngine::~CEngine()
{
	SafeDelete(m_pImpl);

	this->Release();
}

void CEngine::Release()
{
	CTerrainMesh *pTerrain = NULL;
	pTerrain = CMapEditApp::GetInst()->GetTerrain();
	if( pTerrain )
	{
		CTerrainMesh *pWater = NULL;
		pWater = pTerrain->GetWater();
		if( pWater )
		{
			delete pWater;
			pWater = NULL;
		}

		delete pTerrain;
		pTerrain = NULL;
	}
}

bool CEngine::Init(bool bMyGUI, bool bMultipleView)
{
	return m_pImpl->Init(bMyGUI, bMultipleView);
}

void  CEngine::RefreshShader(void)
{
	///shader配置文件
	string config("cfg:shaderconfig.xml");
	config = IoSystem::GetInst()->GetFullPath(config);

	ifstream in(config.c_str());
	if ( !in.good() )
	{
		MessageBox(NULL,"请检查是否有物体属性配置文件, 请检查或者全部更新", "提示",MB_OK);
		return;
	}
	
	CXmlConfig *pShaderConfig = new CXmlConfig( "shaderdirectory", in );

	///lift property
	TiXmlNode  *pPathNode = pShaderConfig->Get<TiXmlElement*>("shader_path");
	if ( pPathNode )
	{
		TiXmlElement *pElement = pPathNode->ToElement();
		if (pElement)
		{
			wstring shader_dir = gbk_to_utf16(pElement->GetText());
			CGraphic::GetInst()->InitShader(CSceneConfig::GetInst()->GetShaderConfigPath());
		}
	}

	SafeDelete(pShaderConfig);

	in.close();
}

HWND CEngine::GetD3DWindow()
{
	return NULL;
}

void CEngine::Update()
{
	//if ( FAILED( CMainWindowTarget::GetInst()->IsRenderActive() ) )
	//	return;
	//COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
	//if( pOperator )
	//{
	//	pOperator->DrawTextInfo();
	//}
	CSceneManager * pScene = CSceneManager::GetInst();
	pScene->UpdateWorldFPSLimited();
	// Update camera conctrlers
	//CCameraManager::GetInst()->Update();
	// eniviroment update
	CEditStateOpertiaonMgr::GetInst()->Update();

	SQR_TRY
	{
		if (CMapEditApp::GetInst()->GetRenderScene())
		{
			if(CMapEditApp::GetInst()->GetEditingMesh())
				CMapEditApp::GetInst()->GetEditingMesh()->UpdateSceneLight();	
			CSceneManager::GetInst()->DrawWorld();
			if(CMapEditApp::GetInst()->GetOperator()->GetShowWireBox())
				CMapEditApp::GetInst()->GetDataScene()->GetTreeScene()->DoCtrl(TSShowBox,NULL);
			CMapEditApp::GetInst()->GetEditingMesh()->RenderLogicHeight();
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CRenderScene::Render()");
		GfkLogExp(exp);
	}
	SQR_TRY_END;
}

//jpg格式 导出顶视图
void CEngine::RenderTopViewMap( uint32 Scale,TCHAR const * const strTitle, IProgressCallback* pProgress )
{
	EDITOR_TRY
	{
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain->IsWater() )
		{
			::MessageBox( 0, "当前为水面层，请选择地表层", 0, 0);
			return;
		}

		string savename = string(strTitle);
		size_t nPos = savename.find("project");
		if ( nPos != -1 )
		{
			savename = savename.substr(0, nPos-1);
		}

		uint32 TextureSize = 4096 / Scale;
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		CRenderDevice * pRender = CRenderDevice::GetInst();
		CSceneManager * pScene = CSceneManager::GetInst();
		CTerrainAdhereRenderer::GetInst()->setIsRender(false);

		int Width = pTerrain->GetWidthInRegion()/4;
		int Hight = pTerrain->GetDepthInRegion()/4;

		bool bFog = CSceneEnvMgr::GetInst()->GetDoesEnableFog();
		CSceneEnvMgr::GetInst()->SetDoesEnableFog(false);

		CCamera			*pRawCamera = CMainWindowTarget::GetInst()->GetCamera(); 
		CMapEditCamera  *pCamera	= CCameraManager::GetInst()->GetMapEditCamera();

		CVector3f pos		= pCamera->GetNewPosition();
		CVector3f view		= pCamera->GetNewTarget();
		float fFarDistance  = pCamera->GetFarClip();
		float	  fFar		= pRawCamera->getFarDist();

		pCamera->SetPatchWidth(4096.0f);
		pCamera->SetPatchHeight(4096.0f);
		pCamera->SetIsOrthogonal(true);
		pRawCamera->setFarDist(STDMINIFARPLANE);
		pCamera->SetFarClip(STDFARPLANE);

		//pDevice->GetRenderTarget(0,&pOldRenderTarget);
		CSceneEnvMgr::GetInst()->SetIsGenerateMinMap(true);
		bool bPlayCue = CSceneEnvMgr::GetInst()->GetIsPlaySceneCues();
		CSceneEnvMgr::GetInst()->SetIsPlaySceneCues(false);
		///隐藏全屏泛光
		bool bGassTarget = CSceneEnvMgr::GetInst()->GetIsTurnOnGaussTarget();
		CSceneEnvMgr::GetInst()->SetIsTurnOnGaussTarget(false);
		TerrainFlag::Mask terrainFlag = pTerrain->m_pTerrain->GetRenderFlag();
		pTerrain->m_pTerrain->SetRenderFlag(TerrainFlag::NormalSpecular);
		EShadowType SType = CMainWindowTarget::GetInst()->GetGroup()->GetShadowType();
		CMainWindowTarget::GetInst()->GetGroup()->SetShadowType(ST_NONE);
		///隐藏特效盒子
		pOperator->HideAll();
		///隐藏音效盒子
		CSceneAudioMgr::GetInst()->SwitchSoundObjectsShow(true);
		///隐藏显示区域的颜色
		CEditStateOpertiaonMgr::EEDIT_STATE state = CEditStateOpertiaonMgr::GetInst()->GetEditState();
		CEditStateOpertiaonMgr::GetInst()->SetEditState(CEditStateOpertiaonMgr::EES_NONE);

		if( !pTerrain->IsWater() )
		{
			//烘焙1 固定0
			pTerrain->ReWriteTerrainBuffer( pTerrain->m_TerrainBufferIndexs, 2 );
		}

		// 不进行裁剪，全部insert到渲染队列中
		// TreeScene 重构
		CMapEditApp::GetInst()->GetDataScene()->GetTreeScene()->DoCtrl(sqr_tools::TSRenderAll,NULL);

		float Total = float(Width*Hight);
		if(NULL!=pProgress)
			pProgress->SetProgress( 0.0f );

		CRenderTexture* pRenderTexture = CGraphic::GetInst()->CreateRenderTarget();
		pRenderTexture->SetWidth(TextureSize);
		pRenderTexture->SetHeight(TextureSize);
		pRenderTexture->SetUseDepthBuffer(true);
		pRenderTexture->SetAntiAlias(false);
		pRenderTexture->Create();
		pRenderTexture->SetRenderGroup(CMainWindowTarget::GetInst()->GetGroup());

		ITexture* pMiniMapSurface = NULL;
		CGraphic::GetInst()->CreateEmptyTexture( Width*TextureSize, Hight*TextureSize, 1, TF_ARGB32, &pMiniMapSurface );
		if (pMiniMapSurface==NULL )
		{
			::MessageBox( 0, "创建表面失败", 0, 0);
			return;
		}

		CVector3f v(2048.0f,0.0f,2048.0f);

		for( int w = 0; w<Width; ++w, v.x += 4096.0f,v.z = 2048.0f )
		{
			for( int h = 0; h<Hight; ++h , v.z += 4096.0f )
			{
				pCamera->SwitchToTopViewMapStandard(v,89.9999f);
				CCameraManager::GetInst()->Update();

				CGraphic::GetInst()->ClearStack();
				pRenderTexture->Render();

				CIRect rectDest;
				rectDest.left = w * TextureSize;
				rectDest.top =  (Hight - h - 1) * TextureSize;
				rectDest.right = rectDest.left + TextureSize;
				rectDest.bottom = rectDest.top + TextureSize;

				if ( pMiniMapSurface->FillFromTexture(pRenderTexture,NULL,&rectDest) )
					::MessageBox( 0, "复制表面到小地图失败", 0, 0);

				if(NULL!=pProgress)
					pProgress->SetProgress( float(w*Hight + h)/Total );
			}
		}

		pRenderTexture->Release();

		///newtools的路径，顶试图就存在newtools下的minmap文件夹中
		string strPath = IoSystem::GetInst()->GetFullPath("home:");
		strPath = strPath + "MiniMap\\" + savename + ".jpg";
		if ( S_OK != pMiniMapSurface->SaveToFile(strPath.c_str(),IFF_JPG ) )
		{
			LogConsole("保存小地图失败");
			::MessageBox( 0, "保存小地图失败", 0, 0);
		}

		CTerrainAdhereRenderer::GetInst()->setIsRender(true);
		CMainWindowTarget::GetInst()->GetGroup()->SetShadowType(SType);
		pTerrain->m_pTerrain->SetRenderFlag(terrainFlag);
		pTerrain->m_pTerrain->TurnOnLightMap( (terrainFlag&TerrainFlag::LightMap) != 0 );

		pCamera->SetIsOrthogonal(false);
		pCamera->SwitchToStandard(CVector3f(0.0f,0.0f,0.0f));
		pCamera->SetNewPosition(pos);
		pCamera->SetNewTarget(view);
		pRawCamera->setFarDist(fFar);
		pCamera->SetFarClip(fFarDistance);
		CSceneEnvMgr::GetInst()->SetDoesEnableFog(bFog);
		CSceneEnvMgr::GetInst()->SetIsGenerateMinMap(false);
		CSceneEnvMgr::GetInst()->SetIsPlaySceneCues(bPlayCue);
		/// 恢复全屏泛光
		CSceneEnvMgr::GetInst()->SetIsTurnOnGaussTarget(bGassTarget);
		CEditStateOpertiaonMgr::GetInst()->SetEditState(state);

		pMiniMapSurface->Release();

		if(NULL!=pProgress)
			pProgress->SetProgress( 1.0f );
	}
	EDITOR_CATCH(exp)
	{
		string log = exp.ErrorMsg();

		::MessageBox(NULL, "生成顶视图失败", "提示", MB_ICONEXCLAMATION);
	}
	EDITOR_TRY_END;
}

///导出小地图
void CEngine::RenderGUIMap(IProgressCallback* pProgress)
{
	EDITOR_TRY
	{
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain->IsWater() )
		{
			::MessageBox( 0, "当前为水面层，请选择地表层", 0, 0);
			return;
		}

		string path = IoSystem::GetInst()->GetFullPath("mini:");
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		CRenderDevice * pRender = CRenderDevice::GetInst();
		CSceneManager * pScene = CSceneManager::GetInst();
		CTerrainAdhereRenderer::GetInst()->setIsRender(false);

		bool bLightMap = CSceneEnvMgr::GetInst()->GetIsLightMapShadow();

		uint32 TextureSize = 256;

		int Width = pTerrain->GetWidthInRegion()/4;
		int Hight = pTerrain->GetDepthInRegion()/4;

		bool bFog =  CSceneEnvMgr::GetInst()->GetDoesEnableFog();
		CEditStateOpertiaonMgr::EEDIT_STATE state = CEditStateOpertiaonMgr::GetInst()->GetEditState();
		CEditStateOpertiaonMgr::GetInst()->SetEditState(CEditStateOpertiaonMgr::EES_NONE);

		CCamera			*pRawCamera = CMainWindowTarget::GetInst()->GetCamera(); 
		CMapEditCamera  *pCamera	= NULL;
		pCamera = CCameraManager::GetInst()->GetMapEditCamera();
		if( !pCamera->IsValid() )
		{
			MessageBox(NULL,"请随便旋动下视角","提示",MB_OK);
			return ;
		}

		CVector3f pos  = pCamera->GetPosition();
		CVector3f view = pCamera->GetTarget();
		float  fFarDis = pCamera->GetFarClip();
		float	  fFar = pRawCamera->getFarDist();

		CSceneEnvMgr::GetInst()->SetDoesEnableFog(false);
		pCamera->SetPatchWidth(4096.0f);
		pCamera->SetPatchHeight(4096.0f);
		pCamera->SetIsOrthogonal(true);
		pRawCamera->setFarDist(STDMINIFARPLANE);
		pCamera->SetFarClip(STDFARPLANE);

		TerrainFlag::Mask terrainFlag = pTerrain->m_pTerrain->GetRenderFlag();
		pTerrain->m_pTerrain->SetRenderFlag(TerrainFlag::NormalSpecular);
		EShadowType SType = CMainWindowTarget::GetInst()->GetGroup()->GetShadowType();
		CMainWindowTarget::GetInst()->GetGroup()->SetShadowType(ST_NONE);
		///隐藏特效盒子
		pOperator->HideAll();
		///隐藏音效盒子
		CSceneAudioMgr::GetInst()->SwitchSoundObjectsShow(true);
		CSceneEnvMgr::GetInst()->SetIsGenerateMinMap(true);
		bool bPlayCue = CSceneEnvMgr::GetInst()->GetIsPlaySceneCues();
		CSceneEnvMgr::GetInst()->SetIsPlaySceneCues(false);
		///隐藏全屏泛光
		bool bGassTarget = CSceneEnvMgr::GetInst()->GetIsTurnOnGaussTarget();
		CSceneEnvMgr::GetInst()->SetIsTurnOnGaussTarget(false);

		if( !pTerrain->IsWater() )
		{
			//烘焙1 固定0
			pTerrain->ReWriteTerrainBuffer( pTerrain->m_TerrainBufferIndexs, 2 );
		}

		// TreeScene 重构
		CMapEditApp::GetInst()->GetDataScene()->GetTreeScene()->DoCtrl(sqr_tools::TSRenderAll,NULL);

		float Total = float(Width*Hight);
		if(NULL!=pProgress)
			pProgress->SetProgress( 0.0f );

		CRenderTexture* pRenderTexture = CGraphic::GetInst()->CreateRenderTarget();
		pRenderTexture->SetWidth(TextureSize);
		pRenderTexture->SetHeight(TextureSize);
		pRenderTexture->SetUseDepthBuffer(true);
		pRenderTexture->SetAntiAlias(false);
		pRenderTexture->Create();
		pRenderTexture->SetRenderGroup(CMainWindowTarget::GetInst()->GetGroup());


		CVector3f v(2048.0f,0.0f,2048.0f);

		for( int w = 0; w<Width; ++w, v.x += 4096.0f,v.z = 2048.0f )
		{
			for( int h = 0; h<Hight; ++h , v.z += 4096.0f )
			{
				pCamera->SwitchToTopViewMapStandard(v,89.9999f);
				CCameraManager::GetInst()->Update();

				CGraphic::GetInst()->ClearStack();
				pRenderTexture->Render();

				string filename;
				format(filename,"M_%d_%d.dds",w,h);

				if ( pRenderTexture->SaveToFile((path + filename).c_str(), IFF_DXT1 ) )
				{
					LogConsole("保存小地图失败");
					::MessageBox( 0, "保存小地图失败", 0, 0);
				}
				if(NULL!=pProgress)
					pProgress->SetProgress( float(w*Hight + h)/Total );
			}
		}
		CTerrainAdhereRenderer::GetInst()->setIsRender(true);
		CMainWindowTarget::GetInst()->GetGroup()->SetShadowType(SType);
		pTerrain->m_pTerrain->SetRenderFlag(terrainFlag);
		pTerrain->m_pTerrain->TurnOnLightMap((terrainFlag&TerrainFlag::LightMap) != 0);

		pCamera->SetIsOrthogonal(false);
		pCamera->SwitchToStandard(CVector3f(0.0f,0.0f,0.0f));
		pCamera->SetNewPosition(pos);
		pCamera->SetNewTarget(view);
		pRawCamera->setFarDist(fFar);
		pCamera->SetFarClip(fFarDis);
		CSceneEnvMgr::GetInst()->SetDoesEnableFog(bFog);
		CSceneEnvMgr::GetInst()->SetIsGenerateMinMap(false);
		CSceneEnvMgr::GetInst()->SetIsPlaySceneCues(bPlayCue);
		/// 恢复全屏泛光
		CSceneEnvMgr::GetInst()->SetIsTurnOnGaussTarget(bGassTarget);
		CEditStateOpertiaonMgr::GetInst()->SetEditState(state);

		if(NULL!=pProgress)
			pProgress->SetProgress( 1.0f );
	}
	EDITOR_CATCH(exp)
	{
		string log = exp.ErrorMsg();
		::MessageBox(NULL, "生成小地图失败", "提示", MB_ICONEXCLAMATION);
	}
	EDITOR_TRY_END;
}


