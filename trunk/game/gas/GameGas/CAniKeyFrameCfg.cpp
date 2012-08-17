#include "stdafx.h"
#include "CAniKeyFrameCfg.h"
#include "LoadSkillCommon.h"
#include "CCfgColChecker.inl"
#include "NpcInfoMgr.h"
#include "CNpcFightBaseData.h"
#include "BaseHelper.h"
#include "DebugHelper.h"
#include "TimeHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
	extern const wstring GAME_PATH_ALIAS_RES;
}

CAniKeyFrameCfg::MapAniKeyFrameCfg	CAniKeyFrameCfg::ms_mapAniKeyFrameCfg;
CAniKeyFrameCfg::MapNpcName2AniFileName CAniKeyFrameCfg::m_mapNpcName2AniFileName;

bool CAniKeyFrameCfg::LoadAniKeyFrameConfig(const string& szFileName)
{
	using namespace CfgChk;

	CTxtTableFile TabFile;
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) 
		return false;
	SetTabFile(TabFile, "动作关键帧表");

	ClearMap(ms_mapAniKeyFrameCfg);
	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		string strAniFileName;
		ReadItem(strAniFileName, szNT_Name);
		string strAniFile = "server/keyframeinfo/" + strAniFileName;

		CIniFile* pIniFile = new CIniFile; 
		if (pIniFile->Open(GAME_PATH_ALIAS_RES.c_str(), utf8_to_utf16(strAniFile).c_str()))
		{
			ms_mapAniKeyFrameCfg.insert(make_pair(strAniFileName, pIniFile));
		}
		else
		{
			SafeDelete(pIniFile);
		}
	}

	return true;
}

void CAniKeyFrameCfg::UnloadAniKeyFrameConfig()
{
	ClearMap(ms_mapAniKeyFrameCfg);
}

CIniFile* CAniKeyFrameCfg::GetAniKeyFrameCfg(const TCHAR* szName)
{
	MapAniKeyFrameCfg::iterator it = ms_mapAniKeyFrameCfg.find(szName);
	if (it == ms_mapAniKeyFrameCfg.end())
	{
		MapNpcName2AniFileName::iterator it = m_mapNpcName2AniFileName.find(szName);
		if (it != m_mapNpcName2AniFileName.end())
		{
			string strAniFileName = (*it).first;
			string strAniFile = "server/keyframeinfo/" + (*it).second + ".fky";

			CIniFile* pIniFile = new CIniFile; 
			if (pIniFile->Open(GAME_PATH_ALIAS_RES.c_str(), utf8_to_utf16(strAniFile).c_str()))
			{
				ms_mapAniKeyFrameCfg.insert(make_pair(strAniFileName, pIniFile));
				return pIniFile;
			}
			else
			{
				SafeDelete(pIniFile);
				return NULL;
			}	
		}
		else
		{
			return NULL;
		}
	}
	return it->second;
}

uint32 CAniKeyFrameCfg::GetNPCAniKeyFrame(const TCHAR* szNPCName, const TCHAR* szAniName, const TCHAR* szKeyName)
{
	if (szNPCName == "" || szAniName == "")
		return 0;

	CIniFile* pIniFile = GetAniKeyFrameCfg(szNPCName);
	if (pIniFile)
		return pIniFile->GetValue(szAniName, szKeyName, 0);
	
	return 0;
}

void CAniKeyFrameCfg::SetNpcName2AinMap(string sNpcName, string sAniFileName)
{
	if (sNpcName == "" || sAniFileName == "")
		return;
	MapNpcName2AniFileName::iterator it = m_mapNpcName2AniFileName.find(sNpcName);
	if (it != m_mapNpcName2AniFileName.end())
	{
		return;
	}
	m_mapNpcName2AniFileName[sNpcName] = sAniFileName;
}
