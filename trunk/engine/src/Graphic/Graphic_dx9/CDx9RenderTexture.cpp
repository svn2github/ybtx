#include "stdafx.h"
#include "CDX9RenderTexture.h"
#include "CDx9DeviceCapacity.h"
#include "CDx9RenderPipeline.h"
#include "CDx9MainWindow.h"
#include "CCamera.h"
#include "CGraphicExp.h"
#include "CGraphic.h"
#include <Dxerr.h>

DX9RenderTexture::DX9RenderTexture()
		: m_oldAspectRatio(1.0f)
{
}


DX9RenderTexture::~DX9RenderTexture()
{
	release();
}

//-----------------------------------------------------------------------------


bool DX9RenderTexture::_Create()
{
	return this->ReCreate();
}

void DX9RenderTexture::Reset(void)
{
	if (NULL != m_pBloomTarget)
		m_pBloomTarget->Release();

	release();
}

bool DX9RenderTexture::ReCreate(void)
{
	if(m_pRenderGroup)
		m_pRenderGroup->GetLightSystem()->Reset();
	if (NULL != m_pBloomTarget)
	{
		m_pBloomTarget->Create();
		m_pBloomTarget->UpdateParmas();
	}

	Ast(!m_renderTarget.isvalid());
	Ast(!m_renderTexture.isvalid());

	HRESULT hr;
	uint32  err_pos = 0;
	D3DFORMAT d3dFormat = DX9RenderPipeline::get(m_Format);
	D3DMULTISAMPLE_TYPE d3dMSAA = (D3DMULTISAMPLE_TYPE)CMainWindowTarget::GetInst()->GetAntiAliasQuality();
	if (!CHardwareCapacity::GetInst()->IsSupportRenderTarget(m_Format))
	{
		string errorMsg = string("显卡不支持RenderTarget格式: ") + RTTFormat_Kit::ToString(m_Format);
		GfkLogErr(GraphicErr::GetErrTypeStr(RenderRuntime_Err),errorMsg);
		d3dFormat = D3DFMT_A8R8G8B8;
	}
	// create render texture
	GetD3DDevice()->EvictManagedResources();
	hr = GetD3DDevice()->CreateTexture(m_rcViewPort.Width(), m_rcViewPort.Height(), 1,
	                                D3DUSAGE_RENDERTARGET, d3dFormat, D3DPOOL_DEFAULT, m_renderTexture.address(), NULL);
	
	if ( FAILED(hr) )
		goto ERROR_OCCURRED;

	// create a render target for anti alias
	if (IsAntiAlias())
	{
		hr = GetD3DDevice()->CreateRenderTarget(m_rcViewPort.Width(), m_rcViewPort.Height(), d3dFormat,
		                                     d3dMSAA, 0, FALSE, m_renderTarget.address(), NULL);
		if ( FAILED(hr) )
		{
			err_pos = 1;
			goto ERROR_OCCURRED;
		}
	}
	else
	{
		// directly render to texture
		hr = m_renderTexture->GetSurfaceLevel(0, m_renderTarget.address());
		if ( FAILED(hr) )
		{
			err_pos = 2;
			goto ERROR_OCCURRED;
		}
	}
	// create depth stencil surface
	if (m_bDepthBuffer)
	{
		m_DepthBuffer.SetRect(m_rcViewPort);
		m_DepthBuffer.SetUseDepthBuffer(true);
		m_DepthBuffer.ReCreate();
	}
	return true;

ERROR_OCCURRED:
	GraphicErrType ErrType = TextureCreate_Err;
	if ( hr == D3DERR_OUTOFVIDEOMEMORY || hr == E_OUTOFMEMORY )
		ErrType = Memory_Err;
	CError exp(GraphicErr::GetErrTypeStr(ErrType));
	stringstream ss;
	ss << DXGetErrorDescription(hr) << " error pos:" << err_pos;
	exp.AppendMsg(ss.str());
	GfkLogExpOnce(exp);
	return false;
}

void DX9RenderTexture::release()
{
	m_renderTarget = NULL;
	m_renderTexture = NULL;
	m_DepthBuffer.release();
}

//------------------------------------------------------------------------------
void
DX9RenderTexture::Begin( bool needClear /*= true*/ )
{
	Ast(NULL != m_renderTarget);

	// capture old state
	CCamera* camera = CGraphic::GetInst()->GetActiveRenderTarget()->GetCamera();

	GetD3DDevice()->GetViewport(&m_oldViewport);

	GetD3DDevice()->GetRenderTarget(0, m_oldRenderTarget.address());

	// begin render target
	V(GetD3DDevice()->SetRenderTarget(0, m_renderTarget));

	m_DepthBuffer.Begin(false);

	m_oldAspectRatio = camera->getAspectRatio();
	camera->setAspectRatio(float(m_rcViewPort.Width()) / float(m_rcViewPort.Height()));

	D3DVIEWPORT9 newViewport;
	ZeroMemory(&newViewport, sizeof(newViewport));
	newViewport.Width = m_rcViewPort.Width();
	newViewport.Height = m_rcViewPort.Height();
	newViewport.MaxZ = 1.0f;
	V(GetD3DDevice()->SetViewport(&newViewport));
	if (needClear)
		CRenderTarget::Clear();
}

//------------------------------------------------------------------------------
void
DX9RenderTexture::End()
{
	Ast(NULL != m_oldRenderTarget);

	// copy anti aliased surface to texture
	if (IsAntiAlias())
	{
		ID3DSurfacePtr surface;
		V(m_renderTexture->GetSurfaceLevel(0, surface.address()));
		V(GetD3DDevice()->StretchRect(m_renderTarget, NULL, surface, NULL, D3DTEXF_NONE));
	}

	// restore old state
	V(GetD3DDevice()->SetRenderTarget(0, m_oldRenderTarget));

	//if(m_pDepthBuffer)
	m_DepthBuffer.End();
	
	CCamera* camera = CGraphic::GetInst()->GetActiveRenderTarget()->GetCamera();
	camera->setAspectRatio(m_oldAspectRatio);

	V(GetD3DDevice()->SetViewport(&m_oldViewport));
	// release surfaces
	m_oldRenderTarget = NULL;
}

//------------------------------------------------------------------------------
void DX9RenderTexture::SetAntiAlias( bool isUseAntiAlias )
{
	m_isAntiAlias = isUseAntiAlias && CHardwareCapacity::GetInst()->IsCanAntiAlias(m_Format);
	m_DepthBuffer.SetAntiAlias(isUseAntiAlias);
}

//------------------------------------------------------------------------------
bool
DX9RenderTexture::IsAntiAlias() const
{
	return (m_isAntiAlias && AA_None != CMainWindowTarget::GetInst()->GetAntiAliasQuality());
}

bool DX9RenderTexture::UpdateDefaultTex(void)
{
	ID3DSurfacePtr		pTempSuf;
	RECT				tRect;
	D3DVIEWPORT9		tVp;
	GetD3DDevice()->GetRenderTarget(0, pTempSuf.address());
	GetD3DDevice()->GetViewport(&tVp);
	tRect.top	= tVp.Y;
	tRect.left	= tVp.X;
	tRect.bottom= tVp.Y + tVp.Height;
	tRect.right	= tVp.X + tVp.Width;

	return SUCCEEDED(GetD3DDevice()->StretchRect( pTempSuf, &tRect,m_renderTarget,NULL,D3DTEXF_NONE ));
}

HRESULT		DX9RenderTexture::SaveToFile(const string& filename, IMAGE_FILEFORMAT Format )
{
	if(Format>=IFF_DXT1)
	{
		ID3DTexturePtr	pSaveTexture;
		HRESULT hr;		
		switch(Format)
		{
		case IFF_DXT1:
			hr =  GetD3DDevice()->CreateTexture(m_rcViewPort.Width(), m_rcViewPort.Height(), 1, 
				0, D3DFMT_DXT1, D3DPOOL_MANAGED, pSaveTexture.address(), NULL); 
			break;
		case IFF_DXT2:
			hr =  GetD3DDevice()->CreateTexture(m_rcViewPort.Width(), m_rcViewPort.Height(), 1, 
				0, D3DFMT_DXT1, D3DPOOL_MANAGED, pSaveTexture.address(), NULL); 
			break;
		case IFF_DXT3:
			hr =  GetD3DDevice()->CreateTexture(m_rcViewPort.Width(), m_rcViewPort.Height(), 1, 
				0, D3DFMT_DXT1, D3DPOOL_MANAGED, pSaveTexture.address(), NULL); 
			break;
		case IFF_DXT4:
			hr =  GetD3DDevice()->CreateTexture(m_rcViewPort.Width(), m_rcViewPort.Height(), 1, 
				0, D3DFMT_DXT1, D3DPOOL_MANAGED, pSaveTexture.address(), NULL); 
			break;
		case IFF_DXT5:
			hr =  GetD3DDevice()->CreateTexture(m_rcViewPort.Width(), m_rcViewPort.Height(), 1, 
				0, D3DFMT_DXT1, D3DPOOL_MANAGED, pSaveTexture.address(), NULL); 
			break;
		default:
			break;
		};

		if ( FAILED(hr) )
			return hr;

		ID3DSurfacePtr	pSaveSurface;
		pSaveTexture->GetSurfaceLevel(0,pSaveSurface.address());
		hr = D3DXLoadSurfaceFromSurface(pSaveSurface, NULL, NULL, m_renderTarget, NULL, NULL, D3DX_FILTER_NONE, 0);
		if ( FAILED(hr) )
			return hr;

		return D3DXSaveSurfaceToFile(filename.c_str(),D3DXIFF_DDS,pSaveSurface,NULL,NULL);
	}

	return D3DXSaveTextureToFile(filename.c_str(),(D3DXIMAGE_FILEFORMAT)Format,m_renderTexture,NULL);
}