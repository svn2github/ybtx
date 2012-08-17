#pragma once
#include <string>
#include <set>
#include "PkgCommon.h"
using namespace std;

struct FILE_INFO : public PackFileIndex
{
	string		name;
};

class CFilePack
{
	vector<FILE_INFO>	m_vecOldFiles;
	vector<FILE_INFO>	m_vecNewFiles;
	set<int>		m_setFileID;

	PackageHead			m_FileHeader;
	string				m_sPackName;

	void	_CreateData();
	void	_UpdateData();
	FILE*	_ReadOldData();
	int32	_FindFile( uint32 id ) const;
	void	_DeleteFile( FILE *f, int32 nOldIndex );
	bool	_WriteFile( FILE *f, FILE_INFO& WrtFile, bool findspace = false );
	void	_WriteList( FILE *f );
	void	_WriteHeader( FILE *f );
	bool	_NeedUpdate( FILE *f, FILE_INFO& NewFile, FILE_INFO& OldFile );
	void	_UpdateFile( FILE *f, FILE_INFO& NewFile, FILE_INFO& OldFile );

public:
	CFilePack( const TCHAR* szPackName, bool bXor );
	void AddFile( string szFileName );
	void MakePackage();
};

