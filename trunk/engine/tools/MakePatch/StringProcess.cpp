
#include "stdafx.h"

// Ìæ»»×Ö´®×Ö·û
void EX_ReplaceChar(char* SrcString, char OriginChar, char ChangeChar)
{
	char* ptr = SrcString;
	while (*ptr != NULL)
	{
		if (*ptr == OriginChar)
			*ptr = ChangeChar;

		++ptr;
	}
}

// »ñÈ¡dirµÄ×Ódir´®
void EX_GetSubDir(char* Src, char* Dest, int UpLevel)
{
	char* SrcPtr = Src;
	int GetIndex = 0;
	int count = 0;

	while (*SrcPtr != NULL)
	{
		if (*SrcPtr == '/' || *SrcPtr == '\\')
			++GetIndex;

		++SrcPtr;
	}

	GetIndex -= UpLevel;

	SrcPtr = Src;
	char* DestPtr = Dest;
	while (*SrcPtr != NULL)
	{
		if (*SrcPtr == '/' || *SrcPtr == '\\')
			++count;

		if (count > GetIndex)
			break;

		*DestPtr++ = *SrcPtr++;
	}
}


void EX_GetRelativePath(char* RelativePath, const char* NewFile, const char* VersionStr)
{
	strcpy(RelativePath, "../..");
	const char* ptr = NULL;
	char* szTemp = new char[strlen(NewFile) + 1];
	strcpy(szTemp, NewFile);
	while (ptr = strstr(szTemp, VersionStr))
	{
		ptr += strlen(VersionStr);
		strcpy(szTemp, ptr);
	}
	strcat(RelativePath, szTemp);
	delete[] szTemp;
	szTemp = NULL;
}


