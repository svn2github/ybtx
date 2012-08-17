#include "stdafx.h"
#include "CDx9TextureCoder.h"
#include "BaseHelper.h"
#include "CDx9MainWindow.h"
#include "DebugHelper.h"

CDx9TexCoder::CDx9TexCoder(CDx9Texture* pText, CRefBufFile* pBufFile)
: m_pTexture(pText)
, m_pBufFile(pBufFile)
{
	Ast(m_pTexture&&"CDx9TexCoder初始化指针不能为空");
	Ast(m_pBufFile&&"CDx9TexCoder BufFile初始化指针不能为空");
	m_pBufFile->AddRef();

	m_pTexture->m_pActiveCoder = this;
}

CDx9TexCoder::~CDx9TexCoder()
{
	m_pBufFile->Release();
}

CDx9DefaultTexCoder::CDx9DefaultTexCoder(CDx9Texture* pText, CRefBufFile* pBufFile,UINT width, UINT height, UINT mipmap )
: CDx9TexCoder( pText,pBufFile )
, m_uWidth(width)
, m_uHeight(height)
, m_uMipMap(mipmap)
{
	m_pTexture->m_IsLoaded	= true;
	AddRef();
}

bool CDx9DefaultTexCoder::AutoDeCode(void)
{
	if(  m_pBufFile && NULL == m_pTexture->m_pTextures[0] )
	{
		HRESULT hr= D3DXCreateTextureFromFileInMemoryEx(GetD3DDevice(), m_pBufFile->GetBuf(), m_pBufFile->GetBufSize(),
			m_uWidth , m_uHeight, m_uMipMap, 0, D3DFMT_UNKNOWN , D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, NULL, NULL, (LPDIRECT3DTEXTURE9*)&m_pTexture->m_pTextures[0]);
		if ( FAILED( hr ) )
		{
			string ErrStr = m_pTexture->GetName();
			GraphicErrType ErrType = TextureCreate_Err;
			switch ( hr )
			{
			case D3DERR_NOTAVAILABLE:
				ErrStr += ": D3DERR_NOTAVAILABLE";
				break;
			case D3DERR_OUTOFVIDEOMEMORY:
				ErrStr += ": D3DERR_OUTOFVIDEOMEMORY";
				ErrType = Memory_Err;
				break;
			case D3DERR_INVALIDCALL:
				ErrStr += ": D3DERR_INVALIDCALL";
				break;
			case D3DXERR_INVALIDDATA:
				ErrStr += ": D3DXERR_INVALIDDATA";
				break;
			case E_OUTOFMEMORY:
				ErrStr += ": E_OUTOFMEMORY";
				ErrType = Memory_Err;
				break;
			default:
				ErrStr += ": D3DXCreateTextureFromFileInMemoryEx failed with unknown error";
			}
			CError exp(GraphicErr::GetErrTypeStr(ErrType));
			exp.AppendMsg(ErrStr);
			GfkLogPkgErr(exp, m_pTexture->GetName(), *m_pBufFile);

			return true;
		}

		SafeRelease(m_pTexture->m_pActiveCoder);
		return true;
	}
	SafeRelease(m_pTexture->m_pActiveCoder);
	return true;
}