#include "stdafx.h"
#include "CTextureStageCoder.h"
#include "CTextureStage.h"

namespace sqr
{
	BEGIN_VERSION_MAP(CTextureStageCoder)
		ON_VERSION(1, DeCode_1)
	END_VERSION_MAP()

	FourCC CTextureStageCoder::GetChunkName()
	{
		return 'TSCK';
	}

	uint16 CTextureStageCoder::GetNewVer()
	{
		return 1;
	}

	bool CTextureStageCoder::Code( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CTextureStage* ts = static_cast<CTextureStage*>(handle);

		wdi << ts->m_eStage;
		wdi << ts->m_nTexIdx;

		return true;
	}

	bool CTextureStageCoder::DeCode_1( const CDataChunk& chunk, OBJ_HANDLE handle )
	{
		CTextureStage* ts = static_cast<CTextureStage*>(handle);
		CBufFile buf( chunk.GetBuffer(), chunk.GetSize() );

		buf >> ts->m_eStage;
		buf >> ts->m_nTexIdx;
			
		return true;
	}
}