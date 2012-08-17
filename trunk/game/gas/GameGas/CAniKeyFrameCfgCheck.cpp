#include "stdafx.h"
#include "CAniKeyFrameCfg.h"
#include "LoadSkillCommon.h"
#include "CCfgColChecker.inl"
#include "CIniFile.h"
#include "NpcInfoMgr.h"
#include "CNpcFightBaseData.h"
#include "BaseHelper.h"
#include "DebugHelper.h"
#include "TimeHelper.h"

bool CAniKeyFrameCfg::CheckNpcMapAniKeyFrameCfg()
{
	uint64 uBeginTime = GetProcessTime();
	map<string, string> mAniLostMap; //map<骨骼名，错误信息>

	MapAniKeyFrameCfg::iterator iter = ms_mapAniKeyFrameCfg.begin();
	for (; iter != ms_mapAniKeyFrameCfg.end(); iter++)
	{
		string sName = (*iter).first;
		const CNpcFightBaseData* pFighterData = CNpcFightBaseDataMgr::GetInst()->GetEntity(sName);
		if (!pFighterData || pFighterData->m_fAttackScope >= 3.0f)		//攻击距离大于3表明是远程的，这里只检查近程怪物
			continue;
		MapNpcName2AniFileName::iterator iter2 = m_mapNpcName2AniFileName.find(sName);	//目前只检测Npc的
		if (iter2 != m_mapNpcName2AniFileName.end() && NpcInfoMgr::BeFightNpc(sName.c_str()))
		{
			string sAniName = (*iter2).second;
			CIniFile* pIniFile = (*iter).second;
			string sAniLostInfo = "";
			if (pIniFile->GetValue("attack01", "k", 0) == 0)
			{
				sAniLostInfo = "缺少攻击关键帧信息！";
			}
			if (pIniFile->GetValue("attack01", "e", 0) == 0)
			{
				sAniLostInfo = sAniLostInfo + "缺少攻击全帧信息！";
			}
			map<string, string>::iterator iter3 = mAniLostMap.find(sAniName);
			if (sAniLostInfo != "" && iter3 == mAniLostMap.end() )
			{
				mAniLostMap[sAniName] = sAniLostInfo;
			}
		}
	}

	//输出错误信息到美术的错误log里面去
	map<string, string>::iterator iter3 = mAniLostMap.begin();
	//cout<<"总共有：【"<<mAniLostMap.size()<<"】个错误"<<endl;
	for (; iter3 != mAniLostMap.end(); iter3++)
	{
		string sErrorInfo = "keyframeinfo: 【" + (*iter3).first + "】 " + (*iter3).second;
		//ArtErr(sErrorInfo.c_str());
	}
	uint64 uEndTime = GetProcessTime();
	cout << "关联检测Npc模型边和美术关建帧信息完毕！耗时：" << (uEndTime - uBeginTime) << "  毫秒！\n";
	return true;
}

