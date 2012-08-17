#pragma once
#include "CFileFormatMgr.h"
#include "CSkeletal.h"

namespace sqr
{
	class CSkeFormat : public CFileFormatMgr
	{
		// friend class CAnyObject;
	public:
		CSkeFormat(void);
		~CSkeFormat(void);
	};

	class CSkeCoder 
		: public CCoder
		, public CCoderHelper
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'SKEC'; }	//Coder区块名
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_Ske( const CDataChunk& inData , OBJ_HANDLE handle );
	};
}