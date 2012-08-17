#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CGameRegionCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'SRGN'; }	//Coder区块名
		DWORD	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WirteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CGameRegionFormat : public CFileFormatMgr
	{
	public:
		CGameRegionFormat(void);
		~CGameRegionFormat(void);

		void InitRegionCode();
		bool LoadRegionFile(const string& strFile, int nRegionIndex);
		bool SaveRegionFile(const string& strFile, int nRegionIndex);
	};
}