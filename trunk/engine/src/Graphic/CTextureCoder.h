#pragma once
#include "CFileFormatMgr.h"
#include "CBufFile.h"
#include "SyncSystem.h"

namespace sqr
{
	class ITexture;


	class CTextureCoder
		: public SyncCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CTextureCoder( ITexture *pTexture, CRefBufFile* pBufFile );
		virtual ~CTextureCoder();

		bool	AutoDeCode(void);
		FourCC	GetChunkName(void);
		uint16	GetNewVer(void)		{ return 1; }
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData , OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );

	protected:
		ITexture*		m_pTexture;
		CRefBufFile*	m_pBufFile;
	};

}
