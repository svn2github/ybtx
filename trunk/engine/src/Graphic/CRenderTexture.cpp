#include "stdafx.h"
#include "CRenderTexture.h"
#include "CRenderPipeline.h"
#include "CRenderGroup.h"
#include "CGraphic.h"

CRenderTexture::CRenderTexture()
		: m_Format(TFMT_X8R8G8B8)
		, m_isAntiAlias(false)
		, m_bDepthBuffer(false)
		, m_bCreate(false)
{
	m_cmtClear = CMT_CLEAR_TARGET;
}

CRenderTexture::~CRenderTexture()
{
}

void	CRenderTexture::Free()
{
	CGraphic::GetInst()->DeleteWindowTarget(this);
	delete this;
}

void CRenderTexture::Render(void)
{
	Begin();
	Clear();
	CRenderPipeline::GetInst()->SetCamera(m_pRenderGroup->GetCamera());
	CWindowTarget::Render();
	End();
}

void CRenderTexture::SetUseDepthBuffer(bool b)
{
	m_bDepthBuffer = b;
	if( m_bDepthBuffer )
		m_cmtClear = CMT_CLEAR_ALL;
	else
		m_cmtClear = CMT_CLEAR_TARGET;
}

void CRenderTexture::SetWidth( uint32 width )
{
	if(!m_bCreate)
		CWindowTarget::SetWidth(width);
}

void CRenderTexture::SetHeight( uint32 height )
{
	if(!m_bCreate)
		CWindowTarget::SetHeight(height);
}

void CRenderTexture::SetRect( const CIRect& rect ) 
{
	if(!m_bCreate)
		CWindowTarget::SetRect(rect);
};
