#pragma once
#include "CTexture.h"


namespace sqr
{
	class CTextureCube
		: public IResTexture
	{
	public:
		CTextureCube( const string& szName, const wstring& szAlias );
		~CTextureCube();
		
		const char*
				GetType() const;

		static const char*
				Type();

		int32	FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes );

		HRESULT	Lock( uint32 level, TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num = 0, uint32 Flags = 0 );

		HRESULT	Unlock( uint32 level,  uint32 Num = 0 );

		HRESULT	SaveToFile( const string& filename, IMAGE_FILEFORMAT Format = IFF_DDS );

		bool	IsTransparent( float x, float y ) const;

		void*	GetBuffer() const;

		int		CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat );

		int		LoadPic( uint Width, uint Height,uint MinMap, eTextureFormat eFormat , DWORD Filter, DWORD MipFilter );

		int		LoadPic();

		int		ReLoadPic();

	private:
				
	};

}
