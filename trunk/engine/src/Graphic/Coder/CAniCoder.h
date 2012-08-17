#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CAniFormat : public CFileFormatMgr
	{
		// friend class CAnyObject;
	public:
		CAniFormat(void);
		~CAniFormat(void);
	};

	class CAniCoder 
		: public CCoder
		, public CCoderHelper
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'ANIC'; }	//Coder区块名
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_Ani( const CDataChunk& inData , OBJ_HANDLE handle );
	};
}