#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CEditorTerGridCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'EGRI'; }	//Coder区块名 edit grid
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};


	class CEditorTerWaterCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'EWTR'; }	//Coder区块名edit water
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );

		void LoadWaveSideInfo(CBufFile& tData);
		void SaveWaveSideInfo(WriteDataInf& wdi);
	};

	class CEditorTerrainFormat : public CFileFormatMgr
	{
	public:
		CEditorTerrainFormat(void);
		~CEditorTerrainFormat(void);

		void InitTerrainCode();
		bool LoadTerrainFile(const string& strFile, int nRegionIndex);
		bool SaveTerrainFile(const string& strFile, int nRegionIndex);
	};
}