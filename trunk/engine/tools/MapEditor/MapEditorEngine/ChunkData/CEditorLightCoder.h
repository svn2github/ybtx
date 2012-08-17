#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CEditorPointLightCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'EPLI'; }	//Coder区块名Edit point light
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CEditorSpotLightCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'ESLT'; }	//Coder区块名Edit spot light
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CEditorLightFormat : public CFileFormatMgr
	{
	public:
		CEditorLightFormat(void);
		~CEditorLightFormat(void);

		void InitLightCode();
		bool LoadLightFile(const string& strFile, int nRegionIndex);
		bool SaveLightFile(const string& strFile, int nRegionIndex);
	};
}