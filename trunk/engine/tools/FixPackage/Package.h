#pragma once
#pragma warning(disable:4996)
#include "PkgCommon.h"

class CFixPackage
{
	typedef list<PackFileIndex>	FileInfoList;

	FileInfoList		 m_vecOldFiles;
	PackageHead			 m_FileHeader;
	vector<uint32>		 m_BegOffsetList;
	string				 m_szPkgName;

	FILE	*m_pFile;
	bool	readOldData( const string &pkg_file );
	void	writeList();
	void	fixSpaces();
	uint32	findNearstNextOffset(uint32 uNodeOffset);

public:
	CFixPackage(const string &pkg_file);
	~CFixPackage();
	void DoFix();
};
