#include "stdafx.h"
#include "CScript.h"
#include "CScriptAppClient.h"
#include "CAppClient.h"
#include "TBaseScriptApp.inl"
#include "ErrLogHelper.h"
#include <iostream>
#include "CAppConfigClient.h"
#include "TScriptAppGCTick.inl"
#include "PkgStream.h"
#include "CLangCfg.h"
#include "CFontCfg.h"
#include "CFileWatcherThread.h"
#include "CFileWatcherClientAdapter.h"
#include "CComputerInfo.h"
#include "CLogThreadMgr.h"

class CGCStepTickTraitsClient
{
public:
	typedef CAppClient						App_t;
	typedef CAppConfigClient				AppConfig_t;
};

typedef TScriptAppGCTick<CGCStepTickTraitsClient>		CGCStepTickClient;


template class TBaseScriptApp<CScriptAppClient>;

CScriptAppClient::CScriptAppClient( const char* szConfig, const char* szGfkConfig)
{
	CLogThreadMgr::Init();

	ipkgstream strmGacConfig(L"", szConfig);
	if(!strmGacConfig)
		GenErr("Can't open Gac Config file.");
	new CComputerInfo();

	ErrLogHeadMsg("OS",CComputerInfo::GetInst()->GetOSInfo().c_str());
	ErrLogHeadMsg("HD",CComputerInfo::GetInst()->GetHDInfo().c_str());
	ErrLogHeadMsg("CPU",CComputerInfo::GetInst()->GetCpuInfo().c_str());
	//win7 下有bug暂时屏蔽
	//ErrLogHeadMsg("VIDEO",CComputerInfo::GetInst()->GetAudioCardInfo().c_str());
	ErrLogHeadMsg("DX",CComputerInfo::GetInst()->GetDirectInfo().c_str());
	ErrLogHeadMsg("MEM",CComputerInfo::GetInst()->GetMemInfo().c_str());

	new CAppConfigClient( strmGacConfig, szGfkConfig );

	//如果打开了pkg file检查开关，同时存在pkg文件，那么我们可以写log
	if(CAppConfigClient::Inst()->PkgFileUsed())
	{
		SetErrLogEnabled(PkgFileUsed());
	}
	else
	{
		//如果没打开开关，那么一直可以写log
		SetErrLogEnabled(true);
	}

	// 添加以及gui文字图片路径包名映射，将gui图片资源包名映射成当前语言包包名
	wstring szLangFolder = utf8_to_utf16(CLangCfg::GetInst()->GetLanFolder(0));
	wstring szOriLangFolder = utf8_to_utf16(CLangCfg::GetInst()->GetOriLanFolder());
	if ( m_bPkgFileUsed )
	{
		CPkgFile::AddMapPkgPreName(L"guitex", szLangFolder.c_str());
		CPkgFile::AddMapPkgPreName(L"guitex", szOriLangFolder.c_str());
	}

	wstring szGUITexPath = GetLangFilePath() + wstring(L"/") + szLangFolder;
	wstring szOriGUITexPath = GetLangFilePath() + wstring(L"/") + szOriLangFolder;
	CPkgFile::AddLoadPath(L"guitex", szGUITexPath.c_str());
	CPkgFile::AddLoadPath(L"guitex", szGUITexPath.c_str());
	CPkgFile::AddLoadPath(L"guitex", CPkgFile::GetLoadPath(L"res"));

	// 加载字体配置文件
	CFontCfg::GetInst()->LoadFongConfig(CLangCfg::GetInst()->GetLanFolder(0));
	
	// 根据当前语言设置，设置配置表文本搜索路径
	
	wstring szCfgLan = szLangFolder + L"/table";
	AddScriptLoadPath("lan", utf16_to_utf8(szCfgLan).c_str());

	CreateMainVM();

	RegisterCoreScript();

	CAppClient::Init();
}

CScriptAppClient::~CScriptAppClient(void)
{
	CAppClient::Unit();

	delete CComputerInfo::GetInst();
	delete CAppConfigClient::Inst();

	CLogThreadMgr::Unit();
}

string CScriptAppClient::Run( const char* szFileName )
{
	string sResult;

	SQR_TRY
	{
#ifdef _WIN32
		if (CAppConfigClient::Inst()->GetReload())
		{
			CFileWatcherThread::Init(new CFileWatcherClientAdapter(), GetCfgFilePath(""), GetLangFilePath(), GetRootFilePath("/game"));
		}
#endif
		//设置单步gc和全gc的tick
		CScript* pScript = GetScript();
		CGCStepTickClient* pGCTick = new CGCStepTickClient(pScript);

		stringstream strm;
		strm<<"engine_require(\"client/CoreClient\")\n"
			"CoreClient.Run(\""<<szFileName<<"\")";

		sResult=GetScript()->RunString(strm.str().c_str(),NULL);
		if (!sResult.empty())
		{
			LogErr(sResult.c_str());
		}

		GetScript()->GCAll();

		delete pGCTick;
#ifdef _WIN32
		if (CAppConfigClient::Inst()->GetReload())
		{
			CFileWatcherThread::Unit();
		}
#endif
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;

	return sResult;
}

void CScriptAppClient::RegisterCoreScript()
{
	CScript* pScript = GetMainVM();

	if( !RegisterModule(*pScript) )
		GenErr("RegisterModule failed");

	if( !RegisterCommon(*pScript) )
		GenErr("RegisterCommon failed");

	if( !RegisterThink(*pScript) )
		GenErr("RegisterThink failed");

	if( !RegisterGraphic(*pScript) )
		GenErr("RegisterGraphic failed");

	if( !RegisterGUI(*pScript) )
		GenErr("RegisterGUI failed");

	if( !RegisterNetwork(*pScript) )
		GenErr("RegisterNetwork failed");

	if( !RegisterScene(*pScript) )
		GenErr("RegisterScene failed");

	if( !RegisterCoreCommon(*pScript) )
		GenErr("RegisterCoreCommon failed");

	if( !RegisterCoreClient(*pScript) )
		GenErr("RegisterClient failed");

	if( !RegisterCoreScriptCommon(*pScript) )
		GenErr("RegisterCoreScriptCommon failed");
}
