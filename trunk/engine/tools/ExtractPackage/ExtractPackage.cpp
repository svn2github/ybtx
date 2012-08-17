// ExtractPackage.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "BaseTypes.h"
#include "CPkgFile.h"

using namespace sqr;

#define  BUFFER_SIZE  4096


void ExchangeSymbol(char* szBuf, char cSrc, char cDst)
{
	char* ptr = szBuf;
	while ( *ptr != '\0' )
	{
		if ( *ptr == cSrc )
			*ptr = cDst;
		++ptr;
	}
}

const char* GetBaseName(const char* szPath)
{
	const char* ptr = szPath;
	const char* idx = NULL;
	while ( *ptr != '\0' )
	{
		if ( *ptr == '/' )
			idx = ptr;
		++ptr;
	}

	return ++idx;
}


int _tmain(int argc, _TCHAR* argv[])
{
	CPkgFile pkg;
	if ( pkg.Open(L"", argv[1]) != eFE_SUCCESS )
	{
		printf("Can not find file: %s\n", argv[1]);
		return 1;
	}

	int32 size = pkg.Size();
	char* szBuf = new char[size];
	pkg.Read(szBuf, size);
	pkg.Close();

	char szDir[BUFFER_SIZE];
	GetCurrentDirectory(BUFFER_SIZE, szDir);
	ExchangeSymbol(szDir, '\\', '/');
	strcat(szDir, "/");
	strcat(szDir, GetBaseName(argv[1]));

	FILE* fp = fopen(szDir, "wb");
	fwrite(szBuf, sizeof(char), size, fp);
	fclose(fp);

	delete[] szBuf;

	return 0;
}

