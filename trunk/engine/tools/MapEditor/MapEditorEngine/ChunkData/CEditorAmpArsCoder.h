#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CEditorAmbientCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'EAMB'; }	//Coder区块名editor ambient
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );

		void SaveOperationInfo(WriteDataInf& wdi);
		void SaveToolMainPlayerInfo(WriteDataInf& wdi);
		void SaveWaveInfo(WriteDataInf& wdi);
		void SaveGridModeProVersion(WriteDataInf& wdi);
		
		void LoadOperationInfo(CBufFile& tData);
		void LoadToolMainPlayerInfo(CBufFile& tData);
		void LoadWaveInfo(CBufFile& tData);
		void LoadGridModeProVersion(CBufFile& tData);
	};

	class CEditorTileResoureCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'ETLR'; }	//Coder区块名editor tile resoure
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );

		void SaveResoureceSetInfo(WriteDataInf& wdi);
		void LoadResoureceSetInfo(CBufFile& tData);///地图所用图素包集合
	};

	///只存储区域光名字
	class CEditorRegionLightCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'ERLT'; }	//Coder区块名editor region light区域光信息
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CEditorMapResCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'ERES'; }	//Coder区块名editor resource
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );

	};

	///record info 
	class CEditorRecordCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'ERRD'; }	//Coder区块名editor record
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );

	};

	class CEditorAmpArsFormat : public CFileFormatMgr
	{
	public:
		CEditorAmpArsFormat(void);
		~CEditorAmpArsFormat(void);

		void InitAmpArsCode();
		bool LoadAmpArsFile(const string& strFile);
		bool SaveAmpArsFile(const string& strFile);
	};
}