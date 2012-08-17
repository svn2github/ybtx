#pragma once
#include "CTexture2DImp.h"
#include "Dx9Base.h"


namespace sqr
{
	class ITexture;

	class CDx9Texture2DImp
		: public CTexture2DImp
	{
	public:
		CDx9Texture2DImp( const ITexture* pParent );
		~CDx9Texture2DImp();

		void*	GetBuffer() const;
		bool	CreateTex( uint32 nWidth, uint32 nHeight, uint32 nMip, eTextureFormat fmt );
		bool	IsValid() const;
		uint32	GetRawTexMipNum();
		bool	GetLevelData( TexBuffer& buf, uint32 level );
		bool	SetLevelData( const TexBuffer& buf, uint32 level );

		const ITexture*		m_pParent;
		IDirect3DTexture9*	m_pD3DTex;
	};
}