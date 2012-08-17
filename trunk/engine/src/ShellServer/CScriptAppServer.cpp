#include "stdafx.h"
#ifndef _WIN32
#include <stdio.h>
#include <sys/file.h>
#endif
#include "CLog.h"
#include "ErrLogHelper.h"
#include "ThreadHelper.h"
#include "CScriptAppServer.h"
#include "CAppServer.h"
#include "CScript.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CTimeCheckPoint.h"
#include "TBaseScriptApp.inl"
#include "CAppConfigServer.h"
#include "CScriptThreadMgr.h"
#include "PkgStream.h"
#include "CodeCvs.h"
#include "DebugHelper.h"
#include "CFileWatcherThread.h"
#include "CFileWatcherServerAdapter.h"
#include "CInstanceConfigServer.h"
#include "TScriptAppGCTick.inl"
#include "CMegaLog.h"
#include "CLogThreadMgr.h"
#include "CCallBackProfiler.h"

class CGCStepTickTraitsServer
{
public:
	typedef CAppServer						App_t;
	typedef CAppConfigServer				AppConfig_t;
};

typedef TScriptAppGCTick<CGCStepTickTraitsServer>		CGCStepTickServer;


template class TBaseScriptApp<CScriptAppServer>;

#ifdef _WIN32
#define USE_SHELL_LOG
#else
#define USE_SHELL_LOG
#endif

CScriptAppServer::CScriptAppServer(const char* szConfig, const string& strInstancePath, const string& strInstanceName)
: m_pShellLog(NULL)
{
#ifndef _WIN32
	string strFileName = "GameServer" + strInstanceName + ".pid";
	string strPathName;
	strPathName.append(utf16_to_utf8(GetLogPath()).c_str()).append("/").append(strFileName.c_str());

	m_fdLock = open( strPathName.c_str(),O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH );
	if (m_fdLock)
	{
		if ( flock(m_fdLock, LOCK_EX|LOCK_NB) )
		{
			cerr << "File: " << strFileName.c_str() << " can't be locked, start service fail, errno is " << errno 
				<< " m_fdLock is :" << m_fdLock << endl;
			exit(0);
		}
	}
	else
	{
		cerr << "File: " << strFileName.c_str() << " can't be opened, start service fail, errno is " << errno << endl;
		exit(0);
	}
	char szPid[32];
	int nLen=sprintf( szPid, "%d", GetCurPID() );
	write( m_fdLock, szPid,nLen );
	ftruncate( m_fdLock, nLen );
	fsync( m_fdLock );

#endif
	CLogThreadMgr::Init();

	ipkgstream strmConfig(L"", szConfig);
	if(!strmConfig)
		GenErr("Can't open Gas Config file.");
	
	new CAppConfigServer( strmConfig );	

	new CInstanceConfigServer( strInstancePath, strInstanceName );

	//如果打开了pkg file检查开关，同时存在pkg文件，那么我们可以写log
	if(CAppConfigServer::Inst()->PkgFileUsed())
	{
		SetErrLogEnabled(PkgFileUsed());
	}
	else
	{
		//如果没打开开关，那么一直可以写log
		SetErrLogEnabled(true);
	}

	// 根据当前语言设置，设置配置表文本搜索路径
	wstring szLangFolder = utf8_to_utf16(CAppConfigServer::Inst()->GetLanFolder());

	wstring szCfgLan = szLangFolder + L"/table";
	AddScriptLoadPath("lan", utf16_to_utf8(szCfgLan).c_str());

	CMegaLog::Init();

	CreateMainVM();

	RegisterCoreScript();
	
	CAppServer::Init();
	
	CScriptThreadMgr::Init();

	CreateAllChildVM();
}

CScriptAppServer::~CScriptAppServer(void)
{
	ReleaseAllChildVM();

	CScriptThreadMgr::Unit();

	CAppServer::Unit();
	//delete CAsynServerTrMsgBufferSwapper::Inst();

	CMegaLog::Unit();

	delete CInstanceConfigServer::Inst();

	delete CAppConfigServer::Inst();

	CLogThreadMgr::Unit();

#ifndef _WIN32
	flock(m_fdLock,LOCK_UN);
	close(m_fdLock);
#endif
}


std::string CScriptAppServer::Run( const char* szFileName, const char* szVMFileName )
{	
	string sResult;

	SQR_TRY
	{		
#ifdef _WIN32
		if (CAppConfigServer::Inst()->GetReload())
		{
			CFileWatcherThread::Init(new CFileWatcherServerAdapter(),GetCfgFilePath(""), GetLangFilePath(), GetRootFilePath("/game"));
		}
#endif
		//启动child vm
		CScriptThreadMgr::Inst()->Start(szVMFileName);

		//设置单步gc和全gc的tick
		CScript* pScript = GetScript();
		CGCStepTickServer *pGCTick = new CGCStepTickServer(pScript);
		
		stringstream strm;

		strm<<"engine_require(\"server/CoreServer\")\n"
			"CoreServer.Run(\""<<szFileName<<"\")";

		sResult=GetScript()->RunString(strm.str().c_str(),NULL);
		if (!sResult.empty())
		{
			LogErr(sResult.c_str());
		}

		CScriptThreadMgr::Inst()->Stop();

		CTimeCheckPoint tcp;

		cout << "关闭虚拟机前进行完整垃圾回收";
		GetScript()->GCAll();
		cout << "花费时间" << tcp.GetElapse();

		delete pGCTick;
#ifdef _WIN32
		if (CAppConfigServer::Inst()->GetReload())
		{
			CFileWatcherThread::Unit();
		}
#endif
		CAppServer::Inst()->CleanUp();
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;

	return sResult;
}

void CScriptAppServer::RegisterCoreScript()
{
	CScript* pScript = GetMainVM();
	
	if( !RegisterModule(*pScript) )
		GenErr("RegisterModule failed");

	if( !RegisterCommon(*pScript) )
		GenErr("RegisterCommon failed");

	if( !RegisterCoreCommon(*pScript) )
		GenErr("RegisterServer failed");

	if( !RegisterNetwork(*pScript) )
		GenErr("RegisterNetwork failed");

	if( !RegisterCoreServer(*pScript) )
		GenErr("RegisterServer failed");

	if( !RegisterCoreScriptCommon(*pScript) )
		GenErr("RegisterCoreScriptCommon failed");

	if( !RegisterCoreScriptServer(*pScript) )
		GenErr("RegisterCoreScriptServer failed");
}

CScript* CScriptAppServer::GetChildVM(uint32 uNum)
{
	return CScriptThreadMgr::Inst()->GetScript(uNum);
}

void CScriptAppServer::CreateAllChildVM()
{
	uint32 uVMNum = GetChildVMNum();
	for(uint32 i = 0; i < uVMNum; i++)
	{
		CScript* pScript = CScriptThreadMgr::Inst()->CreateScript();

		SetScriptLoadPath(pScript);

		RegisterCommon(*pScript);
		RegisterCoreCommon(*pScript);
	}
}

void CScriptAppServer::ReleaseAllChildVM()
{
	CScriptThreadMgr::Inst()->ReleaseAllScript();
}

uint32 CScriptAppServer::GetChildVMNum() const
{
	return CInstanceConfigServer::Inst()->GetChildVMNum();
}


void CScriptAppServer::ChangeChildVMFile(CFileWatcherData* pData)
{
	CScriptThreadMgr::Inst()->AddChangeFileJob(pData);
}

void CScriptAppServer::CreateShellLog(const char* szFileName, const char* szLogFormatVer)
{
	if (m_pShellLog)
		return;

	m_pShellLog = CMegaLog::CreateLog(szFileName, szLogFormatVer, CAppConfigServer::Inst()->MegaLogMaxFileSize(), CAppConfigServer::Inst()->MegaLogInterval());
}

void CScriptAppServer::DestroyShellLog()
{
	m_pShellLog->Release();
	m_pShellLog = NULL;
}

CMegaLog* CScriptAppServer::GetShellLog()
{
	return m_pShellLog;
}

void CScriptAppServer::StartCallBackProfiler()
{
	CCallBackProfiler::StartCallBackProfiler();
}

void CScriptAppServer::StopCallBackProfiler()
{
	CCallBackProfiler::StopCallBackProfiler();
}
