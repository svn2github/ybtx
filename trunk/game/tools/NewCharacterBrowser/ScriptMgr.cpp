#include "stdafx.h"
#include "ExpHelper.h"
#include "ScriptX.h"
#include "Console.h"
#include "CBaseScriptApp.h"
#include "CodeCvs.h"
#include "CCharacter.h"
#include "ScriptMgr.h"
#include "CScript.h"
#include "CDataCache.h"
#include "CReadCfg.h"
#include "CCharacter.inl"
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
		GetScript()->RunString(szContext,NULL);
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
	char dir[1024];
	::GetCurrentDirectory(1024, dir);
	gs_ScriptApp = new CToolScriptApp("etc/gas/GasConfig.xml","etc/gas/","1");
	gs_pScript	 = gs_ScriptApp->GetScript();
	gs_pCahce	 =new CDataCache;

	SetAddRefFunction(&AddRefFun);
	SetDelRefFunction(&DelRefFun);

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

void Release()
{
	gs_pScript->GCAll();
	CScript *temp = GetScript();
	SafeDelete(temp);

	SafeDelete(gs_pCahce);
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