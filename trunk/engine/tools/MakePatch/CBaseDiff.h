#pragma once


#include <string>

using namespace std;

#include "BaseTypes.h"

using namespace sqr;


#include "IDiff.h"


class CBaseDiff
	: public IDiff
{
public:
	CBaseDiff();
	~CBaseDiff();

	virtual bool MakeDiff(const char* szNewFile, const char* szOldFile);
	virtual uint32 GetDiffType() const;
	virtual uint32 GetAllSize() const;
	virtual void WriteDiffData(void* fp);

private:
	uint32      m_uAllSize;
	//char*       m_szDiff;

	string		m_szDiffFile; // weikun

};

