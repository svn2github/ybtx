#include "stdafx.h"
#include "CTxtTableFile.h"
#include "CBurstSoulCfgClient.h"
#include "LoadSkillCommon.h"
#include "CCfgCalc.inl"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CBurstSoulCfgClient::MapBurstSoulCfg CBurstSoulCfgClient::ms_mapBurstSoulCfg;

bool CBurstSoulCfgClient::LoadConfig(const TCHAR* cfgFile)
{
	using namespace CfgChk;
	CTxtTableFile TabFile;
	SetTabFile(TabFile, "爆魂");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;
	ClearMap(ms_mapBurstSoulCfg);
	for (int32 i=1;i<TabFile.GetHeight();++i)
	{
		SetLineNo(i);
		CBurstSoulCfgClient* pCfgNode = new CBurstSoulCfgClient();
		ReadItem(pCfgNode->m_sName,		szBurstSoul_DoubleBurst);
		string sBurstSoulTimesLevel;
		ReadItem(sBurstSoulTimesLevel,			szBurstSoul_TimesLevel,	CANEMPTY,	"");
		string::size_type pos = sBurstSoulTimesLevel.find_first_of(',');
		if(pos != string::npos)
		{
			SetValue(pCfgNode->m_pLevel, sBurstSoulTimesLevel.substr(0, pos));
			SetValue(pCfgNode->m_pNeedBurstTimes, sBurstSoulTimesLevel.substr(pos + 1));
		}
		else
		{
			pCfgNode->m_pLevel = NULL;
			pCfgNode->m_pNeedBurstTimes = NULL;
		}

		CBurstSoulCfgClientSharedPtr* pCfgNodeSharedPtr = new CBurstSoulCfgClientSharedPtr(pCfgNode);
		if(!InsertStringSharedPtrMap(ms_mapBurstSoulCfg, pCfgNodeSharedPtr))
		{
			SafeDelete(pCfgNodeSharedPtr);
			continue;
		}
	}
	return true;
}

uint32 CBurstSoulCfgClient::GetNeedBurstSoulTimesByLevel(uint32 uLevel)
{
	MapBurstSoulCfg::iterator itr = ms_mapBurstSoulCfg.begin();
	for (; itr != ms_mapBurstSoulCfg.end(); ++itr)
	{
		if (itr->second->get()->GetLevel() && itr->second->get()->GetLevel()->GetIntValue() == uLevel)
		{
			return itr->second->get()->GetNeedBurstTimes()->GetIntValue();
		}
	}
	GenErr("不存在这个等级的爆魂蓄力值\n");
	return 5000;
}

void CBurstSoulCfgClient::UnloadConfig()
{
	ClearMap(ms_mapBurstSoulCfg);
}

CBurstSoulCfgClient::~CBurstSoulCfgClient()
{
	SafeDelete(m_pLevel);
	SafeDelete(m_pNeedBurstTimes);
}
