#include "stdafx.h"
#include "CNpcEnmityTargetFilterData.h"
#include "CTxtTableFile.h"
#include "TimeHelper.h"
#include "StringHelper.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CNpcEnmityTargetFilterData::CNpcEnmityTargetFilterData(sqr::uint32 uID, const std::string &sName)
:m_uID(uID)
,m_sName(sName)
{

}

void CNpcEnmityTargetFilterData::InsertNpcArgs(string& strNpcArgs)
{
	if (strNpcArgs.empty())
		return;
	erase(strNpcArgs, " ");
	erase(strNpcArgs,"\"");
	replace(strNpcArgs, "，", ",");
	while(true)
	{
		uint32 upoint = strNpcArgs.find(",");
		if (upoint != -1)
		{
			string sNpcName = strNpcArgs.substr(0, upoint);
			m_setNpcName.insert(sNpcName);
			strNpcArgs = strNpcArgs.substr(upoint+1, strNpcArgs.length());
		}
		else
		{
			m_setNpcName.insert(strNpcArgs);
			break;
		}
	}
}

bool CNpcEnmityTargetFilterData::IsInTargetList(const string& strNpcName) const
{
	return m_setNpcName.find(strNpcName) != m_setNpcName.end();
}


CNpcEnmityTargetFilterDataMgr::CNpcEnmityTargetFilterDataMgr()
{
	m_mapName2Num["所有目标"]		= ENpcTargetType_AllTarget;
	m_mapName2Num["所有玩家"]		= ENpcTargetType_AllPlayer;
	m_mapName2Num["所有Npc"]		= ENpcTargetType_AllNpc;
	m_mapName2Num["指定Npc"]		= ENpcTargetType_AppointNpc;
}

CNpcEnmityTargetFilterDataMgr::~CNpcEnmityTargetFilterDataMgr()
{
	DeleteAll();
}

bool CNpcEnmityTargetFilterDataMgr::LoadConfig(const std::string &sFileName)
{
	CTxtTableFile TabFile;
	CNpcEnmityTargetFilterData*  pValue;
	uint64 uBeginTime = GetProcessTime();

	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), sFileName.c_str())) 
		return false;

	DeleteAll();

	for(int32 i=1; i<TabFile.GetHeight(); ++i)
	{   
		pValue = new CNpcEnmityTargetFilterData(GetSize(), TabFile.GetString(i, "Name"));

		string sTatargetType = TabFile.GetString(i, "TargetType");
		pValue->m_eTargetType = m_mapName2Num[sTatargetType];
		string sNpcTargetArgs = TabFile.GetString(i, "TargetArg");
		pValue->InsertNpcArgs(sNpcTargetArgs);
		
		AddEntity(pValue);
	} 
	uint64 uEndTime = GetProcessTime();
	cout << "读取配置表：【NpcEnmiyTargetFilter 】完毕！    耗时：" << (uEndTime - uBeginTime) << " 毫秒\n" ;

	return true;
}
