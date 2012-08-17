// ResourceCheckerDlg.h : 头文件
//

#pragma once

#include "IResProcessorMgr.h"
#include <map>
#include <set>
#include <string>

using namespace std;

#define MAX_PRIORITY_LEVEL 9

inline uint32 T2n( const char* szType )
{
	uint32 n = 0;
	while( *szType )
		n = ((uint32)(uint8)(*szType++))|( n<<8 );
	return n;
}

class IProcessHandler;
class CResProcessorMgr : public IResProcessorMgr
{
	struct CProcessRegist
	{
		CProcessRegist() : m_pProcessor(NULL){}
		IResProcessor*		m_pProcessor;
		map<string,string>	m_mapFileRepository;
	};

	CProcessRegist			m_ProcessRegist[MAX_PRIORITY_LEVEL+1]; // 存储注册类
	map<uint32,uint32>		m_mapTypeToRegist;
	vector<string>			m_vecProcessDir;
	set<uint32>				m_setExcludeType;
	set<string>				m_setExcludeFile;
	string					m_strSrcPath;
	string					m_strDstPath;
	string					m_strCurProcessFile;
	string					m_strFinalPath;

	bool                    m_bCompileLua;
	bool					m_bNewResFile;

	struct DirFile
	{
		DirFile(bool ir)
			:isRoot(ir){}
		bool		isRoot;
		set<string> FilePool;
	};
	typedef	map<string,DirFile> MapDir;	
	MapDir					m_mapDirStream;

	vector<string>			m_vecSearchDir;
	struct MapDirFile
	{
		MapDir		mapDir;
	};
	map<string,MapDirFile>	m_mapSerDirStream;

	struct SetStr
	{
		set<string> setStr;
	};

	typedef map<string,SetStr>	MapCltFile;
	MapCltFile				m_CollectFiles;

public:
	CResProcessorMgr();
	virtual void			ProcessWith( const char* szFileName, const char* szParentFileName );
	virtual void			RegisterProcessor( IResProcessor* pProcessor, int32 nPriority );
	virtual const string&	GetSrcDir(){ return m_strSrcPath; }
	virtual const string&	GetDstDir(){ return m_strDstPath; }
	virtual const string&	GetFinalDir(){ return m_strFinalPath; }
	virtual void			PostProcessed( const char* szFileName );
	virtual void			AddPrcDir( const char* s );
	virtual void			AddSerDir( const char* s );
	virtual void			SetNewSerFile( const char* s );
	virtual void			InsertCollectFile( string& p, string& s );
	virtual void			AddExType( const char* s ) { m_setExcludeType.insert(T2n(s)); };
	virtual void			AddExFile( const char* s ) { m_setExcludeFile.insert(s); };
	virtual void			SetSrcDir( const char* s ) { m_strSrcPath = s; }
	virtual void 			SetDstDir( const char* s ) { m_strDstPath = s; }
	virtual void			SetFinalDir( const char* s ) { m_strFinalPath = s; }
	virtual void 			ProcessPath();
	virtual void 			Process( IProcessHandler* pHandler );
	virtual void 			SetCompileLua(bool flag);
	virtual bool 			GetCompileLua();

	virtual void			ProcessDir(IProcessHandler* pHandler);
	virtual void			BuildDir(const string& path,bool isRoot = false);
	virtual void			InsertFile(const string& file);

	virtual void			BuildSerDir();
	virtual void			BuildSerDir(MapDir& mapdir, const string& path, bool isRoot = false);
	virtual void			InsertSerFile(MapDir& mapdir, const string& file);
	virtual void			BuildSerLogFile();
};

