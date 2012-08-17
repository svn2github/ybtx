#include "stdafx.h"
#include "CTextureCube.h"


namespace sqr
{
	CTextureCube::CTextureCube( const string& szName, const wstring& szAlias )
		: IResTexture(szName, szAlias)
	{

	}

	CTextureCube::~CTextureCube()
	{

	}

	const char* CTextureCube::GetType() const
	{
		return CTextureCube::Type();
	}

	const char* CTextureCube::Type()
	{
		return "tcube";
	}

	int32 CTextureCube::FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes )
	{
		return 0;
	}

	HRESULT CTextureCube::Lock( uint32 level, TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num /*= 0*/, uint32 Flags /*= 0 */ )
	{
		return S_OK;
	}

	HRESULT CTextureCube::Unlock( uint32 level, uint32 Num /*= 0 */ )
	{
		return S_OK;
	}

	HRESULT CTextureCube::SaveToFile( const string& filename, IMAGE_FILEFORMAT Format /*= IFF_DDS */ )
	{
		return S_OK;
	}

	bool CTextureCube::IsTransparent( float x, float y ) const
	{
		return false;
	}

	void* CTextureCube::GetBuffer() const
	{
		return 0;
	}

	int CTextureCube::CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat )
	{
		return 0;
	}

	int CTextureCube::LoadPic( uint Width, uint Height,uint MinMap, eTextureFormat eFormat , DWORD Filter, DWORD MipFilter )
	{
		return 0;
	}

	int CTextureCube::ReLoadPic()
	{
		return 0;
	}

}
