#pragma once
#include "CTexture.h"

namespace sqr
{
	class CTexture2DImp;


	class CTexture2D 
		: public IResTexture
	{
	public:
		CTexture2D( const string& szName, const wstring& szAlias );
		~CTexture2D();

		virtual const char*
				GetType() const;

		static const char*
				Type();

		int32	FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask = NULL );

		int32	FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes );

		HRESULT	Lock( uint32 level, TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num, uint32 Flags );

		HRESULT	Unlock( uint32 level,  uint32 Num );

		HRESULT	SaveToFile( const string& filename, IMAGE_FILEFORMAT Format = IFF_DDS );

		bool	IsTransparent( float x, float y ) const;

		bool	FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes );

		int		CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat );

		int		LoadPic( uint Width, uint Height,uint MinMap, eTextureFormat eFormat, DWORD Filter, DWORD MipFilter );

		int		LoadPic();

		int		ReLoadPic();

		void	ReadDataFromMem( CRefBufFile& pMemData );

		void*	GetBuffer() const;

	protected:
		CTexture2DImp*	m_pActiveTex;
	};

}
