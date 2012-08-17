#include "stdafx.h"
#include "CGridAnimTex.h"
#include "CTexture.h"
#include "BaseHelper.h"

namespace sqr
{
	CGridAnimTex::CGridAnimTex( IResTexture* pParent )
		: CTextureImp(pParent)
		, m_pTex(0)
	{
	}

	CGridAnimTex::~CGridAnimTex()
	{
		SafeDelete(m_pTex);
	}

	const char* CGridAnimTex::GetType() const
	{
		return CGridAnimTex::Type();
	}

	const char* CGridAnimTex::Type()
	{
		return "txga";
	}

	void CGridAnimTex::UpdateAnim( float fRadio )
	{
	}

	int32 CGridAnimTex::FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes )
	{
		return m_pTex ? m_pTex->FillFromMemory(pMemory, Format, nWidth, rtSrc, rtDes) : 0;
	}

	HRESULT CGridAnimTex::Lock( uint32 level, TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num /*= 0*/, uint32 Flags /*= 0 */ )
	{
		return m_pTex ? m_pTex->Lock(level, pTexLocked, Rect, Num, Flags) : E_FAIL;
	}

	HRESULT CGridAnimTex::Unlock( uint32 level, uint32 Num /*= 0 */ )
	{
		return m_pTex ? m_pTex->Unlock(level, Num) : E_FAIL;
	}

	HRESULT CGridAnimTex::SaveToFile( const string& filename, IMAGE_FILEFORMAT Format /*= IFF_DDS */ )
	{
		return m_pTex ? m_pTex->SaveToFile(filename, Format) : E_FAIL;
	}

	bool CGridAnimTex::IsTransparent( float x, float y ) const
	{
		return m_pTex ? m_pTex->IsTransparent(x, y) : false;
	}

	void* CGridAnimTex::GetBuffer() const
	{
		return m_pTex ? m_pTex->GetBuffer() : 0;
	}

	void CGridAnimTex::AutoGenMip()
	{
		if ( m_pTex )
			m_pTex->AutoGenMip();
	}

	void CGridAnimTex::SetLod( uint32 lod )
	{
	}
}
