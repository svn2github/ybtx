#include "stdafx.h"
#include "CNpcAIBaseData.h"
#include "CTxtTableFile.h"
#include "TimeHelper.h"
#include "StringHelper.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CNpcAIBaseData::CNpcAIBaseData(sqr::uint32 uID, const std::string &sName)
:m_uID(uID)
,m_sName(sName)
{

}

CNpcAIBaseDataMgr::CNpcAIBaseDataMgr()
{

}

CNpcAIBaseDataMgr::~CNpcAIBaseDataMgr()
{
	DeleteAll();
}

uint32 CNpcAIBaseData::GetAlertTime(uint32 uTargetNum) const
{
	if (m_mapAlert.size() == 0)
		return 0;
	else
	{
		MapAlertTime::const_iterator iter_end = m_mapAlert.end();
		MapAlertTime::const_iterator iter = m_mapAlert.find(uTargetNum);
		if (iter != iter_end)
			return (*iter).second;
		else
			return m_mapAlert.rbegin()->second;
	}
}

void CNpcAIBaseData::CreateAlertMap(string strAlert)
{
	if (strAlert.empty())
		return;
	string sTempArgList = strAlert;
	erase(strAlert, " ");
	erase(strAlert,"\"");
	replace(strAlert, "£¬", ",");
	replace(strAlert, "£¨", "(");
	replace(strAlert, "£©", ")");
	uint32 startpos = strAlert.find('{');
	uint32 endpos = strAlert.find('}');
	if (startpos == -1 || endpos == -1)
		return;
	else
	{
		strAlert = strAlert.substr(startpos+1,endpos-1);
	}

	if (strAlert.empty())
		return;
	uint32 uNumber = 1;
	while (true)
	{
		if (strAlert.empty())
			break;
		int32 ipos = strAlert.find(',');
		string strAlertTime;
		if (ipos == -1)
		{
			strAlertTime = strAlert;
			strAlert = "";
		}
		else
		{
			strAlertTime = strAlert.substr(0, ipos - 1);
			strAlert = strAlert.substr(ipos + 1, strAlert.length());
		}
		uint32 uAlertTime = static_cast<uint32>(atoi(strAlertTime.c_str()));
		m_mapAlert.insert(make_pair(uNumber, uAlertTime));
		uNumber ++;
	}
}

bool CNpcAIBaseDataMgr::LoadConfig( const string& sFileName )
{
	CTxtTableFile TabFile;
	CNpcAIBaseData*  pValue;
	uint64 uBeginTime = GetProcessTime();

	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), sFileName.c_str())) return false;

	DeleteAll();

	for(int32 i=1; i<TabFile.GetHeight(); ++i)
	{   
		pValue = new CNpcAIBaseData(GetSize(), TabFile.GetString(i, "Name"));
		pValue->m_uMoveRange			= TabFile.GetInteger(i, "MoveRange", 0);
		pValue->m_uWalkSpeed			= TabFile.GetFloat(i, "WalkSpeed", 0.0f);
		pValue->m_uRunSpeed				= TabFile.GetFloat(i, "RunSpeed", 0.0f);
		pValue->m_uEyeSize				= TabFile.GetFloat(i, "EyeSize", 0.0f);
		pValue->m_fExistTime			= TabFile.GetInteger(i, "ExistTime", 0);
		pValue->m_uChaseDistance		= TabFile.GetInteger(i, "ChaseDistance", 0);
		pValue->m_uLockEnemyDis			= TabFile.GetInteger(i, "LockEnemyDis", 0);
		pValue->m_bDelInBattle			= TabFile.GetInteger(i, "DelInBattle", 0) == 1;
		pValue->m_uEnmityDist			= TabFile.GetInteger(i, "EnemyDist", 0);
		pValue->m_uMinRandMoveTime		= TabFile.GetInteger(i, "MinRandMoveTime", 0);
		pValue->m_uMaxRandMoveTime		= TabFile.GetInteger(i, "MaxRandMoveTime", 0);
		pValue->m_uEscapeTime			= TabFile.GetInteger(i, "EscapeTime", 2);
		pValue->m_uEscapeDistace		= TabFile.GetInteger(i, "EscapeDistance", 2);
		pValue->m_uAlertDist			= TabFile.GetInteger(i, "AlertDist", 0);
		pValue->m_uPatrolPointStayTime	= TabFile.GetInteger(i, "PatrolPointStayTime", 0);
		string sAlertStr				= TabFile.GetString(i, "AlertTime");
		pValue->CreateAlertMap(sAlertStr);
		AddEntity(pValue);
	} 
	uint64 uEndTime = GetProcessTime();
	cout << "¶ÁÈ¡ÅäÖÃ±í£º¡¾NpcAIBaseData_Server¡¿Íê±Ï£¡    ºÄÊ±£º" << (uEndTime - uBeginTime) << " ºÁÃë\n" ;
	
	return true;
}
