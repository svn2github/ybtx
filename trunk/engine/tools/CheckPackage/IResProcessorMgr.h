#pragma once
class IResProcessorMgr;

#include <string>
//#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include "ExpHelper.h"
#include "CPathMgr.h"
#include "CodeCvs.h"


inline size_t flength( FILE* fp )
{
	long curPos = ftell( fp );
	fseek( fp, 0, SEEK_END );
	long endPos = ftell( fp );
	fseek( fp, curPos, SEEK_SET );
	return endPos;
}

inline void SureChangeToDir(const char* szDir)
{
	if( !_chdir( szDir ) )
		return;

	ostringstream strm;
	char* szCurDir=_getcwd(NULL,0);
	strm<<"Change to dir \""<<szDir<<"\" failed.Current dir is \""<<szCurDir<<"\"";
	free(szCurDir);
	GenErr(strm.str());
}

inline bool CreateFileDirectory( const char* szFileName )
{
	const char* szSperate = max( strrchr( szFileName, '\\' ), strrchr( szFileName, '/' ) );
	if( !szSperate )
		return false;
	
	return MakeDirectory( gbk_to_utf16(string( szFileName, szSperate - szFileName )).c_str() );
}

inline void SureCreateFileDirectory( const char* szFileName )
{
	if( CreateFileDirectory( szFileName ))
		return;

	ostringstream strm;
	char* szCurDir=_getcwd(NULL,0);
	strm<<"CreateFileDirectory \""<<szFileName<<"\" failed.Current dir is \""<<szCurDir<<"\"";
	free(szCurDir);
	GenErr(strm.str());
}

class IResProcessor
{
public:
	virtual ~IResProcessor(){}
	virtual void			Init( IResProcessorMgr* ) = 0;
	virtual bool			RunProcess( const char* szFilename ) = 0;
	virtual size_t			GetCountCanBeProcess() = 0;
	virtual unsigned long	GetTypeCanBeProcess( size_t nNum ) = 0;
};

class IProcessHandler
{
public:
	virtual void			OnProcessed( const char* szFileName, const char* szParentFileName, bool bMiss ) = 0;
};

class IResProcessorMgr
{
public:
	virtual void			PostProcessed( const char* szFileName ) = 0;
	virtual void			ProcessWith( const char* szFileName,const char* szParentFileName ) = 0;
	virtual void			RegisterProcessor( IResProcessor* pProcessor, int32 nPriority ) = 0;
	virtual const string&	GetSrcDir() = 0;
	virtual const string&	GetDstDir() = 0;
	virtual void 			SetSrcDir(const char*) = 0;
	virtual void 			SetDstDir(const char*) = 0;
	virtual void 			ProcessPath() = 0;
	virtual void 			Process( IProcessHandler* pHandler ) = 0;
	virtual void 			SetCompileLua(bool flag) = 0;
	virtual bool 			GetCompileLua() = 0;
};

class CBaseResProcessor : public IResProcessor
{
protected:
	IResProcessorMgr*		m_pMgr;

public:
	CBaseResProcessor() : m_pMgr(NULL){};
	virtual ~CBaseResProcessor(){};

	IResProcessorMgr*		GetMgr()
	{
		return m_pMgr;
	}

	virtual void			Init( IResProcessorMgr* pMgr )
	{
		m_pMgr = pMgr; 
	}

	void ChangeToSrcDir()
	{
		SureChangeToDir(m_pMgr->GetSrcDir().c_str());
	}

	FILE* SureOpenFile( const char* szFileName, const char* szFlag )
	{
		FILE* fp = fopen( szFileName, szFlag );
		if( fp )
			return fp;

		char* szCurDir=_getcwd(NULL,0);
		ostringstream strm;
		strm<<"Open file \""<<szFileName<<"\" failed.Current dir is \""<<szCurDir<<"\"";
		free(szCurDir);
		GenErr(strm.str());
		return NULL;
	}

	virtual bool			RunProcess( const char* szFilename )
	{
		m_pMgr->PostProcessed(szFilename);
		return true;
	}

	virtual size_t			GetCountCanBeProcess() = 0;
	virtual unsigned long	GetTypeCanBeProcess( size_t nNum ) = 0;
};