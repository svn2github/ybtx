// MakeCrcCodeFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define MAX_LENGTH 8192

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1)
	{
		return 0;
	}

	FILE *pCrcCodeFile = NULL;
	fopen_s(&pCrcCodeFile, argv[1], "w");
	if (pCrcCodeFile == NULL)
	{
		return 0;
	}

	FILE *pFile = NULL;
	for (int i = 2; i < argc; i++)
	{
		fopen_s(&pFile, argv[i], "rb");
		if (pFile == NULL)
		{
			fclose(pCrcCodeFile);
			return 0;
		}
		fseek(pFile, 0, SEEK_END);
		int nLength = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		uint32 uCrc32Code = 0xffffffff;
		
		while (nLength)
		{
			int nLen = nLength < MAX_LENGTH ? nLength : MAX_LENGTH;
			uint8 *pData = new uint8[nLen];
			if (pData == NULL)
			{
				fclose(pCrcCodeFile);
				return 0;
			}
			int nRead = (int)fread(pData, sizeof(uint8), nLen, pFile);
			if (nRead == 0)
			{
				break;
			}
			uCrc32Code = Crc32WithInit(pData, nLen, uCrc32Code);
			delete[] pData;
			pData = NULL;
			nLength -= nRead;
		}
		
		uCrc32Code = ~uCrc32Code;
		fwrite(argv[i], sizeof(char), strlen(argv[i]), pCrcCodeFile);
		fwrite("\t", sizeof(char), strlen("\t"), pCrcCodeFile);
		
		char *szCrc32Code = new char[11];
		_ultoa_s(uCrc32Code, szCrc32Code, 11, 10);

		fwrite(szCrc32Code, sizeof(char), strlen(szCrc32Code), pCrcCodeFile);
		fwrite("\n", sizeof(char), strlen("\n"), pCrcCodeFile);

		delete[] szCrc32Code;
		szCrc32Code = NULL;
		fclose(pFile);
	}

	fclose(pCrcCodeFile);
	return 0;
}

