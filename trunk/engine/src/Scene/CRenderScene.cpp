#include "stdafx.h"
#include "CRenderScene.h"
#include "CMetaSceneClient.h"
#include "CCoreSceneClient.h"
#include "CCoreObjectFollower.h"
#include "CDataScene.h"
#include "CRenderRegion.h"
#include "CMetaStaticData.h"
#include "CGraphic.h"
#include "CRenderSystem.h"
#include "TPart.inl"
#include "CSceneConfig.h"
#include "CCameraConfig.h"
#include "CCamera.h"
#include "IAudioModule.h"
#include "IGUIManager.h"
#include "CAppClient.h"
#include "IProcessCallBack.h"
#include "CSkyMgr.h"
#include "CWaterMgr.h"
#include "CLogicDataMgr.h"
#include "CRenderGroup.h"
#include "CBakedColorData.h"
#include "CRenderModel.h"
#include "CRenderAudio.h"
#include "SyncSystem.h"
#include "CTerrainModelCoder.h"
#include "CCameraPath.h"
#include "CSceneLoadProcess.h"
#include "CClientSceneQuery.h"
#include "SyncSystem.h"
#include "CMath.h"
#include "CRenderRegionEffect.h"
#include "CDebugText.h"
#include "CShadowedGrids.h"
#include "CColor.inl"

#define Layer_One_Shader 0xFFFF07D1
#define Layer_Two_Shader 0xFFFF07D1
#define Layer_Tree_Shader 0xFFFF07D1
#define ZFAR				50000.0f
#define GridInBorderMax	19
#define GridOutBorderMax 25
#define	GridOffset		6
#define REALTIMECALlFOGDISTANCE //是否实时计算雾的起始距离
#define REALTIMEPICK

const float compareValue = eSubRegionSpan * eGridSpan;

CRenderScene::CRenderScene( CDataScene* pDataScene )
		: m_pDataScene( pDataScene )
		, m_pSkyTexture(NULL)
		, m_pSelectedObj(0)
		, m_IsDynamicShadowEnable( true )
		, m_bShowDebugInfo( false )
		, m_uUpdateFrame(0)
		, m_fTimeofDay(12.0f)
		, m_eLoadState(eLoaded_Null)
		, m_bFogEnable( false )
		, m_fPercent(0.0f)
		, m_fCameraFogStart(0.0f)
		, m_fCameraFogEnd(0.0f)
 		, m_stGX(0)
 		, m_stGZ(0)
		, m_pRenderGroup(pDataScene->GetRenderGroup())
		, m_fDefCamDist(22.0f)
{
	SString strState = "CRenderScene开始构造:" + m_sSceneName;
	GraphicErr::SetErrState(strState.c_str());
	m_pAmbientMgr = new CRegionAmbientEffectMgr;
	m_pCamera = NULL;
	m_pActiveCamCtrl = &m_RTSCamerCtrl;

	if(m_pRenderGroup)
	{
		InitSceneParams();
		Initialize();
	}
}

void CRenderScene::InitSceneParams(void)
{
	const CSceneInfo& SceneInf = m_pDataScene->GetSceneInfo(EST_AFTERNOON);
	m_vSunDir = SceneInf.vSunDir;
	m_dwSunColor = SceneInf.dwSunColor;
	m_dwShadowColor = SceneInf.dwShadowColor;
	m_dwAmbientColor = SceneInf.dwAmbientColor;
	m_dwPlayerSunColor = SceneInf.dwPlayerSunColor;
	m_dwPlayerAmbientColor = SceneInf.dwPlayerAmbientColor;
	m_dwFogColor = SceneInf.dwFogColor;
	m_FogStart = SceneInf.fFogStart;
	m_FogEnd = SceneInf.fFogEnd;
	m_fCameraFogStart = m_FogStart - m_RTSCamerCtrl.GetMaxDist();// * cosvalue;
	m_fCameraFogEnd   = m_FogEnd - m_RTSCamerCtrl.GetMaxDist();//   * cosvalue;

	if(!gIsZero(CSceneConfig::GetInst()->GetMinFogDist()))
		m_FogStart	= CSceneConfig::GetInst()->GetMinFogDist() * eGridSpan;
	if(!gIsZero(CSceneConfig::GetInst()->GetMinFogDist()))
		m_FogEnd	= CSceneConfig::GetInst()->GetMaxFogDist() * eGridSpan;
}

void CRenderScene::Initialize(void)
{
	m_sSceneName	= m_pDataScene->GetSceneName();
	m_pTreeScene	= m_pDataScene->GetTreeScene();


	CGraphic* pGraphic = CGraphic::GetInst();
	pGraphic->SetAsynSwitch(false);

	m_vecCameraDest = CVector3f(0.0f, 0.0f, 0.0f);
	m_fCamDist = m_fDefCamDist;
	m_pCamera = m_pRenderGroup->GetCamera();

	m_CamMinSpeed = CCameraConfig::GetInst()->GetCamMinSpeed() * eGridSpan;
	m_pActiveCamCtrl->Bind(m_pCamera);

	m_RTSCamerCtrl.BeginCtrl(0);
	m_RTSCamerCtrl.SetMaxYDegree( CSceneConfig::GetInst()->IsFreeCameraEnabled()?CMath::two_pi:0.0f);
	m_RTSCamerCtrl.SetYRotateDegree(0.0f);

	m_pCamera->setNearDist(10.0f);
	m_pCamera->setFarDist(ZFAR);

	m_pLightSys = m_pRenderGroup->GetLightSystem();


	UpdateCameraOffset();

	m_posCurCenterGrid.x = (int)( m_vecCameraDest.x/eGridSpan );
	m_posCurCenterGrid.y = (int)( m_vecCameraDest.z/eGridSpan );

	int n[12] = { 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3 };
	float fDay = m_fTimeofDay*0.5f;
	int n1 = (int)fDay;
	int n2 = (n1+1)%12;

	//npc light attribute
	m_pLightSys->SetGlobalLightDir( m_vSunDir );
	m_pLightSys->SetAmbient(m_dwPlayerAmbientColor,1);
	m_pLightSys->SetGlobalLightColor(m_dwPlayerSunColor,1);

#ifndef AMBIENT_SHADOW_COLOR
	m_pLightSys->SetGlobalShadowColor(m_dwShadowColor);
#endif

	//scene light attribute
	m_pLightSys->SetAmbient(m_dwAmbientColor,0);
	m_pLightSys->SetGlobalLightColor(m_dwSunColor,0);
	m_pLightSys->TurnOnGlobalLight(0);

	CreateAmbientEffect();
	m_eLoadState |= eLoaded_LightAndModeInfo;
	m_pSceneQuery = new CClientSceneQuery(m_pTreeScene,m_pDataScene);

	SString strState = "CRenderScene构造完成:" + m_sSceneName;
	GraphicErr::SetErrState(strState.c_str());
}

CRenderScene::~CRenderScene(void)
{	
	SString strState = "CRenderScene开始析构:" + m_sSceneName;
	GraphicErr::SetErrState(strState.c_str());

	IEffectManager::GetInst()->ClearEffectDesc();
	CGraphic::GetInst()->SetAsynSwitch(false);
	m_RTSCamerCtrl.EndCtrl();
	//m_TPSCamerCtrl.EndCtrl();
	SafeRelease( m_pSkyTexture );
	GfkSafeDelete(m_pTreeScene);
	GfkSafeDelete(m_pSceneQuery);

	// stop background music
	//SString bgMusic = m_pDataScene->GetBackMusicInfo().szName;
	//if (bgMusic.length() > 1)
	//	StopCue(bgMusic.c_str());
	RenderQueueFlag SceneFlag;
	if(m_pRenderGroup)
	{
		m_pRenderGroup->SetRenderQueueFlag(SceneFlag);
		m_pRenderGroup->SetFogColor(0xFF000000);	
	}
 
	//m_StaticScene.Release();
	if( IGUIManager::HasInst() )
		IGUIManager::GetInst()->GetRootWnd()->SetLimitedState( false );

	size_t size = m_pEffectRenderObjList.size();
	for( size_t i = 0; i < size; ++i )
	{
		SafeRelease(m_pEffectRenderObjList[i]);
	}
	
	SafeDelete(m_pAmbientMgr);
	m_LogicRenderObjMap.clear();

	strState = "CRenderScene析构完成:" + m_sSceneName;
	GraphicErr::SetErrState(strState.c_str());
}

CDataScene*	CRenderScene::GetDataScene()const
{
	return m_pDataScene;
}

void CRenderScene::DoProcess( float fPercent )
{
	if(fPercent>100.0f)
		fPercent = 100.0f;

	if ((m_fPercent==100.0f || fPercent != 100.0f)&&(fPercent - m_fPercent < 2.0f || fPercent<0.0f))
		return;


	IProcessCallBack* pHandler = CRenderSystem::GetInst()->GetProcessCallBack();
	if ( pHandler )
		pHandler->DoProcess( fPercent );

	m_fPercent = fPercent;
}

//void CRenderScene::CreateLight( CMetaRegion* pRegion )
//{
	/*SVector<CRegionLIEx>& LightInfo = pRegion->GetLightInfo();
	size_t size = LightInfo.size();
	for ( size_t i = 0; i < size; ++i )
	{
		uint32 xGrid	= (pRegion->GetRegionId() % m_pDataScene->GetWidthInRegion()) * eRegionSpan + LightInfo[i].uGridIndex%eRegionSpan;
		uint32 yGrid	= (pRegion->GetRegionId() / m_pDataScene->GetWidthInRegion()) * eRegionSpan + LightInfo[i].uGridIndex/eRegionSpan;
		float  xPixel	= GetFloatPixelOfGrid( xGrid );
		float  yPixel	= GetFloatPixelOfGrid( yGrid );
		SList<CRenderLight*>& listLight = GetRegion( xGrid, yGrid )->GetLightList();
		listLight.push_front( new CRenderLight( LightInfo[i].LightInfo, xPixel, LightInfo[i].nHeight, yPixel ) );
	}*/
//}

void CRenderScene::UpdateCameraYOffset()
{
	m_vecCameraDest.y = m_pDataScene->GetOffsetHeightInPixel( m_vecCameraDest.x, m_vecCameraDest.z );
	m_TPSCamerCtrl.SetTargetPos(m_vecCameraDest);
	m_RTSCamerCtrl.SetTargetPos(m_vecCameraDest);
}

void CRenderScene::SetCameraMaxSpeed( float speed )
{
	m_RTSCamerCtrl.SetMaxCameraSpeed(speed * eGridSpan);
}


void CRenderScene::InitSceneRes()
{
	if(m_eLoadState!=eLoaded_Finished)
	{		
		SString strState = "开始载入第一屏资源:" + m_sSceneName;
		GraphicErr::SetErrState(strState.c_str());

		IProcessCallBack* pSynPro = NULL;

		SQR_TRY	
		{
			if( CRenderSystem::GetInst()->GetProcessCallBack() )
				pSynPro = new CSceneLoadProcess(CRenderSystem::GetInst()->GetProcessCallBack());
			CSyncSystem::BeginSyncFun(pSynPro);
		}
		SQR_CATCH(exp)
		{
			exp.AppendMsg("载入第一屏资源出现异常:");
			GfkLogExp(exp);
		}
		SQR_TRY_END;
		GfkSafeDelete(pSynPro);

		m_uCurTime	= (uint32)GetProcessTime();
		m_pActiveCamCtrl->DoQueryCtrl(m_uCurTime,m_pSceneQuery);

		m_pRenderGroup->SetFog( FMT_LINEAR, m_dwFogColor, m_FogStart, m_FogEnd );//SetFogMode(3);
		
		RenderQueueFlag SceneFlag;
		SceneFlag
			<<ID_GEOMETRY_INST_FILTER
			<<ID_SCENE_FILTER
			<<ID_TERRAIN_FILTER
			<<ID_SHOW_ZFAILD_FILTER
			<<ID_NPC_FILTER
			<<ID_SKYBOX_FILTER
			<<ID_ALPHA_NPC_FILTER
			<<ID_NPC_EFFECT_FILTER
			<<ID_WATER_FILTER
			<<ID_ALPHA_SCENE_FILTER
			<<ID_SCENE_EFFECT_FILTER;

		m_pRenderGroup->SetRenderQueueFlag(SceneFlag);

		#ifdef REALTIMECALlFOGDISTANCE
			m_pRenderGroup->SetFogParams(FMT_LINEAR,	1000.0f,	2000.0f);
		#else
			m_pRenderGroup->SetFogParams(FMT_LINEAR,	m_FogStart, m_FogEnd);
		#endif
		m_eLoadState = eLoaded_Finished;
		CalRegionLightParam();

		CGraphic::GetInst()->SetAsynSwitch(true);
		CGraphic::GetInst()->EnableAsynRead(true);

		if(IGUIManager::HasInst())
			IGUIManager::GetInst()->GetRootWnd()->SetLimitedStateWithZoom(true, CSceneConfig::GetInst()->GetGuiZoom());
		
		strState = "载入完成:" + m_sSceneName;
		GraphicErr::SetErrState(strState.c_str());
		CRenderSystem::GetInst()->_SyncTime();
		DoProcess(100.0f);
		//CRenderSystem::GetInst()->Render();

		strState = "游戏正常启动:" + m_sSceneName;
		GraphicErr::SetErrState(strState.c_str());
	}
}

bool CRenderScene::SetCameraDest( float fPixelX,float fPixelZ )
{
	uint32 uGridX = GetGridByPixel(fPixelX);
	uint32 uGridZ = GetGridByPixel(fPixelZ);

	if ( uGridX >= m_pDataScene->GetWidthInGrid() || uGridZ >= m_pDataScene->GetHeightInGrid() )
	{
		ostringstream strm;
		strm<<"Scene: " << m_sSceneName << " SetCameraDest:Invalid position("<<uGridX<<","<<uGridZ<<")" << ", ("<<fPixelX<<","<<fPixelZ<<")";
		GenErr("图形：地图坐标错误",strm.str());
	}
	m_vecCameraDest.x = fPixelX;
	m_vecCameraDest.z = fPixelZ;
	int ux = uGridX - m_stGX;
	int uz = uGridZ - m_stGZ;

	if ( (m_eLoadState & eLoaded_LightAndModeInfo) )
	{
		m_eLoadState			|= eLoaded_TerranStart;
		m_posCurCenterGrid.x	= (int)( m_vecCameraDest.x/eGridSpan );
		m_posCurCenterGrid.y	= (int)( m_vecCameraDest.z/eGridSpan );
	}


	if ( m_stGX!=uGridX || m_stGZ!=uGridZ || m_eLoadState!=eLoaded_Finished )
	{
		m_stGZ = uGridZ;
		m_stGX = uGridX;
		SQR_TRY	
		{
			m_pDataScene->SetActivePos( m_stGX / 64, m_stGZ / 64 );
		}
		SQR_CATCH(exp)
		{
			exp.AppendMsg("动态加载场景管理出现异常:");
			GfkLogExp(exp);
		}
		SQR_TRY_END;
	}
	
	UpdateCameraYOffset();

	///区域光

	CalRegionLightParam();
	m_pLightSys->EnableGlobalLight( TRUE );
	m_pLightSys->UpdateLights(m_pCamera->GetPosition());
	
	return true;
}

void CRenderScene::SetCameraOffset( float fDist )
{
	m_fCamDist = limit2( fDist,m_RTSCamerCtrl.GetMinDist(),m_RTSCamerCtrl.GetMaxDist() );
	m_RTSCamerCtrl.SetDist(m_fCamDist);
	m_TPSCamerCtrl.SetTargetDist( m_fCamDist );
	// limit2(fDist,m_fCamMinDist,m_fCamMaxDist);
}
//#define PRINTTIME

#ifdef PRINTTIME

#define CHECK_START	\
	static bool bPrint=true;\
	bPrint=!bPrint;\
	CTimeCheckPoint tcp;\
	static SVector<uint64>	vecTime;\
	if(!bPrint)\
{\
	vecTime.clear();\
	vecTime.reserve(20);\
	vecTime.push_back(tcp.GetHDElapse());\
}




#define CHECK_POINT	if(!bPrint)vecTime.push_back(tcp.GetHDElapse());

#define CHECK_END if(bPrint){for(size_t i=1;i<vecTime.size();++i){cout<<vecTime[i]-vecTime[i-1]<<"\t";}cout<<endl;}

#else

#define CHECK_START
#define CHECK_POINT
#define CHECK_END

#endif

void CRenderScene::Render()
{
	//if ( m_eLoadState != eLoaded_Finished )
	//	return;

	CHECK_START;

	PreDraw();

#ifdef UseRuntimeDebug
#ifdef UseVedioRecord
	m_pRenderSystem->m_RenderObjAlpha += m_pRenderSystem->m_fIncRenderObj;
	m_pRenderSystem->m_RenderObjAlpha = limit2(m_pRenderSystem->m_RenderObjAlpha,int16(0),int16(255));

	m_pRenderSystem->m_MainPlayerAlpha += m_pRenderSystem->m_fIncPlayer;
	m_pRenderSystem->m_MainPlayerAlpha = limit2(m_pRenderSystem->m_MainPlayerAlpha,int16(0),int16(255));

	m_pRenderSystem->m_GUIAlpha += m_pRenderSystem->m_fIncGUI;
	m_pRenderSystem->m_GUIAlpha = limit2(m_pRenderSystem->m_GUIAlpha,int16(0),int16(255));
	//if(sqr::SQRGUIManager::GetInst())
	//	sqr::SQRGUIManager::GetInst()->GetRootWnd()->SetTransparent( float(m_pRenderSystem->m_GUIAlpha)/255.0f );
#endif
#endif
	CHECK_POINT;

	MoveSKyBox();	

	CHECK_POINT;

	DrawObject();

	CHECK_POINT;

	DrawSelect();

	CHECK_POINT;

	DrawAmbientEffect();
	m_pAmbientMgr->Render(m_uCurTime, m_vecCameraDest);
	
	CHECK_POINT;

	DrawWater();

	CHECK_POINT;
	
	IEffectManager::GetInst()->Update( GetCurTime() );

	CHECK_POINT;

	SceneDebugRun();

	CHECK_POINT;

	CHECK_END;
}

void CRenderScene::Update()
{
	IEffectManager::GetInst()->ClearEffectDesc();
	m_uPreTime	= m_uCurTime;
	m_uCurTime	= (uint32)GetProcessTime();
	m_RenderQueue.UpdateTime(m_uCurTime);
}

void CRenderScene::PlayCameraPath(uint32 Index)
{
	m_pActiveCamCtrl = m_pDataScene->GetCameraPath(Index);
	SetCameraController(m_pActiveCamCtrl);
}

void CRenderScene::SetCameraController(CCameraController* pCtrl)
{
	if(pCtrl==NULL)
		m_pActiveCamCtrl = &m_RTSCamerCtrl;
	else
		m_pActiveCamCtrl = pCtrl;

	if(m_pCamera)
	{
		m_pActiveCamCtrl->Bind(m_pCamera);
	}
	m_pActiveCamCtrl->BeginCtrl((uint32)GetProcessTime());
}


void CRenderScene::PreDraw()
{
	m_uUpdateFrame			= m_uUpdateFrame + 1;
	m_uPreTime				= m_uCurTime;
	m_uCurTime				= (uint32)GetProcessTime();
	if( CS_END == m_pActiveCamCtrl->DoQueryCtrl(m_uCurTime,m_pSceneQuery) )
	{
		m_pActiveCamCtrl = &m_RTSCamerCtrl;
		m_pActiveCamCtrl->Bind(m_pCamera);
		m_pActiveCamCtrl->BeginCtrl(m_uCurTime);
	}

#ifdef REALTIMECALlFOGDISTANCE
	float sinvalue = abs(m_pCamera->getDIRECTION().y);
	float cosvalue = sqrt(1 - sinvalue * sinvalue);

	float fogStart = m_RTSCamerCtrl.GetDist() + m_fCameraFogStart;// * cosvalue;
	float fogEnd = m_RTSCamerCtrl.GetDist() + m_fCameraFogEnd;// * cosvalue;
	m_pRenderGroup->SetFogParams(m_pRenderGroup->GetFogType(),fogStart, fogEnd);
#endif

	// update 3d audio listener
	if(IAudioListener::HasInst())
		IAudioListener::GetInst()->SetTransform(m_pCamera->getDIRECTION(),
			 m_pCamera->getUP(), m_vecCameraDest );
}
	
void CRenderScene::CalRegionLightParam()
{
	if( m_eLoadState!=eLoaded_Finished )
		return; 

	static uint16    lastSubRgnIndex = -1;

	int x = m_posCurCenterGrid.x / eSubRegionSpan;
	int y = m_posCurCenterGrid.y / eSubRegionSpan;

	uint ErrFlag = 0;
	SQR_TRY
	{
		int	nHotSubRgnIndex =  x + y * m_pDataScene->GetWidthInRegion() * eRegDivideNum;
		SubRegionData *pSubRgnData = m_pDataScene->GetSubRegion(nHotSubRgnIndex);

		if(pSubRgnData && pSubRgnData->m_bNeedRealCal )
		{
			ErrFlag = 1;
			SVector<int> regionlist = pSubRgnData->m_vSubRgnAjenctList;
			size_t size = regionlist.size();

			uint16 curSubRgnIndex =pSubRgnData->m_nSetRegionIndex;
			RegionsInfo	*pRgnInfo = m_pDataScene->GetRegionInfo(curSubRgnIndex);
			SString bgMusic = pRgnInfo->strMusicName;

			if( lastSubRgnIndex != curSubRgnIndex )
			{
				m_pAmbientMgr->CreateRegionAmbientEffect(pRgnInfo->rgnAmbientFxList);
				lastSubRgnIndex = curSubRgnIndex;
			}
	
			RegionLightParam lightParam;
			RegionLightParam maxLightParam;
			bool isChanged = false;
			ErrFlag = 2;
			for ( size_t i = 0; i < size; ++i )
			{
				SubRegionData *pSubRgnData = m_pDataScene->GetSubRegion(regionlist[i]);
				if( pSubRgnData == NULL )
					continue;
				RegionsInfo	  *pRgnInfo	   = m_pDataScene->GetRegionInfo(pSubRgnData->m_nSetRegionIndex);
				if( pRgnInfo == NULL )
					continue;

				const CVector3f& center	= pSubRgnData->m_vCenter;

				float x = center.x - m_vecCameraDest.x;
				float z = center.z - m_vecCameraDest.z;
				float dis = sqrt(x * x + z * z);

				float ratio = 1 - dis / compareValue;
				if ( ratio < 0.0f )
					continue;

				lightParam += pRgnInfo->lightParam * ratio;
				maxLightParam = RegionLightParam::Max(pRgnInfo->lightParam, maxLightParam);
				isChanged = true;
			}

			if (isChanged)
			{
				lightParam = RegionLightParam::Min(lightParam, maxLightParam);

				m_fCameraFogStart = lightParam.fogStart - m_RTSCamerCtrl.GetMaxDist();// * cosvalue;
				m_fCameraFogEnd   = lightParam.fogEnd   - m_RTSCamerCtrl.GetMaxDist();// * cosvalue;

				m_pLightSys->SetAmbient(lightParam.playerAmbient,1);
				m_pLightSys->SetGlobalLightColor(lightParam.playerDiffuse,1);
				m_pLightSys->SetAmbient(lightParam.globalAmbient,0);
				m_pLightSys->SetGlobalLightColor(lightParam.globalDiffuse,0);

#ifndef AMBIENT_SHADOW_COLOR
				m_pLightSys->SetGlobalShadowColor(lightParam.globalShadow);
#endif

				m_pRenderGroup->SetFogColor(lightParam.fogColor);
			}
		}
		else
		{
			m_pLightSys->SetAmbient(m_dwPlayerAmbientColor,1);
			m_pLightSys->SetGlobalLightColor(m_dwPlayerSunColor,1);
			m_pLightSys->SetAmbient(m_dwAmbientColor,0);
			m_pLightSys->SetGlobalLightColor(m_dwSunColor,0);

#ifndef AMBIENT_SHADOW_COLOR
			m_pLightSys->SetGlobalShadowColor(m_dwShadowColor);
#endif

			m_pRenderGroup->SetFogColor( m_dwFogColor );

			m_fCameraFogStart = m_FogStart - m_RTSCamerCtrl.GetMaxDist();// * cosvalue;
			m_fCameraFogEnd   = m_FogEnd - m_RTSCamerCtrl.GetMaxDist();//   * cosvalue;

			return;
		}
	}
	SQR_CATCH(exp)
	{
		switch(ErrFlag)
		{
		case 0:
			exp.AppendMsg("Begin nHotSubRgnIndex");
			break;
		case 1:
			exp.AppendMsg("Over nHotSubRgnIndex");
			break;
		case 2:
			exp.AppendMsg("regionlist[i]");
			break;
		default:
			break;
		}
		ostringstream strm;
		strm << "Scene:" << m_sSceneName 
			<< "(" <<m_posCurCenterGrid.x << "," <<m_posCurCenterGrid.y << ")";
		exp.AppendMsg(strm.str());
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;	
}

void CRenderScene::MoveSKyBox()
{
	SQR_TRY	
	{
		m_pDataScene->GetSkyMgr()->SetCameraDest(m_vecCameraDest);
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
	}
	SQR_TRY_END;
}

CRenderSceneQueue& CRenderScene::GetRenderSceneQuene()
{
	return m_RenderQueue;
}

void CRenderScene::DrawObject()
{
	uint32 CurTime = GetCurTime();

//#ifdef UseVedioRecord
	//for (it=m_RenderQueue.begin();it!=eit;++it)
	//{
	//	(*it)->OnPreRender( CurTime );

	//	if ((*it)==m_pRenderSystem->m_pMainPlayer)
	//		(*it)->SetAlphaValue(uint8(m_pRenderSystem->m_MainPlayerAlpha));
	//	else
	//		(*it)->SetAlphaValue(uint8(m_pRenderSystem->m_RenderObjAlpha));
	//	(*it)->Render( CurTime );
	//}
//#else
	CGeometryParams Params;
	Params.m_CurTime = CurTime;
	m_pRenderGroup->SetGeomeotryInstParams(Params);

	if(CRenderSystem::GetInst()->m_bShowObj)
	{
		m_RenderQueue.RenderEx(m_pCamera,CurTime);
	}
	else
	{
		m_RenderQueue.UnRenderEx(CurTime);
	}
//#endif
	//m_StaticScene.Render(m_pCamera,CurTime);

	m_pTreeScene->Update(m_pCamera);
	m_pTreeScene->Render(CurTime);
	return;
}

void CRenderScene::DrawSelect()
{
	m_pSelectedObj=NULL;
#ifdef REALTIMEPICK
	{
		if( IGUIManager::HasInst() )
		{
			SQRWnd*	pWnd		 = IGUIManager::GetInst()->GetRootWnd();
			CFPos	posCursor	 = pWnd->GetCursorPos();

			CFRect  rect;
			pWnd->GetWndRect(rect);

			float	width		 = rect.right - rect.left;
			float	height		 = rect.bottom - rect.top;
			float	x			 = posCursor.x/width;
			float	y			 = posCursor.y/height;
			SelectPosByCursor(x, y);
		}
	}
#endif

	DrawSelectObj(GetSelectedObj());

#ifdef DRAW_SELECTED_OBJECT
	if (m_pSelectedTerrainObj!=NULL)
	{
		CRenderable* pRenderObj = m_pSelectedTerrainObj->m_pRenderObj;
		DELTASH DSH1 = { (m_pSelectedTerrainObj->m_uAlpha<<23&0xff000000) | 0x001E78FF, CMatrix() };
		RenderObjStyle RS1 = { ROS_SHADOW, false, true, DELTASH_ZTEST, (DWORD)&DSH1 };
		pRenderObj->Render( CMatrix(),GetCurTime(), true,&RS1 );
	}
#endif
}

void CRenderScene::DrawSelectObj(CRenderObject* pRenderObj)
{
	if ( pRenderObj!=NULL )
	{
		//		//0x66F8DD89
		//DELTASH DSH1 = { (pRenderObj->m_uAlpha<<23&0xff000000) | 0x00FFFFFF,CMatrix() };
		sMaterial Mat =
		{
			CColor4((uint32)CColor( 0xff000000) ),
			CColor4((uint32)CColor( 0xff, 0xB0, 0xB0, 0xB0 )),
			CColor4((uint32)0x00),
			CColor4((uint32)0x00),
			0.0f
		};

		DWORD oldStyle = pRenderObj->GetRenderStyle();
		DWORD newStyle = ROS_SHADOW_LIGHT2;

		if ( (oldStyle & ROS_ZFAILD) != 0 )
			newStyle |= ROS_ZFAILD;

		RenderObjStyle RS1 = { (ERenderObjStyle)(newStyle), true, true, DELTARS_DIFFUSE|DELTARS_AMBIENT, (DWORD)&Mat };
		RS1.m_bRenderLink = false;
		RS1.m_bRenderMesh = true;
		pRenderObj->RenderSel( pRenderObj->GetRenderTime()?pRenderObj->GetRenderTime()->GetCurTime()
			:GetCurTime(), &RS1 );
	}
}

void CRenderScene::DrawWater()
{
	//CMetaSceneClient* pMetaScene = GetDataScene();
	//CGraphic* pGraphic = CGraphic::GetInst();
	//CCamera* camera = m_pRenderTarget->GetCamera();
	//pGraphic->GetCamera(&camera);
	m_pDataScene->GetWaterMgr()->RenderWave(GetCurTime());
	//for (uint32 i = 0; i < m_pDataScene->m_vecWaterRegionIndex.size(); i++)
	//{
	//	CMetaRegion* pRegion = m_pDataScene->GetRegion(m_pDataScene->m_vecWaterRegionIndex[i]);
	//	RegionWater* pRegionWater = pRegion->GetWaterInfo();
	//	// is visible?
	//	if (m_pCamera->IsVisible(pRegionWater->aabb))
	//	{
	//		for ( uint32 j = 0; j < pRegionWater->m_Waters.size(); j++ )
	//		{
	//			CWater& water = pRegionWater->m_Waters[j];
	//			// water
	//			uint GridN = water.GetWaterGridNum();
	//			if ( 0 == GridN )
	//				continue;
	//			// wave
	//			water.RenderWave(pGraphic, texture, GetCurTime());
	//			// dimple
	//			water.RenderDimple();
	//		}
	//	}
	//}
}

void CRenderScene::CreateAmbientEffect()
{
	const CAmbientFxInfo& ambientEffectxInfo = GetDataScene()->GetAmbientFxInfo();
	if( ambientEffectxInfo.Fx.empty() )
		return;

	size_t size = ambientEffectxInfo.Fx.size();
	SString effectname = "", effectfilename = "";

	CPkgFile::EnableAsyncRead( false );

	for ( size_t i = 0; i < size; ++i )
	{
		IEffectGroup* pGroup = NULL;
		effectfilename = ambientEffectxInfo.Fx[i].szFxFileName;
		if (FAILED(IEffectManager::GetInst()->CreateEffectGroup( effectfilename.c_str(), &pGroup )))
		{
			ostringstream strm;
			strm<<"CreateEffectGroup failed with file \""<<effectfilename<<"\"!";
			ArtErr(strm.str());
		}

		effectname = ambientEffectxInfo.Fx[i].szFxName;

		IEffect* pEffect			= NULL;
		if( SUCCEEDED( IEffectManager::GetInst()->CreateEffect( pGroup, effectname.c_str(), &pEffect ) ) )
			m_pEffectRenderObjList.push_back(pEffect);
		else
		{
			ostringstream strm;
			strm<<"CreateEffect failed with file \""<<effectname<<"\"!";
			ArtErr(strm.str());
			SafeRelease( pGroup );
		}
		SafeRelease( pGroup );
	}

	CPkgFile::EnableAsyncRead( true );
}

void CRenderScene::DrawAmbientEffect()
{
	if( m_pEffectRenderObjList.empty() )
		return;

	CMatrix matWorld;
	CVector3f pos = m_vecCameraDest;
	pos.y = 0.0f;
	matWorld.SetTranslate(pos);

	CColor matColor = CColor( 0xff, 0x80, 0x80, 0x80 );

	sMaterial Mat = 
	{
		CColor4((uint32)matColor ), 
		CColor4((uint32)matColor ), 
		CColor4((uint32)0),
		CColor4((uint32)0xffffffff),
		0
	};

	RenderObjStyle objStyle = 
	{
		ROS_LIGHTMAT1,
		true,
		true,
		DELTARS_DIFFUSE|DELTARS_AMBIENT,
		(uint32)&Mat
	};

	size_t size = m_pEffectRenderObjList.size();
	for ( size_t i = 0; i < size; ++i )
	{
		m_pEffectRenderObjList[i]->UpdateTime(m_uCurTime);
		m_pEffectRenderObjList[i]->Render( matWorld, &objStyle );
	}
}

class CGridFinder : public ILineAction
{
public:
	CRenderScene*	m_pScene;
	CVector3f		m_vecOrig;
	float			m_fK;
	CPos			m_posPre;
	CVector3f		m_vecPrePixel;
	float			m_fHeightOfLine;

	CGridFinder( CRenderScene* pScene, CVector3f vecOrig, CVector3f vecDest )
			: m_pScene(pScene)
			, m_vecOrig(vecOrig)
			, m_posPre( -1, -1 )
	{
		CVector3f vecDelta = vecDest - vecOrig;
		m_fK = vecDelta.y/sqrtf( vecDelta.x*vecDelta.x + vecDelta.z*vecDelta.z );	//仰角的tan值
	}

	bool Do( int32 x, int32 y )
	{
		CDataScene* pDataScene = m_pScene->GetDataScene();
		CPos posGrid = CPos( x/eGridSpan, y/eGridSpan );
		if ( posGrid == m_posPre || !pDataScene->IsGridValid( posGrid.x, posGrid.y ) )
			return true;

		//先看当前地表是否高于视线，高于则前一个格子被选中
		CVector3f vecPixel = CVector3f( (float)x, (float)pDataScene->GetOffsetHeightInPixel( (float)x, (float)y ), (float)y );
		float fHeightOfLine = m_vecOrig.y + CVector2f( x - m_vecOrig.x, y - m_vecOrig.z ).Mag()*m_fK;
		if ( fHeightOfLine < vecPixel.y && m_fHeightOfLine >= m_vecPrePixel.y )
		{
			CVector2f vecDel = CVector2f( x - m_vecPrePixel.x, y - m_vecPrePixel.z );
			float fDist = vecDel.Mag();
			if ( fDist > 3 )
			{
				vecDel = vecDel/fDist;
				for ( float f = 3.0f; f <= fDist; f++ )
				{
					float x = m_vecPrePixel.x + vecDel.x*f;
					float z = m_vecPrePixel.z + vecDel.y*f;
					float y = pDataScene->GetOffsetHeightInPixel( x, z );
					if ( y > m_vecOrig.y + CVector2f( x - m_vecOrig.x, z - m_vecOrig.z ).Mag()*m_fK )
					{
						m_vecPrePixel = CVector3f( x, y, z );
						break;
					}
				}
			}
			return false;
		}

		m_vecPrePixel	= vecPixel;
		m_fHeightOfLine = fHeightOfLine;
		m_posPre		= posGrid;
		return true;
	}
};

bool CRenderScene::SelectRenderObjDir(float xCursorPercent, float yCursorPercent, CVector3f& vecOrig, CVector3f& vDir)
{
	//CMetaSceneClient* pMetaScene = GetDataScene()
	float fHeight		= CMath::TanTable( m_pCamera->getFOVy()/2.0f );					//视口高度与1的比值
	float fWidth		= fHeight*m_pCamera->getAspectRatio();							//视口宽度与1的比值
	float SrcnX			= ( (xCursorPercent)*2.0f - 1 )*fWidth ;
	float SrcnY			= -((yCursorPercent)*2.0f - 1 )*fHeight ;
	CVector3f vecZ		= m_pCamera->getDIRECTION();												//摄像机观察方向Z方向（前方）
	CVector3f tCamera   = CVector3f( 0.0f, 1.0f, 0.0f ).Cross( vecZ );
	if(CVector3f::ZERO == tCamera)
		return false;
	CVector3f vecX		= tCamera.Normalize();		//摄像机观察方向X方向（左方）
	CVector3f vecY		= vecZ.Cross( vecX );
	if (CVector3f::ZERO == vecY)
		return false;
	vecY.Normalize();								//摄像机观察方向Y方向（上方）
	vDir				= vecZ + SrcnX*vecX + SrcnY*vecY;								//鼠标向量
	vecOrig				= m_pCamera->GetPosition();												//摄像机位置
	CVector3f vecDest	= vecOrig + vDir*ZFAR;											//摄像机位置+鼠标向量*ZFAR

	CPos posStart		= CPos( (int32)vecOrig.x, (int32)vecOrig.z );
	CPos posEnd			= CPos( (int32)vecDest.x, (int32)vecDest.z );
	//CIRect rtView;						//VisibleArea的最小包络矩形
	//rtView.left			= ( m_posCurCenterGrid.x + m_pVisibleArea->xMin() )*eGridSpan;
	//rtView.right		= ( m_posCurCenterGrid.x + m_pVisibleArea->xMax() )*eGridSpan;
	//rtView.top			= ( m_posCurCenterGrid.y + m_pVisibleArea->zMin() )*eGridSpan;
	//rtView.bottom		= ( m_posCurCenterGrid.y + m_pVisibleArea->zMax() )*eGridSpan;
	CIRect rtScale		= CIRect( 1, 1, m_pDataScene->GetWidthInGrid(), m_pDataScene->GetHeightInGrid() )*eGridSpan;	//场景矩形

	if ( /*!Cut2DLine( rtView.Intersection(rtScale), posStart, posEnd ) ||*/ posStart == posEnd )
	{
		m_posSelectedGrid.x = m_posSelectedGrid.y = -1;
		m_vecSelectedLogic.x = m_vecSelectedLogic.y = m_vecSelectedLogic.z = -1;
		return false;
	}

	CGridFinder GridFinder = CGridFinder( this, vecOrig, vecDest );

	Line( posStart.x, posStart.y, posEnd.x, posEnd.y, GridFinder );

	m_vecSelectedLogic	= GridFinder.m_vecPrePixel;
	m_posSelectedGrid	= GridFinder.m_posPre;
	return true;
}

//确定当前选中的格子和物体
bool CRenderScene::IsSelectRenderObj( float xCursorPercent, float yCursorPercent, CRenderObject* renderObj )
{
	if ( m_eLoadState != eLoaded_Finished )
		return false;

	CVector3f vDir, vecOrig;
	if( !SelectRenderObjDir(xCursorPercent, yCursorPercent, vecOrig, vDir) )
		return false;

	//下面开始查找鼠标射线穿过哪些对象并决定最终选择哪个。

	return  renderObj->IntersectRay( vecOrig, vDir ) == IF_EXCELLENT;
}

//确定当前选中的格子和物体
void CRenderScene::SelectPosByCursor( float xCursorPercent, float yCursorPercent )
{
	if ( m_eLoadState != eLoaded_Finished )
		return;

	if(CRenderSystem::GetInst()&&!CRenderSystem::GetInst()->IsSelectByCursor())
		return;

	CVector3f vDir, vecOrig;
	if( !SelectRenderObjDir(xCursorPercent, yCursorPercent, vecOrig, vDir) )
		return ;

	m_RenderQueue.SelectRendObj(vecOrig,vDir,m_pCamera->getDIRECTION());
}

//将对象移动到某处
void CRenderScene::AttachRenderObject( CRenderObject& RenderObject, const CVector3f& Pos )
{
	SetRenderObjectPosition( RenderObject, Pos, true );
}

void CRenderScene::AttachRenderObjectAbs( CRenderObject& RenderObject, const CVector3f& Pos )
{
	SetRenderObjectPosition( RenderObject, Pos, false );
}

void CRenderScene::SetRenderObjectPosition( CRenderObject& renderObject, const CVector3f& Pos, bool bTerrain )
{
	//CMetaSceneClient* pMetaScene	= GetDataScene();

	//CMetaGridMgr* pGridMgr = m_pDataScene->GetGridMgr();
	//Ast( pGridMgr );
	int32 xCurGrid	= GetGridByFloatPixel( Pos.x );
	int32 yCurGrid	= GetGridByFloatPixel( Pos.z );

	if ( !m_pDataScene->IsGridValid( xCurGrid, yCurGrid ) )
		return;
	// original position
	CFPos posPre		= renderObject.Get2DPosition();
	int32 xPreGrid	= GetGridByFloatPixel( static_cast<float>(posPre.x) );
	int32 yPreGrid	= GetGridByFloatPixel( static_cast<float>(posPre.y) );
	// new height & normal

	//bool bHasLogicHeight = false;
	float nTerranHeight = m_pDataScene->GetOffsetHeightInPixel( Pos.x, Pos.z );
	CVector3f normal = m_pDataScene->GetLogicDataMgr()->GetLogicNormal(xPreGrid,yPreGrid);

	//if( bHasLogicHeight )
	//	normal = m_pDataScene->GetTerranLogicNomal( xPreGrid, yPreGrid );
	//else
	//	normal = CMetaStaticData::GetNormal( m_pDataScene->GetTerranNomal( xPreGrid, yPreGrid ) );

	if ( bTerrain )
	{
		renderObject.SetPosition( CVector3f( Pos.x, Pos.y + nTerranHeight, Pos.z ) );
		renderObject.SetNormal( normal );
	}
	else
	{
		renderObject.SetPosition( CVector3f( Pos.x, Pos.y, Pos.z ) );
	}

	if ( xCurGrid != xPreGrid || yCurGrid != yPreGrid )
	{
		CColor shadow = m_pDataScene->GetShadowedGrids()->GetShadowedColor(xCurGrid, yCurGrid);
		CColor diffuse = shadow.Modulate(0xff808080/*m_pDataScene->GetTerranDiffuseColor(xCurGrid, yCurGrid)*/);
		int8   nMaterialIndex = m_pDataScene->GetLogicDataMgr()->GetMaterialIndex(xCurGrid, yCurGrid);
		renderObject.SetDiffuse(diffuse);
		renderObject.SetAmbient(diffuse);
		renderObject.SetTerrainMaterialIndex(nMaterialIndex);
	}

	if ( m_pDataScene->IsGridValid( xCurGrid, yCurGrid ) )
	{
		//int32 Grid = (yCurGrid%eRegionSpan)*m_pDataScene->GetWidthInRegion() + xCurGrid%eRegionSpan;
		const uint GridNum = 16;// 这货要搞掉
		uint32 orgId = (yCurGrid /16)*m_pDataScene->GetWidthInRegion() + (xCurGrid / 16);
		int32 nGrid = ( yCurGrid % 16 ) * 16 + (xCurGrid % 16);

		if( m_pDataScene->GetWaterMgr()->GetWater(orgId, nGrid)!=NULL )
			return renderObject.MakeWaterFx( this );
		renderObject.MakeWaterFx( NULL );
	}
}

void CRenderScene::DetachRenderObject( CRenderObject& RenderObject )
{
	CFPos Pos=RenderObject.Get2DPosition();
	int32 xCurGrid	= GetGridByFloatPixel( static_cast<float>(Pos.x) );
	int32 yCurGrid	= GetGridByFloatPixel( static_cast<float>(Pos.y) );

	Ast ( m_pDataScene->IsGridValid( xCurGrid, yCurGrid ) );
}

//产生动态水波
void CRenderScene::DoDimple( CRenderObject& RenderObject, const CVector2f& vDelta, bool bMove )
{
	CVector3f vecPos = RenderObject.GetPosition();

	int32 xGrid = GetGridByFloatPixel( static_cast<float>(vecPos.x) );
	int32 yGrid = GetGridByFloatPixel( static_cast<float>(vecPos.z) );
	//CMetaRegion* pRegion = m_pDataScene->GetRegion( xGrid, yGrid );

	const uint GridNum = 16;// 这货要搞掉
	uint32 orgId = (yGrid /16)*m_pDataScene->GetWidthInRegion() + (xGrid / 16);
	int32 nGrid = ( yGrid % 16 ) * 16 + (xGrid % 16);

	CWater* pWater = NULL;
	if ( !m_pDataScene->IsGridValid(xGrid,yGrid) ||
		 !(pWater = m_pDataScene->GetWaterMgr()->GetWater( orgId, nGrid )))
		return;

	float fCenHeight = 2 + m_pDataScene->GetLogicDataMgr()->GetWaterHeight(xGrid,yGrid);

	//for ( int32 x = xGrid; x < xGrid + 2; x++ )
	//	for ( int32 y = yGrid; y < yGrid + 2; y++ )
	//		fCenHeight += m_pDataScene->GetWaterMgr()->GetWaterInfo()->m_nWaterHeight[y*( eRegionSpan + 1 ) + x]/4.0f;
	
	//for ( int v = 0; v < 4; v ++ )
	if ( RenderObject.GetPosition().y > fCenHeight )
			return;

	const CWaterWaveInfo& WaveInf = GetDataScene()->GetWaterMgr()->GetWaterWaveInfo();
	const SString& szFxFile = bMove ? WaveInf.szMoveFxFile : WaveInf.szStandFxFile;
	const SString& szFxName = bMove ? WaveInf.szMoveFxName : WaveInf.szStandFxName;
	IEffectGroup* pGroup;
	IEffect* pDimpleFx;
	if ( FAILED( IEffectManager::GetInst()->CreateEffectGroup( szFxFile.c_str(), &pGroup ) ) )
		return;
	if ( FAILED( IEffectManager::GetInst()->CreateEffect( pGroup, szFxName.c_str(), &pDimpleFx ) ) )
	{
		SafeRelease( pGroup );
		return;
	}
	SafeRelease( pGroup );

	CMatrix matObject;
	matObject.SetTranslate( vecPos );
	CVector2f vDir  = RenderObject.GetDirection();
	matObject._11 =  vDir.y;
	matObject._13 = -vDir.x;
	matObject._31 = -matObject._13;
	matObject._33 =  matObject._11;

	//CMatrix matWorld;
	//float fSize = ( Scale.x + Scale.z )*0.5f/eGridSpan;
	//matWorld.SetScale( fSize, fSize, fSize );
	//matWorld = matWorld*matObject;
	matObject._41 += vDelta.x;
	matObject._42  = fCenHeight;
	matObject._43 += vDelta.y;

	new CDimple( pWater, pDimpleFx, matObject );
	SafeRelease( pDimpleFx );
}

CRenderObject* CRenderScene::GetSelectedObj()const
{
	return m_pSelectedObj;
}

void CRenderScene::SetSelectedObj(CRenderObject* pRenderObj)
{
	m_pSelectedObj = pRenderObj;
}

const CVector3f& CRenderScene::GetSelectedPos()const
{
	return m_vecSelectedLogic;
}

void CRenderScene::GetSelectedPos(CVector3f& vecPos)
{
	vecPos = GetSelectedPos();
}

CVector3f CRenderScene::GetCameraDest()const
{
	return m_pActiveCamCtrl?m_pActiveCamCtrl->GetTargetPos():m_vecCameraDest;
}

void CRenderScene::GetCameraDest(CVector3f& vecDest)
{
	vecDest = GetCameraDest();
}

//#pragma optimize( "", off )
float CRenderScene::GetCameraOffset()
{
	//__asm finit;
	return m_fCamDist;//m_vecCameraOffset.Mag();
}
//#pragma optimize( "", on )

//inline CGridObjInfo& CRenderScene::GetGridInfo( const DeltaPos& Pos )
//{
//	int32 xGrid = Pos.m_nX + m_posCurCenterGrid.x;
//	int32 yGrid = Pos.m_nZ + m_posCurCenterGrid.y;
//	return GetRegion( xGrid, yGrid )->GetOtherGridInfo( xGrid, yGrid );
//}

//inline CGridObjInfo& CRenderScene::GetGridInfo( int32 xGrid,int32 yGrid)
//{
//	return GetRegion( xGrid, yGrid )->GetOtherGridInfo( xGrid, yGrid );
//}

CRenderObject*	CRenderScene::CreateRenderObject(IRenderTime* pRenderTime)
{
	CRenderObject* pRenderObj = new CRenderObject(pRenderTime,this);
	if (NULL!=pRenderObj)
		m_RenderQueue.AddChild(pRenderObj,LT_UNKNOW,"");
	return pRenderObj;
}

void	CRenderScene::DestroyRenderObject(CRenderObject* pObj)
{
	m_RenderQueue.DestroyRenderChild(pObj);
}
//}

CVector3f  CRenderScene::GetPickCameraPos()
{
	return m_pCamera->GetPosition();
}

CVector3f  CRenderScene::GetPickCameraOffset()
{
	return m_pCamera->getDIRECTION();
}

//uint32 CRenderScene::GetGridIndex( uint32 z, uint32 x )
//{
//	uint32 w = m_vecRegion.GetWidth();
//	return z * ( w * 16 ) + x;
//}

void CRenderSceneQueue::_DestoryRenderChild(CRenderSpaceNode* pObj)
{
	if(pObj->GetNodeType() == CRenderObject::st_RenderObjectType)
	{
		CRenderObject* pRenderObj=class_cast<CRenderObject*>(pObj);
		if(pRenderObj) 
		{
			pRenderObj->SetModelHandler(NULL);
			pRenderObj->SetPosition(pRenderObj->GetWorldMatrix().GetLoc());
			AddChild(pRenderObj,LT_CENTER,"");
			m_LapseQueue.InsertNode(new CLapseDeleteNode(pObj,pRenderObj->GetLapseTime()));
		}
	}
	else
	{
		delete pObj;
	}
}

CRenderSceneQueue::CRenderSceneQueue()
{
	m_fpDestoryFn = &CRenderSceneQueue::_DestoryRenderChild;
}

CRenderSceneQueue::~CRenderSceneQueue()
{

}

void	CRenderSceneQueue::DestroyRenderChild(CRenderSpaceNode* pObj)
{
	(this->*m_fpDestoryFn)(pObj);
}

void	CRenderSceneQueue::_DestoryRenderChildtInRender(CRenderSpaceNode* pObj)
{
	SpaceNodeList::iterator it =  m_RenderDelete.find(pObj);
	if (it==m_RenderDelete.end())
	{
		m_RenderDelete.insert(pObj);
	}
}

void	CRenderSceneQueue::RenderEx( CCamera* pCamera, uint32 uCurTime )
{
	LockRenderQueue();
	CRenderSpace::RenderEx(pCamera,uCurTime);
	UnLockRenderQueue();
	m_LapseQueue.Update(uCurTime);
}

void	CRenderSceneQueue::UnRenderEx(uint32 uCurTime)
{
	LockRenderQueue();
	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
	{
		if((*it)->GetNodeType() == CRenderObject::st_RenderObjectType)
		{
			CRenderObject* pRenderObj=class_cast<CRenderObject*>(*it);
			pRenderObj->SetVisible(false);
		}
	}

	UpdateTime(uCurTime);
	UnLockRenderQueue();
	m_LapseQueue.Update(uCurTime);
}

void	CRenderSceneQueue::UpdateTime(uint32 uCurTime )
{
	LockRenderQueue();
	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
	{
		(*it)->UpdateTime( uCurTime );
	}
	UnLockRenderQueue();
	m_LapseQueue.Update(uCurTime);
}

void	CRenderSceneQueue::LockRenderQueue(void)
{
	m_RenderDelete.clear();
	m_fpDestoryFn = &CRenderSceneQueue::_DestoryRenderChildtInRender;
}

void	CRenderSceneQueue::UnLockRenderQueue(void)
{
	m_fpDestoryFn = &CRenderSceneQueue::_DestoryRenderChild;
	SpaceNodeList::iterator it = m_RenderDelete.begin();
	SpaceNodeList::iterator ite = m_RenderDelete.end();
	for (;it!=ite;++it)
		_DestoryRenderChild(*it);

}

void CRenderScene::RegisterLogicModel( const SString& strLogicName, CRenderObject *pLoginObj )
{
	m_LogicRenderObjMap[strLogicName] = pLoginObj;
}

void CRenderScene::UnRegisterLogicModel( const SString& strLogicName, CRenderObject *pLoginObj )
{
	LocalModelMap::iterator it = m_LogicRenderObjMap.find(strLogicName);
	if(it!=m_LogicRenderObjMap.end())
	{
		if( it->second == pLoginObj )
			m_LogicRenderObjMap.erase(it);
	}
}

bool CRenderScene::SetLogicModelHandler( const char* szLogicName, IModelHandler* pHandler )
{
	SString strLogicName = szLogicName;
	LocalModelMap::iterator it = m_LogicRenderObjMap.find(strLogicName);
	if(it!=m_LogicRenderObjMap.end())
	{
		it->second->SetModelHandler(pHandler);
		return true;
	}
	else
	{
		ostringstream strm;
		strm<<strLogicName<<"<"<<m_LogicRenderObjMap.size()<<">";
		if(m_LogicRenderObjMap.size()>0)
			strm<<m_LogicRenderObjMap.begin()->first;
	}
	return false;	
}

bool CRenderScene::SetLogicModelNextAni(const char* szLogicName, const char* szAniFileName, bool bLoop, int32 uDelayTime, float AniSpeed)
{
	SString strLogicName = szLogicName;
	SMap<SString, CRenderObject*>::iterator iter = m_LogicRenderObjMap.find(strLogicName);
	if( iter != m_LogicRenderObjMap.end())
	{
		iter->second->DoAni(szAniFileName, -1,bLoop, uDelayTime,0,-1, AniSpeed);
		return true;
	}
	else
	{
		ostringstream strm;
		strm<<strLogicName<<"<"<<m_LogicRenderObjMap.size()<<">";
		if(m_LogicRenderObjMap.size()>0)
			strm<<m_LogicRenderObjMap.begin()->first;
	}
	return false;
}

bool CRenderScene::SetLogicModelEffect(const char* szLogicName, const char* szFxFile, const char* szFx )
{
	SString strLogicName = szLogicName;
	SMap<SString, CRenderObject*>::iterator iter = m_LogicRenderObjMap.find(strLogicName);
	if( iter != m_LogicRenderObjMap.end())
	{
		//iter->second->AddNoneActEffect(szFxFile, szFx, -1, NULL, NULL);
		return true;
	}
	else
	{
		ostringstream strm;
		strm<<strLogicName<<"<"<<m_LogicRenderObjMap.size()<<">";
	}
	return false;
}

void	CRenderScene::SetDist(float minDist ,float maxDist)
{
	m_RTSCamerCtrl.SetMinDist(minDist*eGridSpan);
	m_RTSCamerCtrl.SetMaxDist(maxDist*eGridSpan);
	m_RTSCamerCtrl.SetDist(maxDist*eGridSpan);

	m_TPSCamerCtrl.SetMinTargetDist(minDist*eGridSpan);
	m_TPSCamerCtrl.SetMaxTargetDist(maxDist*eGridSpan);
	m_TPSCamerCtrl.SetTargetDist(maxDist*eGridSpan);
	UpdateCameraOffset();
}

void	CRenderScene::SetFov(float minfov, float maxfov )
{
		const float fPI=3.14159265f;
	m_RTSCamerCtrl.SetMinFov(minfov*fPI/180);
	m_RTSCamerCtrl.SetMaxFov(maxfov*fPI/180);
}

void CRenderScene::SetXZDist(float xzdist)
{
	m_RTSCamerCtrl.SetMaxXZ_Dist(xzdist);
}

void CRenderScene::SetDistSpeed(float distSpeed)
{
	m_RTSCamerCtrl.SetDistSpeed(distSpeed);
}

void CRenderScene::SetCameraXRotate( float dgree )
{
	m_TPSCamerCtrl.SetXRotateDegree(dgree);
}

void CRenderScene::SetCameraYRotate( float dgree )
{
	m_TPSCamerCtrl.SetYRotateDegree(dgree + m_RTSCamerCtrl.GetYRadian());
	m_RTSCamerCtrl.SetYRotateDegree(dgree);
}

float CRenderScene::GetCameraXRotate( void )
{
	//return 0.0f;
	return m_TPSCamerCtrl.GetXRotateDegree();
}
float CRenderScene::GetCameraYRotate( void )
{
	//return m_TPSCamerCtrl.GetYRotateDegree();
	return m_RTSCamerCtrl.GetYRotateDegree();
}

float CRenderScene::GetCameraYDegree( void )
{
	return m_RTSCamerCtrl.GetYRadian();
}

void	CRenderScene::SetRadian(float mindegree, float maxdegree )
{
	const float fPI=3.14159265f;
	m_RTSCamerCtrl.SetMinRadian(mindegree*fPI/180);
	m_RTSCamerCtrl.SetMaxRadian(maxdegree*fPI/180);
	m_RTSCamerCtrl.SetYRadian(maxdegree*fPI/180);
	UpdateCameraOffset();
}

void	CRenderScene::SetYOffset(float minyoffset ,float maxyoffset)
{
	m_RTSCamerCtrl.SetMinYOffset(minyoffset*eGridSpan);
	m_RTSCamerCtrl.SetMaxYOffset(maxyoffset*eGridSpan);
	UpdateCameraOffset();
}



void CRenderScene::OnRTSConfigChanged(float DefDist)
{
	m_fDefCamDist = DefDist;
	m_RTSCamerCtrl.Bind(m_pCamera);
	m_pActiveCamCtrl->Bind(m_pCamera);
	SetCameraOffset( m_fDefCamDist );
}

void CRenderScene::UpdateCameraOffset()
{
	CMatrix matRotX;
	matRotX.SetRotateX( m_RTSCamerCtrl.GetMinRadian() 
		+ (m_RTSCamerCtrl.GetDist() - m_RTSCamerCtrl.GetMinDist())
		* (m_RTSCamerCtrl.GetMaxRadian() - m_RTSCamerCtrl.GetMinRadian())
		/ (m_RTSCamerCtrl.GetMaxDist()-m_RTSCamerCtrl.GetMinDist()) );
	CVector3f CameraOffset = CVector3f( 0.0f, 0.0f, -1.0f );
	CameraOffset.Rotate( matRotX );

	CMatrix matRotY;
	matRotY.SetRotateY( m_RTSCamerCtrl.GetYRadian() );
	CameraOffset.Rotate( matRotY );

	float fBack = m_fDefCamDist;
	SetCameraOffset( (CameraOffset*fBack).Mag() );
}

void CRenderSceneQueue::SelectRendObj(const CVector3f& rayPos, const CVector3f& rayDir, const CVector3f CameraDir)
{
	CRenderObject* pPreSelectObj = NULL;
	IntersectFlag tempIF = IF_IMPOSSIBLE;
	IntersectFlag tIF = IF_IMPOSSIBLE;
	bool isBefore = false;
	CVector3f CurSelectObjVec, PreSelectObjVec;
	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
	{
		CRenderObject* pCurSelectObj = class_cast<CRenderObject*>(*it);
		if ( pPreSelectObj )
		{
			pCurSelectObj->GetRootPosition(&CurSelectObjVec);
			pPreSelectObj->GetRootPosition(&PreSelectObjVec);
		}
		isBefore =pPreSelectObj? ( CurSelectObjVec - PreSelectObjVec ).Dot( CameraDir ) < 0:false;
		if (isBefore|| tempIF != IF_EXCELLENT )
		{
			tIF = (*it)->IntersectRay( rayPos, rayDir );
			if( tIF > tempIF || (tIF == tempIF &&  isBefore) )
			{
				pPreSelectObj = pCurSelectObj;
				tempIF = tIF;
			}
		}
	}
	if (pPreSelectObj)
	{
		pPreSelectObj->Activate();
	}
}

float CRenderScene::GetTerrainHeight( float xGround, float yGround )
{
	if ( !(	xGround <= 0.0f || xGround >= GetDataScene()->GetWidthInGrid() * eGridSpan ||
		yGround <= 0.0f || yGround >=  GetDataScene()->GetHeightInGrid() * eGridSpan ) )
	{
		return GetDataScene()->GetOffsetHeightInPixel( xGround, yGround );
	}
	return 0.0f;
}

//------------------------------------------------------------------------------
/**
true的情况, pos为左下角的点:
*---*
| \ |
*---*
*/
bool
CRenderScene::IsBackGrid( const CVector3f& pos ) const
{
	uint xGrid = uint(pos.x / eGridSpan);
	uint zGrid = uint(pos.z / eGridSpan);
	return ((xGrid + zGrid) % 2 == 0);
}
