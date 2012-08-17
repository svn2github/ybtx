#include "stdafx.h"
#include "CUVCtrlStageCoder.h"
#include "CUVCtrlStage.h"

namespace sqr
{
	BEGIN_VERSION_MAP(CUVCtrlStageCoder)
		ON_VERSION(1, DeCode_1)
	END_VERSION_MAP()

	FourCC CUVCtrlStageCoder::GetChunkName()
	{
		return 'UVCK';
	}

	DWORD CUVCtrlStageCoder::GetVer()
	{
		return 1;
	}

	bool CUVCtrlStageCoder::Code( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CUVCtrlStage* uv = static_cast<CUVCtrlStage*>(handle);

		wdi << uv->m_eStage;
		wdi << uv->m_uvSrc;
		wdi << uv->m_uv;

		return true;
	}

	bool CUVCtrlStageCoder::DeCode_1( const CDataChunk& chunk, OBJ_HANDLE handle )
	{
		CUVCtrlStage* uv = static_cast<CUVCtrlStage*>(handle);
		CBufFile buf( chunk.GetBuffer(), chunk.GetSize() );

		buf >> uv->m_eStage;
		buf >> uv->m_uvSrc;
		buf >> uv->m_uv;

		return true;
	}
}