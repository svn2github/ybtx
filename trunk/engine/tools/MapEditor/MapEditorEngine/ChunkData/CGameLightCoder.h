#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CGamePointLightCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'SPLT'; }	//Coder区块名scene point light
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CGameSpotLightCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'SSLT'; }	//Coder区块名scene spot light
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CGameLightFormat : public CFileFormatMgr
	{
	public:
		CGameLightFormat(void);
		~CGameLightFormat(void);

		void InitLightCode();
		bool LoadLightFile(const string& strFile, int nRegionIndex);
		bool SaveLightFile(const string& strFile, int nRegionIndex);
	};
}