#include "stdafx.h"
#include "CFacialAniCfgMgr.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "BaseHelper.h"
#include "StringHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CFacialAniCfgMgr:: MapFacialAniCfgByName		CFacialAniCfgMgr::ms_mapFacialAniCfgByName;
CFacialAniCfgMgr::MapIndexToNameFicial			CFacialAniCfgMgr::ms_mapIndexToNameFicial;


bool CFacialAniCfgMgr::LoadConfig(const string& szFileName)
{
	stringstream ExpStr;

	CTxtTableFile TabFile;
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) 
		return false;

	ClearMap(ms_mapFacialAniCfgByName);

	for( int i = 1; i < TabFile.GetHeight(); ++i )
	{
		CFacialAniCfgMgr* pFacialAniCfg		= new CFacialAniCfgMgr;
		pFacialAniCfg->m_strFacialAniName		= TabFile.GetString(i, szFacialAni_Name);
		trimend(pFacialAniCfg->m_strFacialAniName);
		pFacialAniCfg->m_strStartAniName		= TabFile.GetString(i, szFacialAniStart_Name);
		trimend(pFacialAniCfg->m_strStartAniName);
		pFacialAniCfg->m_strContinueAniName		= TabFile.GetString(i, szFacialAniContinue_Name);
		trimend(pFacialAniCfg->m_strContinueAniName);
		pFacialAniCfg->m_uIndex	 =		i;
		ms_mapIndexToNameFicial[i] = pFacialAniCfg->m_strFacialAniName;
	
		pair<MapFacialAniCfgByName	::iterator, bool> prByAniNme = ms_mapFacialAniCfgByName.insert(make_pair(pFacialAniCfg->m_strFacialAniName, pFacialAniCfg));
		if(!prByAniNme.second)
		{
			ExpStr << "第" << i << "行的表情动作 " << pFacialAniCfg->m_strFacialAniName << " 错误";
			GenErr(ExpStr.str());
		}
	}
	return true;
}

void CFacialAniCfgMgr::UnloadConfig()
{
	ClearMap(ms_mapFacialAniCfgByName);
}

CFacialAniCfgMgr* CFacialAniCfgMgr::GetFicialAniCfgByName(const TCHAR* actionName)
{
	stringstream ExpStr;
	MapFacialAniCfgByName::iterator itFicialCfg;
	itFicialCfg = ms_mapFacialAniCfgByName.find(actionName);

	if (itFicialCfg == ms_mapFacialAniCfgByName.end())
	{
		ExpStr << "表情动作" << actionName << "不存在";
		GenErr(ExpStr.str());
	}

	return itFicialCfg->second;
}

CFacialAniCfgMgr* CFacialAniCfgMgr::GetFicialAniCfgByIndex(uint8 uIndex)
{
	string strFicialName = ms_mapIndexToNameFicial[uIndex];
	return GetFicialAniCfgByName(strFicialName.c_str());
}

const string& CFacialAniCfgMgr::GetFacialAniName(EActionState eActionState)
{
	if (eAS_Fun_Start == eActionState)
		return GetStartAniName();
	else
		return GetContinueAniName();
}

bool CFacialAniCfgMgr::isHaveFirstAni()
{
	if(GetStartAniName() == "")
		return false;
	else
		return true;
}