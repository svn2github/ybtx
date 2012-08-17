#include "stdafx.h"
#include "FileTreeWalk.h"
#include <cassert>
#include <Windows.h>

FILE_FOUND*	FindFirstFile( const wchar_t* szDir )
{
	FILE_FOUND* pFound = new FILE_FOUND;

	if( ( pFound->pFound = FindFirstFileW( ( wstring( szDir ) + L"\\*.*" ).c_str(), &pFound->FindData ) ) == INVALID_HANDLE_VALUE )
	{
		delete pFound;
		pFound = NULL;
	}

	return pFound;
}

const wchar_t* GetFileName( FILE_FOUND* pFound )
{
	return pFound->FindData.cFileName;
}

bool FindNextFile( FILE_FOUND* pFound )
{
	if( !FindNextFileW( pFound->pFound, &pFound->FindData ) )
		return false;
	return true;
}

int FileTreeWalk( const wchar_t* szDir, FILE_PROC pfnFileProc, void* pContext )
{
	assert(szDir);
	assert(szDir[0]);
	assert(pfnFileProc);

	FILE_FOUND* pFound = FindFirstFile( szDir );
	if ( !pFound )
		return -1;

	do
	{
		const wchar_t* szFileName = GetFileName( pFound );
		if ( !wcscmp( szFileName, L"." ) || !wcscmp( szFileName, L".." ) )
			continue;

		wchar_t szBuffer[MAX_PATH];
		
		struct _stat FileStat;
		swprintf_s( szBuffer, _countof(szBuffer), L"%s/%s", szDir, szFileName );
		_wstat( szBuffer, &FileStat );

		FTW_FLAG eFlag = ( FileStat.st_mode & S_IFDIR ) ? _FTW_DIR : _FTW_FILE;
		FTW_RESULT ret = pfnFileProc( szBuffer, &FileStat, eFlag, pContext );
		if( ret == _FTW_STOP )
		{
			FindClose( pFound );
			return 1;
		}

		if( (FileStat.st_mode & S_IFDIR) && ret != _FTW_IGNORE )
		{
			if(1 == FileTreeWalk(szBuffer, pfnFileProc, pContext))
			{
				FindClose(pFound);	
				return 1;
			}
		}
	}
	while (FindNextFile(pFound));

	FindClose(pFound->pFound);

	return 0;
}