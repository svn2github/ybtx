#include "stdafx.h"
#include "ExpHelper.h"
#include "BaseTypes.h"
#include "ScriptX.h"
#include "CPathMgr.h"
#include <direct.h>
#include "Console.h"
#include "BaseHelper.h"
#include "CCharacter.h"
#include "ScriptMgr.h"
#include "CEditorConfig.h"
#include "CScript.h"
#include "CDataCache.h"
#include "CBaseScriptApp.h"
#include "CodeCvs.h"
#include "TSqrAllocator.inl"
#include "PathBrowser\PathBrowser.h"
#include "CReadCfg.h"
#include "CScriptAppServer.h"

CScript*						gs_pScript=NULL;
CDataCache*						gs_pCahce =NULL;
CBaseScriptApp*		gs_ScriptApp = NULL;

using namespace sqr_tools;

CScript* GetScript()
{
	return gs_pScript;
}

CDataCache*	GetCCache()
{
	return gs_pCahce;
}

void RunLuaFile(const TCHAR* szContext)
{
	SQR_TRY
	{
		//GetScript()->RunString(szContext,NULL);
		string strResult  =	GetScript()->RunFile(szContext,NULL);
		if (strResult != "")
		{
			MessageBox(0,"Please Set ResourceWorkPath","NOTE",NULL);
		}
		//GetScript()->GCAll();
	}
	SQR_CATCH(exp)
	{
		cout<<exp.ErrorMsg()<<endl;
	}
	SQR_TRY_END;
}

void AddRefFun(CBaseObject* pObj)
{
	gs_pScript->RefScriptObj(pObj);
}


void DelRefFun(CBaseObject* pObj)
{
	gs_pScript->UnrefScriptObj(pObj);
}

class CToolScriptApp : public CScriptAppServer//public CBaseScriptApp
{
public:
	friend string GetCfgAlias();
	string GetCfgAlias();
	//CToolScriptApp() {
	//	CreateMainVM();
	//}
	CToolScriptApp(const char* szConfig, const string& strInstancePath, const string& strInstanceName)
		: CScriptAppServer(szConfig, strInstancePath, strInstanceName)
	{

	}
};

string GetCfgAlias()
{
	CToolScriptApp* app = dynamic_cast<CToolScriptApp*>(gs_ScriptApp);
	return  app->GetCfgAlias();
}

string CToolScriptApp::GetCfgAlias()
{
	//CToolScriptApp* app = dynamic_cast<CToolScriptApp*>(gs_ScriptApp);
	return m_ScriptLoadPath["cfg"];
}


void Init()
{
	_wchdir(L"etc/common");
	//gs_ScriptApp = new CToolScriptApp;
	gs_ScriptApp = new CToolScriptApp("etc/gas/GasConfig.xml","etc/gas/","1");
	gs_pScript= gs_ScriptApp->GetScript();
	gs_pCahce =new CDataCache;

	SetAddRefFunction(&AddRefFun);
	SetDelRefFunction(&DelRefFun);
	
	//string tempPath = CEditorConfig::GetInst()->GetEditorAppPath() + "/etc/Directory.xml";
	//ifstream config(tempPath.c_str());//tools/SceneEditor/etc/Directory.xml etc/common/Directory.xml
	//if(!config)
	//	GenErr("Can't open Directory.xml\n");
	//
	//tempPath = CEditorConfig::GetInst()->GetEditorAppPath() + "/etc/SearchDir.xml";
	//ifstream SearchConfig(tempPath.c_str());//tools/SceneEditor/etc/SearchDir.xml etc/common/SearchDir.xml
	//if(!SearchConfig)
	//	GenErr("Can't open SearchDir.xml");	

	//TiXmlDocument doc, SearchDir;
	//config>>doc;
	//SearchConfig>>SearchDir;

	//ostringstream strmPath;
	////产生长串的搜索路径字符串

	//TiXmlNode* pDirectory = doc.FirstChild("directory");
	//if(pDirectory)
	//{
	//	CPathMgr ScriptPathMgr(gbk_to_utf16(pDirectory->FirstChild("script_path")->ToElement()->GetText()).c_str());
	//	TiXmlNode* pSearchDir = SearchDir.FirstChild("directory");
	//	if(pDirectory)
	//	{
	//		for(TiXmlNode* Path=pSearchDir->FirstChild("script_path")->FirstChild("path");
	//			Path;Path=Path->NextSibling())
	//		{
	//			if (!Path->ToElement())
	//				continue;
	//			ScriptPathMgr.SetCurPath(gbk_to_utf16(Path->ToElement()->GetText()).c_str());
	//			strmPath << utf16_to_utf8(ScriptPathMgr.GetCurPath()) << ";";
	//		}
	//	}

	//	CPathMgr SettingPathMgr(gbk_to_utf16(pDirectory->FirstChild("setting_path")->ToElement()->GetText()).c_str());
	//	strmPath << utf16_to_utf8(SettingPathMgr.GetCurPath()) << "/?.lua;";

	//	GetScript()->SetSearchPath(strmPath.str().c_str());
	//}

	//设置策划配置文件的搜索路径


//#ifdef _DEBUG
	ShowConsole(true);
//#endif

	int res = InterfaceExport();
	Ast(res);
	string strCurPath = gs_ScriptApp->GetRootFilePath(NULL);
	strCurPath = utf8_to_gbk(strCurPath);
	strCurPath = strCurPath + "/" + "lua/LoadModelRes";
	//strCurPath = "\"" + strCurPath + "\"";
	//strCurPath = "require " + strCurPath;
	//RunLuaFile("require \"lua/LoadModelRes\"");
#ifndef DEBUG
	EnableCatchError(true);
#endif
	if (gs_ScriptApp->GetIsCfgPkgUsed())
	{

	}
	CReadCfg table;
	table.InitData();
	RunLuaFile(strCurPath.c_str());
}

int32 InterfaceExport()
{
	sqr::CScript& Script = *GetScript();

	REGIST_B_CLASS(Script,CDataCache);
	REGIST_B_CLASS(Script,CModelPiece);
	REGIST_B_CLASS(Script,CCharacter);
	REGIST_B_CLASS(Script,CPlayerModelPart);

	REGIST_CLASSFUNCTION(Script,CCharacter,SetName,"s");
	REGIST_CLASSFUNCTION(Script,CCharacter,SetScaling,"f");
	REGIST_CLASSFUNCTION(Script,CCharacter,SetAniFileName,"s");
	REGIST_CLASSFUNCTION(Script,CCharacter,AddModelPiece,"(CModelPiece)");
	REGIST_CLASSFUNCTION(Script,CCharacter,SetModPiece,"(CModelPiece)");

	REGIST_CLASSFUNCTION(Script,CModelPiece,SetIndexName,"s");
	REGIST_CLASSFUNCTION(Script,CModelPiece,SetModelFileName,"s");
	REGIST_CLASSFUNCTION(Script,CModelPiece,SetPieceName,"s");
	REGIST_CLASSFUNCTION(Script,CModelPiece,SetRenderStyle,"s");
	REGIST_CLASSFUNCTION(Script,CModelPiece,AddHidePiece,"s");

	REGIST_CLASSFUNCTION(Script,CDataCache,AddCharacter,"(CCharacter)i");
	REGIST_CLASSFUNCTION(Script,CDataCache,AddNPC,"ss");
	REGIST_CLASSFUNCTION(Script,CDataCache,AddPlayerModelPart,"(CPlayerModelPart)");

	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetName,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetModelFileName,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetAniFileName,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetEffectFileName,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetPieceName,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetRenderStyle,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetHumanMaleHidePiece,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetHumanFeMaleHidePiece,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetDwarfHidePiece,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetElfHidePiece,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetOrcHidePiece,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetDynamic,"s");
	REGIST_CLASSFUNCTION(Script,CPlayerModelPart,SetTransform,"s");
	
	//REGIST_CLASSFUNCTION(Script,CDataCache,)
	REGIST_GLOBALFUNCTION(Script, GetCCache, ":(CDataCache)");
	return 1;
}