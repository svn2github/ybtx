#pragma once
#include "GraphicBase.h"
#include "CTexFmtMapper.h"

namespace sqr
{
	class CTexture2DImp
	{
	public:
		CTexture2DImp();
		virtual ~CTexture2DImp();

		bool	IsLoaded() const;

		virtual void* 
				GetBuffer() const = 0;

		virtual bool
				CreateTex( uint32 nWidth, uint32 nHeight, uint32 nMip, eTextureFormat fmt ) = 0;
		
		virtual uint32	
				GetRawTexMipNum() = 0;

		virtual bool
				GetLevelData( TexBuffer& buf, uint32 level ) = 0;

		virtual bool
				SetLevelData( const TexBuffer& buf, uint32 level ) = 0;

		// ---- for old tex -----------------------------------------------------------------------------

	protected:
		bool	m_bIsLoaded;
	};

}
