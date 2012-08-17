// Patcher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Patcher.h"
#include "CBaseDiff.h"
#include "CAudioDiff.h"
#include "PatchCommonDef.h"



static int GetDiffType(unsigned int flag)
{
	if ( flag == 0x42415345 ) // BASE
		return 0;
	else if ( flag == 0x41554449 ) // AUDI
		return 1;

	return -1;
}


Patcher::Patcher()
	: m_pContent(NULL)
{
	m_szDiff[0] = new CBaseDiff;
	m_szDiff[1] = new CAudioDiff;
}

Patcher::~Patcher()
{
	int i = 0;
	for (i=0; i<PROCESSOR_NUMBER; ++i)
		delete m_szDiff[i];

	delete[] m_pContent;
}

void Patcher::CheckHeadInfo(const char* szPatchFile)
{
	PatchHead sPatchHead;

	FILE* fp = fopen(szPatchFile, "rb");
	fread(&sPatchHead, sizeof(PatchHead), 1, fp); // 读出头文件内容

	m_pContent = new BYTE[sPatchHead.uNonCompressSize];
	fread(m_pContent, 1, sPatchHead.uNonCompressSize, fp);

	fclose(fp);

	FileHead sFileHead = {0};

	BYTE* ptr = m_pContent;
	int nHeadSize = sizeof(FileHead) - 8;
	unsigned int i = 0;
	for (i=0; i<sPatchHead.uFileCount; ++i)
	{
		memcpy(&sFileHead, ptr, nHeadSize);
		ptr += nHeadSize;
		sFileHead.pPath = (char*)ptr;
		ptr += sFileHead.uPathSize;
		sFileHead.pData = ptr;

		printf("%s\n", sFileHead.pPath);
		FILE* fp = NULL;
		int idx = 0;
		if ( sFileHead.uFlag == 0x41444446 ) // ADDF
		{
			fp = fopen(sFileHead.pPath, "wb");
			fwrite( ptr, 1, sFileHead.uDiffSize, fp);
			fclose(fp);
		}
		else if ( sFileHead.uFlag == 0x4d4f4449 ) // MODI
		{
			idx = GetDiffType(sFileHead.uDiffType);

			if ( idx != -1 )
				m_szDiff[idx]->PatchExecute(&sFileHead);
		}

		ptr += sFileHead.uDiffSize;
	}
}

