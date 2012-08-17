/*
*	FileTreeWalk.h	启动器文件校验文件搜索
*/
#ifndef _FILETREEWALK_H
#define _FILETREEWALK_H

#include <Windows.h>
#include <sys/types.h>
#include <sys/stat.h>

enum FTW_FLAG
{
	_FTW_FILE = FILE_ATTRIBUTE_NORMAL,
	_FTW_DIR = FILE_ATTRIBUTE_DIRECTORY,
	_FTW_TMP = FILE_ATTRIBUTE_TEMPORARY,
	_FTW_SL,
	_FTW_NS,
};

enum FTW_RESULT
{
	_FTW_STOP,
	_FTW_IGNORE,
	_FTW_CONTINUNE,
};

struct FILE_FOUND
{
	HANDLE			 pFound;
	WIN32_FIND_DATAW FindData;
};

typedef FTW_RESULT(* FILE_PROC)( const wchar_t*, const struct _stat *, FTW_FLAG, void* );

int FileTreeWalk( const wchar_t* szDir, FILE_PROC pfnFileProc, void* pContext );

#endif