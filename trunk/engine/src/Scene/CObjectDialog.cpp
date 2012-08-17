#include "stdafx.h"
#include "CObjectDialog.h"
#include "SQRRootWnd.h"
#include "CGraphic.h"
#include "CTexture.h"
#include "CRenderTexture.h"
#include "CWindowTarget.h"
#include "CRenderObject.h"
#include "BaseHelper.h"
#include "CMath.h"
#include "IGUIManager.h"
#include "CRenderTarget.h"
#include "CRenderGroup.h"
#include "CSkyMgr.h"

DYNAMIC_CREATE_WND( CObjectDialog, SQRDialog )

const uint CObjectDialog::PORTRAIT_SIZE = 128;// 再小某些显卡会出现花屏了

CObjectDialog::CObjectDialog()
: m_bViewPortrait(false)
, m_pPortraitInfo(NULL)
, m_pTexAlpha(NULL)
{
	m_pRenderGroup = new CRenderGroup;
	m_pRenderGroup->EnableFog(false);
	
	InitCameraInfo();

	RenderQueueFlag SceneFlag;
	SceneFlag<<ID_NPC_FILTER<<ID_ALPHA_NPC_FILTER<<ID_NPC_EFFECT_FILTER;
	m_pRenderGroup->SetRenderQueueFlag(SceneFlag);
	m_pRenderGroup->SetFogColor(0xFF000000);

	m_pRenderTarget = CMainWindowTarget::GetInst()->AddViewport();
	m_pRenderTarget->SetRenderGroup(m_pRenderGroup);
	
	CVector3f lightDir(-0.450000f,-0.495984f,0.860000f);
	lightDir.Normalize();
	m_pRenderGroup->GetLightSystem()->SetGlobalLightDir( lightDir );
	m_pRenderGroup->GetLightSystem()->SetAmbient(0xFFDCDCDC,1);
	m_pRenderGroup->GetLightSystem()->SetAmbient(0xFFDCDCDC,0);
	m_pRenderGroup->GetLightSystem()->SetGlobalLightColor(0xFF505050,1);
	m_pRenderGroup->GetLightSystem()->SetGlobalLightColor(0xFF505050,0);
}

CObjectDialog::~CObjectDialog()
{	
	if (NULL != m_pRenderGroup)
	{
		if( m_pRenderGroup->GetBatch(ID_SKYBOX_FILTER) )
			m_pRenderGroup->GetBatch(ID_SKYBOX_FILTER)->Release();
	}
	SafeRelease(m_pRenderGroup);
	SafeRelease(m_pRenderTarget);
	SafeRelease(m_pTexAlpha);
}

void CObjectDialog::InitCameraInfo()
{
	CCamera* pCamera = m_pRenderGroup->GetCamera();
	m_pActiveCamCtrl = &m_GUICamCtrl;
	m_pActiveCamCtrl->Bind(pCamera);

	m_GUICamCtrl.BeginCtrl(0);
	m_GUICamCtrl.InitCamCtrller();
}

void CObjectDialog::AddChild( CRenderSpaceNode* pChild, eLinkType linkType, const char* szAttachName )
{
	CRenderSpace::AddChild(pChild,linkType,szAttachName);
	m_pPortraitInfo = NULL;
}

uint8 CObjectDialog::GetAlphaValue()
{
	return (uint8)( SQRWnd::GetTransparent()*255 );
}

void CObjectDialog::SetAlphaValue( uint8 uAlpha )
{
	SQRWnd::SetTransparent( uAlpha/255.0f );
	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		(*it)->SetAlphaValue( uAlpha );
}

void CObjectDialog::DrawWndBackground()
{
	SQR_TRY	
	{
		FlushDraw();
		CGraphic::GetInst()->DrawStack();
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CObjectDialog::DrawWndBackground");
		GfkLogExp(exp);	
		return;
	}
	SQR_TRY_END;

	if(NULL==m_pPortraitInfo && m_listNode.size()>0 )
	{
		PortraitInfo* pInfo =  static_cast<CRenderObject*>(*m_listNode.begin())->GetPortraitInfo();
		if(NULL!=pInfo)
		{ 
			m_pPortraitInfo = pInfo;
			if(m_bViewPortrait)
			{
				CCamera* pCamera = NULL;
				pCamera = m_pRenderGroup->GetCamera();
				if( pCamera == NULL )
				{
					GfkLogErr("CObjectDialog摄像机为空");
					return;
				}

				SQR_TRY	
				{
					m_GUICamCtrl.InitCamCtrller( m_pPortraitInfo->cameraPos - m_pPortraitInfo->translate, -m_pPortraitInfo->translate );
				}
				SQR_CATCH(exp)
				{
					stringstream strTmp;
					strTmp<<"pos:"<<m_pPortraitInfo->cameraPos<<" tran:"<<m_pPortraitInfo->translate;
					exp.AppendType("头像信息获取错误");
					exp.AppendMsg(strTmp.str());
					GfkLogExp(exp);	
				}
				SQR_TRY_END;
			}
		}
	}

	m_pActiveCamCtrl->DoCtrl((uint32)GetProcessTime());
	Render( m_pRenderGroup->GetCamera(),(uint32)::GetProcessTime() );
}

void CObjectDialog::SetDepthAndRS( float fDepth, ERenderStyle eRS, SQRWnd& Wnd )
{
	Wnd.SetDepth( fDepth );
	Wnd.SetRenderStyle( eRS );
	SQRWnd* pChildWnd = Wnd.FindLastChild();
	while( pChildWnd )
	{
		SetDepthAndRS( fDepth, eRS, *pChildWnd );
		pChildWnd = pChildWnd->FindPreWnd();
	}
}

void CObjectDialog::SetUseSceneDepth( bool bUseSceneDepth )
{
	if( !bUseSceneDepth )
		SetDepthAndRS( 0.0f, RS_GUI, *this );
}

void CObjectDialog::Render( CCamera* pCamera, uint32 uCurTime )
{ 
	if( !IsCreated() || !m_pRenderTarget )
		return;

	CGraphic::GetInst()->ActiveRenderTarget(m_pRenderTarget);
	CRenderSpace::RenderEx( NULL ,uCurTime );
	CGraphic::GetInst()->ActiveRenderTarget(NULL);	
	m_pRenderTarget->Render();
	CMainWindowTarget::GetInst()->UpdateViewport();

	if(m_bViewPortrait && m_pTexAlpha)
	{
		CFRect rtWnd;
		float size = GetRootWnd()->GetZoomSize();
		this->GetWndRect(rtWnd);
		CVector2f rect[4] = {
			CVector2f(rtWnd.left, rtWnd.top),
			CVector2f(rtWnd.right, rtWnd.top),
			CVector2f(rtWnd.left, rtWnd.bottom),
			CVector2f(rtWnd.right, rtWnd.bottom)
		};
		CFRect rectTex(0, 0, (float)m_pTexAlpha->GetWidth(), (float)m_pTexAlpha->GetHeight());
		CVector2f rectAlpha[4] = {
			CVector2f(0.0f, 0.0f),
			CVector2f(float(PORTRAIT_SIZE), 0.0f),
			CVector2f(0.0f, float(PORTRAIT_SIZE)),
			CVector2f(float(PORTRAIT_SIZE), float(PORTRAIT_SIZE))
		};
		CColor uColor = CColor::White;
		uColor.A = static_cast<uint8>( uColor.A*GetWndHandle()->m_fTransparent );
		this->DrawRect(rect, uColor, m_pTexAlpha, static_cast<CRenderTexture*>(m_pRenderTarget)
			, &rectTex, rectAlpha, RS_GUI_ALPHABLEND, GetWndHandle()->m_fZ);
	}
}

void CObjectDialog::ResizeRect( const CFRect& rt, bool refreshbak  )
{
	SQRDialog::ResizeRect(rt, refreshbak);
	RefreshRect();
}

void  CObjectDialog::SetWndRect( const CFRect& rt )
{
	SQRDialog::SetWndRect(rt);
	RefreshRect();
}

void CObjectDialog::_ResizeRect( const CFRect& rt, bool refreshbak  )
{
	SQRDialog::_ResizeRect(rt, refreshbak);
	RefreshRect();
}

void  CObjectDialog::_SetWndRect( const CFRect& rt )
{
	SQRDialog::_SetWndRect(rt);
	RefreshRect();
}

void CObjectDialog::OnParentWndMoved( float dx, float dy )
{
	SQRDialog::OnParentWndMoved(dx,dy);
	RefreshRect();
}

void CObjectDialog::OnWndMoved( float dx, float dy )
{
	SQRDialog::OnWndMoved(dx,dy);
	RefreshRect();
}

int32 CObjectDialog::Create( const WndCreateParam& param )
{
	int32 ret = SQRDialog::Create( param );
	WndHandle* pWndH = GetWndHandle();
	if(NULL!=pWndH->m_ParentWnd)
	{
		CFRect rtWnd;
		pWndH->m_ParentWnd->GetWndRect( rtWnd );
		pWndH->m_ParentWnd->ResizeRectOneChild( rtWnd,this );
	}
	RefreshRect();
	return ret;
}

void  CObjectDialog::RefreshRect()
{
	CFRect rtWnd;
	CIRect	rtView;
	GetWndRect( rtWnd );
	rtView.left		= (int)(rtWnd.left		+ GetRootWnd()->GetWndHandle()->m_xScreen);
	rtView.right	= (int)(rtWnd.right		+ GetRootWnd()->GetWndHandle()->m_xScreen);
	rtView.bottom	= (int)(rtWnd.bottom	+ GetRootWnd()->GetWndHandle()->m_yScreen);
	rtView.top		= (int)(rtWnd.top		+ GetRootWnd()->GetWndHandle()->m_yScreen);

	m_pRenderTarget->SetRect(rtView);
}

void CObjectDialog::OnScreenPosChanged()
{
	SQRWnd::OnScreenPosChanged();
	RefreshRect();
}

void CObjectDialog::SetViewPortrait( bool bViewP, const char* BackTexture, const char* sFramePhoto )
{
	if(m_bViewPortrait==bViewP)
		return;
	
	m_bViewPortrait = bViewP && sFramePhoto != NULL;
	SafeRelease(m_pTexAlpha);
	SafeRelease(m_pRenderTarget);		
	CCamera* pCamera = m_pRenderGroup->GetCamera();

	if(m_bViewPortrait)
	{
		if( sFramePhoto != NULL )
		{	
			CRenderTexture* pRttTex;
			SString pkgName = sFramePhoto;
			CGraphic::GetInst()->CreateTexture(PATH_ALIAS_RES.c_str(),pkgName.c_str(), &m_pTexAlpha);
			pRttTex = CGraphic::GetInst()->CreateRenderTarget();
			pRttTex->SetWidth(PORTRAIT_SIZE);
			pRttTex->SetHeight(PORTRAIT_SIZE);
			pRttTex->SetUseDepthBuffer(true);
			pRttTex->SetAntiAlias(true);
			if ( pRttTex->Create() )
				m_pRenderTarget = pRttTex;
			//m_pRenderTarget->AddRef();
		}
		else
		{
			m_pRenderTarget = CMainWindowTarget::GetInst()->AddViewport();
			//m_pRenderTarget->AddRef();
		}
		m_pRenderTarget->SetRenderGroup(m_pRenderGroup);
		if(NULL != m_pPortraitInfo)
		{
			m_GUICamCtrl.InitCamCtrller( m_pPortraitInfo->cameraPos - m_pPortraitInfo->translate, -m_pPortraitInfo->translate );
		}
		else
		{
			m_GUICamCtrl.InitCamCtrller( CVector3f( -5.0f, 110.0f, 55.0f ), CVector3f( 0, 95, 1 ) );

		}
	}
	else
	{
		m_pRenderTarget = CMainWindowTarget::GetInst()->AddViewport();
		//m_pRenderTarget->AddRef();
		m_pRenderTarget->SetRenderGroup(m_pRenderGroup);
		m_GUICamCtrl.InitCamCtrller( CVector3f( 0.0f, 100.0f, -600.0f ), CVector3f( 0, 0, 1 ) );
	}


	if(BackTexture!=NULL)
	{
		RenderQueueFlag SceneFlag;
		SceneFlag
			<<ID_SKYBOX_FILTER
			<<ID_NPC_FILTER
			<<ID_ALPHA_NPC_FILTER;
		m_pRenderGroup->SetRenderQueueFlag(SceneFlag);
		CSkyMgr* pSkyBox = new CSkyMgr(m_pRenderTarget);
		string pkgName = BackTexture;
		pSkyBox->SetSkyBoxTextureName(pkgName);
	}
	else
	{
		RenderQueueFlag SceneFlag;
		SceneFlag<<ID_NPC_FILTER<<ID_ALPHA_NPC_FILTER;
		m_pRenderGroup->SetRenderQueueFlag(SceneFlag);
	}
}

void CObjectDialog::SwitchCamState()
{
	m_GUICamCtrl.SwitchCamState();
}


void CObjectDialog::GetAttachMatrix( CMatrix& matLink, uint32 uChildLinkID )
{
	matLink.Identity();
}

/*IntersectFlag CObjectDialog::CanBeSelected( const CVector3f& rayPos, const CVector3f& rayDir )
{
	if( IsCreated() )
	{
		const CFPos& pos = GetRootWnd()->GetCursorPos();
		if( IsInWndArea( pos.x, pos.y ) )
			return IF_POSSIBLE;
	}

	return CRenderSpace::CanBeSelected( rayPos, rayDir );
}
*/

CRenderObject*	CObjectDialog::CreateRenderObject()
{
	CRenderObject* pObj = new CRenderObject();
	if (NULL!=pObj)
		AddChild(pObj,LT_UNKNOW,"");
	return pObj;	
}

void	CObjectDialog::DestroyRenderObject(CRenderObject* pObj)
{
	CRenderSpace::DestroyRenderChild(pObj);
}

void	CObjectDialog::_DestoryRenderChild(CRenderSpaceNode* pObj)
{
	if(pObj->GetNodeType() == CRenderObject::st_RenderObjectType)
	{
		CRenderObject* pRenderObj=class_cast<CRenderObject*>(pObj);
		if(pRenderObj) pRenderObj->SetModelHandler(NULL);
	}
	if(pObj) delete pObj;
}
