#include "stdafx.h"
#include "ScriptMgr.h"
#include "CScriptAppServer.h"
#include "CEditorConfig.h"
#include "CPathMgr.h"
#include <direct.h>
#include "CError.h"
#include "CXmlConfig.inl"
#include <fstream>
#include "Console.h"
#include "BaseHelper.h"
#include "ScriptX.h"
#include "SEPathMgr.h"
#include "CodeCvs.h"
#include "CBaseScriptApp.h"
#include "TSqrAllocator.inl"



class tag_Npc;


CScript*						g_pScript=NULL;
CCache*							g_pCache=NULL;
COriginalData*					g_pOriginalData=NULL;
CScriptAppServer*		gs_ScriptApp = NULL;

using namespace std;
using namespace sqr_tools;

CScript* GetScript()
{
	return g_pScript;
}


CCache&	GetCCache()
{
	return *g_pCache;
}

COriginalData& GetOriginalData()
{
	return *g_pOriginalData;
}

void RunLuaFile(const char* szContext)
{
	SQR_TRY
	{
		GetScript()->RunString(szContext,NULL);
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
	g_pScript->RefScriptObj(pObj);
}


void DelRefFun(CBaseObject* pObj)
{
	g_pScript->UnrefScriptObj(pObj);
}


void Init()
{
#ifdef _DEBUG
	ShowConsole(true);
#endif
	string tempPath = CEditorConfig::GetInst()->GetEditorAppPath() + "/etc/common/Directory.xml";
	ifstream config(tempPath.c_str());
	if(!config)
		GenErr("Can't open Directory.xml\n");

	CXmlConfig *pXmlCfg = new CXmlConfig( "directory" , config );
	config.close();

	const char DEFAULT_SEARCH_PATH_NODE[128] = "default";
 	string setting_path = pXmlCfg->Get<string>(DEFAULT_SEARCH_PATH_NODE, "setting_path");
 	string res_path = pXmlCfg->Get<string>(DEFAULT_SEARCH_PATH_NODE, "res_path");

	CSEPathMgr::Inst().SetArtistPath(res_path.c_str());	
	CSEPathMgr::Inst().SetDesignerPath(setting_path.c_str());

	delete pXmlCfg;
	pXmlCfg = NULL;

	char curPath[1024];
	::GetCurrentDirectory(1024,curPath);
	CSEPathMgr::Inst().SetWorkingPath(curPath);
	CString CurrentPath(curPath);


	_wchdir(L"etc/common");

	gs_ScriptApp = new CScriptAppServer("etc/gas/GasConfig.xml","etc/gas/","1");
	g_pScript= gs_ScriptApp->GetScript();
	
	SetAddRefFunction(&AddRefFun);
	SetDelRefFunction(&DelRefFun);

	g_pOriginalData = new COriginalData();
	g_pCache = new CCache();

	int res = InterfaceExport();
	Ast(res);
	RunLuaFile("sceneEditor_require \"load_npc\"");
}

int32 InterfaceExport()
{
	sqr::CScript& Script = *GetScript();

	REGIST_B_CLASS(Script, COriginalData);
	REGIST_B_CLASS(Script, CCache);
	REGIST_B_CLASS(Script, CPlaceNpc);
	REGIST_B_CLASS(Script, tag_Npc);

	REGIST_CLASSFUNCTION( Script, COriginalData, Exp_AddNpc,"sssCCCCIC:(tag_Npc)");
	REGIST_CLASSFUNCTION( Script, COriginalData, Exp_AddIntObj,"sCC");
	REGIST_CLASSFUNCTION( Script, COriginalData, Exp_AddTrap,"sC");
	REGIST_CLASSFUNCTION( Script, COriginalData, Exp_AddSceneCfg, "ssss:b");	

	REGIST_CLASSFUNCTION( Script, CCache, Exp_AddPlaceNpc,"isff:(CPlaceNpc)");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_AddPlaceObj,"isffi:b");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_AddPlaceTrap,"isffi:b");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_AddAreaLine,"iiis:b");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_AddAreaMusic,"ss:b");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_SetAreaMaxPlayerNum,"si:b");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_SetAreaRatingPlayerNum,"si:b");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_AddDynamicFile,"ss");
	
	REGIST_CLASSFUNCTION( Script, CCache, Exp_SaveCurDynamicPath,"");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_SaveSceneBegin,"s");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_SaveSceneEnd,"");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_SavePathBegin,"siiiiff");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_SavePathNode,"ff");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_SavePathEnd,"");
	REGIST_CLASSFUNCTION( Script, CCache, Exp_SetDynamicCenterPos,"ff");
	

	REGIST_CLASSFUNCTION( Script, CPlaceNpc, Exp_AddPathKeyPos,"ff:b");
	REGIST_CLASSFUNCTION( Script, CPlaceNpc, Exp_SetData,"iiiiiiiiii");

	REGIST_GLOBALFUNCTION(Script, GetCCache, ":(CCache)");
	REGIST_GLOBALFUNCTION(Script, GetOriginalData, ":(COriginalData)");

	return 1;
}

void ReleaseGlobalPtr()
{
	if (g_pScript != NULL)
	{
		delete g_pScript;
		g_pScript = NULL;
	}
	if (g_pCache != NULL)
	{
		delete g_pCache;
		g_pCache = NULL;
	}
	if (g_pOriginalData !=NULL)
	{
		delete g_pOriginalData;
		g_pOriginalData = NULL;
	}
}
