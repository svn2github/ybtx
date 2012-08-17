#include "stdafx.h"
#include "CDx9Texture2DImp.h"
#include "BaseHelper.h"
#include "CDx9TexFmtMapper.h"
#include "CTexture.h"
#include "CDx9MainWindow.h"

namespace sqr
{
	CDx9Texture2DImp::CDx9Texture2DImp( const ITexture* pParent )
		: m_pParent(pParent)
		, m_pD3DTex(0)
	{
	}

	CDx9Texture2DImp::~CDx9Texture2DImp()
	{
	}

	uint32 CDx9Texture2DImp::GetRawTexMipNum()
	{
		if ( 0 == m_pD3DTex )
			return 0;

		return m_pD3DTex->GetLevelCount();
	}

	bool CDx9Texture2DImp::GetLevelData( TexBuffer& buf, uint32 level )
	{
		if ( !m_pD3DTex )
			return false;

		D3DLOCKED_RECT lockedRect;
		HRESULT hr = m_pD3DTex->LockRect(level, &lockedRect, 0, 0);
		if ( FAILED(hr) )
			return false;

		CopySurfaceToMem(buf, (uint8*)lockedRect.pBits, m_pParent->GetWidth(), m_pParent->GetHeight(), level, m_pParent->GetTexFormat(), lockedRect.Pitch);

		m_pD3DTex->UnlockRect(level);

		return true;
	}

	bool CDx9Texture2DImp::SetLevelData( const TexBuffer& buf, uint32 level )
	{
		if ( !m_pD3DTex )
			return false;

		D3DLOCKED_RECT lockedRect;
		HRESULT hr = m_pD3DTex->LockRect(level, &lockedRect, 0, 0);
		if ( FAILED(hr) )
			return false;

		CopyMemToSurface((uint8*)lockedRect.pBits, buf, m_pParent->GetWidth(), m_pParent->GetHeight(), level, m_pParent->GetTexFormat(), lockedRect.Pitch);

		m_pD3DTex->UnlockRect(level);
		m_pD3DTex->SetLOD(level);

		return true;
	}

	void* CDx9Texture2DImp::GetBuffer() const
	{
		return (void*)m_pD3DTex;
	}

	bool CDx9Texture2DImp::CreateTex( uint32 nWidth, uint32 nHeight, uint32 nMip, eTextureFormat fmt )
	{
		HRESULT hr = GetD3DDevice()->CreateTexture(nWidth, nHeight,  nMip, 0, 
			CDx9TexFmtMapper::GetInst()->GetDx9Format(fmt), 
			D3DPOOL_MANAGED, (LPDIRECT3DTEXTURE9*)(&m_pD3DTex), NULL);
		
		return hr == S_OK;
	}

	bool CDx9Texture2DImp::IsValid() const
	{
		return m_pD3DTex != 0;
	}
}