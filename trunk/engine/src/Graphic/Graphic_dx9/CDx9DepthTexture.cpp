#include "stdafx.h"
#include "CDx9DepthTexture.h"
#include "CDX9MainWindow.h"
#include "CDx9Texture.h"
#include "CDx9TexFmtMapper.h"


DX9DepthTexture::DX9DepthTexture()
{		
	m_cmtClear = CMT_CLEAR_ZBUFFER_STENCI;
	m_bDepthBuffer = true;
}

bool DX9DepthTexture::ReCreate(void)
{
	if(m_pRenderGroup)
		m_pRenderGroup->GetLightSystem()->Reset();
	if (NULL != m_pBloomTarget)
	{
		m_pBloomTarget->Create();
		m_pBloomTarget->UpdateParmas();
	}

	GetD3DDevice()->EvictManagedResources();
	if( m_bDepthBuffer )
	{
		GetD3DDevice()->CreateDepthStencilSurface(m_rcViewPort.Width(), m_rcViewPort.Height(), D3DFMT_D24S8,
			(D3DMULTISAMPLE_TYPE)(CMainWindowTarget::GetInst())->GetAntiAliasQuality(), 0, TRUE,
						m_depthStencil.address(), NULL);
	}
	else
	{
		D3DFORMAT fmtDepth = CDx9TexFmtMapper::GetInst()->GetSuitableTextureFormat(TF_DEPTH);
		if ( fmtDepth == D3DFMT_UNKNOWN )
		{
			GfkLogErr("深度图创建失败", "");
			return false;
		}

		if ( fmtDepth == D3DFMT_D16 || fmtDepth == D3DFMT_D32F_LOCKABLE )
			m_cmtClear = CMT_CLEAR_ZBUFFER;

		GetD3DDevice()->CreateTexture( m_rcViewPort.Width(), m_rcViewPort.Height(),
			1 , D3DUSAGE_DEPTHSTENCIL, fmtDepth, D3DPOOL_DEFAULT, m_depthTexture.address(), NULL);
		
		m_depthTexture->GetSurfaceLevel(0, m_depthStencil.address());
	}
	return true;
}


void DX9DepthTexture::release()
{
	m_depthStencil = NULL;
	m_depthTexture = NULL;
}

void DX9DepthTexture::Reset()
{
	if (NULL != m_pBloomTarget)
		m_pBloomTarget->Release();

	release();
}

DX9DepthTexture::~DX9DepthTexture()
{
	release();
}

bool DX9DepthTexture::_Create()
{
	return this->ReCreate();

}

void DX9DepthTexture::Begin(bool needClear)
{
	GetD3DDevice()->GetDepthStencilSurface( m_oldDepthStencil.address());
	GetD3DDevice()->SetDepthStencilSurface(m_depthStencil);
	if( needClear )
		CRenderTarget::Clear();
}

void DX9DepthTexture::End()
{
	GetD3DDevice()->SetDepthStencilSurface(m_oldDepthStencil);
	m_oldDepthStencil = NULL;
}