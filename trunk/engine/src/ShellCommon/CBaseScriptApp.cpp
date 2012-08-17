#include "stdafx.h"
#include "CBaseScriptApp.h"
#include "BaseHelper.h"
#include "CScript.h"
#include "ExpHelper.h"
#include "CXmlConfig.inl"
#include "CPathMgr.h"
#include <fstream>
#include <stdio.h>
#include "ExpHelper.h"
#include "CPkgFile.h"
#include "PkgStream.h"
#include "ThreadHelper.h"
#include "CallBackHelper.h"
#include "CodeCvs.h"
#include "ErrLogHelper.h"
#include "CThreadWatcherMap.h"
#include <algorithm>
#include "PkgCommon.h"

#if defined _WIN32

#include <direct.h>
//#define _getcwd getcwd

#else

#include <unistd.h>
#define _chdir chdir

#endif

#ifndef _WIN32
#define _chdir chdir
#define _mkdir mkdir
#define _getcwd getcwd
#endif


#define  BUFLEN  1024

const wstring CFG_PKG_NAME = L"config";

HTLSKEY			CBaseScriptApp::m_tlsCfgKey				= 0;
HTLSKEY			CBaseScriptApp::m_tlsStrIndexKey		= 0;
HTLSKEY			CBaseScriptApp::m_tlsStr64IndexKey	= 0;
const wstring	CBaseScriptApp::LANGUAGE_FOLDER_NAME	= L"language";

static FTW_RESULT IsUsingPkgFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void *pContext )
{	
	if( eFlag == _FTW_DIR )
		return _FTW_IGNORE;

	if( eFlag == _FTW_FILE) 
	{
		wstring str(szFileName);
		if(str.find(L".pkg") != wstring::npos)
		{
			*(bool*)pContext = true;
			return _FTW_STOP;
		}
	}
	return _FTW_CONTINUNE;
}

static void ErrorCallBackFunc(CError& exp)
{
	ostringstream strm;
	CScript::GetStack(strm);
	exp.AppendStackInfo(strm.str().c_str());
}

CBaseScriptApp::CBaseScriptApp(void)
{
	m_bCfgPkgFileUsed=false;
	m_bPkgFileUsed=false;
	m_pRootPathMgr=NULL; 
	m_pCfgPathMgr=NULL;
	m_pXMLPathMgr=NULL;

	InitAltSigStack();
	SetAltSigStackEnabled(true);

	CThreadWatcherMap::Inst();

#ifdef _WIN32
	_wchdir(L"../..");
#else
	chdir("../..");
#endif

	LoadPkgFileConfig();

	m_pRootPathMgr = new CPathMgr;

	CPkgFile::InitTLSKey();

	CPkgFile::AddLoadPath(L"", m_pRootPathMgr->GetRootPath());

	SetPkgFileNameMapping();
	
	LoadScriptSearchPathXml();	

	ConfigSearchPath();

	CheckIsPkgFileUsed();
	
	ChangeCurPathToRes();

	ms_pInst=this;
}


CBaseScriptApp::~CBaseScriptApp(void)
{
	ms_pInst=NULL;

	ReleaseMainVM();

	if ( m_pRootPathMgr )
	{
#ifdef _WIN32
		_wchdir(m_pRootPathMgr->GetRootPath());
#else
		chdir( utf16_to_utf8(m_pRootPathMgr->GetRootPath()).c_str() );
#endif
		delete m_pRootPathMgr;
		m_pRootPathMgr = NULL;
	}

	delete m_pCfgPathMgr;
	m_pCfgPathMgr = NULL;

	delete m_pXMLPathMgr;
	m_pXMLPathMgr = NULL;

	delete m_pLangPathMgr;
	m_pLangPathMgr = NULL;

	delete m_pGUIPathMgr;
	m_pGUIPathMgr = NULL;

	delete m_pResPathMgr;
	m_pResPathMgr = NULL;

	CPkgFile::UnitTLSKey();

	SetAltSigStackEnabled(false);
	UnitAltSigStack();
}

CScript* CBaseScriptApp::GetScript()
{
	return CScript::Inst();
}


void CBaseScriptApp::AddRefFun(CBaseObject* pObj)
{
	CScript* pScript=Inst()->GetScript();
	if (!pScript)
		return;
	pScript->RefScriptObj(pObj);
}


void CBaseScriptApp::DelRefFun(CBaseObject* pObj)
{
	CScript* pScript=Inst()->GetScript();
	if (!pScript)
		return;
	pScript->UnrefScriptObj(pObj);
}


void CBaseScriptApp::DisbindFun(CBaseObject* pObj)
{
	CScript* pScript=Inst()->GetScript();
	if (!pScript)
		return;
	pScript->UnlinkCppObj(pObj);
}

void CBaseScriptApp::AddPvRefFun(void* pObj)
{
	CScript* pScript=Inst()->GetScript();
	if (!pScript)
		return;
	pScript->RefScriptObj(pObj);
}

void CBaseScriptApp::DelPvRefFun(void* pObj)
{
	CScript* pScript=Inst()->GetScript();
	if (!pScript)
		return;
	pScript->UnrefScriptObj(pObj);
}

void CBaseScriptApp::DisbindPvFun(void* pObj)
{
	CScript* pScript=Inst()->GetScript();
	if (!pScript)
		return;
	pScript->UnlinkCppObj(pObj);
}

const char* CBaseScriptApp::GetCfgFilePath(const char* szRelaFilePath) const
{	
	if ( !szRelaFilePath )
		m_szCfgFilePath = utf16_to_utf8(m_pCfgPathMgr->GetFullPath(NULL)).c_str();
	else
		m_szCfgFilePath = utf16_to_utf8(m_pCfgPathMgr->GetFullPath(utf8_to_utf16(szRelaFilePath).c_str()));

	return m_szCfgFilePath.c_str();
}

const char* CBaseScriptApp::GetRootFilePath(const char* szRelaFilePath) const
{
	if ( !szRelaFilePath )
		m_szRootFilePath = utf16_to_utf8(m_pRootPathMgr->GetFullPath(NULL));
	else
		m_szRootFilePath = utf16_to_utf8(m_pRootPathMgr->GetFullPath(utf8_to_utf16(szRelaFilePath).c_str()));

	return m_szRootFilePath.c_str();
}

const char* CBaseScriptApp::GetXMLFilePath(const char* szRelaFilePath) const
{
	if ( !szRelaFilePath )
		m_szXMLFilePath = utf16_to_utf8(m_pXMLPathMgr->GetFullPath(NULL));
	else
		m_szXMLFilePath = utf16_to_utf8(m_pXMLPathMgr->GetFullPath(utf8_to_utf16(szRelaFilePath).c_str()));

	return m_szXMLFilePath.c_str();
}

const wchar_t* CBaseScriptApp::GetLangFilePath()
{
	return m_pLangPathMgr->GetRootPath();
}

const wchar_t* CBaseScriptApp::GetGUIFilePath()
{
	return m_pGUIPathMgr->GetRootPath();
}


CBaseScriptApp* CBaseScriptApp::ms_pInst=NULL;

CBaseScriptApp* CBaseScriptApp::Inst()
{
	return ms_pInst;
}

CScript* CBaseScriptApp::GetMainVM()
{
	return GetScript();	
}

CScript* CBaseScriptApp::CreateMainVM(float fCallbackGCRatio,float fNormalUnpackGCRatio,float fSerialUnpackGCRatio)
{
	InitCfgTableTlsKey();
	InitCfgTableTlsValue();

	InitStrIndexTlsKey();
	InitStrIndexTlsValue();

	InitStr64IndexTlsKey();
	InitStr64IndexTlsValue();	

	if ( CatchErrorEnabled() )
	{
		CScript::Init(eSH_None,fCallbackGCRatio,fNormalUnpackGCRatio,fSerialUnpackGCRatio);
	}
	else
	{
		CScript::Init(eSH_Debuger,fCallbackGCRatio,fNormalUnpackGCRatio,fSerialUnpackGCRatio);
	}

	SetAddRefFunction(&AddRefFun);
	SetDelRefFunction(&DelRefFun);
	SetDisbindFunction(&DisbindFun);

	SetAddPvRefFunction(&AddPvRefFun);
	SetDelPvRefFunction(&DelPvRefFun);
	SetDisbindPvFunction(&DisbindPvFun);


	SetCallBackDataFunc(&GetCallBackData);
	SetCallBackAttrFunc(&GetCallBackAttr);
	SetClassCallBackFunc(&CallClassCallBack);

	SetErrorCallBackFunc(ErrorCallBackFunc);

	CScript* pScript = GetScript();

	SetScriptLoadPath(pScript);

	return pScript;
}

void CBaseScriptApp::ReleaseMainVM()
{
	UnInitCfgTableTlsValue();
	UnInitCfgTableTlsKey();

	UnInitStrIndexTlsValue();
	UnInitStrIndexTlsKey();

	UnInitStr64IndexTlsValue();
	UnInitStr64IndexTlsKey();

	SetAddPvRefFunction(NULL);
	SetDelPvRefFunction(NULL);
	SetDisbindPvFunction(NULL);

	SetAddRefFunction(NULL);
	SetDelRefFunction(NULL);
	SetDisbindFunction(NULL);

	SetCallBackDataFunc(NULL);
	SetCallBackAttrFunc(NULL);
	SetClassCallBackFunc(NULL);

	SetErrorCallBackFunc(NULL);

	CScript::Unit();
}

void* CBaseScriptApp::GetCallBackData(CBaseObject* pObject, const char* szFunName)
{
	CScript* pScript=Inst()->GetScript();
	if (!pScript)
		return NULL;

	return pScript->GetCallBackData(pObject, szFunName);
}

void  CBaseScriptApp::GetCallBackAttr(void* pCall, size_t& stBufSize, size_t*& pParamOffset, 
									  size_t& iRetOffset)
{
	CScript* pScript=Inst()->GetScript();
	if (!pScript)
		return;

	pScript->GetCallBackAttr(pCall, stBufSize, pParamOffset, iRetOffset);
}

bool CBaseScriptApp::CallClassCallBack(void* pCall, CBaseObject* pObject, char* pDataBuf)
{
	CScript* pScript=Inst()->GetScript();
	if (!pScript)
		return false;

	return pScript->CallClassCallBack(pCall, pObject, pDataBuf);
}

void CBaseScriptApp::InitCfgTableTlsValue()
{
	string *pString = new string;
	TLS_SetValue(m_tlsCfgKey, pString);
}

void CBaseScriptApp::UnInitCfgTableTlsValue()
{
	string *pString = (string*)TLS_GetValue(m_tlsCfgKey);
	delete pString;
	pString = 0;
	TLS_SetValue(m_tlsCfgKey, 0);
}

void CBaseScriptApp::InitCfgTableTlsKey()
{
	TLS_CreateKey(&m_tlsCfgKey);
}

void CBaseScriptApp::UnInitCfgTableTlsKey()
{
	TLS_DestroyKey(m_tlsCfgKey);
}

string* CBaseScriptApp::GetCfgTableTlsData()
{
	return (string*)TLS_GetValue(m_tlsCfgKey);
}

CXmlConfig* CBaseScriptApp::LoadCfgFile( const string& cfg_file_name, const string& root_name )
{
	ipkgstream file_strm(L"", cfg_file_name.c_str());
	if (!file_strm)
		GenErr(string("Can't open ") + cfg_file_name);
	CXmlConfig *pXmlCfg = new CXmlConfig( root_name.c_str() , file_strm );
	file_strm.close();
	return pXmlCfg;
}

void CBaseScriptApp::SetPkgFileNameMapping()
{
	// 设置包名映射表
	//下面这段映射代码是不应该存在于这里的。具体有哪些包是由具体的项目决定的，引擎是不应该知道的
	//将来要把这些代码移到游戏逻辑去。
	CPkgFile::AddMapPkgName(L"cfg", CFG_PKG_NAME.c_str());
	CPkgFile::AddMapPkgName(L"engine", L"library");
	CPkgFile::AddMapPkgName(L"etc", L"other");
	CPkgFile::AddMapPkgName(L"game", L"logic");
	CPkgFile::AddMapPkgName(L"gui", L"gui");
	CPkgFile::AddMapPkgName(L"character", L"role");
	CPkgFile::AddMapPkgName(L"font", L"font");
	CPkgFile::AddMapPkgName(L"fx", L"effect");
	CPkgFile::AddMapPkgName(L"map", L"terrain");
	CPkgFile::AddMapPkgName(L"object", L"item");
	CPkgFile::AddMapPkgName(L"tile", L"scene");
	CPkgFile::AddMapPkgName(L"guitex", L"guires");
}

void CBaseScriptApp::InitStrIndexTlsValue()
{
	string *pString = new string;
	TLS_SetValue(m_tlsStrIndexKey, pString);
}

void CBaseScriptApp::UnInitStrIndexTlsValue()
{
	string *pString = (string*)TLS_GetValue(m_tlsStrIndexKey);
	delete pString;
	pString = 0;
	TLS_SetValue(m_tlsStrIndexKey, 0);
}

string* CBaseScriptApp::GetStrIndexTlsData()
{
	return (string*)TLS_GetValue(m_tlsStrIndexKey);
}

void CBaseScriptApp::InitStrIndexTlsKey()
{
	TLS_CreateKey(&m_tlsStrIndexKey);
}

void CBaseScriptApp::UnInitStrIndexTlsKey()
{
	TLS_DestroyKey(m_tlsStrIndexKey);
}

void CBaseScriptApp::InitStr64IndexTlsValue()
{
	string *pString = new string;
	TLS_SetValue(m_tlsStr64IndexKey, pString);
}
void CBaseScriptApp::UnInitStr64IndexTlsValue()
{
	string *pString = (string*)TLS_GetValue(m_tlsStr64IndexKey);
	delete pString;
	pString = 0;
	TLS_SetValue(m_tlsStr64IndexKey, 0);
}
string* CBaseScriptApp::GetStr64IndexTlsData()
{
	return (string*)TLS_GetValue(m_tlsStr64IndexKey);
}

void CBaseScriptApp::InitStr64IndexTlsKey()
{
	TLS_CreateKey(&m_tlsStr64IndexKey);
}

void CBaseScriptApp::UnInitStr64IndexTlsKey()
{
	TLS_DestroyKey(m_tlsStr64IndexKey);
}

bool CBaseScriptApp::PkgFileUsed()
{
	return m_bPkgFileUsed;
}

void CBaseScriptApp::LoadScriptSearchPathXml()
{
	CXmlConfig*		pXmlSearchDir	= LoadCfgFile( "etc/common/SearchDir.xml", "search_dir" );

	TiXmlNode*		pLoadPath = pXmlSearchDir->Get<TiXmlElement*>("load_path");

	const wchar_t* szRootPath = m_pRootPathMgr->GetRootPath();

	TiXmlNode* pNode = pLoadPath->FirstChild("path"); 
	while(pNode)
	{
		TiXmlElement* pElement = pNode->ToElement();
		if (pElement)
		{
			TiXmlAttribute* attribute = pElement->FirstAttribute();
			AddScriptLoadPath(attribute->Value(), pElement->GetText());
			wstring alias = utf8_to_utf16(attribute->Value());
			CPkgFile::AddLoadPath(alias.c_str(), szRootPath);
		}
		pNode = pNode->NextSibling();

	}
	
	SafeDelete(pXmlSearchDir);		
}

void CBaseScriptApp::CheckIsPkgFileUsed()
{
	wstring cfg_path	= wstring(m_pRootPathMgr->GetRootPath()) 
						+ L"/" + CPkgFile::PkgFileMapping(L"cfg") + L".pkg";

	bool bResPkgUsed = false;
	FileTreeWalk(m_pResPathMgr->GetCurPath(), IsUsingPkgFile, &bResPkgUsed);

	m_bPkgFileUsed = (m_bCfgPkgFileUsed && bResPkgUsed);
}

void CBaseScriptApp::ChangeCurPathToRes()
{
#ifdef _WIN32
	_wchdir(m_pResPathMgr->GetCurPath());
#else
	chdir( utf16_to_utf8(m_pResPathMgr->GetCurPath()).c_str() );
#endif
}

void CBaseScriptApp::ConfigSearchPath()
{
	const char DEFAULT_SEARCH_PATH_NODE[128] = "default";

	// 加载搜索路径配置文件
	CXmlConfig* pXmlDirectory = LoadCfgFile( "etc/common/Directory.xml", "directory" );
	string setting_path = pXmlDirectory->Get<string>(DEFAULT_SEARCH_PATH_NODE, "setting_path");
	string xml_path = pXmlDirectory->Get<string>(DEFAULT_SEARCH_PATH_NODE, "xml_path");
	string gui_path = pXmlDirectory->Get<string>(DEFAULT_SEARCH_PATH_NODE, "gui_path");
	string lang_path = pXmlDirectory->Get<string>(DEFAULT_SEARCH_PATH_NODE, "lang_path");
	string res_path = pXmlDirectory->Get<string>(DEFAULT_SEARCH_PATH_NODE, "res_path");

	AddLoadPath("cfg", setting_path, true);
	AddLoadPath("gui", gui_path, false);
	AddLoadPath("res", res_path, false);
	AddLoadPath("lan", lang_path, false);
	AddLoadPath("shd", utf16_to_utf8(m_pRootPathMgr->GetRootPath()).c_str(), false);

	// 创建各路径管理器
	m_pCfgPathMgr	= new CPathMgr( setting_path.c_str() );
	m_pXMLPathMgr	= new CPathMgr( xml_path.c_str() );
	m_pGUIPathMgr	= new CPathMgr( gui_path.c_str() );
	m_pLangPathMgr	= new CPathMgr( lang_path.c_str() );
	m_pResPathMgr	= new CPathMgr();
	m_pResPathMgr->SetCurPath( utf8_to_utf16(res_path).c_str() );

	SafeDelete(pXmlDirectory);	
}


void CBaseScriptApp::AddScriptLoadPath(const char* szAliasPath, const char* szPath)
{
	Ast(szAliasPath);
	Ast(szPath);

	m_ScriptLoadPath[szAliasPath] = szPath;
}

void CBaseScriptApp::SetScriptLoadPath(CScript* pScript)
{
	map<string, string>::iterator iter = m_ScriptLoadPath.begin();
	while(iter != m_ScriptLoadPath.end())
	{
		pScript->SetLoadPath(iter->first.c_str(), iter->second.c_str());
		++iter;
	}

}

void CBaseScriptApp::AddLoadPath(const string& szAliasPath, const string& szPath, bool UseScript)
{
	wstring sPath = utf8_to_utf16(szPath);
	replace(sPath.begin(), sPath.end(), L'\\', L'/');
	
	wstring sAliasPath = utf8_to_utf16(szAliasPath);

	//取path的第一个子串，以/分隔的，作为其pkg name
	wstring sPkgName = sPath;
	wstring::size_type pos = sPkgName.find_first_of(L'/');
	if (pos != wstring::npos)
	{
		sPkgName = sPkgName.substr(0, pos);
	}
	
	const wchar_t* szPkgName = CPkgFile::GetPkgName(sPkgName.c_str());

	//如果存在对应的包名，则填入的路径是一个包，我们按照包的方式处理
	if (szPkgName)
	{
		if ( wstring(szPkgName) == CFG_PKG_NAME )
		{
			m_bCfgPkgFileUsed = true;
		}

		//将当前root路径作为其package的alias路径
		CPkgFile::AddLoadPath(sAliasPath.c_str(), m_pRootPathMgr->GetRootPath());

		//如果这项配置被脚本库使用，那么我们将其加入脚本库的搜索路径里面
		if (UseScript)  
		{
			AddScriptLoadPath(szAliasPath.c_str(), szPath.c_str());
		}
	}
	else
	{
		//如果填入的是绝对路径，那么我们直接设置成package的alias
		//如果是相对路径，那么我们和root路径拼接
		if (sPath[1] == L':')
		{
			CPkgFile::AddLoadPath(sAliasPath.c_str(), sPath.c_str());
		}
		else
		{
			wstring sRootPath = m_pRootPathMgr->GetRootPath();
			sRootPath.append(L"/").append(sPath);
			CPkgFile::AddLoadPath(sAliasPath.c_str(), sRootPath.c_str());
		}

		if (UseScript) 
		{
			AddScriptLoadPath(szAliasPath.c_str(), "");
		}
	}
}

void CBaseScriptApp::LoadPkgFileConfig()
{
	/*
	首先尝试打开etc/common/PkgConfig.xml文件，如果没有
	该文件，那么我们pkg只进行包内读取
	如果有该文件，那我们就优先读取包外
	*/

	ifstream fsPkgConfig( "etc/common/PkgConfig.xml", ios::binary );

	if( fsPkgConfig )
	{
		EnableReadPkgOnly(false);
	}
	else
	{
		EnableReadPkgOnly(true);
	}
}


bool CBaseScriptApp::GetIsCfgPkgUsed()
{
	return m_bCfgPkgFileUsed;
}
