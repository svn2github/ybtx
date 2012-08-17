#include "StdAfx.h"
#include "CCoreViewImp.h"
#include "CGraphic.h"
#include "CRenderSystem.h"
#include "CSceneConfig.h"
#include "CEditorCore.h"
#include "CEditPostProcessManager.h"
#include "CRenderGroup.h"
#include "CWindowTarget.h"
#include "CEditContext.h"
#include "ContextMsg.h"

namespace sqr_tools
{
DEF_UI_IMP(CCoreViewImp)
CCoreViewImp::ViewList	CCoreViewImp::m_EditorViewList;

CCoreViewImp::CCoreViewImp( void* _handle )
: CCtrlBase()
{
	m_isWindowType=false;
	m_DependHandle=_handle;
	m_pWindowTarget=NULL;
	m_pRenderGroup=NULL;
	//创建完成后 自动初始化
	Initialize();
	ReSize();
}

CCoreViewImp::~CCoreViewImp(void)
{
	ViewList::iterator  it;
	if ( !m_EditorViewList.empty() )
	{
		it = m_EditorViewList.find(this);

		//if( it!= m_EditorViewList.end() )
		//{
		//	m_EditorViewList.erase(it);
		//	if( m_pWindowTarget == CMainWindowTarget::GetInst() )
		//	{
		//		if( !m_EditorViewList.empty() )
		//		{
		//			(*st_EditorViewList.begin())->_ActiveViewToMain();
		//			SafeRelease(m_pWindowTarget);
		//		}
		//		else
		//		{
		//			IEffectManager::Destroy();
		//			CEditorCore::Destroy();
		//			CGraphic::Destroy();
		//		}
		//	}		
		//}
	}
	
	SafeRelease(m_pRenderGroup);
}

//消息处理

void CCoreViewImp::SwitchWindowType()
{
	m_isWindowType=!m_isWindowType;
	RenderSettings& setting=(RenderSettings&)CMainWindowTarget::GetInst()->GetRenderSettings();
	if(m_isWindowType)
	{
		setting.SetWindowed(true);
	}
	else
	{
		setting.SetWindowed(false);
	}
	CRenderSystem::GetInst()->ChangeRenderSettings(&setting);
	CMainWindowTarget::GetInst()->RestoreSize();
}

void CCoreViewImp::Initialize()
{
	if(!m_DependHandle)
	{
		//给出警告信息 Please use AttachWindow() function 
	}

	if(m_EditorViewList.size()==0)
	{
		//初始化渲染系统
		RenderSettings settings;                                        //默认值
		CSceneConfig::GetInst()->SetWindowWidth(settings.GetWidth());
	    CSceneConfig::GetInst()->SetWindowHeight(settings.GetHeight());
		new CRenderSystem();
		CRenderSystem::GetInst()->InitRenderSystem((HWND)m_DependHandle);
		CEditorCore::Initialize();
		m_pRenderGroup=CMainWindowTarget::GetInst()->GetGroup();
		m_pWindowTarget=CMainWindowTarget::GetInst();
		//需要接管后处理框架
		CPostProcessManager* tmpPostProcessMgr=CPostProcessManager::GetInst();
		tmpPostProcessMgr->SetNull();
		new CEditPostProcessManager(tmpPostProcessMgr);
		CEditPostProcessManager::GetInst()->Setup();
		CPostProcessManager::GetInst()->Setup();
		CPostProcessManager::GetInst()->Discard();
	}
	else
	{
		RECT rc;
		::GetClientRect((HWND)m_DependHandle,&rc);
		RenderSettings settings;  
		settings.SetWindowHandle((HWND)m_DependHandle);
		settings.SetWidth(rc.right-rc.left);
		settings.SetHeight(rc.bottom-rc.top);
		m_pWindowTarget = CGraphic::GetInst()->CreateNewWindow(settings);
		m_pRenderGroup = new CRenderGroup;
		m_pWindowTarget->SetRenderGroup(m_pRenderGroup);
	}

	ViewList::iterator  it = m_EditorViewList.find(this);
	if( it== m_EditorViewList.end() )
		m_EditorViewList.insert(this);

	ReSize();
}

void CCoreViewImp::ReSize()
{
	if(m_pWindowTarget)
	{
		if(m_pOwnContext)
			m_pOwnContext->EventCollect(MSG_SYS_RESIZE());
		m_pWindowTarget->RestoreSize();   //内部实现 合它绑定的窗口大小一致
	}
}

CIRect CCoreViewImp::GetViewRect()     //获取Viewport的大小
{
	CIRect rect;
	if(m_pWindowTarget)
		m_pWindowTarget->GetRect(rect);
	return rect;
}

CWindowTarget* CCoreViewImp::GetWindowTarget()
{
	return m_pWindowTarget;
}

CRenderGroup* CCoreViewImp::GetRenderGroup()
{
	return m_pRenderGroup;
}

bool CCoreViewImp::OnCtrlSize(int width,int height)
{
	this->ReSize();
	return true;
}

void	CCoreViewImp::SetContext(CEditContext* pContext)
{
	if(pContext == m_pOwnContext)
		return;

	if(m_pOwnContext)
	{
		m_pOwnContext->SetRenderTarget(NULL);
	}

	m_pOwnContext = pContext;
	if(m_pOwnContext)
	{
		m_pOwnContext->SetRenderTarget(m_pWindowTarget);
		SetOwner(m_pOwnContext);
	}
}
//!namespace
}
