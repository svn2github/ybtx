#pragma once
#pragma warning(disable:4996)
#include "PkgCommon.h"

class CFixOverflowPkg
{
public:
	typedef list<PackFileIndex>	FileInfoSet;

protected:
	class CFileInfoComp
	{
	public:
		bool operator()(const PackFileIndex& Left, const PackFileIndex& Right) const;
	};

	FileInfoSet			 m_fileList;
	PackageHead			 m_FileHeader;
	string				 m_szPkgName;
	stringstream		 m_ErrMsg;

	FILE	*m_pFile;
	bool	readPkgData( const string &pkg_file );
	bool	isPkgCrash( void );
	bool	fixPkgData( void );
	void	writeList( void );
	void	outErrFile( const string& msg, const PackFileIndex& curIndex );

public:
	CFixOverflowPkg(const string &pkg_file);
	~CFixOverflowPkg();
	bool DoFix();
};
