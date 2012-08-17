#pragma once
#include "CTexture.h"
#include "CEditInfo.h"
#include "CEditTexFormat.h"

namespace sqr_tools
{
	class CEditTexture 
		: public IResTexture
		, public CEditInfo
	{
	public:
		static CEditTexFormat Format;

		CEditTexture( const string& szName , IResTexture* pTexture );
		~CEditTexture();

		uint32			FrameCount();
		eTextureFormat	GetTexFormat() const;
		uint32			GetWidth(void) const;
		uint32			GetHeight(void) const;
		void*			GetBuffer() const;
		bool			IsTransparent(float x,float y) const;
		int				FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask = NULL );
		int				FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes );
		bool			FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes );
		HRESULT			Lock( uint32 level, _TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num = 0, uint32 Flags = 0 );
		HRESULT			Unlock( uint32 level,  uint32 Num = 0 );
		HRESULT			SaveToFile(const string& filename, IMAGE_FILEFORMAT Format = IFF_DDS );

		int				ReLoadPic( void );						//ÔØÈëÎÄ¼þ
		int				CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat, EmptyTexType type = ETT_OLD );
		int				LoadPic( uint Width, uint Height,uint MinMap, eTextureFormat eFormat , DWORD Filter,	DWORD MipFilter );
		int				LoadPic( void );
		bool			IsValid(void);

		void			AutoGenMip();


		int32		AddRef()			{ return IResTexture::AddRef(); }
		const char*	GetName() const		{ return m_pTexture->GetName(); }

		void		OnRead( CRefBufFile& GraphicFile ) { m_pTexture->OnRead(GraphicFile); }
	protected:
		sqr::IResTexture*	m_pTexture;
	};
}