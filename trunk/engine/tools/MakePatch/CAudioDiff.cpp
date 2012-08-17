#include "stdafx.h"
#include "AssertTreat.h"
#include "FileProcess.h"
#include "StringProcess.h"
#include "GenerateHash.h"
#include "CAudioDiff.h"

CAudioDiff::CAudioDiff()
	: m_DstWaveData(NULL)
	, m_Offset(0)
	, m_TotalNameLen(0)
	, m_uAllSize(0)
	, m_DataOffset(0)
	, m_InfoData(NULL)
{
}

CAudioDiff::~CAudioDiff()
{
}

// 返回值为写入文件大小，max(uSrcSize, uDstSize)；如果需要非异或写入，uSrcSize传0即可
static uint32 WriteXorContent(FILE* fp, uint32 uSrcSize, uint8* szSrc, uint32 uDstSize, uint8* szDst)
{
	uint32 i = 0;
	uint32 k = 0;
	uint8 szXorBuf[MAX_BUFFER];
	uint8* pSrcData = szSrc;
	uint8* pDstData = szDst;
	for ( i=0; i<uSrcSize && i<uDstSize; ++i )
	{
		szXorBuf[k] = *pDstData ^ *pSrcData;
		++k;
		++pDstData;
		++pSrcData;

		if ( k >= MAX_BUFFER )
		{
			fwrite(szXorBuf, sizeof(uint8), MAX_BUFFER, fp);
			k = 0;
		}
	}

	// 写入已经读取的数据
	if ( k > 0 )
	{
		fwrite(szXorBuf, sizeof(uint8), k, fp);
		k = 0;
	}

	// 如果源文件与目标文件同样大小至此结束
	if ( uSrcSize == uDstSize )
		return uSrcSize;

	uint32 uLeaveSize = abs(uSrcSize - uDstSize);
	if ( uSrcSize > uDstSize )
	{
		fwrite(pSrcData, sizeof(uint8), uLeaveSize, fp);
		return uSrcSize;
	}
	else
	{
		fwrite(pDstData, sizeof(uint8), uLeaveSize, fp);
		return uDstSize;
	}
}

// 载入音频数据
void CAudioDiff::LoadAudioData(const char* FileName, WaveInfo* info)
{
	FILE* fp;
	fp = fopen(FileName, "rb");
	AstFile(fp, FileName);
	int FileLen = GetFileLength(fp);

	fread(&info->Head, sizeof(WAVEBANKHEADER), 1, fp);
	fseek(fp, info->Head.Segments[WAVEBANK_SEGIDX_BANKDATA].dwOffset, SEEK_SET);

	fread(&info->BankData, sizeof(WAVEBANKDATA), 1, fp);
	fseek(fp, info->Head.Segments[WAVEBANK_SEGIDX_ENTRYMETADATA].dwOffset, SEEK_SET);

	int uFileCount = info->BankData.dwEntryCount;
	info->MetaDataList = new WAVEBANKENTRY[uFileCount];

	int i = 0;
	for (i=0; i<uFileCount; ++i)
		fread(&info->MetaDataList[i], sizeof(WAVEBANKENTRY), 1, fp);

	fclose(fp);
}

// 读取文件数据内容
void CAudioDiff::ReadData(FILE* fp, uint32 idx, uint32 uFileLen, WaveInfo* info)
{
	uint32 uHash = 0;
	// 跳到数据区
	uint32 uSrcDataOffset = info->Head.Segments[WAVEBANK_SEGIDX_ENTRYWAVEDATA].dwOffset;
	uint32 uSrcFileCount = info->BankData.dwEntryCount;

	fseek(fp, uSrcDataOffset + info->MetaDataList[idx].PlayRegion.dwOffset, SEEK_SET);
	uint32 uFileSize = info->MetaDataList[idx].PlayRegion.dwLength;

	// 读取文件内容
	uint8* szBuf = new uint8[uFileSize];
	fread(szBuf, sizeof(uint8), uFileSize, fp);

	uHash = HashCode(szBuf, uFileSize);
	AudioNode* pData = new AudioNode;
	memset(pData, 0, sizeof(AudioNode));
	pair<FileDataMap_t::iterator, bool> it = m_mapSrcWaveData.insert(make_pair(uHash, pData));

	it.first->second->uFileNumber = idx;
	it.first->second->uFileSize = uFileSize;
	it.first->second->Content = szBuf;

	// 读取文件间隙内容
	if (idx < uSrcFileCount - 1)
		it.first->second->uSpaceSize = info->MetaDataList[idx+1].PlayRegion.dwOffset -
			( info->MetaDataList[idx].PlayRegion.dwOffset + info->MetaDataList[idx].PlayRegion.dwLength );
	else
		it.first->second->uSpaceSize = uFileLen - uSrcDataOffset -
			(info->MetaDataList[idx].PlayRegion.dwOffset + info->MetaDataList[idx].PlayRegion.dwLength);

	it.first->second->Space = new uint8[it.first->second->uSpaceSize];
	fread(it.first->second->Space, sizeof(uint8), it.first->second->uSpaceSize, fp);
}

// 清除diff后即可释放的内存
void CAudioDiff::Clear()
{
	FileDataMap_t::iterator iter = m_mapSrcWaveData.begin();
	for (; iter != m_mapSrcWaveData.end(); ++iter)
		delete[] iter->second->Content;

	m_mapSrcWaveData.clear();

	delete[] m_SrcInfo.MetaDataList;
	delete[] m_DstInfo.MetaDataList;
}

bool CAudioDiff::MakeDiff(const char* szNewFile, const char* szOldFile)
{
	bool bNeedPatch = false;
	uint32 i = 0;

	// 读取结构信息
	ReadInfo(szNewFile, szOldFile);

	// 打开源文件、目标文件和临时缓存文件
	FILE* ofp = fopen(szOldFile, "rb");
	AstFile(ofp, szOldFile);
	uint32 uSrcFileLen = GetFileLength(ofp);

	FILE* nfp = fopen(szNewFile, "rb");
	AstFile(nfp, szNewFile);
	uint32 uDstFileLen = GetFileLength(nfp);

	FILE* tfp = fopen(AUDIO_TEMP_FILE, "wb");
	AstFile(tfp, AUDIO_TEMP_FILE);

	// 读取数据块
	uint32 uSrcDataOffset = m_SrcInfo.Head.Segments[WAVEBANK_SEGIDX_ENTRYWAVEDATA].dwOffset;
	uint8* SrcInfoData = new uint8[uSrcDataOffset];
	fread(SrcInfoData, sizeof(uint8), uSrcDataOffset, ofp);

	if ( uSrcDataOffset != m_DstInfo.Head.Segments[WAVEBANK_SEGIDX_ENTRYWAVEDATA].dwOffset )
	{
		bNeedPatch = true;
	}
	else
	{
		for (i=0; i<uSrcDataOffset; ++i)
		{
			if (m_InfoData[i] != SrcInfoData[i])
			{
				bNeedPatch = true;
				break;
			}
		}
	}

	delete[] SrcInfoData;

	// 读取源文件数据内容
	uint32 uSrcFileCount = m_SrcInfo.BankData.dwEntryCount;
	for (i=0; i<uSrcFileCount; ++i)
		ReadData(ofp, i, uSrcFileLen, &m_SrcInfo);

	// 读取目标文件数据内容
	uint32 uDstFileCount = m_DstInfo.BankData.dwEntryCount;
	uint32 uDstDataOffset = m_DstInfo.Head.Segments[WAVEBANK_SEGIDX_ENTRYWAVEDATA].dwOffset;
	fseek(nfp, uDstDataOffset, SEEK_SET);
	uint32 uSpaceSize = 0;

	m_DstWaveData = new AudioNode[uDstFileCount];
	uint32 uContentOffset = uDstDataOffset;
	m_uAllSize = uDstDataOffset + (sizeof(AudioNode) - 8) * uDstFileCount;

	uint32 uHash = 0;
	uint32 uFileSize = 0;
	for (i=0; i<uDstFileCount; ++i)
	{
		fseek(nfp, uDstDataOffset + m_DstInfo.MetaDataList[i].PlayRegion.dwOffset, SEEK_SET);
		// 获得文件大小
		uFileSize = m_DstInfo.MetaDataList[i].PlayRegion.dwLength;
		uint8* szBuf = new uint8[uFileSize];
		fread(szBuf, sizeof(uint8), uFileSize, nfp);
		// 因为文件可能会有多份引用(重名)，所以判断文件是否存在必须用内容hash
		// 音频更新没有modify形式原因也在此，无法判断一个文件是原文件的修改还是完全不同的另一文件
		uHash = HashCode(szBuf, uFileSize);

		m_DstWaveData[i].uContentOffset = m_DstInfo.MetaDataList[i].PlayRegion.dwOffset;

		FileDataMap_t::iterator EndIter = m_mapSrcWaveData.end();
		FileDataMap_t::iterator it = m_mapSrcWaveData.find(uHash);
		if (it == EndIter) // 由于hash可能存在冲突，这里应该使用文件名和内容hash二重判断
		{
			m_DstWaveData[i].uFlag = 'addf'; // 只要内容有修改hash值不同就当是addf(包含modi情况)
			m_DstWaveData[i].uFileSize = uFileSize;
			m_DstWaveData[i].uFileNumber = -1;
			m_DstWaveData[i].Content = szBuf;

			bNeedPatch = true;
		}
		else
		{
			m_DstWaveData[i].uFlag = 'inva';
			m_DstWaveData[i].uFileSize = uFileSize;
			m_DstWaveData[i].uFileNumber = it->second->uFileNumber;
			m_DstWaveData[i].Content = NULL;
		}

		// 获得数据间未明空隙大小
		if ( i < uDstFileCount - 1 )
			uSpaceSize = m_DstInfo.MetaDataList[i+1].PlayRegion.dwOffset -
				(m_DstInfo.MetaDataList[i].PlayRegion.dwOffset + m_DstInfo.MetaDataList[i].PlayRegion.dwLength);
		else
			uSpaceSize = uDstFileLen - uDstDataOffset -
				(m_DstInfo.MetaDataList[i].PlayRegion.dwOffset + m_DstInfo.MetaDataList[i].PlayRegion.dwLength);

		m_DstWaveData[i].uSpaceSize = uSpaceSize;

		fwrite(&m_DstWaveData[i].uFlag, sizeof(uint32), 1, tfp);
		fwrite(&m_DstWaveData[i].uContentOffset, sizeof(uint32), 1, tfp);
		fwrite(&m_DstWaveData[i].uFileSize, sizeof(uint32), 1, tfp);
		fwrite(&m_DstWaveData[i].uSpaceSize, sizeof(uint32), 1, tfp);
		fwrite(&m_DstWaveData[i].uFileNumber, sizeof(uint32), 1, tfp);

		if (m_DstWaveData[i].uFlag == 'addf')
		{
			WriteXorContent( tfp, 0, NULL, m_DstWaveData[i].uFileSize, m_DstWaveData[i].Content );
			m_uAllSize += m_DstWaveData[i].uFileSize;
		}

		// 记录文件间空白部分
		if (m_DstWaveData[i].uSpaceSize != 0)
		{
			m_DstWaveData[i].Space = new uint8[m_DstWaveData[i].uSpaceSize];
			fread(m_DstWaveData[i].Space, sizeof(uint8), m_DstWaveData[i].uSpaceSize, nfp);

			// 记录数据间的空隙异或数据
			if (m_DstWaveData[i].uFlag == 'addf')
				WriteXorContent( tfp, 0, NULL, m_DstWaveData[i].uSpaceSize, m_DstWaveData[i].Space );
			else
				WriteXorContent( tfp, it->second->uSpaceSize, it->second->Space,
					m_DstWaveData[i].uSpaceSize, m_DstWaveData[i].Space );

			delete[] m_DstWaveData[i].Space;
			m_DstWaveData[i].Space = NULL;
			m_uAllSize += m_DstWaveData[i].uSpaceSize;
		}

		delete[] szBuf;
	}

	fclose(ofp);
	fclose(nfp);
	fclose(tfp);

	// 清除已经可以释放的内容
	Clear();

	return bNeedPatch; // 需要有返回false的情况
}


void CAudioDiff::ReadInfo(const char* szNewFile, const char* szOldFile)
{
	LoadAudioData(szOldFile, &m_SrcInfo);
	LoadAudioData(szNewFile, &m_DstInfo);

	m_DataOffset = m_DstInfo.Head.Segments[WAVEBANK_SEGIDX_ENTRYWAVEDATA].dwOffset;
	FILE *fp = fopen(szNewFile, "rb");
	AstFile(fp, szNewFile);
	m_InfoData = new uint8[m_DataOffset]; // 除wav数据区外的所有数据
	fread(m_InfoData, sizeof(uint8), m_DataOffset, fp);
	fclose(fp);
	m_Offset = m_DataOffset;
}


uint32 CAudioDiff::GetAllSize() const
{
	return m_uAllSize;
}

uint32 CAudioDiff::GetDiffType() const
{
	return 'AUDI';
}


void CAudioDiff::WriteDiffData(void* fp)
{
	FILE* FW = (FILE*)fp;
	FILE* tfp = fopen(AUDIO_TEMP_FILE, "rb");

	fwrite(m_InfoData, sizeof(uint8), m_DataOffset, FW);
	delete[] m_InfoData;
	m_InfoData = NULL;

	int nSize = 0;
	uint8 szBuf[MAX_BUFFER];
	while ( (nSize = fread(szBuf, sizeof(uint8), MAX_BUFFER, tfp)) > 0 )
		fwrite(szBuf, sizeof(uint8), nSize, FW);

	fclose(tfp);
}


