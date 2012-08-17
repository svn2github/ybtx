
#include "stdafx.h"
#include "CAudioDiff.h"
#include "PatchFileProcess.h"

extern std::wstring Utf8ToUCS2( const string& src );

CAudioDiff::CAudioDiff()
{
}

CAudioDiff::~CAudioDiff()
{
	// 暂时没有释放内存
}

// 读入旧xwb头文件信息
void CAudioDiff::LoadHead(const char* szFileName)
{
	FILE* fp = _wfopen(Utf8ToUCS2(szFileName).c_str(), L"rb");
	m_uFileSize = GetFileLength(fp);
	m_FileData = new uint8[m_uFileSize];
	fread(m_FileData, sizeof(uint8), m_uFileSize, fp); // 该处需要改为读新旧文件较小的大小
	fclose(fp);

	uint8* ptr = m_FileData;
	memcpy(&m_SrcHead, ptr, sizeof(WAVEBANKHEADER));

	ptr = m_FileData + m_SrcHead.Segments[WAVEBANK_SEGIDX_BANKDATA].dwOffset; // SEEK_SET

	memcpy(&m_SrcBankData, ptr, sizeof(WAVEBANKDATA));

	ptr = m_FileData + m_SrcHead.Segments[WAVEBANK_SEGIDX_ENTRYMETADATA].dwOffset; // SEEK_SET

	unsigned long uSrcFileCount = m_SrcBankData.dwEntryCount; // 获取文件数
	m_SrcMetaDataList = new WAVEBANKENTRY[uSrcFileCount];

	unsigned long i = 0;
	for (i=0; i<uSrcFileCount; ++i)
	{
		memcpy(&m_SrcMetaDataList[i], ptr, sizeof(WAVEBANKENTRY));
		ptr += sizeof(WAVEBANKENTRY);
	}
}

// 读入旧xwb头文件数据
void CAudioDiff::LoadData(const char* szFileName)
{
	m_SrcWaveData = new sFileData[m_SrcBankData.dwEntryCount];
	unsigned long uOffset = m_SrcHead.Segments[WAVEBANK_SEGIDX_ENTRYWAVEDATA].dwOffset;
	uint8* ptr = m_FileData;
	ptr += uOffset;

	unsigned long i = 0;
	unsigned long uSrcFileCount = m_SrcBankData.dwEntryCount;
	for (i=0; i<uSrcFileCount; ++i)
	{
		m_SrcWaveData[i].ContentLen = m_SrcMetaDataList[i].PlayRegion.dwLength;
		m_SrcWaveData[i].Content = ptr;
		ptr += m_SrcWaveData[i].ContentLen;

		if (i < uSrcFileCount - 1)
			m_SrcWaveData[i].uSpaceSize = m_SrcMetaDataList[i+1].PlayRegion.dwOffset - m_SrcMetaDataList[i].PlayRegion.dwOffset - m_SrcMetaDataList[i].PlayRegion.dwLength;
		else
			m_SrcWaveData[i].uSpaceSize = m_uFileSize - m_SrcMetaDataList[i].PlayRegion.dwOffset - m_SrcMetaDataList[i].PlayRegion.dwLength - uOffset;

		m_SrcWaveData[i].Space = ptr;
		ptr += m_SrcWaveData[i].uSpaceSize;
	}
}

// 读取Patch文件的头文件信息
void CAudioDiff::LoadPatchInfo(FileHead* pPatchHead)
{
	uint8* ptr = pPatchHead->pData;

	memcpy(&m_DstHead, ptr, sizeof(WAVEBANKHEADER));
	ptr += sizeof(WAVEBANKHEADER);
	unsigned long uOffSet = 0;
	uOffSet = m_DstHead.Segments[WAVEBANK_SEGIDX_BANKDATA].dwOffset;
	ptr = pPatchHead->pData + uOffSet; // SEEK_SET
	memcpy(&m_DstBankData, ptr, sizeof(WAVEBANKDATA));

	uOffSet = m_DstHead.Segments[WAVEBANK_SEGIDX_ENTRYMETADATA].dwOffset;
	ptr = pPatchHead->pData + uOffSet; // SEEK_SET

	unsigned long uDstFileCount = m_DstBankData.dwEntryCount;
	m_DstMetaDataList = new WAVEBANKENTRY[uDstFileCount];

	unsigned long i = 0;
	for (i=0; i<uDstFileCount; ++i)
	{
		memcpy(&m_DstMetaDataList[i], ptr, sizeof(WAVEBANKENTRY));
		ptr += sizeof(WAVEBANKENTRY);
	}
}

// 开始上Patch
void CAudioDiff::StartPatch(FileHead* pPatchHead)
{
	string strTempPath(pPatchHead->pPath);
	strTempPath += ".tem";

	FILE* fp = _wfopen(Utf8ToUCS2(strTempPath).c_str(), L"wb");
	// 试图拷贝tem文件
	if ( !fp )
	{
		fp = _wfopen(Utf8ToUCS2(strTempPath).c_str(), L"rb");
		if ( ! fp )
		{
			printf("打开Patch对应文件错误，也不存在相应.tem文件");
			return;
		}
		fclose(fp);
		rename(strTempPath.c_str(), pPatchHead->pPath);
		return;
	}

	// 写入头文件部分
	uint8* ptr = pPatchHead->pData;
	unsigned long uInfoLen = 0;
	uInfoLen = m_DstHead.Segments[WAVEBANK_SEGIDX_ENTRYWAVEDATA].dwOffset;

	fwrite(ptr, sizeof(uint8), uInfoLen, fp);

	// 开始分析内部结构
	unsigned long uDstFileCount = m_DstBankData.dwEntryCount;
	AudioNode node;
	int nNodeSize = sizeof(AudioNode) - 8; // 减去指针的大小
	ptr += uInfoLen;

	unsigned long i = 0;
	uint8* pData = NULL;
	for (i=0; i<uDstFileCount; ++i)
	{
		memcpy(&node, ptr, nNodeSize);
		ptr += nNodeSize;

		pData = ptr;
		if (node.uFlag == 0x61646466) // addf 新增
		{
			fwrite(pData, sizeof(uint8), node.uFileSize, fp);
			pData += node.uFileSize;
			ptr += node.uFileSize;

			if ( node.uSpaceSize != 0 )
			{
				fwrite(pData, sizeof(uint8), node.uSpaceSize, fp);
				ptr += node.uSpaceSize;
			}
		}
		else if (node.uFlag == 0x696e7661) // inva 无变化
		{
			int nIndex = node.uFileNumber;
			fwrite(m_SrcWaveData[nIndex].Content, sizeof(uint8), m_SrcWaveData[nIndex].ContentLen, fp);

			if ( node.uSpaceSize != 0 )
			{
				fwrite(pData, sizeof(uint8), node.uSpaceSize, fp);
				ptr += node.uSpaceSize;
			}
		}
	}

	fclose(fp);

	remove(pPatchHead->pPath);
	rename(strTempPath.c_str(), pPatchHead->pPath);
}

// Audio Patch on
bool CAudioDiff::ExecuteModify(FileHead* pPatchHead)
{
	// 读取xwb文件数据
	LoadHead(pPatchHead->pPath);
	LoadData(pPatchHead->pPath);

	// 读取Patch文件数据
	LoadPatchInfo(pPatchHead);
	StartPatch(pPatchHead);

	delete[] m_FileData;
	delete[] m_SrcMetaDataList;
	delete[] m_DstMetaDataList;
	delete[] m_SrcWaveData;

	return true;
}
