/*
*	CPathMgr.cpp	文件路径管理器的实现
*/

#include "stdafx.h"
#include "CPathMgr.h"
#include "ExpHelper.h"
#include <errno.h>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <sys/types.h>
#include "BaseHelper.h"
#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#define  MAX_PATH  PATH_MAX
#else
#include <direct.h>
#endif
#include "CodeCvs.h"

namespace sqr
{
	// 初始化当前路径,应该从注册表中读,是是Exe的
#ifndef _WIN32
#define _chdir chdir
#define _mkdir mkdir
#define _countof(array)	(sizeof(array)/sizeof(array[0]))
#define _wgetcwd getcwd
#endif
	
	//CleanPathName函数把路径名整理干净
	//@.将"\\"替换成"/"
	//@.将"./"去掉
	//@.将"//"替换成"/"
	static wstring CleanPathName(const wstring& sPath)
	{
		wstring sResult=sPath;

		replace(sResult.begin(), sResult.end(), '\\', '/');

		for(;;)
		{
			wstring::size_type stPos;

			stPos=sResult.find(L"//");
			if(stPos != string::npos)
			{
				sResult.erase(stPos,1);
				continue;
			}

			stPos=sResult.find(L"./");
			if(  (stPos!=string::npos)  &&  ((stPos==0) || (sResult[stPos-1]==L'/'))  )
			{
				sResult.erase(stPos,2);
				continue;
			}

			break;
		}
		return sResult;
	}

	CPathMgr::CPathMgr( const char* szPathName )
	{
		initialize(utf8_to_utf16(szPathName).c_str());
	}

	// 设置root路径和当前路径
	CPathMgr::CPathMgr(const wchar_t* szPathName)
	{
		initialize(szPathName);
	}
	
	CPathMgr::~CPathMgr()
	{
	}

	void CPathMgr::initialize( const wchar_t* szPathName )
	{
		memset(m_szRootPath, 0, sizeof(m_szRootPath));
		memset(m_szCurrPath, 0, sizeof(m_szCurrPath));

		if(szPathName == NULL || wcscmp(szPathName, L"" ) == 0 )
		{
			//无效字符串，或者空字符串
#ifndef _WIN32
			char szBufTmp[MAX_PATH];
			getcwd(szBufTmp, sizeof(szBufTmp));
			wcscpy(m_szRootPath, utf8_to_utf16(szBufTmp).c_str());
#else
			_wgetcwd(m_szRootPath, _countof(m_szRootPath));
#endif
			wstring sPath = m_szRootPath;
			sPath=CleanPathName(sPath);
			wcscpy(m_szCurrPath, sPath.c_str());
			wcscpy(m_szRootPath, m_szCurrPath);
		}
		else if( szPathName[0]==L'.' && wcscmp(szPathName, L".." ) != 0)
		{
			//相对路径
#ifndef _WIN32
			char szBufTmp[MAX_PATH];
			getcwd(szBufTmp, sizeof(szBufTmp));
			wcscpy(m_szRootPath, utf8_to_utf16(szBufTmp).c_str());
#else
			_wgetcwd(m_szRootPath, _countof(m_szRootPath));
#endif

			wstring sPath = m_szRootPath;
			sPath=sPath+L"/"+wstring(szPathName);
			sPath=CleanPathName(sPath);
			wcscpy(m_szCurrPath, sPath.c_str());
			wcscpy(m_szRootPath, m_szCurrPath);
		}
		else
		{
			//绝对路径
			wstring sPath = szPathName;
			sPath=CleanPathName(sPath);
			if( *(sPath.end() - 1) == L'/' )
				sPath.erase(sPath.end() - 1);
			wcsncpy(m_szCurrPath, sPath.c_str(), _countof(m_szCurrPath) - 1);
			wcsncpy(m_szRootPath, m_szCurrPath, _countof(m_szRootPath) - 1);
		}
	}

	// 取得本程序的根路径, 根路径名结尾处不带"\\"
	const wchar_t* CPathMgr::GetRootPath() const
	{
		return m_szRootPath;
	}

	void CPathMgr::ChangeRootPath(const wchar_t* RootPath)
	{
		memset(m_szRootPath, 0, sizeof(m_szRootPath));
		wcscpy(m_szRootPath, RootPath);
	}

	// 取得当前文件路径
	const wchar_t* CPathMgr::GetCurPath() const
	{
		return m_szCurrPath;
	}

	const wchar_t* CPathMgr::GetFullPath(const wchar_t* szPath)
	{
		SetCurPath(szPath);
		return GetCurPath();
	}

	/******************************************************************************
	// Purpose		: 设置当前文件路径
	// Argumant		: char* szPathName[in]	传入的是相对于根目录的相对路径或者绝对路径
	// Comments		: 如果传入的是 NULL 指针,则将原始的根目录设为当前目录
	*****************************************************************************/
	void CPathMgr::SetCurPath(const wchar_t* szPathName)
	{
		if( NULL == szPathName || szPathName[0]==0 )
		{
			wcscpy(m_szCurrPath, m_szRootPath);
		}
		else
		{
			wstring sPathName=szPathName;
			sPathName=CleanPathName(sPathName);
			if(sPathName[1] == L':')
			{
				wcscpy(m_szCurrPath, sPathName.c_str());
			}
			else
			{
#ifdef _WIN32
				if(L':' == sPathName[1]) 
#else
				if(L'/' == sPathName[0]) 
#endif
				{
#ifdef _WIN32
					swprintf_s(m_szCurrPath, _countof(m_szCurrPath), L"%s", sPathName.c_str());
#else
					swprintf(m_szCurrPath, _countof(m_szCurrPath), L"%ls", sPathName.c_str());
#endif
				}
				else
				{
					wstring sPath=wstring(m_szRootPath)+L"/"+sPathName;
					sPath=CleanPathName(sPath);

#ifdef _WIN32
					swprintf_s(m_szCurrPath, _countof(m_szCurrPath), L"%s", sPath.c_str());
#else
					swprintf(m_szCurrPath, _countof(m_szCurrPath), L"%ls", sPath.c_str());
#endif

				}
			}
		}
	}

	struct FILE_FOUND
	{
#ifdef _WIN32
		HANDLE			 pFound;
		WIN32_FIND_DATAW FindData;
#else       
		DIR*			pFound;
		dirent*			pDrt;
		wstring			szRetName;
#endif
	};

	FILE_FOUND*	FindFirstFile( const wchar_t* szDir )
	{
		FILE_FOUND* pFound = new FILE_FOUND;
#ifdef _WIN32
		if( ( pFound->pFound = FindFirstFileW( ( wstring( szDir ) + L"\\*.*" ).c_str(), &pFound->FindData ) ) == INVALID_HANDLE_VALUE )
			SafeDelete( pFound );
#else       
		if( !( pFound->pFound = opendir( utf16_to_utf8(szDir).c_str() ) ) || !( pFound->pDrt = readdir( pFound->pFound ) ) )
			SafeDelete( pFound );
#endif
		return pFound;
	}

	bool FindNextFile( FILE_FOUND* pFound )
	{
#ifdef _WIN32
		if( !FindNextFileW( pFound->pFound, &pFound->FindData ) )
			return false;
#else       
		if( !( pFound->pDrt = readdir( pFound->pFound ) ) )
			return false;
#endif
		return true;
	}

	const wchar_t* GetFileName( FILE_FOUND* pFound )
	{
#ifdef _WIN32
		return pFound->FindData.cFileName;
#else       
		pFound->szRetName = utf8_to_utf16(pFound->pDrt->d_name);
		return pFound->szRetName.c_str();
#endif
	}

	void FindClose( FILE_FOUND* pFound )
	{
		if( !pFound )
			return;
#ifdef _WIN32
		::FindClose( pFound->pFound );
#else       
		closedir( pFound->pFound );
#endif
		delete pFound;
	}

	int32 FileTreeWalk( const wchar_t* szDir, FILE_PROC pfnFileProc, void* pContext )
	{
		Ast(szDir);
		Ast(szDir[0]);
		Ast(pfnFileProc);

		FILE_FOUND* pFound = FindFirstFile( szDir );
		if ( !pFound )
			return -1;

		do
		{
			const wchar_t* szFileName = GetFileName( pFound );
			if ( !wcscmp( szFileName, L"." ) || !wcscmp( szFileName, L".." ) )
				continue;

			wchar_t szBuffer[MAX_PATH];
			
#ifdef _WIN32
			struct _stat FileStat;
			swprintf_s( szBuffer, _countof(szBuffer), L"%s/%s", szDir, szFileName );
			_wstat( szBuffer, &FileStat );
#else
			struct stat FileStat;
			char szU8Buf[MAX_PATH];
			memset(szU8Buf, 0, sizeof(szU8Buf));
			swprintf( szBuffer, _countof(szBuffer), L"%ls/%ls", szDir, szFileName);
			strcpy(szU8Buf, utf16_to_utf8(szBuffer).c_str());
			stat(szU8Buf, &FileStat);
#endif

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

		FindClose(pFound);

		return 0;
	}
	bool MakeDirectoryUTF8(const char * szDirectory)
	{
		return MakeDirectory(utf8_to_utf16(szDirectory).c_str());
	}
	bool MakeDirectory( const wchar_t* szDirectory )
	{
		wchar_t szCurDir[MAX_PATH];
		wchar_t szBuf[MAX_PATH];

#ifndef _WIN32
		char szBufTmp[MAX_PATH];
		getcwd(szBufTmp, sizeof(szBufTmp));
		wcscpy(szCurDir, utf8_to_utf16(szBufTmp).c_str());
#else
		_wgetcwd(szCurDir, MAX_PATH);
#endif

		wcscpy( szBuf, szDirectory );

		wchar_t* pCur = szBuf;
		for(;;)
		{
			wchar_t* pCurDir = pCur;
			while( pCurDir[0] != L'\\' && pCurDir[0] != L'/' )
			{
				if( !pCurDir[0] )
				{
					int32 nResult;
					if( pCurDir != pCur )
#ifdef _WIN32
						nResult=_wmkdir( pCur );
#else
						nResult=_mkdir( utf16_to_utf8(pCur).c_str(), S_IRWXU );
#endif
					//noted by jiaben, here is a very terrible modify
					//
					/*
					if( nResult && errno!=EEXIST )
						return false;
					*/
#ifdef _WIN32
					if( _wchdir( szCurDir ) )
						return false;
#else
					if(_chdir(utf16_to_utf8(szCurDir).c_str()))
						return false;
#endif
					
					return true;
				}
				pCurDir++;
			}
			pCurDir[0] = 0;
#ifdef _WIN32
			_wmkdir( pCur ); 
#else
			_mkdir( utf16_to_utf8(pCur).c_str(), S_IRWXU );
#endif
			/*
			//noted by jiaben, 
			if( nResult && errno!=EEXIST ){
				//printf("这里的ErrorNo是:%d\n", errno);
				//_getcwd( szCurDir, MAX_PATH );
				//printf("当前位置到底是什么呢?%s\n", szCurDir);
				return false;
			}
			*/
#ifdef _WIN32
			if ( _wchdir( ( wstring( pCur ) + L"/" ).c_str() ) )
				return false;			
#else
			if ( _chdir( (utf16_to_utf8(pCur) + "/").c_str() ) )
				return false;
#endif
			pCur = pCurDir + 1;
		}
	}

}
