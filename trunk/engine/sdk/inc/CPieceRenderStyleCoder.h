#pragma once
#include "CCoder.h"

namespace sqr
{
	class CPieceRenderStyleCoder 
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP()
	public:
		CPieceRenderStyleCoder();

		FourCC	GetChunkName(void);
		uint16	GetNewVer(void) { return 1; }
		uint16	GetChunkType()	{ return CDataChunk::DCT_AUTO; }
		
		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );

		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );
	};
}