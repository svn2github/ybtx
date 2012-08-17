
#include "stdafx.h"

// Ìæ»»×Ö´®×Ö·û
void EX_ReplaceChar(wchar_t* SrcString, wchar_t OriginChar, wchar_t ChangeChar)
{
	wchar_t* ptr = SrcString;
	while (*ptr != NULL)
	{
		if (*ptr == OriginChar)
			*ptr = ChangeChar;

		++ptr;
	}
}

// »ñÈ¡dirµÄ×Ódir´®
void EX_GetSubDir(wchar_t* Src, wchar_t* Dest, int UpLevel)
{
	wchar_t* SrcPtr = Src;
	int GetIndex = 0;
	int count = 0;

	while (*SrcPtr != NULL)
	{
		if (*SrcPtr == L'/' || *SrcPtr == L'\\')
			++GetIndex;

		++SrcPtr;
	}

	GetIndex -= UpLevel;

	SrcPtr = Src;
	wchar_t* DestPtr = Dest;
	while (*SrcPtr != NULL)
	{
		if (*SrcPtr == L'/' || *SrcPtr == L'\\')
			++count;

		if (count > GetIndex)
			break;

		*DestPtr++ = *SrcPtr++;
	}
}

