#include "stdafx.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "UserMsg.h"
#include "SceneRegionMgr.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"
#include "CLightSystem.h"
#include "CRenderPipeline.h"
#include "IAudioModule.h"
#include "CTerrainMgr.h"
#include "MEGraphic.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "ToolMainPlayer.h"
#include "EditStateOpertiaonMgr.h"
#include "..\src\Scene\CShadowedGrids.h"
#include "IoSystem.h"
#include "StringHelper.h"
#include "CBloomTarget.h"
#include "Render.h"
#include "CEditDataScene.h"
#include "IApplication.h"
#include "CEditSkyMgr.h"
#include "CEditContext.h"
namespace sqr
{
	string  g_strRegionMusicName;
	/////////////////////////////////////////////////////
	const int nSaveMinStep  = 30;						///隔多少分进行自动保存
	const int nSaveStepTime = 1000 * 60 * nSaveMinStep; ///隔多少秒进行自动保存

	CSceneEnvMgr::CSceneEnvMgr()
	{
		m_byStarVisibility		 = 0x00;

		m_pShadowedGrids		 = NULL;		
		m_pShadowedGrids		 = new CShadowedGrids;	
		m_bSetLightByScreenRange = false;
		m_bIsPlaySceneCues		 = true;
		m_bGenerateMinMap		 = false;
		m_bTurnOnGassTarget		 = false;


		m_UVCenter				= CVector2f(0.5f, 0.5f);
		m_fSkyBoxScale			= 1.0f;

		m_pTarget				= NULL;
	}

	CSceneEnvMgr::~CSceneEnvMgr()
	{
		if( m_pShadowedGrids )
		{
			delete m_pShadowedGrids;
			m_pShadowedGrids = NULL;
		}
	}

	void CSceneEnvMgr::Initialize()
	{
		m_fGaussBrightPass	= 0.000f; //光通量 0~1
		m_fGaussMultiplier  = 0.35f; //泛光强度 0~2
		m_fGaussMean		= 0.292f; //泛光偏移 -1~+1
		m_fGaussStdDev		= 0.363f; //泛光融和程度  0~1
		m_fGaussExposure	= 1.00f; //最终亮度0~2
		m_fGaussSam			= 0.25f; //混合度	
		
		this->CreateBloomTarget();
		this->SetGaussBrightPass(m_fGaussBrightPass);
		this->SetGaussMultiplier(m_fGaussMultiplier);
		this->SetGaussMean(m_fGaussMean);
		this->SetGaussStdDev(m_fGaussStdDev);
		this->SetGaussExposure(m_fGaussExposure);
		this->SetGaussSample(m_fGaussSam);

		m_fLightMapZOffset  = 0.0f;

		this->SetDoesEnableFog(false);
		this->SetVertexFog(FMT_LINEAR);

		CVector3f gLightDir = CVector3f(0.390625f, -0.70347214f, -0.59375f);

		m_fSReflectionPower[0] = m_fSReflectionPower[1] = 5.0f;

		this->SetSceneAmbientColor(0xf0f0f0);
		this->SetSceneDirColor(0xa08c68);
		this->SetDirDir(gLightDir);
		this->SetPlayerAmbientColor(0xffa08c68); 
		this->SetPlayerDirColor(0x505050);
		
		this->SetShadowColor(0x999999);
		
		this->SetVertexFog(FMT_LINEAR);
		this->SetFogColor(0xffffff);
		this->SetFogStart(FOGSTART);
		this->SetFogEnd(FOGEND);
		this->SetDensity(FOGDENSITY);

		m_bOnAutoSaveMap  = true;
		m_bEnableFog	  = true;
		m_bSetBackPicMove = false;
		m_bLightMapShadow = false;

		CMainWindowTarget::GetInst()->GetGroup()->SetShadowType(ST_SHADOWMAP);
	}

	void CSceneEnvMgr::InitiaRgnDefault()
	{
		SceneRegion *pCurSceneRegion =  new SceneRegion;
		if( pCurSceneRegion )
		{
			pCurSceneRegion->lightParam.globalAmbient	= m_envs.m_dwSceneAmbient;
			pCurSceneRegion->lightParam.globalDiffuse	= m_envs.m_dwSceneDirColor;
			pCurSceneRegion->lightParam.playerAmbient	= m_envs.m_dwPlayerAmbientColor;
			pCurSceneRegion->lightParam.playerDiffuse	= m_envs.m_dwPlayerDirColor;
			pCurSceneRegion->lightParam.globalShadow	= m_envs.m_dwShadowColor;
			pCurSceneRegion->lightParam.globalSpecular	= m_envs.m_dwSceneSpecularColor;
			pCurSceneRegion->lightParam.playerSpecular	= m_envs.m_dwPlayerSpecularColor;
			pCurSceneRegion->lightParam.fogColor		= m_envs.m_dwFogColor;
			pCurSceneRegion->lightParam.fogStart		= m_envs.m_fFogStart;
			pCurSceneRegion->lightParam.fogEnd			= m_envs.m_fFogEnd;
			pCurSceneRegion->lightParam.fogDensity		= m_envs.m_fFogDensity;
			pCurSceneRegion->SetSceneRgnMusicName(m_strBackgroundMusic);
		}
		CSceneRegionMgr::GetInst()->AddSceneRegion("default", pCurSceneRegion);

		this->SetShadowColor(m_envs.m_dwShadowColor);
	}

	void CSceneEnvMgr::UpdateClock( const DWORD dwTimeElapsed )
	{
		if(m_dwLastAutoSaveTime != 0 && this->m_bOnAutoSaveMap)
		{
			if( ::GetTickCount() < m_dwLastAutoSaveTime )
			{
				m_dwLastAutoSaveTime = ::GetTickCount();
			} 
			else
			{
				if ( ::GetTickCount() - m_dwLastAutoSaveTime > nSaveStepTime )
				{
					m_dwLastAutoSaveTime = ::GetTickCount();
					//::SendMessage( hMainFrame, WM_AUTOSAVE, 0, 0);
				}
			}
		}
	}

	void CSceneEnvMgr::UpdateSkyBoxFPSLimited()
	{
		static CVector3f predir;
		static float moveu = 0.0f, movev = 0.0f;

		if( /*state == COperator::EEDIT_STATE::EES_RUN && */m_bSetBackPicMove )
		{
			CVector3f dir = CToolMainPlayer::GetInst()->GetCurPlayerDirect();

			if( predir != dir )
			{
				if( dir.x > 0.0f )
				{
					moveu -= m_fBackRightMove * 0.00001f;
				}
				else
				{
					moveu += m_fBackLeftMove * 0.00001f;
				}

				if( dir.z > 0.0f )
				{
					movev -= m_fBackTopMove * 0.00001f;
				}
				else
				{
					movev += m_fBackBottonMove * 0.00001f;
				}
			}

			CMapEditApp::GetInst()->GetDataScene()->GetSkyMgr()->SetEditSkyUVOffset(moveu, movev);
			predir = dir;
		}
	}

	void CSceneEnvMgr::Update()
	{
		static CLightSystem* pLightSystem = CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem();

		// npc
		pLightSystem->SetAmbient(m_envs.m_dwPlayerAmbientColor, 1);
		pLightSystem->SetGlobalLightColor(m_envs.m_dwPlayerDirColor, 1);

		//scene
		pLightSystem->SetAmbient(m_envs.m_dwSceneAmbient, 0);
		pLightSystem->SetGlobalLightColor(m_envs.m_dwSceneDirColor, 0);

// 		///specular
// 		pLightSystem->SetSpecular(m_envs.m_dwSceneSpecularColor, 0);
// 		pLightSystem->SetSpecular(m_envs.m_dwPlayerSpecularColor, 1);
//  		CMapEditApp::GetInst()->GetTerrain()->m_pTerrain->SetTerMatSpecularPower(m_fSReflectionPower[0]);

		pLightSystem->SetGlobalLightDir( m_envs.m_vDirDir );

		pLightSystem->EnableGlobalLight( true );
		pLightSystem->EnableDynamicLight( true );

		CCamera * pCamera = CMainWindowTarget::GetInst()->GetCamera();
		pLightSystem->UpdateLights( pCamera->GetPosition(), false );

		// fog
 		if( m_bEnableFog )
 		{
 			this->UpdateSceneFog();
		}

		// 音效
		GetAudioModule()->GetAudioSystem()->Update();
	}

	void CSceneEnvMgr::SetSkyFarBackTextureName( string name )
	{
		m_strSkyboxTexture = name;
		CMapEditApp::GetInst()->GetDataScene()->GetSkyMgr()->SetSkyBoxTextureName(name);
	}

	void CSceneEnvMgr::SetSceneAmbientColor( DWORD color )
	{
		if( color != m_envs.m_dwSceneAmbient )
		{
			m_envs.m_dwSceneAmbient = color;

			SceneRegion *pDefaultSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion("default");
			if( pDefaultSceneRgn )
				pDefaultSceneRgn->lightParam.globalAmbient = color;
		}
	}

	void CSceneEnvMgr::SetPlayerAmbientColor( DWORD color )
	{
		if( color != m_envs.m_dwPlayerAmbientColor )
		{
			m_envs.m_dwPlayerAmbientColor = color;

			SceneRegion *pDefaultSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion("default");
			if( pDefaultSceneRgn )
				pDefaultSceneRgn->lightParam.playerAmbient = color;
		}
	}

	void CSceneEnvMgr::SetSceneDirColor( DWORD color)
	{
		if( color != m_envs.m_dwSceneDirColor )
		{
			m_envs.m_dwSceneDirColor = color;

			SceneRegion *pDefaultSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion("default");
			if( pDefaultSceneRgn )
				pDefaultSceneRgn->lightParam.globalDiffuse = color;
		}
	}

	void CSceneEnvMgr::SetPlayerDirColor( DWORD color )
	{
		if( m_envs.m_dwPlayerDirColor != color )
		{
			m_envs.m_dwPlayerDirColor = color;

			SceneRegion *pDefaultSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion("default");
			if( pDefaultSceneRgn )
				pDefaultSceneRgn->lightParam.playerDiffuse = color;
		}
	}

	void CSceneEnvMgr::SetShadowColor( DWORD color )
	{
		m_envs.m_dwShadowColor = color;

		SceneRegion *pDefaultSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion("default");
		if( pDefaultSceneRgn )
			pDefaultSceneRgn->lightParam.globalShadow = color;

		CLightSystem* lightSystem = CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem();
		lightSystem->SetGlobalShadowColor(color);
	}

	void CSceneEnvMgr::SetSceneSpecurlarColor( DWORD dw )
	{
		if( dw != m_envs.m_dwSceneSpecularColor )
		{
			m_envs.m_dwSceneSpecularColor = dw;
			SceneRegion *pDefaultSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion("default");
			if( pDefaultSceneRgn )
				pDefaultSceneRgn->lightParam.globalSpecular = dw;
		}
	}

	void CSceneEnvMgr::SetPlayerSpecurlarColor( DWORD dw )
	{
		if( dw != m_envs.m_dwPlayerSpecularColor )
		{
			m_envs.m_dwPlayerSpecularColor = dw;
			SceneRegion *pDefaultSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion("default");
			if( pDefaultSceneRgn )
				pDefaultSceneRgn->lightParam.playerSpecular = dw;
		}
	}

	void CSceneEnvMgr::SetSpecularReflectionPower( const int index, const float value )
	{
		if( index > 1)
			return;

		m_fSReflectionPower[index] = value;
	}

	void CSceneEnvMgr::SetDoesEnableFog( bool b )
	{
		m_bEnableFog = b;

		if( m_bEnableFog )
		{
			this->UpdateSceneFog();
		}

		CMainWindowTarget::GetInst()->GetGroup()->EnableFog(m_bEnableFog);
	}

	void CSceneEnvMgr::SetFogColor( DWORD dw )
	{
		if( m_envs.m_dwFogColor != dw )
		{
			m_envs.m_dwFogColor = dw;

			SceneRegion *pDefaultSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion("default");
			if( pDefaultSceneRgn )
				pDefaultSceneRgn->lightParam.fogColor = dw;
		}
	}

	void CSceneEnvMgr::SetFogStart( float fStart )
	{
		if( m_envs.m_fFogStart != fStart )
		{
			m_envs.m_fFogStart = fStart;

			SceneRegion *pDefaultSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion("default");
			if( pDefaultSceneRgn )
				pDefaultSceneRgn->lightParam.fogStart = fStart;

			//CMapEditApp::GetInst()->m_pRenderScene->SetFogParams(m_envs.m_fFogStart, m_envs.m_fFogEnd);
		}
	}

	void CSceneEnvMgr::SetFogEnd(float fEnd)
	{
		if( m_envs.m_fFogEnd != fEnd )
		{
			m_envs.m_fFogEnd = fEnd;

			SceneRegion *pDefaultSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion("default");
			if( pDefaultSceneRgn )
				pDefaultSceneRgn->lightParam.fogEnd = fEnd;

			//CMapEditApp::GetInst()->m_pRenderScene->SetFogParams(m_envs.m_fFogStart, m_envs.m_fFogEnd);
		}
	}

	void CSceneEnvMgr::SetDensity( float fDensity )
	{
		m_envs.m_fFogDensity = fDensity;

		SceneRegion *pDefaultSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion("default");
		if( pDefaultSceneRgn )
			pDefaultSceneRgn->lightParam.fogDensity = fDensity;

		CRenderPipeline::GetInst()->_SetRenderStyle(RS_FOGDENSITY, *(DWORD *)(&fDensity));
	}

 	void CSceneEnvMgr::ShowLightMap( bool b )
 	{
 		m_bLightMapShadow = b;

		CMapEditApp::GetInst()->GetEditingMesh()->ReLoadLightMap(b);
 	}

	void CSceneEnvMgr::ShowRealtimeShadow( bool b )
	{
		CMainWindowTarget::GetInst()->GetGroup()->SetShadowType(b?ST_SHADOWMAP:ST_NONE);
	}

	bool CSceneEnvMgr::GetIsRealtimeShadow()
	{
		EShadowType shadowType = CMainWindowTarget::GetInst()->GetGroup()->GetShadowType();
		bool bShadowMap = shadowType == ST_SHADOWMAP;
		return bShadowMap;
	}

	void CSceneEnvMgr::SetBackGroundMusic( string & str, bool bRegion )
	{
		if( str == "" )
			return;

		if( bRegion )
		{
			g_strRegionMusicName = str;
			PlayCue(str.c_str());
		}
		else
		{
			if( m_strBackgroundMusic != str )
			{ 
				m_strBackgroundMusic = str;
				PlayCue(m_strBackgroundMusic.c_str());
			}
		}
	}

	void CSceneEnvMgr::StopBackMusic()
	{
		if ( m_strBackgroundMusic.empty() )
			return;

		static bool bStopMusic = true;

		if ( bStopMusic )
		{
			if( !g_strRegionMusicName.empty() )
				StopCue(g_strRegionMusicName.c_str());

			StopCue(m_strBackgroundMusic.c_str());
		}
		else
			PlayCue(m_strBackgroundMusic.c_str());

		bStopMusic = !bStopMusic;
	}

	sqr::CColor CSceneEnvMgr::GetShadowedColor( int xGrid, int yGrid )
	{
		return m_pShadowedGrids->GetShadowedColor(xGrid, yGrid);
	}

	void CSceneEnvMgr::LoadShadowedColor()
	{
		///sh1文件 shadow.sh1
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

#ifdef USE_OLD_RESOURCEDIR_NOTCODER
		string ligpath = "map/" + pTerrain->m_strMapName;
		m_pShadowedGrids->Load(ligpath, pTerrain->GetWidth(), pTerrain->GetDepth());
#else

		int offset = 1;
		string fileName = format("lit:shadow.sh%d", offset);
		fileName = IoSystem::GetInst()->GetFullPath("tgtmap:");

		m_pShadowedGrids->Load(fileName, pTerrain->GetWidth(), pTerrain->GetDepth());
#endif
	}

	bool CSceneEnvMgr::ShadowedColorIsEmpty() const
	{
		return m_pShadowedGrids->ShadowedColorIsEmpty();
	}

	void CSceneEnvMgr::CreateBloomTarget()
	{
		if( m_pTarget == NULL )
		{
			CRenderDevice *pRender = CRenderDevice::GetInst();
			m_pTarget = CMainWindowTarget::GetInst()->GetTargetPipeline(); 
		}
	}

	void CSceneEnvMgr::SetGaussBrightPass( const float fBrightPass )
	{
		m_fGaussBrightPass = fBrightPass;
		if( m_pTarget )
			m_pTarget->setBrightPass(m_fGaussBrightPass);
	}

	void CSceneEnvMgr::SetGaussMultiplier( const float fGaussMultiplier )
	{
		m_fGaussMultiplier = fGaussMultiplier;
		if( m_pTarget )
			m_pTarget->setGaussMultiplier(fGaussMultiplier);
	}

	void CSceneEnvMgr::SetGaussMean( const float fGaussMean )
	{
		m_fGaussMean = fGaussMean;
		if( m_pTarget )
			m_pTarget->setMean(fGaussMean);
	}

	void CSceneEnvMgr::SetGaussStdDev( const float fGaussStdDev )
	{
		m_fGaussStdDev = fGaussStdDev;
		if( m_pTarget )
			m_pTarget->setStdDev(fGaussStdDev);
	}

	void CSceneEnvMgr::SetGaussExposure( const float fExposure )
	{
		m_fGaussExposure = fExposure;
		if( m_pTarget )
			m_pTarget->setExposure(fExposure);
	}

	void CSceneEnvMgr::SetGaussSample( const float fSam )
	{
		m_fGaussSam = fSam;
		if( m_pTarget )
			m_pTarget->setSample(fSam);
	}

	void CSceneEnvMgr::InitGaussParams()
	{
		if( m_pTarget )
			m_pTarget->InitParams();
	}

	void CSceneEnvMgr::SetIsTurnOnGaussTarget( const bool bTurn )
	{
		m_bTurnOnGassTarget = bTurn;

		if( m_pTarget )
			m_pTarget->SetTurnOn("Bloom", bTurn);
	}

	void CSceneEnvMgr::SetIsTurnOnRefractt(const bool bTurn)
	{
		if( m_pTarget )
			m_pTarget->SetTurnOn("Refract", bTurn);
	}

	void CSceneEnvMgr::SaveGaussParamsInfo( FILE * fp )
	{
		fwrite(&(this->m_fGaussBrightPass),sizeof(float),6,fp);
	}

	void CSceneEnvMgr::LoadGaussParamsInfo( FILE * fp )
	{
		fread(&(this->m_fGaussBrightPass),sizeof(float),6,fp);
	}

	void CSceneEnvMgr::UpdateSceneFog()
	{
		if( m_fogMode == m_fogMode )
		{
			CMainWindowTarget::GetInst()->GetGroup()->SetFog(sqr::FMT_LINEAR, m_envs.m_dwFogColor, m_envs.m_fFogStart, m_envs.m_fFogEnd);
		}
		else
		{
			CMainWindowTarget::GetInst()->GetGroup()->SetFog(sqr::FMT_EXP ,m_envs.m_dwFogColor, m_envs.m_fFogDensity);
		}
	}
}