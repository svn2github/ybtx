#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CModFormat : public CFileFormatMgr
	{
	public:
		CModFormat(void);
		~CModFormat(void);
	};

	class CPieceGroupCoder 
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CPieceGroupCoder();
		FourCC	GetChunkName(void);						// Coder区块名
		uint16	GetNewVer(void)		{ return 1; }		// Coder版本号, 增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData , OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};
}