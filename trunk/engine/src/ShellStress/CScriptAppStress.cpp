#include "stdafx.h"
#include "CScript.h"
#include "CScriptAppStress.h"
#include "CAppStress.h"
#include "TBaseScriptApp.inl"
#include "ErrLogHelper.h"
#include "PkgStream.h"
#include "CAppConfigStress.h"
#include "CLangCfg.h"
#include "CodeCvs.h"
#include "TableFileHelper.h"

template class TBaseScriptApp<CScriptAppStress>;

CScriptAppStress::CScriptAppStress(const char* szConfig)
{
	ipkgstream strmConfig(L"", szConfig);
	if(!strmConfig)
		GenErr("Can't open Stress Config file.");

	new CAppConfigStress( strmConfig );
	
 	// 根据当前语言设置，设置配置表文本搜索路径
 	wstring szLangFolder = gbk_to_utf16("chinese_simple");
	SetCurLangPath("chinese_simple");
	SetOriLangPath("chinese_simple");
	
 	wstring szCfgLan = szLangFolder + L"/table";
 	AddScriptLoadPath("lan", utf16_to_utf8(szCfgLan).c_str());
 	CPkgFile::AddLoadPath(L"lan", GetLangFilePath());

	CreateMainVM();

	RegisterCoreScript();
}

CScriptAppStress::~CScriptAppStress(void)
{
	delete CAppConfigStress::Inst();
}


string CScriptAppStress::Run( const char* szFileName)
{
	string sResult;

	SQR_TRY
	{
		CAppStress::Init();

		stringstream strm;
		strm<<"engine_require(\"stress/CoreStress\")\n"
			"CoreStress.Run(\""<<szFileName<<"\")";

		sResult=GetScript()->RunString(strm.str().c_str(),NULL);
		GetScript()->GCAll();

		CAppStress::Unit();
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
		sResult=exp.ErrorMsg();
	}
	SQR_TRY_END;

	return sResult;
}

void CScriptAppStress::RegisterCoreScript()
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

	if( !RegisterCoreStress(*pScript) )
		GenErr("RegisterClient failed");
}
