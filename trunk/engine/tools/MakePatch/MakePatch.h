#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

#include "BaseTypes.h"

using namespace sqr;


class IDiff;


namespace sqr
{
	struct ReleaseVersion;
}


class MakePatch
{
public:
	MakePatch();
	~MakePatch();

	void GetPatchVersion(char* szPackDir);
	void SetPatchName(char* str);
	int  GetPatchFile(string sNewDirName, string sOldDirName);
	void FetchPatchData();
	void GeneratePatch();
	void Execute(const char* szNewFile, const char* szOldFile, const char* VersionStr);

	void TestDecompress();

	void SetType(char* type);
	void SetCompress(bool flag);
	bool GetCompress();

private:
	typedef  map<string,string>  StringMap;

	StringMap            m_szChangeFiles[PROCESSOR_NUMBER];
	IDiff*               m_szDiff[PROCESSOR_NUMBER];

	ReleaseVersion*      m_pVersion;
	string               m_strVersion;
	string               m_strPatchName;

	uint32       m_uFileCount;
	int          m_nMode;
	bool         m_bCompress;
	set<string>  m_setType;
};

