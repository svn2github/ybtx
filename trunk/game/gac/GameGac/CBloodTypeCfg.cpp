#include "stdafx.h"
#include "CBloodTypeCfg.h"
#include "CTxtTableFile.h"
#include "CPropertyCfg.h"
#include "CScriptAppClient.h"
#include "TSqrAllocator.inl"
#include "CodeCvs.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CBloodTypeCfg::MapClassBloodType CBloodTypeCfg::ms_mapClassBloodType;

bool CBloodTypeCfg::LoadCfg()
{
	string path;
	if ( CBaseScriptApp::Inst()->GetIsCfgPkgUsed() )
	{
		path = "cfg";
	}
	else
	{
		path  =  utf8_to_gbk(CBaseScriptApp::Inst()->GetCfgFilePath(""));
	}
	string pathPlayer = path + "/player/";
	string pathBloodType = pathPlayer + "BloodType_Client.txt";

	CTxtTableFile TabFile;
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), pathBloodType.c_str())) return false;

	ms_mapClassBloodType.clear();
	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{
		string strClassName = TabFile.GetString(i, "职业");
		EClass eClass			= CPropertyCfg::ms_mapClassType[strClassName];
		EBloodDialogType eType	= (EBloodDialogType)TabFile.GetInteger(i, "血条类型", 0);
		ms_mapClassBloodType.insert(make_pair(eClass, eType));
	}
	return true;
}

void CBloodTypeCfg::UnLoadCfg()
{
	ms_mapClassBloodType.clear();
}

CBloodTypeCfg::~CBloodTypeCfg()
{
	ms_mapClassBloodType.clear();
}

EBloodDialogType CBloodTypeCfg::GetBloodType(EClass eClass)
{
	MapClassBloodType::iterator itr = ms_mapClassBloodType.find(eClass);
	if (itr != ms_mapClassBloodType.end())
	{
		return itr->second;
	}
	return eBDT_Normal;
}