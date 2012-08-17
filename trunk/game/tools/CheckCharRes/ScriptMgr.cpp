#include "stdafx.h"
#include "CPathMgr.h"
#include "direct.h"
#include "ExpHelper.h"
#include "CDomXml.h"
#include "Console.h"
#include "BaseHelper.h"
#include "ScriptX.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>
#include "PropResChecker.h"
#include "ReadSkillCfg.h"

using namespace std;
using namespace sqr;

CScript*						gs_pScript=NULL;
string							g_ResPath;
string							g_SettingCfgPath;

CScript* GetScript()
{
	return gs_pScript;
}

void RunLuaFile(const TCHAR* szContext)
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

void AddRefFun(void* pObj)
{
	gs_pScript->RefScriptObj(pObj);
}


void DelRefFun(void* pObj)
{
	gs_pScript->UnrefScriptObj(pObj);
}

int32 InterfaceExport()
{
	sqr::CScript& Script = *GetScript();

	REGIST_UNSAFE_B_CLASS(Script, PropResChecker);
	REGIST_STATICFUNCTION(Script, PropResChecker, CheckPropResLine, "Isssssssb");
	REGIST_STATICFUNCTION(Script, PropResChecker, CheckWeaponLine, "Issss");
	REGIST_STATICFUNCTION(Script, PropResChecker, BeginCheck, "");
	REGIST_STATICFUNCTION(Script, PropResChecker, EndCheck, "");
	
	REGIST_GLOBALFUNCTION( Script, CheckAllConfig, ":b" );

	return 1;
}

void Init()
{
	_chdir("../../");
	gs_pScript=new CScript(eSH_Debuger);

	//TCHAR path[1024];
	//::GetCurrentDirectory(1024, path);

	//SetAddRefFunction(&AddRefFun);
	//SetDelRefFunction(&DelRefFun);


	ifstream config("tools/CheckCharRes/etc/Directory.xml");//tools/SceneEditor/etc/Directory.xml etc/common/Directory.xml
	if(!config)
	{
		config.clear();
		config.open("etc/common/Directory.xml");
		if(!config)
		{
			GenErr("Can't open Directory.xml");
		}		
	}
	ifstream SearchConfig("tools/CheckCharRes/etc/SearchDir.xml");//tools/SceneEditor/etc/SearchDir.xml etc/common/SearchDir.xml
	if(!SearchConfig)
	{
		SearchConfig.clear();
		SearchConfig.open("etc/common/SearchDir.xml");
		if(!SearchConfig)
		{
			GenErr("Can't open SearchDir.xml");
		}
	}		

	CDomXmlDocument doc("dummyroot"),SearchDir("dummyroot");

	config>>doc;
	SearchConfig>>SearchDir;

	ostringstream strmPath;
	//产生长串的搜索路径字符串
	CPathMgr ScriptPathMgr(doc("script_path").GetText().c_str());
	for(CDomXmlDocument* Path=&SearchDir("script_path")("path");Path;Path=Path->GetNextSibling())
	{
		ScriptPathMgr.SetCurPath(Path->GetText().c_str());
		strmPath<<ScriptPathMgr.GetCurPath()<<";";
	}

	//设置策划配置文件的搜索路径
	string str =  doc("setting_path").GetText();
	printf(str.c_str());
	CPathMgr SettingPathMgr(doc("setting_path").GetText().c_str());
	strmPath<<SettingPathMgr.GetCurPath()<<"/?.lua;";
	g_SettingCfgPath = SettingPathMgr.GetCurPath();

	//设置美术配置文件的搜索路径
	string strRes =  doc("res_path").GetText();
	printf(strRes.c_str());
	CPathMgr ResPathMgr(doc("res_path").GetText().c_str());
	//strmPath<<ResPathMgr.GetCurPath()<<"/?.lua;";
	g_ResPath = ResPathMgr.GetCurPath();

	GetScript()->SetSearchPath(strmPath.str().c_str());

	//#ifdef _DEBUG
	ShowConsole(true);
	//#endif

	int res = InterfaceExport();
	Ast(res);
	RunLuaFile("require \"LoadProperRes\"");
}

