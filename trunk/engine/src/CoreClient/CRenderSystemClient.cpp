#include "stdafx.h"
#include "CRenderSystemClient.h"
#include "CAppConfigClient.h"
#include "CRenderSystemClient.h"
#include "CSceneBlockDebug.h"
#include "CSceneGridDeubg.h"
#include "CScenePositionDebug.h"
#include "CSceneBoxDeubg.h"
#include "CSkeletalsMgr.h"
#include "CGraphic.h"
#include "CHardwareCapacity.h"
#include "StringHelper.h"
#include "CTextureQualityMgr.h"
#include "CRenderPipeline.h"
#include "CSqrAllocPoolMgr.h"
#include "CCameraConfig.h"
#include "CTimeSystemClient.h"
#include "CAppConfigClient.h"
#include "CSceneConfig.h"

CClientRenderScene::CClientRenderScene( CDataScene* pDataScene )
: CRenderScene(pDataScene)
{

}

CClientRenderScene::~CClientRenderScene(void)
{

}

void CRenderSystemClient::_SyncTime(void)
{
	while(!(CTimeSystemClient::Inst()->IsOnTick()||CTimeSystemClient::Inst()->PushLogicTime()));
}

CRenderSystemClient::CRenderSystemClient( HWND hWnd )
:m_CamMode(ECam_Mode16_10)
, m_NoChange(false)
{
	SQR_TRY
	{
		if( NULL == new CGraphic(eRCT_DX9))
			GenErr("渲染系统创建失败");			
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;
		strm << "初始化图形模块失败" << endl
			 << exp.ErrorTitle() << endl;
		GfkLogExp(exp);
		ShowErrorMsgAndExit(strm.str().c_str());
	}
	SQR_TRY_END;
	CTimeCheckPoint tcp;
	new CSkeletalsMgr(!CAppConfigClient::Inst()->IsTestClient());
	printf("读取骨架花费%d毫秒\n",tcp.GetElapse());

	this->InitRenderSystem(hWnd);
	
	//设置贴图质量	
	SQR_TRY
	{
		CSceneConfig* config = CSceneConfig::GetInst();
		//if(CHardwareCapacity::GetInst()->GetVideoMem() != 0)
		//{
		//	UINT QLevel = config->GetTexQualityLevel();
		//	string Info = "";
		//	if( QLevel == 0 && CHardwareCapacity::GetInst()->GetVideoMem() < config->GetTextHightMem() )
		//	{
		//		//++QLevel;
		//		format(Info,"您的有效显存(%uMB)过低，建议不要开启高质量纹理",CHardwareCapacity::GetInst()->GetVideoMem());
		//	}

		//	if( QLevel <= 1 && CHardwareCapacity::GetInst()->GetVideoMem() < config->GetTextMidMem() )
		//	{
		//		//++QLevel;
		//		format(Info,"您的有效显存(%uMB)过低，建议不要开启中质量以上纹理",CHardwareCapacity::GetInst()->GetVideoMem());
		//	}

		//	if(Info.size()>0)
		//		::MessageBox(hWnd,Info.c_str(),"提示",0);
		//	CTextureQualityMgr::GetInst()->SetQualityLevel(QLevel);
		//}
		//else
		CTextureQualityMgr::GetInst()->SetQualityLevel(config->GetTexQualityLevel()); 	
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("设置纹理质量失败");
		GfkLogExp(exp);
	}
	SQR_TRY_END;
}

CRenderSystemClient* CRenderSystemClient::Inst()
{
	return (CRenderSystemClient*)GetInst();
}

CClientRenderScene* CRenderSystemClient::CreateRenderScene( CDataScene* pMetaScene )
{
	//CCameraTick::GetInst()->Reset();
	m_isShowDebug		= 0;
	m_isShowASynLoad	= 0;
	CClientRenderScene* pScene = new CClientRenderScene( pMetaScene );
	InitCameraInfo();
	return pScene;
}

void CRenderSystemClient::DestroyRenderScene(CClientRenderScene* pScene)
{
	delete pScene;
	IEffectManager::GetInst()->ClearEffectDesc();
}

void CRenderSystemClient::Render()
{	
	if( m_isShowFps )
	{
		CRenderPipeline* pRender = CRenderPipeline::GetInst();
		static uint lastTime = timeGetTime();
		static uint frameNum = 0;
		static float fps = 0.0f;
		++frameNum;
		uint deltaTime = timeGetTime() - lastTime;
		if (deltaTime > CLOCKS_PER_SEC)
		{
			fps = frameNum / float(deltaTime) * CLOCKS_PER_SEC;
			lastTime += deltaTime;
			frameNum = 0;
		}
		CIRect rt;
		CMainWindowTarget::GetInst()->GetRect(rt);
		uint tcSize = 1048576;

		ostringstream tmp;
		CSqrAllocPoolMgr::Inst().GetAllMemInfo( tmp );
		DebugText(0xff00ff00, "FPS:%.2f \t分辨率:%uX%u 绘制数: %u",fps, rt.Width(),rt.Height(), pRender->GetPrimitiveCount());
		if(m_isShowFps>1)
		{
			DebugText(0xffff0000, "模型内存: %u MB \t 场景纹理: %u MB \tGUI 纹理: %u MB", CHardwareBufferMgr::GetInst()->GetBufferSize()/tcSize,CTextureQualityMgr::GetInst()->GetInCtrlSize()/tcSize,  CTextureQualityMgr::GetInst()->GetOutCtrlSize()/tcSize );
			DebugText(0xff00ff00, tmp.str().c_str());
		}
	}

	if ( FAILED( CMainWindowTarget::GetInst()->IsRenderActive() ) )
	{
		Update();
		return;
	}

	SQR_TRY
	{
		if (CClientRenderScene::HasInst()&&m_isRender)
			CClientRenderScene::GetInst()->Render();
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CRenderScene::Render()");
		GfkLogExp(exp);
	}
	SQR_TRY_END;

	Refresh();	
}

void	CRenderSystemClient::Update(void)
{
	if (CClientRenderScene::HasInst())
		CClientRenderScene::GetInst()->Update();
}

void	CRenderSystemClient::SetIsRender(bool isRender)
{
	if( CAppConfigClient::Inst()->IsTestClient() )
		CRenderSystem::SetIsRender(isRender);
}

bool	CRenderSystemClient::IsFreeCameraEnabled(void)
{
	return m_bFreeCamera;
}

void	CRenderSystemClient::SetFreeCameraEnabled(bool isFree)
{
	m_bFreeCamera = isFree;
	CSceneConfig::GetInst()->SetFreeCameraEnabled(isFree);
	if (CClientRenderScene::GetInst())
	{	
		CClientRenderScene::GetInst()->m_pActiveCamCtrl->DoCtrl((uint32)GetProcessTime());
		CClientRenderScene::GetInst()->m_RTSCamerCtrl.SetMaxYDegree(m_bFreeCamera?CMath::two_pi:0.0f);
		CClientRenderScene::GetInst()->m_pActiveCamCtrl->Bind(CClientRenderScene::GetInst()->m_pCamera);
		CClientRenderScene::GetInst()->m_pActiveCamCtrl->BeginCtrl((uint32)GetProcessTime());
	}
}

void	CRenderSystemClient::SetFree3DCameraEnabled(bool isFree)
{
	if( CAppConfigClient::Inst()->IsTestClient() )
	{
		if (CClientRenderScene::GetInst())
		{	
			if(isFree)
			{
				m_bFreeCamera = true;
				CClientRenderScene::GetInst()->m_pActiveCamCtrl = &CClientRenderScene::GetInst()->m_TPSCamerCtrl;
				CClientRenderScene::GetInst()->m_TPSCamerCtrl.SetYRotateDegree(CClientRenderScene::GetInst()->m_RTSCamerCtrl.GetYRotateDegree() + CClientRenderScene::GetInst()->m_RTSCamerCtrl.GetYRadian());
			}
			else
			{
				CClientRenderScene::GetInst()->m_pActiveCamCtrl = &CClientRenderScene::GetInst()->m_RTSCamerCtrl;
				SetFreeCameraEnabled(true);
			}

			CClientRenderScene::GetInst()->m_pActiveCamCtrl->Bind(CClientRenderScene::GetInst()->m_pCamera);
			CClientRenderScene::GetInst()->m_pActiveCamCtrl->BeginCtrl((uint32)GetProcessTime());
		}
	}
}

void	CRenderSystemClient::SetBackBufferNum(uint32 Num)
{
	if( CAppConfigClient::Inst()->IsTestClient() )
		CRenderSystem::SetBackBufferNum(Num);
}

void	CRenderSystemClient::ShowFpsSwitch(void)
{
	if( CAppConfigClient::Inst()->IsTestClient() )
		CRenderSystem::ShowFpsSwitch();
}

#include "CCoreSceneClient.h"
void	CRenderSystemClient::ShowDebugSwitch(void)
{
	class CClientBlockDebugNode : public CSceneBlockDebugNode
	{
	public:
		CClientBlockDebugNode(ERenderObjStyle style , CRenderScene* pScene)
			: CSceneBlockDebugNode(style,pScene){}

		EBarrierType	GetBarrierType(CPos pos)
		{
			return CCoreSceneClient::MainScene()->GetBarrier(pos);
		}
	};

	if( CAppConfigClient::Inst()->IsTestClient() )
	{
		CRenderSystem::ShowDebugSwitch();

		static CSceneDebugNode*		stBlockDebuger;
		static CSceneDebugNode*		stpGridDebuger;
		static CSceneDebugNode*		stBoxDebuger;
		static CSceneDebugNode*		stPosDebug;
		CClientRenderScene::GetInst()->DelelteDebugNode(stBlockDebuger);
		CClientRenderScene::GetInst()->DelelteDebugNode(stpGridDebuger);
		CClientRenderScene::GetInst()->DelelteDebugNode(stBoxDebuger);
		CClientRenderScene::GetInst()->DelelteDebugNode(stPosDebug);

		if(CRenderSystem::GetInst()->m_isShowDebug)
		{
			if(CRenderSystem::GetInst()->m_isShowDebug%2==0)
			{
				CClientRenderScene::GetInst()->RegistDebugNode( stBlockDebuger = new CClientBlockDebugNode(ROS_LIGHTMAT1,CClientRenderScene::GetInst()));
				CClientRenderScene::GetInst()->RegistDebugNode( stpGridDebuger = new CSceneGridDebugNode(ROS_LIGHTMAT1,CClientRenderScene::GetInst()));
			}
			else
			{
				CClientRenderScene::GetInst()->RegistDebugNode(stBlockDebuger = new CClientBlockDebugNode(ROS_UNKNOWN,CClientRenderScene::GetInst()));
				CClientRenderScene::GetInst()->RegistDebugNode(stpGridDebuger = new CSceneGridDebugNode(ROS_UNKNOWN,CClientRenderScene::GetInst()));
				CClientRenderScene::GetInst()->RegistDebugNode(stBoxDebuger = new CSceneBoxDebugNode(CClientRenderScene::GetInst()));
				
			}
			CClientRenderScene::GetInst()->RegistDebugNode(stPosDebug = new CScenePositionDebugNode);
		}
	}
}

//void	CRenderSystemClient::ShowRenderObjSwitch(void)
//{
//	if( CAppConfigClient::Inst()->IsTestClient() )
//		CRenderSystem::ShowRenderObjSwitch();
//}

void	CRenderSystemClient::ShowGUISwith(void)
{
	if( CAppConfigClient::Inst()->IsTestClient() )
		CRenderSystem::ShowGUISwith();
}

void	CRenderSystemClient::ShowPlayerSwitch(void)
{
	if( CAppConfigClient::Inst()->IsTestClient() )
		CRenderSystem::ShowPlayerSwitch();
}

void	CRenderSystemClient::ShowASynLoadSwitch(void)
{
	if( CAppConfigClient::Inst()->IsTestClient() )
		CRenderSystem::ShowASynLoadSwitch();
}

void	CRenderSystemClient::LockAspectSwitch(void)
{
	if( CAppConfigClient::Inst()->IsTestClient() )
		CRenderSystem::LockAspectSwitch();
}

void	CRenderSystemClient::SwitchHidenMode(void)
{
	/*if( CAppConfigClient::Inst()->IsTestClient() )
		*/CRenderSystem::SwitchHidenMode();
}

void CRenderSystemClient::InitCameraInfo()
{
	CIRect rtClient;
	CMainWindowTarget::GetInst()->GetRect(rtClient);

	if (rtClient.Width() == 0 || rtClient.Height() == 0)
		return;
	m_CamMode = ECam_Mode16_10;
	SetCameraInfo((float)(rtClient.Width()), (float)(rtClient.Height()));
}

void CRenderSystemClient::SetCameraInfo(ECamMode c)
{
	CCameraConfig::GetInst()->SetCameraInfo(CClientRenderScene::GetInst(), c);
}

void CRenderSystemClient::SetCameraInfo(float w, float h)
{
	int	idx	= (int)(ECam_Mode16_10);
	if (w == 0.0f)
	{
		SetCameraInfo( ECam_Mode16_10 );
	}
	else
	{
		float ratio[4] = { 9.0f/16, 10.0f/16, 3.0f/4, 4.0f/5 };
		float dis	= 100.0f;
		float cur_ratio = h / w;
		for (int i = 0; i < 4; i++)
		{
			if ( dis > abs(cur_ratio - ratio[i]) )
			{
				dis = abs(cur_ratio - ratio[i]);
				idx = i;
			}
		}

		if (m_NoChange)
			return;
		SetCameraInfo( (ECamMode)(idx) );
	}
	m_CamMode = (ECamMode)(idx);
}

CRenderSystemClient::~CRenderSystemClient()
{
}

//-----------------------------------
// 获取默认摄象机配置
#include "CCameraConfig.h"

float	CRenderSystemClient::GetCamMaxXRadian(void)	const
{
	return CCameraConfig::GetInst()->GetCamMaxXRadian(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamMinXRadian(void)	const
{
	return CCameraConfig::GetInst()->GetCamMinXRadian(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamYRadian(void)		const
{
	return CCameraConfig::GetInst()->GetCamYRadian(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamMaxFovRadian(void)	const
{
	return CCameraConfig::GetInst()->GetCamMaxFovRadian(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamMinFovRadian(void)	const
{
	return CCameraConfig::GetInst()->GetCamMinFovRadian(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamMaxNearPlan(void)	const
{
	return CCameraConfig::GetInst()->GetCamMaxNearPlan(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamMinNearPlan(void)	const
{
	return CCameraConfig::GetInst()->GetCamMinNearPlan(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamMinYOffset(void)	const
{
	return CCameraConfig::GetInst()->GetCamMinYOffset(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamMaxYOffset(void)	const
{
	return CCameraConfig::GetInst()->GetCamMaxYOffset(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamMinDist(void)		const
{
	return CCameraConfig::GetInst()->GetCamMinDist(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamMaxDist(void)		const
{
	return CCameraConfig::GetInst()->GetCamMaxDist(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamMaxHOffset(void)	const
{
	return CCameraConfig::GetInst()->GetCamMaxHOffset(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamSpeed(void)			const
{
	return CCameraConfig::GetInst()->GetCamSpeed(ECam_ModeDefault);
}

float	CRenderSystemClient::GetCamDefDist(void)		const
{
	return CCameraConfig::GetInst()->GetCamDefDist(ECam_ModeDefault);
}
