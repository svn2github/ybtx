#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CEditorRegionCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'EDRG'; }	//Coder区块名 edit rengion
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CEditorRegionFormat : public CFileFormatMgr
	{
	public:
		CEditorRegionFormat(void);
		~CEditorRegionFormat(void);

		void InitRegionCode();
		bool LoadRegionFile(const string& strFile);
		bool SaveRegionFile(const string& strFile);
	};
}