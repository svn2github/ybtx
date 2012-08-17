#pragma once
#include "CFileFormatMgr.h"
#include "CAnimation.h"
namespace sqr
{
	class CAgpFormat : public CFileFormatMgr
	{
	public:
		CAgpFormat(void);
		~CAgpFormat(void);
	};

	class CAgpCoder 
		: public CCoder
		, public CCoderHelper
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'AGPC'; }	//Coder区块名
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_Agp( const CDataChunk& inData , OBJ_HANDLE handle );
	};
}