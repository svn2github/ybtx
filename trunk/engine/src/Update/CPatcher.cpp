// Patcher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CPatcher.h"
#include "CBaseDiff.h"
#include "CAudioDiff.h"
#include "PatchCommonDef.h"
#include "IAsyncUpdateHandler.h"
#include "PatchFileProcess.h"
//#include "ExpHelper.h"

#include "BaseTypes.h"

#include "bzip2/bzlib.h"

extern std::wstring Utf8ToUCS2( const string& src );

static int GetDiffType(unsigned int flag)
{
	if ( flag == 0x42415345 ) // BASE
		return 0;
	else if ( flag == 0x41554449 ) // AUDI
		return 1;

	return -1;
}

CPatcher::CPatcher()
	: m_pContent(NULL)
{
	m_szDiff[0] = new CBaseDiff;
	m_szDiff[1] = new CAudioDiff;
}

CPatcher::~CPatcher()
{
	int i = 0;
	for (i=0; i<PROCESSOR_NUMBER; ++i)
		delete m_szDiff[i];

	delete[] m_pContent;
}

void CPatcher::CheckHeadInfo(IAsyncUpdateHandler* handler, const wchar_t* szPatchFile)
{
	bool bStarterChange = false;
	PatchHead sPatchHead;
	char szPath[8192];
	uint8* szBuff = NULL;
	unsigned int uBuf = 0;
	unsigned int uCount = 0;
	FILE* fp = _wfopen(szPatchFile, L"rb");
	fread(&sPatchHead, sizeof(PatchHead), 1, fp);
	BZFILE* b;
	int bzerror;
	b = BZ2_bzReadOpen( &bzerror, fp, 0, NULL, 0, 0 );
	bzerror = BZ_OK;
	ElementHead head;
	FileHead sFileHead;
	while ( bzerror == BZ_OK )
	{
		BZ2_bzRead( &bzerror, b, &head, sizeof(ElementHead) );
		if ( bzerror == BZ_OK )
		{
			if ( uBuf < head.uDiffSize )
			{
				if ( szBuff != NULL )
					delete[] szBuff;

				uBuf = head.uDiffSize;
				szBuff = new uint8[uBuf];
			}

			BZ2_bzRead( &bzerror, b, szPath, head.uPathSize );
			BZ2_bzRead( &bzerror, b, szBuff, head.uDiffSize );

			if ( strstr(szPath, "Ybtx.exe") != NULL )
			{
				bStarterChange = true;
#ifdef TEST_MODE
				continue;
#endif
			}

			if ( head.uFlag == 0x41444446 ) // ADDF
			{
				FILE* fw = _wfopen(Utf8ToUCS2(string(szPath)).c_str(), L"wb");
				fwrite( szBuff, 1, head.uDiffSize, fw);
				fclose(fw);
			}
			else if ( head.uFlag == 0x4d4f4449 ) // MODI
			{
				int idx = GetDiffType(head.uDiffType);

				if ( idx == -1 )
					continue;

				sFileHead.uFlag = head.uFlag;
				sFileHead.uFileSize = head.uFileSize;
				sFileHead.uDiffType = head.uDiffType;
				sFileHead.uDiffSize = head.uDiffSize;
				sFileHead.uPathSize = head.uPathSize;
				sFileHead.pPath = szPath;
				sFileHead.pData = szBuff;

				m_szDiff[idx]->ExecuteModify(&sFileHead);
			}
		}

		uCount += head.uDiffSize + head.uPathSize + sizeof(ElementHead);
		handler->SetSetupState( (int)((float)uCount / sPatchHead.uNonCompressSize * 100) );
	}

	BZ2_bzReadClose( &bzerror, b );

	fclose(fp);
	handler->SetDownState(0);
	handler->SetSetupState(0);

	if ( bStarterChange )
		handler->CallBackFinishPatch(UPDATING_FLAG);
	else
		handler->PatchContinue();
}

