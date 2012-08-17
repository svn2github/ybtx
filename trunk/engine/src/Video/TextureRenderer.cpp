#include "stdafx.h"
#include "TextureRenderer.h"
#include "CGraphic.h"

namespace VideoUtils
{
	//-----------------------------------------------------------------------------
	// CTextureRenderer constructor
	//-----------------------------------------------------------------------------
	CTextureRenderer::CTextureRenderer( LPUNKNOWN pUnk, HRESULT *phr )
		: CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer),
		NAME("Texture Renderer"), pUnk, phr),
		m_pTexture(NULL)
	{
		// Store and AddRef the texture for our use.
		ASSERT(phr);
		if (phr)
			*phr = S_OK;
	}


	//-----------------------------------------------------------------------------
	// CTextureRenderer destructor
	//-----------------------------------------------------------------------------
	CTextureRenderer::~CTextureRenderer()
	{
		// Do nothing
	}


	//-----------------------------------------------------------------------------
	// CheckMediaType: This method forces the graph to give us an R8G8B8 video
	// type, making our copy to texture memory trivial.
	//-----------------------------------------------------------------------------
	HRESULT CTextureRenderer::CheckMediaType(const CMediaType *pmt)
	{
		return S_OK;
	}

	//-----------------------------------------------------------------------------
	// SetMediaType: Graph connection has been made.
	//-----------------------------------------------------------------------------
	HRESULT CTextureRenderer::SetMediaType(const CMediaType *pmt)
	{
		UINT uintWidth = 2;
		UINT uintHeight = 2;

		VIDEOINFO *pviBmp;                      // Bitmap info header
		pviBmp = (VIDEOINFO *)pmt->Format();
		if (pviBmp)
		{
			m_lVidWidth  = pviBmp->bmiHeader.biWidth;
			m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);

			while( (LONG)uintWidth < m_lVidWidth )
			{
				uintWidth = uintWidth << 1;
			}
			while( (LONG)uintHeight < m_lVidHeight )
			{
				uintHeight = uintHeight << 1;
			}
			if(m_pTexture)
			{
				m_pTexture->Release();
				m_pTexture=NULL;
			}

			CGraphic::GetInst()->CreateEmptyTexture( uintWidth,uintHeight,TF_ARGB32,&m_pTexture );
		}
		return S_OK;
	}


	//-----------------------------------------------------------------------------
	// DoRenderSample: A sample has been delivered. Copy it to the texture.
	//-----------------------------------------------------------------------------
	HRESULT CTextureRenderer::DoRenderSample( IMediaSample * pSample )
	{
		if (!m_pTexture)
			return S_OK;

		unsigned int i, idx;
		int x, y;

		BYTE  *pBmpBuffer;

		CheckPointer(pSample,E_POINTER);

		// Get the video bitmap buffer
		pSample->GetPointer( &pBmpBuffer );
		long cbBuffer = m_lVidWidth*m_lVidWidth*3;

		if (cbBuffer<=0)
		{
			// nothing to do here yet
			return S_FALSE;
		}

		char *pBuffer = new char[cbBuffer];
		if (!pBuffer) 
		{
			// out of memory!
			throw("[DSHOW] Out of memory or empty buffer");
			return S_FALSE;
		}

		BYTE* bmpTmp=pBmpBuffer;

		TEXLOCKED_RECT rect;
		m_pTexture->Lock(0, &rect, NULL, 0,LOCK_DISCARD);

		BYTE* pDest = (BYTE*)rect.pBits;

		bool shouldBeMirrored=m_bHorizontalMirroring;
		if (shouldBeMirrored){
			x=m_lVidWidth-1; y=m_lVidHeight-1;
		}else{
			x=0; y=m_lVidHeight-1;
		}

		// go set all bits...
		for (i=0; i<(unsigned int)(m_lVidWidth*m_lVidHeight*3); i+=3){
			idx=(x*4)+y*rect.Pitch;

			// paint
			BYTE P = pBmpBuffer[i];//b
			P = pBmpBuffer[i+1];//g
			P = pBmpBuffer[i+2];//r
			//pDest[idx+3]=255;//a

			if (shouldBeMirrored){
				x--;
				if (x<0){
					x=m_lVidWidth-1;
					y--; if (y<0) y=0;
				}
			}else{
				x++;
				if (x>=m_lVidWidth){
					x=0;
					y--; if (y<0) y=0;
				}
			}
		}

		m_pTexture->Unlock(0);

		// bye
		delete[] pBuffer;

		return S_OK;
	}

	ITexture* CTextureRenderer::GetMovieTexture()
	{
		return m_pTexture;
	}

	LONG CTextureRenderer::GetMovieWidth()
	{
		return m_lVidWidth;
	}

	LONG CTextureRenderer::GetMovieHeight()
	{
		return m_lVidHeight;
	}
}
