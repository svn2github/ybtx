#include "stdafx.h"
#include "CWindowTarget.h"
#include "CViewport.h"
#include "CRenderPipeline.h"
#include "CGraphic.h"
#include "TSqrAllocator.inl"

CWindowTarget::CWindowTarget()
{
	m_rcViewPort.left	= 0;
	m_rcViewPort.right	= 400;
	m_rcViewPort.top	= 0;
	m_rcViewPort.bottom = 300;
	m_beFullScreen		= false;
	m_uHeight			= 300;
	m_uWidth			= 400;
	m_BeProtected		= false;
}

CWindowTarget::~CWindowTarget()
{
	if (m_lRenderTarget.size()>0)
		OutputDebugString( "存在Viewport没有被释放\n" );
}

CViewport*	CWindowTarget::AddViewport( void )
{
	CViewport* pViewport = new CViewport(this);
	pViewport->AddRef();
	if ( pViewport )
		m_lRenderTarget.push_back(pViewport);
	return pViewport;
}

void CWindowTarget::DeleteViewport( CViewport* pTarget )
{
	for ( int i = 0; i < (int)m_lRenderTarget.size(); i++ )
	{
		if (	m_lRenderTarget[i] == pTarget )
		{
			m_lRenderTarget.erase( m_lRenderTarget.begin() + i );
			break;
		}
	}
}

void CMainWindowTarget::SetBackBufferNum( UINT Num ) 
{
	RenderSettings tempSettings = m_RenderSettings;
	tempSettings.SetBackBufferNum(Num);
	ChangeRenderSettings(tempSettings);
	m_RenderSettings = tempSettings;
}

UINT CMainWindowTarget::GetBackBufferNum()
{
	return m_RenderSettings.GetBackBufferNum();
}


void CMainWindowTarget::Render( void )
{
	SQR_TRY
	{
		CRenderPipeline* pRenderPipeline = CRenderPipeline::GetInst();
		if( !pRenderPipeline->Begin() )
		{
			CGraphic::GetInst()->ClearStack();
			return;
		}	
		CGraphic::GetInst()->UpdateRenderTarget();
	
		CIRect viewport;
		CMainWindowTarget::GetInst()->GetRect(viewport);
		pRenderPipeline->SetViewport(viewport);

		UpdateViewport();
		Clear();
		CWindowTarget::Render();

		CGraphic::GetInst()->DrawStack();
		pRenderPipeline->End();
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CMainWindowTarget::Render");
		GfkLogExp(exp);
	}
	SQR_TRY_END;
	CGraphic::GetInst()->ClearStack();
}