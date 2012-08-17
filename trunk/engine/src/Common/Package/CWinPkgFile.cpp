
#include "stdafx.h"
#include "CWinPkgFile.h"
#include "CPathMgr.h"
#include "CodeCvs.h"


using namespace sqr;

#ifdef _WIN32

CWinPkgFile::CWinPkgFile()
	: m_hFile(NULL)
{
}

CWinPkgFile::~CWinPkgFile()
{
	//if ( m_hFile ) 有些需要Windows系统自己Close 所以需要手动Close
	//	CloseHandle(m_hFile);
}

HANDLE CWinPkgFile::Open(const wchar_t* alias, const char *szFileName)
{
	wstring wcs_buf = utf8_to_utf16(szFileName);
	return Open(alias, wcs_buf.c_str());
}

HANDLE CWinPkgFile::Open(const wchar_t* alias, const wchar_t* szFileName)
{
	CPkgFile::SearchPathList pathList		= CPkgFile::GetSearchList(alias);
	for ( CPkgFile::SearchPathList::iterator it = pathList.begin(); it != pathList.end(); ++it )
	{
		wstring szLoadPath = (*it) + szFileName;
		m_hFile = CreateFileW(szLoadPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

		if( m_hFile != INVALID_HANDLE_VALUE ) // 打开成功
			return m_hFile;
	}

	//switch (eFlag)
	//{
	//case eFOP_Read:
	//	m_hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	//	break;
	//case eFOP_Write:
	//	m_hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	//	break;
	//case eFOP_Append:
	//	m_hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
	//	break;
	//}

	return INVALID_HANDLE_VALUE;
}

unsigned int CWinPkgFile::Read(char* szBuf, unsigned int uSize)
{
	unsigned long nBytesRead;
	ReadFile(m_hFile, szBuf, uSize, &nBytesRead, NULL);
	return nBytesRead;
}

unsigned int CWinPkgFile::Write(char* szBuf, unsigned int uSize)
{
	unsigned long nBytesWrite;
	WriteFile(m_hFile, szBuf, uSize, &nBytesWrite, NULL);
	return nBytesWrite;
}

unsigned int CWinPkgFile::Seek(long nSize, unsigned int uFlag)
{
	return SetFilePointer(m_hFile, nSize, NULL, uFlag);
}

int CWinPkgFile::Flush()
{
	return FlushFileBuffers(m_hFile);
}

int CWinPkgFile::Close()
{
	int ret = CloseHandle(m_hFile);
	m_hFile = NULL;
	return ret;
}

#endif
