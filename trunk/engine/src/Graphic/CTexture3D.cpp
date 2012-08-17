#include "stdafx.h"
#include "CTexture3D.h"


namespace sqr
{
	CTexture3D::CTexture3D( const string& szName, const wstring& szAlias )
		: IResTexture(szName, szAlias)
	{

	}

	CTexture3D::~CTexture3D()
	{

	}

	const char* CTexture3D::GetType() const
	{
		return CTexture3D::Type();
	}

	const char* CTexture3D::Type()
	{
		return "t3d";
	}

	int32 CTexture3D::FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes )
	{
		return 0;
	}

	HRESULT CTexture3D::Lock( uint32 level, TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num /*= 0*/, uint32 Flags /*= 0 */ )
	{
		return S_OK;
	}

	HRESULT CTexture3D::Unlock( uint32 level, uint32 Num /*= 0 */ )
	{
		return S_OK;
	}

	HRESULT CTexture3D::SaveToFile( const string& filename, IMAGE_FILEFORMAT Format /*= IFF_DDS */ )
	{
		return S_OK;
	}

	bool CTexture3D::IsTransparent( float x, float y ) const
	{
		return false;
	}

	void* CTexture3D::GetBuffer() const
	{
		return 0;
	}

	int CTexture3D::CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat )
	{
		return 0;
	}

	int CTexture3D::LoadPic( uint Width, uint Height,uint MinMap, eTextureFormat eFormat , DWORD Filter, DWORD MipFilter )
	{
		return 0;
	}

	int CTexture3D::LoadPic()
	{
		return 0;
	}

	int CTexture3D::ReLoadPic()
	{
		return 0;
	}

}
