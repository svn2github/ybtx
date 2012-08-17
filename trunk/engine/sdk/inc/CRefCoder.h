#pragma once
#include "CCoder.h"

namespace sqr
{
	class CRefCoder : public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		uint16	GetChunkType()	{ return CDataChunk::DCT_DEPEND; }
		uint16	GetNewVer()		{ return 1; }
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		virtual bool _DecodeImpl( const string& refName, PRIVATE_HANDLE handle ) = 0;
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};
}