#include "stdafx.h"
#include "CTexture2D.h"
#include "CTexture2DImp.h"
#include "CDataSources.h"

namespace sqr
{
	CTexture2D::CTexture2D( const string& szName, const wstring& szAlias )
		: IResTexture( szName, szAlias )
		, m_pActiveTex(0)
	{
	}

	CTexture2D::~CTexture2D()
	{
	}

	const char* CTexture2D::GetType() const
	{
		return CTexture2D::Type();
	}

	const char* CTexture2D::Type()
	{
		return "t2d";
	}

	int32 CTexture2D::FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask /*= NULL */ )
	{
		return 0;
	}

	int32 CTexture2D::FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes )
	{
		return 0;
	}

	HRESULT CTexture2D::Lock( uint32 level, TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num, uint32 Flags )
	{
		return S_OK;
	}

	HRESULT CTexture2D::Unlock( uint32 level, uint32 Num )
	{
		return S_OK;
	}

	HRESULT CTexture2D::SaveToFile( const string& filename, IMAGE_FILEFORMAT Format /*= IFF_DDS */ )
	{
		return S_OK;
	}

	bool CTexture2D::IsTransparent( float x, float y ) const
	{
		return true;
	}

	bool CTexture2D::FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes )
	{
		return true;
	}

	int CTexture2D::CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat )
	{
		return 0;
	}

	int CTexture2D::LoadPic( uint Width, uint Height,uint MinMap, eTextureFormat eFormat, DWORD Filter, DWORD MipFilter )
	{
		return 0;
	}

	int CTexture2D::LoadPic()
	{
		return 0;
	}

	int CTexture2D::ReLoadPic()
	{
		return 0;
	}

	void* CTexture2D::GetBuffer() const
	{
		return m_pActiveTex->GetBuffer();
	}

	void CTexture2D::ReadDataFromMem( CRefBufFile& pMemData )
	{
		

		m_pActiveTex = CDataSources::GetInst()->NewTex2DImp(this);		
	}

}
