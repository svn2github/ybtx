#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	enum CODER_RES_TYPE
	{
		RFT_MOD = MAKEFOURCC( 'M', 'O', 'D', 0 ),		//模型配置文件
		//RFT_SKE = MAKEFOURCC( 'S', 'K', 'E', 0 ),		//动画配置文件 RenderObject define
		RFT_EFX = MAKEFOURCC( 'E', 'F', 'X', 0 ),		//特效文件
	};

	class CGameSceneObjectCoder : public CCoder
	{

	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'OBJT'; }	//Coder区块名 Object
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CGameLogicObjectCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'LOGI'; }	//Coder区块名 Object
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CGameObjectFormat : public CFileFormatMgr
	{
	public:
		CGameObjectFormat(void);
		~CGameObjectFormat(void);

		void InitObjectCode();
		bool LoadObjectFile(const string& strFile, int nRegionIndex);
		bool SaveObjectFile(const string& strFile, int nRegionIndex);
	};
}