#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CRsFormat : public CFileFormatMgr
	{
	public:
		CRsFormat();
		~CRsFormat();
	};

	class CRsCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'RSCK'; }
		uint16	GetNewVer(void)		{ return 3; }

		bool	Code( WriteDataInf& wdi , OBJ_HANDLE handle );
		bool	DeCode_old_10007( const CDataChunk& chunk, OBJ_HANDLE handle );
		bool	DeCode_old_10008( const CDataChunk& chunk, OBJ_HANDLE handle );
	};

}
