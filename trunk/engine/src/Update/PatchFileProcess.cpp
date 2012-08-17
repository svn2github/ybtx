
#include "stdafx.h"

#include "TraceLog.h"


unsigned int GetFileLength(FILE* FH)
{
	int Size = 0;
	fseek(FH, 0, SEEK_END);
	Size = ftell(FH);
	fseek(FH, 0, SEEK_SET);

	return Size;
}

