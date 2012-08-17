#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CGameAmbientCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'AMBT'; }	//Coder区块名ambient
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );

		void SaveAmbientFXInfo(WriteDataInf& wdi);
		void LoadAmbientFXInfo(CBufFile& tData);
	};

	///只存储区域光信息
	class CGameRegionLightCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'RGLT'; }	//Coder区块名region light区域光信息
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CGameMapResCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'MRES'; }	//Coder区块名map resource
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );

		//------------------------------------------------------------------------------
		void MakeAtlasTexture();
	};

	class CGameAmpArsFormat : public CFileFormatMgr
	{
	public:
		CGameAmpArsFormat(void);
		~CGameAmpArsFormat(void);

		void InitAmpArsCode();
		bool LoadAmpArsFile(const string& strFile);
		bool SaveAmpArsFile(const string& strFile);
	};
}