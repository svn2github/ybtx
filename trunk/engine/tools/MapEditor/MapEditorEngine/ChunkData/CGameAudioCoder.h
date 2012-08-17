#pragma once
#include "CFileFormatMgr.h"

namespace sqr
{
	class CGameAudioCoder : public CCoder
	{
	public:
		DECLARE_VERSION_MAP();
		FourCC	GetChunkName(void)	{ return 'AUID'; }	//Coder区块名 Audio
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CGameAudioFormat : public CFileFormatMgr
	{
	public:
		CGameAudioFormat(void);
		~CGameAudioFormat(void);

		void InitAudioCode();
		bool LoadAudioFile(const string& strFile, int nRegionIndex);
		bool SaveAudioFile(const string& strFile, int nRegionIndex);
	};
}