#pragma once

#include "BaseTypes.h"

using namespace sqr;

class IDiff
{
public:
	virtual bool MakeDiff(const char* szNewFile, const char* szOldFile)=0;
	virtual uint32 GetDiffType() const=0;
	virtual uint32 GetAllSize() const=0;
	virtual void WriteDiffData(void* fp)=0;
};

