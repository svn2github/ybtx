#pragma once
#include "CCoder.h"

namespace sqr
{
	class CSamplerStageCoder : public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		FourCC		GetChunkName();
		uint16		GetNewVer();

		bool		Code( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool		DeCode_1( const CDataChunk& chunk, OBJ_HANDLE handle );
	};
}