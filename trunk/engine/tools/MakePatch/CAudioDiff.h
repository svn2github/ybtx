#pragma once


#include <string>
#include <map>

using namespace std;

#include "IDiff.h"
#include "PatchCommonDef.h"

#include "BaseTypes.h"

using namespace sqr;


#define  WAVEBANK_BANKNAME_LENGTH  64

struct WaveInfo
{
	WAVEBANKHEADER Head;
	WAVEBANKDATA   BankData;
	WAVEBANKENTRY* MetaDataList;
};


class CAudioDiff
	: public IDiff
{
public:
	CAudioDiff();
	~CAudioDiff();

	virtual bool MakeDiff(const char* szNewFile, const char* szOldFile);
	virtual uint32 GetDiffType() const;
	virtual uint32 GetAllSize() const;
	virtual void WriteDiffData(void* fp);

private:
	typedef  map<uint32, AudioNode*>  FileDataMap_t;
	void LoadAudioData(const char* FileName, WaveInfo* info);
	void ReadInfo(const char* szNewFile, const char* szOldFile);
	void ReadData(FILE* fp, uint32 idx, uint32 uFileLen, WaveInfo* info);
	void Clear();

	WaveInfo       m_SrcInfo;
	FileDataMap_t  m_mapSrcWaveData;

	WaveInfo       m_DstInfo;
	AudioNode*     m_DstWaveData;

	int     m_Offset;
	int     m_TotalNameLen;
	uint8*  m_InfoData; // 音频数据之前部分
	string  m_strVersion;
	uint32  m_uAllSize;
	uint32  m_DataOffset;
};


