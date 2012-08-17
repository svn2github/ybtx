#pragma once
#include "CCoder.h"

namespace sqr
{
	class CUVCtrlStageCoder : public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		FourCC	GetChunkName();
		DWORD	GetVer();

		bool	Code( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	DeCode_1( const CDataChunk& chunk, OBJ_HANDLE handle );
	};
}