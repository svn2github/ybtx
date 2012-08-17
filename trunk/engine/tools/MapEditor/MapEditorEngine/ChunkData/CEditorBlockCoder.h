#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CEditorBlockCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'LBTI'; }	//Coder区块名 Low Block Texture Info
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CEditorBlcokFormat : public CFileFormatMgr
	{
	public:
		CEditorBlcokFormat(void);
		~CEditorBlcokFormat(void);

		void InitBlockCode();
		bool LoadBlockFile(const string& strFile);
		bool SaveBlockFile(const string& strFile);
	};
}