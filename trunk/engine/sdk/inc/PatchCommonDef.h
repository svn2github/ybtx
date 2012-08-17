#pragma once


#include "VersionManager.h"


namespace sqr
{

struct PatchHead
{
	uint32          uFlag;
	ReleaseVersion  uVersion;
	uint32          uNonCompressSize;
	uint32          uFileCount;
	uint8           md5Code[16];
};

struct FileHead
{
	uint32 uFlag;
	uint32 uFileSize;
	uint32 uDiffType;
	uint32 uDiffSize;
	uint32 uPathSize;
	char*  pPath;
	uint8* pData;
};

struct ElementHead
{
	uint32 uFlag;
	uint32 uFileSize;
	uint32 uDiffType;
	uint32 uDiffSize;
	uint32 uPathSize;
};

struct ModifyNode
{
	uint32 uSkip;
	uint32 uSize;
};


// 音频部分
#define  WAVEBANK_BANKNAME_LENGTH  64
#define  MAX_FILE_NAME  256


typedef enum {
	WAVEBANK_SEGIDX_BANKDATA = 0,  // Bank data
	WAVEBANK_SEGIDX_ENTRYMETADATA, // Entry meta-data
	WAVEBANK_SEGIDX_SEEKTABLES,    // Storage for seek tables for the encoded waves.
	WAVEBANK_SEGIDX_ENTRYNAMES,    // Entry friendly names
	WAVEBANK_SEGIDX_ENTRYWAVEDATA, // Entry wave data
	WAVEBANK_SEGIDX_COUNT
} WAVEBANKSEGIDX;


// 寻找文件各部分的偏移和长度
typedef struct {
	uint32 dwOffset; // Region offset, in bytes.
	uint32 dwLength; // Region length, in bytes.
} WAVEBANKREGION;

// 整个.xwb文件头文件
typedef struct {
	uint32   dwSignature;     // File signature
	uint32   dwVersion;       // Version of the tool that created the file
	uint32   dwHeaderVersion; // Version of the file format
	WAVEBANKREGION  Segments[WAVEBANK_SEGIDX_COUNT]; // 构成一个wav文件的各部分
} WAVEBANKHEADER;



// Blank Data 部分数据

typedef union {
	struct {
		uint32  wFormatTag      : 2;  // Format tag
		uint32  nChannels       : 3;  // Channel count (1 - 6)
		uint32  nSamplesPerSec  : 18; // Sampling rate
		uint32  wBlockAlign     : 8;  // Block alignment.  For WMA, lower 6 bits block alignment index, upper 2 bits bytes-per-second index.
		uint32  wBitsPerSample  : 1;  // Bits per sample (8 vs. 16, PCM only); WMAudio2/WMAudio3 (for WMA)
	};
	uint32  dwValue;
} WAVEBANKMINIWAVEFORMAT;


typedef struct {
    uint32 dwLowDateTime;
    uint32 dwHighDateTime;
} sFILETIME;

typedef struct {
	uint32  dwFlags;                               // Bank flags, 0x01000800, 524289
	uint32  dwEntryCount;                          // Number of entries in the bank. ( wav文件数 )
	char    szBankName[WAVEBANK_BANKNAME_LENGTH];  // Bank friendly name, 64
	uint32  dwEntryMetaDataElementSize;            // Size of each entry meta-data element, in bytes
	uint32  dwEntryNameElementSize;                // Size of each entry name element, in bytes
	uint32  dwAlignment;                           // Entry alignment, in bytes
	WAVEBANKMINIWAVEFORMAT  CompactFormat;         // Format data for compact bank
	sFILETIME               BuildTime;             // Build timestamp
} WAVEBANKDATA;



// Meta Data 部分数据

typedef struct {
	uint32  dwStartSample;  // Start sample for the region.
	uint32  dwTotalSamples; // Region length in samples.
} WAVEBANKSAMPLEREGION;


typedef struct {
	union {
		struct {
			uint32  dwFlags  :  4; // Entry flags
			uint32  Duration : 28; // Duration of the wave, in units of one sample.
		};
		uint32 dwFlagsAndDuration;
	};

	WAVEBANKMINIWAVEFORMAT  Format;         // Entry format.
	WAVEBANKREGION          PlayRegion;     // Region within the wave data segment that contains this entry.
	WAVEBANKSAMPLEREGION    LoopRegion;     // Region within the wave data (in samples) that should loop.
} WAVEBANKENTRY;




// 文件名 => 数据 存储结构
typedef struct {
	char*  FileName;
	uint32 ContentLen;
	uint32 uSpaceSize;
	uint8* Content;
	uint8* Space;
} sFileData;


typedef struct {
	uint32 uFlag;
	uint32 uContentOffset;
	uint32 uFileLen;
	uint32 uDiffLen;
	uint32 uSpaceSize;
	uint32 uFileNameLen;
	char   FileName[MAX_FILE_NAME];
	uint8* Space;
	uint8* Data;
} sPatchData;


// Patch WAV Node
typedef struct {
	uint32 uFlag;
	uint32 uContentOffset;
	uint32 uFileSize;
	uint32 uSpaceSize;
	uint32 uFileNumber;
	uint8* Content;
	uint8* Space;
} AudioNode;

}


