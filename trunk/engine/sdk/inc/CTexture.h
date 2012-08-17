#pragma once
#include "GraphicRes.h"
#include "GraphicBase.h"
#include "CResourceContainer.h"


namespace sqr
{
	class CTextureImp;

	class ITexture
		: public GraphicRes
		, public CGraphicMallocObject
	{
		friend class CJPGDecoder;
	public:
		ITexture();
		virtual ~ITexture();

		virtual const char*	
				GetName() const;

		virtual const char*
				GetType() const;

		virtual int32			
				FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes ) = 0;
		
		virtual HRESULT		
				Lock( uint32 level, TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num = 0, uint32 Flags = 0 ) = 0;
		
		virtual HRESULT		
				Unlock( uint32 level,  uint32 Num = 0 ) = 0;

		virtual HRESULT		
				SaveToFile( const string& filename, IMAGE_FILEFORMAT Format = IFF_DDS ) = 0;

		virtual bool	
				IsTransparent( float x, float y ) const = 0;

		virtual void*
				GetBuffer() const = 0;

		virtual eTextureFormat	
				GetTexFormat() const;

		virtual uint32			
				GetWidth() const = 0;

		virtual uint32			
				GetHeight() const = 0;

		virtual uint32
				GetLevelNum() const;

		virtual bool		
				FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes );

		virtual int32			
				FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask = NULL ) = 0;

		virtual void
				UpdateAnim( float fRadio );

		virtual void
				AutoGenMip();

		// ---- for old tex -----------------------------------------------------------------------------

		virtual uint32		
				FrameCount();

		void	AddTexRef();

		void	ReleaseTexRef();

		virtual HDC	GetHDC(void);

	private:
		virtual void
				SetLod(uint32 lod);

	protected:
		int32	m_nTexRef;
	};

	class IResTexture
		: public ITexture
		, public CResUnit
	{
	public:
		enum EmptyTexType
		{
			ETT_OLD,
			ETT_2D,
			ETT_3D,
			ETT_CUBE,
		};

	public:
		IResTexture( const string& szName, const wstring& szAlias );
		virtual ~IResTexture();

		const char*
				GetType() const;

		int32	FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes );

		HRESULT	Lock( uint32 level, TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num = 0, uint32 Flags = 0 );

		HRESULT	Unlock( uint32 level,  uint32 Num = 0 );

		HRESULT	SaveToFile( const string& filename, IMAGE_FILEFORMAT Format = IFF_DDS );

		bool	IsTransparent( float x, float y ) const;

		void*	GetBuffer() const;

		eTextureFormat	
				GetTexFormat() const;

		uint32	GetWidth() const;

		uint32	GetHeight() const;

		uint32	GetLevelNum() const;

		bool	FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes );

		int32	FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask = NULL );

		void	UpdateAnim( float fRadio );

		virtual int		
				CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat, EmptyTexType type = ETT_OLD );
		
		virtual int		
				LoadPic( uint Width, uint Height,uint MinMap, eTextureFormat eFormat , DWORD Filter, DWORD MipFilter );
		
		virtual int		
				LoadPic();

		virtual int		
				ReLoadPic();

		virtual void
				AutoGenMip();

		void	OnRead( CRefBufFile& GraphicFile );
		
		int32	AddRef();
		
		const char*	
				GetName() const;

		bool	IsValid();

		void	Free();

		void	SetBufSize( uint32 uSize );

		// ---- for old tex -----------------------------------------------------------------------------

		virtual uint32	FrameCount();

		HDC		GetHDC(void);

	private:
		virtual void
				SetLod(uint32 lod);

	protected:
		void			_Reclaim();
		CTextureImp*	m_pImp;
		bool			m_bCustom;

		uint			m_uCustomWidth;
		uint			m_uCustomHeight;
		uint			m_uCustomMinMap;
		eTextureFormat  m_eCustomFormat;
		uint32			m_uCustomFilter;
		DWORD			m_uCustomMipFilter;
	};

}
