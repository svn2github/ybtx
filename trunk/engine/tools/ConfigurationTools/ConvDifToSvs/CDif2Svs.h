#pragma once
#include "TSingleton.h"

class CDifFile;

class CDif2Svs : public TSingleton<CDif2Svs>
{
public:
	static FTW_RESULT  CDif2Svs::Convert(const wchar_t* wszFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	static void Convert(string& sDifFile, string& sSvsFile);

private:
	static uint32 uCount;
};
