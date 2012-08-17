#pragma once
#include "CTextureImp.h"


namespace sqr
{
	class IResTexture;

	class CGridAnimTex
		: public CTextureImp
	{
	public:
		CGridAnimTex( IResTexture* pParent );
		~CGridAnimTex();
		
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

		void	UpdateAnim( float fRadio );
		
		virtual void
				AutoGenMip();

		virtual void
				SetLod(uint32 lod);

	private:
		IResTexture* m_pTex;
	};

}
