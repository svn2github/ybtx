#pragma once
#include "CCoder.h"

namespace sqr
{
	class CRenderStyleCoder 
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		FourCC	GetChunkName();
		uint16	GetNewVer();

		bool	Code( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	DeCode1( const CDataChunk& chunk, OBJ_HANDLE handle );
	};
}