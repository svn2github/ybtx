#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CGameBlcokCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'BLCK'; }	//Coder区块名
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CGameBlcokFormat : public CFileFormatMgr
	{
	public:
		CGameBlcokFormat(void);
		~CGameBlcokFormat(void);

		void InitBlockCode();
		bool LoadBlcokFile(const string& strFile);
		bool SaveBlcokFile(const string& strFile);
	};
}