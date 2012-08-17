#pragma once
#include "GraphicBase.h"
#include "CGraphicMallocObject.h"

namespace sqr
{
	class IResTexture;
	class CRefBufFile;

	class CTextureImp 
		: public CGraphicMallocObject
	{
	public:
		CTextureImp( IResTexture* pParent );
		virtual ~CTextureImp();

		virtual const char*
				GetType() const = 0;

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
				GetWidth() const;

		virtual uint32			
				GetHeight() const;

		virtual uint32
				GetLevelNum() const;

		virtual bool		
				FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes );

		virtual int32			
				FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask = NULL ) = 0;

		virtual void
				UpdateAnim( float fRadio );

		virtual bool	
				IsValid();

		virtual int			
				CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat ) = 0;

		virtual void			
				SetCustomParams( uint Width, uint Height,uint MinMap, eTextureFormat eFormat, uint32 Filter, uint32 MipFilter );

		virtual int			
				ReLoadPic();

		// ---- for old tex -----------------------------------------------------------------------------

		virtual uint32		
				FrameCount();

		virtual void
				ReadDataFromMem( CRefBufFile& pMemData ) = 0;

		virtual void
				AutoGenMip() = 0;

		virtual	void _Reclaim(){};

		virtual HDC	GetHDC(void){ return 0; }

		virtual void
				SetLod(uint32 lod) = 0;

	protected:
		eTextureFormat	m_eFormat;
		uint32			m_uWidth;
		uint32			m_uHeight;
		uint32			m_uMipMap;
		uint32			m_uOrgWidth;
		uint32			m_uOrgHeight;
		uint32			m_uOrgMipMap;
		
		IResTexture*	m_pParent;
	};
}