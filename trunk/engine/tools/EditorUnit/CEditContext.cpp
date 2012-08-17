#include "stdafx.h"
#include "CEditContext.h"
#include "CEditToolApp.h"
#include "CRenderGroup.h"
#include "CWindowTarget.h"
#include "CEditDataScene.h"
#include "CEditTerrainMgr.h"
#include "CGraphic.h"
#include "StringHelper.h"
#include "ContextMsg.h"
#include "CEditBlockDebugNode.h"

URegionID GetRegionID(const CVector3f& v3)
{
	URegionID id;

	id.X = int16(CMath::Abs(v3.x / (eSGridSpan * eSRegionSpan)));
	id.Z = int16(CMath::Abs(v3.z / (eSGridSpan * eSRegionSpan)));
	return id;
}

CEditContext::CEditContext(void)
: CRenderScene( new CEditDataScene )
{
	((CEditToolApp*)(CEditToolApp::GetInst()))->_AddContext(this);
	m_pWindowTarget = NULL;
	m_u64LastTime	= 0;
	m_uUseTime		= 0;
	m_uLimitCyc		= 33;
	m_pActiveModel	= NULL;

	m_vSunDir=CVector3f(0.0f,-1.0f,0.0f);
}


CEditContext::~CEditContext(void)
{
	SafeRelease(m_pRenderGroup);
	((CEditToolApp*)(CEditToolApp::GetInst()))->_DelContext(this);
}

void CEditContext::Bump( uint32 uTime )
{
	uint64 nUsedTime= uTime - m_u64LastTime;

	m_u64LastTime = uTime;
	m_uUseTime += nUsedTime;

	if( m_uUseTime < m_uLimitCyc )
		return;
	if(_IsRuning())
	{
		while(!IsEmpty())
			DoNext();

		_Update(uTime);
	}
	if(m_pWindowTarget)
	{
		_Render(uTime);
		m_pWindowTarget->Render();
	}
	m_uUseTime = 0;
}

void CEditContext::SetCycSpeed( uint32 cyc )
{
	m_uLimitCyc = cyc;
}

bool CEditContext::_ProcMsg( const CMsg& msg )
{
	switch( msg.msgSource )
	{
	case MGS_KEYMSG:
		{
			switch(msg.msgType.key_type)
			{
			case MGT_KEY_DOWN:
				{
					switch (msg.msgInfo.key_info)
					{
					case MGI_KEY_Question:
						{
							bool bWire = GetEditDataScene()->GetTerrainMgr()->m_Param.m_RS.m_FillMode == 2;
							GetEditDataScene()->GetTerrainMgr()->m_Param.m_RS.m_FillMode = 3 - !bWire;
							break;
						}

					case MGI_KEY_B:
						{
							static bool bShow = true;
							if( bShow )
							{
								CSceneDebugNode* stBlockDebuger = new CEditBlockDebugNode(ROS_UNKNOWN, this);
								RegistDebugNode(stBlockDebuger);
							}
							else
							{
								SceneDebugClear();
							}

							bShow = !bShow;

							break;
						}

					default:
						break;
					}
				}
				break;

			}
		}
	default:
		break;
	}

	return true;
}

void CEditContext::_Update( uint32 uTime )
{

}

void CEditContext::_Render( uint32 uTime )
{
	if(m_pWindowTarget)
	{
		CGraphic::GetInst()->ActiveRenderTarget(m_pWindowTarget);
		CRenderScene::Render();
		_PostMsg(MSG_SYS_RENDER());	
	}
}

CRenderObject*	CEditContext::CreateRenderObject(IRenderTime* pRenderTime)
{
	return CreateEditModel(pRenderTime);
}

CEditModel*	CEditContext::CreateEditModel(IRenderTime* pRenderTime)
{
	CEditModel* pRenderObj = new CEditModel(this);
	if (NULL!=pRenderObj)
		m_RenderQueue.AddChild(pRenderObj,LT_UNKNOW,"");
	return pRenderObj;
}

void CEditContext::AddToTreeScene( CEditModel* pChild )
{
	SQR_TRY
	{
		pChild->OnPreRender(0, NULL);
		CSceneTreeNode* pTreeNode = NULL;
		if( pTreeNode = m_pTreeScene->GetTreeNode(GetRegionID( pChild->GetPosition() )))
		{
			pTreeNode->AddChild(pChild);
		}
	}
	SQR_CATCH(exp)
	{
		string msg = exp.ErrorMsg();
		MessageBox(NULL, exp.ErrorMsg(), "提示", MB_OK);
	}
	SQR_TRY_END;
}

void CEditContext::AddToRenderScene( CEditModel* pChild )
{
	SQR_TRY
	{
		pChild->OnPreRender(0, NULL);
		m_RenderQueue.AddChild(pChild,LT_CENTER,"");
	}
	SQR_CATCH(exp)
	{
		string msg = exp.ErrorMsg();
		MessageBox(NULL, exp.ErrorMsg(), "提示", MB_OK);
	}
	SQR_TRY_END;
}

struct SelectContex
{
	CRenderNode*	m_pPreSelectObj;// = NULL;
	IntersectFlag	m_Flag;
	float			m_Dist;
	CVector3f		m_rayPos;
	CVector3f		m_rayDir;
	CCamera*		m_pCamera;
	SelectContex(void)
	{
		m_pPreSelectObj = NULL;
		m_Flag			= IF_IMPOSSIBLE;
		m_Dist			= 0.0f;
		m_pCamera		= NULL;
	}
};

void SelectFun(CRenderNode* pChild,void* param)
{
	SelectContex* pContex = (SelectContex*)param;
	if( pChild == NULL || pContex == NULL )
		return;

	CRenderNode *pPreSelectObj = pContex->m_pPreSelectObj;
	CRenderNode *pCurSelectObj = pChild;

	IntersectFlag tempIF = pCurSelectObj->IntersectRay( pContex->m_rayPos, pContex->m_rayDir );
	if( pContex->m_Flag > tempIF || tempIF == IF_IMPOSSIBLE)
		return;

	if( pContex->m_Flag < tempIF )
	{
		pContex->m_pPreSelectObj = pCurSelectObj;
		pContex->m_Dist = (pContex->m_pCamera->getViewProjectMatrix() * pCurSelectObj->GetPosition()).z;

	}
	else
	{
		float dist = (pContex->m_pCamera->getViewProjectMatrix() * pCurSelectObj->GetPosition()).z;
		if( dist < pContex->m_Dist)
		{
			pContex->m_Dist = dist;
			pContex->m_pPreSelectObj = pCurSelectObj;
		}
	}
}

CRenderNode* CEditContext::SelectModelByCursor( const float xCursorPercent, const float yCursorPercent )
{
	SelectContex Contex;
	Contex.m_pCamera = m_pRenderGroup->GetCamera();

	float fHeight		= CMath::TanTable( Contex.m_pCamera->getFOVy()/2.0f );					//视口高度与1的比值
	float fWidth		= fHeight*Contex.m_pCamera->getAspectRatio();							//视口宽度与1的比值
	float SrcnX			= ( (xCursorPercent)*2.0f - 1 )*fWidth ;
	float SrcnY			= -((yCursorPercent)*2.0f - 1 )*fHeight ;
	CVector3f vecZ		= Contex.m_pCamera->getDIRECTION();												//摄像机观察方向Z方向（前方）
	CVector3f tCamera   = CVector3f( 0.0f, 1.0f, 0.0f ).Cross( vecZ );
	if(CVector3f::ZERO == tCamera)
		return false;

	CVector3f vecX		= tCamera.Normalize();		//摄像机观察方向X方向（左方）
	CVector3f vecY		= vecZ.Cross( vecX ).Normalize();								//摄像机观察方向Y方向（上方）
	Contex.m_rayDir		= vecZ + SrcnX*vecX + SrcnY*vecY;								//鼠标向量
	Contex.m_rayPos		= m_pCamera->GetPosition();;												//摄像机位置
	CVector3f vecDest	= Contex.m_rayPos + Contex.m_rayDir * Contex.m_pCamera->getFarDist();											//摄像机位置+鼠标向量*ZFAR

	CPos posStart		= CPos( (int32)Contex.m_rayPos.x, (int32)Contex.m_rayPos.z );
	CPos posEnd			= CPos( (int32)vecDest.x, (int32)vecDest.z );
	if ( posStart == posEnd )
	{
		return NULL;
	}

	GetEditDataScene()->GetTreeScene()->DoCtrl(SelectFun, &Contex);
	return Contex.m_pPreSelectObj;
}

CEditDataScene* sqr_tools::CEditContext::GetEditDataScene( void )
{
	return (CEditDataScene*)m_pDataScene;
}

void sqr_tools::CEditContext::SetRenderTarget( CWindowTarget* target )
{
	if(target == m_pWindowTarget)
		return;

	m_pWindowTarget = target;

	SafeRelease(m_pRenderGroup);
	if(m_pWindowTarget)
	{
		m_pRenderGroup = m_pWindowTarget->GetGroup();
		m_pRenderGroup->AddRef();

	
		GetEditDataScene()->InitEditDateScene(m_pWindowTarget);
		GetEditDataScene()->Initialize();
		string tSceneName = m_pDataScene->GetSceneName();
		if(tSceneName.empty())
		{
			Initialize();
			m_FogStart = 1000;
			m_FogEnd = 3000;
			m_fCameraFogStart = m_FogStart - m_RTSCamerCtrl.GetMaxDist();
			m_fCameraFogEnd   = m_FogEnd - m_RTSCamerCtrl.GetMaxDist();
		}
		else
		{
			m_pDataScene->Create();
			InitSceneParams();
			Initialize();
			m_pDataScene->SetActivePos(0,0);
		}

		InitSceneRes();
		_PostMsg(MSG_SYS_INIT());
	}
}

CVector3f CEditContext::GetSelectedLogicPoint() const
{
	return m_vecSelectedLogic;
}

void CEditContext::SetFogParams( float fStart, float fEnd )
{
	m_FogStart	= fStart;
	m_FogEnd	= fEnd;
	m_fCameraFogStart = m_FogStart - m_RTSCamerCtrl.GetMaxDist();// * cosvalue;
	m_fCameraFogEnd   = m_FogEnd - m_RTSCamerCtrl.GetMaxDist();//   * cosvalue;
}

CWindowTarget* CEditContext::GetWindowTarget( void )
{
	return m_pWindowTarget;
}

bool CEditContext::ClearScene( void )
{
	m_eLoadState = eLoaded_Null; 
	SafeDelete(m_pDataScene);
	return true;
}

#include "SceneCoder/ybtx_gac_coder/ybtx_gac_coder.h"
bool CEditContext::LoadScene( const string& sName )
{
	ClearScene();
	string SceneName = sName.substr(0, sName.find_last_of("_"));

	m_pDataScene = new CEditDataScene(SceneName);
	if(SceneName.find("arg"))
	{
		m_pDataScene->SetCoder(new CYbtxGacCoder(m_pDataScene));
	}

	if(m_pWindowTarget)
	{
		GetEditDataScene()->InitEditDateScene(m_pWindowTarget);
		GetEditDataScene()->Initialize();
		m_pDataScene->Create();
		InitSceneParams();
		Initialize();
		m_pDataScene->SetActivePos(0,0);
		InitSceneRes();
	}
	return true;
}

CRenderNode* CEditContext::GetActiveObj(void)
{
	return m_pActiveModel;
}

bool CEditContext::SetActiveObj(CRenderNode* pObj)
{
	if(m_pActiveModel == pObj)
		return false;

	m_pActiveModel = pObj;
	_PostMsg(MSG_SYS_SELCHANGE(m_pActiveModel));
	return true;
}


CColor CEditContext::GetAmbientColor(void)
{
	return m_dwAmbientColor;
}

CColor CEditContext::GetSunLightColor(void)
{
	return m_dwSunColor;
}

CVector3f CEditContext::GetSunLightDir()
{
	CVector3f lPos;
	m_pLightSys->GetGlobalLightDir(m_vSunDir,lPos);
	return m_vSunDir;
}

CColor CEditContext::GetPlayerSunColor()
{
	return m_dwPlayerSunColor;
}

CColor CEditContext::GetPlayerAmbientColor()
{
	return m_dwPlayerAmbientColor;
}

CColor CEditContext::GetFogColor()
{
	return m_dwFogColor;
}

float CEditContext::GetFogStart()
{
	return m_FogStart;
}

float CEditContext::GetFogEnd()
{
	return m_FogEnd;
}

void CEditContext::SetAmbientColor( CColor color )
{
	m_dwAmbientColor = color;
	CalRegionLightParam();
}

void CEditContext::SetSunLightColor( CColor color )
{
	m_dwSunColor = color;
	CalRegionLightParam();
}

void CEditContext::SetSunLightDir( const CVector3f& dir )
{
	m_vSunDir = dir;
	//只能获取灯光系统去做这件事了
	m_pLightSys->SetGlobalLightDir( m_vSunDir );
	//CalRegionLightParam();
}

void CEditContext::SetPlayerSunColor(CColor color)
{
	m_dwPlayerSunColor = color;
	CalRegionLightParam();
}

void CEditContext::SetPlayerAmbientColor(CColor color)
{
	m_dwPlayerAmbientColor = color;
	CalRegionLightParam();
}

void CEditContext::SetFogColor(CColor color)
{
	m_dwFogColor=color;
	CalRegionLightParam();
}

void CEditContext::SetFogStart(float value)
{
	m_FogStart=value;
	CalRegionLightParam();
}

void CEditContext::SetFogEnd(float value)
{
	m_FogEnd=value;
	CalRegionLightParam();
}
