#pragma once
#pragma warning(disable:4996)
#include "PkgCommon.h"

struct CFileHeader 
{
	uint32		id;
	bool		bXor;
	int32		number;
	uint32		offset;
public:
	CFileHeader() : id('SQRP'), bXor(false) {};
};

class CVerifyPackage
{
protected:
	class CFileInfoComp
	{
	public:
		bool operator()(const PackFileIndex& Left, const PackFileIndex& Right) const;
	};

	typedef list<PackFileIndex>	FileInfoList;

	FileInfoList		 m_FileList;
	PackageHead			 m_FileHeader;
	wstring				 m_szPkgName;
	wstringstream		 m_ErrMsg;

	FILE	*m_pFile;
	bool	readPkgData( const wstring &pkg_file );
	bool	checkOverFlow( void );
	bool	checkCrcCodeWrong();
	void	addErrMsg( const wstring szMsg, const PackFileIndex& fileIndex );

public:
	CVerifyPackage(const wstring &pkg_file);
	~CVerifyPackage();
	bool IsVerify(wstring& szErrMsg);
};