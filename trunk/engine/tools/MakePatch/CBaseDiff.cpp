
#include "stdafx.h"

#include <sys/stat.h>
#include <sys/types.h>

#include "CBaseDiff.h"

#include "FileProcess.h"
#include "StringProcess.h"
#include "AssertTreat.h"



CBaseDiff::CBaseDiff()
	: m_uAllSize(0)
	//, m_szDiff(NULL)
{
}


CBaseDiff::~CBaseDiff()
{
}


// 比较两个文件
bool CBaseDiff::MakeDiff(const char* szNewFile, const char* szOldFile)
{
	uint32 uNewSize  = 0;
	uint32 uOldSize  = 0;
	uint32 uCurSize  = 0;
	uint32 uClipSize = 0;
	uint32 uSize     = 0;
	uint32 uOffset   = 0;
	uint32 uSame     = 0;
	uint32 uSkip     = 0;
	bool bContinueFlag = false; // true代表新Buffer连接旧Buffer

	char szNewContent[MAX_BUFFER];
	char szOldContent[MAX_BUFFER];

	FILE* nfp = fopen(szNewFile, "rb");
	uNewSize = GetFileLength( nfp );
	FILE* ofp = fopen(szOldFile, "rb");
	uOldSize = GetFileLength( ofp );
	
	//m_szDiff = new char[uNewSize+8]; // 临界情况，文件本身大小不足8byte
	//char* pDiff = m_szDiff;
	
	m_szDiffFile = "MakeYBTXPatchTemp.tmp";
	FILE* pfDiffFile = fopen(m_szDiffFile.c_str(), "wb");
	if (!pfDiffFile)
	{
		printf("can not create temp file %s\n", m_szDiffFile.c_str());
		fclose(nfp);
		fclose(ofp);
		return false;
	}


	fseek( pfDiffFile, 0, SEEK_SET );


	//char* pRetSize = NULL;
	long posRetSize = 0;

	int j = 0;
	int i = 0;
	char szBuf[MIN_SAME_SIZE];
	while ( j < uNewSize && j < uOldSize )
	{
		if (uOldSize - j < MAX_BUFFER)
			uCurSize=fread(szNewContent, sizeof(char), uOldSize - j, nfp);
		else
			uCurSize=fread(szNewContent, sizeof(char), MAX_BUFFER, nfp);
		fread(szOldContent, sizeof(char), uCurSize, ofp);

		for ( i=0; i<uCurSize; ++i, ++uSkip )
		{
			if ( ! bContinueFlag ) // countine标签代表为true代表原来8字节相同检测没有完结
			{
				if (szNewContent[i] == szOldContent[i]) // 略过相同内容
					continue;

				//memcpy(pDiff, (char*)&uSkip, sizeof(uint32));
				//pDiff += sizeof(uint32);
				fwrite( &uSkip, sizeof(uint32), 1, pfDiffFile );


				//pRetSize = pDiff; // 记录ClipSize地址，以后回填
				posRetSize = ftell(pfDiffFile);

				//memcpy(pDiff, (char*)&uClipSize, sizeof(uint32));
				//pDiff += sizeof(uint32);
				fwrite(&uClipSize, sizeof(uint32), 1, pfDiffFile);

				uSame = 0;
				bContinueFlag = true;
			}

			// 记录不同部分，如果有8字节以上相同，则认为当前clip结束
			while( uSame < MIN_SAME_SIZE && i < uCurSize )
			{
				szBuf[uSame] = szNewContent[i];
				++uSame;

				// 当新旧文件有不同时，记录当前clip的一部分(8字节以内)
				if( szNewContent[i] != szOldContent[i] )
				{
					//memcpy(pDiff, szBuf, uSame);
					//pDiff += uSame;
					fwrite(szBuf, 1, uSame, pfDiffFile);

					uSame = 0;
				}

				++i;
				++uClipSize;
			}

			// 8字节相同，重新开始累计不同部分
			if ( uSame >= MIN_SAME_SIZE || j + i >= uNewSize )
			{
				uClipSize -= uSame;
				//*( (uint32*)pRetSize ) = uClipSize;				
				fseek(pfDiffFile, posRetSize, SEEK_SET);
				fwrite(&uClipSize, sizeof(uClipSize), 1, pfDiffFile);
				fseek(pfDiffFile, 0, SEEK_END);


				uSize += uClipSize + sizeof(uint32) * 2;
				--i;           // 因为接下来的循环第一次就会进行++i操作
				uSkip = uSame; // 临界情况，新文件比旧文件大，最后一次uSkip可能小于8
				--uSkip;       // 因为i回退了一位，skip也需要随之回退一位
				uClipSize = 0;
				bContinueFlag = false;
			}
		}

		j += uCurSize;
	}

	// 如果目标文件比源文件大，剩余的内容均作为Patch内容处理
	if ( uNewSize > uOldSize )
	{
		uint32 uLeaveSize = uNewSize - uOldSize;
		char* szLeaveBuf = new char[uLeaveSize];

		fread(szLeaveBuf, sizeof(char), uLeaveSize, nfp);

		// 结束处刚好8位相同，clip头还没有创建
		if (uClipSize == 0)
		{
			uClipSize += uLeaveSize;
			//memcpy(pDiff, (char*)&uSkip, sizeof(uint32));
			//pDiff += sizeof(uint32);
			fwrite(&uSkip, sizeof(uint32), 1, pfDiffFile);


			//memcpy(pDiff, (char*)&uClipSize, sizeof(uint32));
			//pDiff += sizeof(uint32);
			fwrite(&uClipSize, sizeof(uint32), 1, pfDiffFile);
		}
		else
		{
			if (uSame != 0)
			{
				//memcpy(pDiff, szBuf, uSame);
				//pDiff += uSame;
				fwrite(szBuf, 1, uSame, pfDiffFile);
				uSame = 0;
			}

			uClipSize += uLeaveSize;
			//*( (uint32*)pRetSize ) = uClipSize;
			fseek(pfDiffFile, posRetSize, SEEK_SET);
			fwrite(&uClipSize, sizeof(uClipSize), 1, pfDiffFile);
			fseek(pfDiffFile, 0, SEEK_END);
		}

		// 新文件比旧文件多出部分不异或(或当作与ASCII 0异或)，直接记录原值
		//memcpy(pDiff, szLeaveBuf, uLeaveSize);
		fwrite(szLeaveBuf, 1, uLeaveSize, pfDiffFile);

		uSize += uClipSize + sizeof(uint32) * 2;

		delete[] szLeaveBuf;
	}

	fclose(nfp);
	fclose(ofp);

	fclose(pfDiffFile);

	m_uAllSize = uSize;

	if (uSize==0) // 新旧文件内容相同，无需Patch
	{

		remove(m_szDiffFile.c_str());
		m_szDiffFile.clear();

		//delete[] m_szDiff;
		//m_szDiff = NULL;
		
		if (uNewSize != uOldSize)
		{
			return true;
		}

		return false;
	}

	return true;
}

uint32 CBaseDiff::GetAllSize() const
{
	return m_uAllSize;
}

uint32 CBaseDiff::GetDiffType() const
{
	return 'BASE';
}

void CBaseDiff::WriteDiffData(void* fp)
{
	if (m_szDiffFile.empty())
	{
		printf("the diffFile temp file not exist!\n");
		return;
	}
	FILE* fDiffFile = fopen(m_szDiffFile.c_str(), "rb");
	if (!fDiffFile)
	{
		printf( "can not open file: %s\n", m_szDiffFile.c_str() );
		return;
	}

	const int BUFF_LEN = 64*1024; // 64K
	char* pTemp = new char[BUFF_LEN];
	if (!pTemp)
	{
		printf("can not new memory!\n");
		fclose(fDiffFile);
		return;
	}

	//fwrite(m_szDiff, sizeof(char), m_uAllSize, (FILE*)fp);
	size_t Read = 0;
	while (Read = fread(pTemp, sizeof(char), BUFF_LEN, fDiffFile))
	{
		fwrite(pTemp, sizeof(char), Read, (FILE*)fp);		
	}
	if (pTemp)
	{
		delete[] pTemp;
		pTemp = NULL;
	}
	fclose(fDiffFile);


	//if ( m_szDiff != NULL )
	//{
	//	delete[] m_szDiff;
	//	m_szDiff = NULL;
	//}

	if (!m_szDiffFile.empty())
	{
		remove(m_szDiffFile.c_str());
		m_szDiffFile.clear();
	} 
}

