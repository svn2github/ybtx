#include "stdafx.h"
#include "CPkgDirectory.h"
#include "PkgStream.h"
#include "CodeCvs.h"
#include "CPathMgr.h"
#include "StringHelper.h"
#include "ErrLogHelper.h"

#ifdef _WIN32
	#include <windows.h>
#endif

#ifndef SUCCEEDED
#define SUCCEEDED(hr) ((hr) >= 0)
#endif

#ifndef FAILED
#define FAILED(hr) ((hr) < 0)
#endif

#define DIR_FILE_NAME ".dir"
#define DIR_ROOT_NAME "/root.dir"
#define DIR_POOL_NUM	8

CPkgDir::CPkgDir()
{
	m_lstFileName.reserve(DIR_POOL_NUM);
}

CPkgDir::CPkgDir(const wchar_t* alias, const char* szDirName,const char* Filter)
{
	Open(alias, szDirName,Filter);
}

CPkgDir::~CPkgDir()
{}

void CPkgDir::Register(const string& filename, const string& filter)
{
	if( filter.size()>0 && filename.size()>filter.size() &&
		filter != filename.substr(filename.size()-filter.size(),filter.size()) )
		return;
	m_lstFileName.push_back(filename);
}

struct RegisterStruct
{
	CPkgDir* pPkgDir;
	const string& strFilter;
};

static FTW_RESULT RegisterFile( const wchar_t* szFileName, const struct _stat * pState, FTW_FLAG eFlag, void *pContext )
{
	RegisterStruct* pRs = (RegisterStruct*)pContext;
	const wstring& wrFileName = szFileName;
	size_t nop = wrFileName.rfind(L"/");
	string FileName = utf16_to_gbk(wrFileName.substr( nop + 1, wrFileName.size()- nop ));
	
	if( FileName[0] == '.' )
		return _FTW_IGNORE;

	if( eFlag == _FTW_DIR )
	{
		pRs->pPkgDir->Register(FileName + DIR_FILE_NAME, pRs->strFilter);
		return _FTW_IGNORE;
	}	
	
	pRs->pPkgDir->Register(FileName,pRs->strFilter);
	return _FTW_CONTINUNE;
}

uint32 CPkgDir::Open(const wchar_t* alias, const char* szDirName, const char* Filter)
{
#ifndef _DEBUG
	string		m_strFilter;
#endif
	
	m_strDirPath = szDirName;
	if(m_strDirPath[m_strDirPath.size()-1] == '/' || m_strDirPath[m_strDirPath.size()-1] == '\\' )
	{
		m_strDirPath = m_strDirPath.substr(0,m_strDirPath.size()-1);
	}

	tolower(m_strDirPath);
	m_strDirPath = m_strDirPath.substr(0,m_strDirPath.rfind(DIR_FILE_NAME));
	m_strFilter = Filter;	

	m_lstFileName.clear();
	ipkgstream stream;
	stream.open(alias, (m_strDirPath + DIR_FILE_NAME).c_str());
	if(!stream.isopen())
		stream.open(alias, (m_strDirPath + DIR_ROOT_NAME).c_str());

	if(stream.isopen())
	{
		string tempFileName;
		while(!stream.eof())
		{
			tempFileName.clear();
			stream>>tempFileName;
			if( tempFileName.size() == 0 )
				continue;
			Register(tempFileName, m_strFilter);
		}
		stream.close();
	}
	else
	{	
		RegisterStruct RegStruct = { this, m_strFilter };
		if( FAILED(FileTreeWalk(gbk_to_utf16(m_strDirPath).c_str(), RegisterFile, &RegStruct )) )
		{
			wstring tempFileName;
			if( '/'!=m_strDirPath[0] ||  '\\'!=m_strDirPath[0])
				tempFileName = L"/" + gbk_to_utf16(m_strDirPath);			
			
			CPkgFile::SearchPathList pathList = CPkgFile::GetSearchList(alias);
			for ( CPkgFile::SearchPathList::iterator it = pathList.begin(); it != pathList.end(); ++it )
			{
				wstring tPath = *it + tempFileName;
				if( SUCCEEDED(FileTreeWalk(tPath.c_str(), RegisterFile, &RegStruct )) )
					break;
			}
		}
	}
	m_strDirPath += "/";
	return GetFileNum();
}

void CPkgDir::GetFileName(uint32 Index,  string& str)
{
	str = m_lstFileName[Index];
}

void  CPkgDir::GetFullFileName(uint32 Index,  string& str)
{
	str = m_strDirPath + m_lstFileName[Index];
}
