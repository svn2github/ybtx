#include "stdafx.h"
#include "CNpcResServerData.h"
#include "CAniKeyFrameCfg.h"

CNpcResServerDataMgr::CNpcResServerDataMgr()
{
}

CNpcResServerDataMgr::~CNpcResServerDataMgr()
{
	DeleteAll();
}

CNpcResServerData::CNpcResServerData(const string& sNpcName, uint32 uID)
:m_sNpcName(sNpcName)
,m_uID(uID)
{
}

CNpcResServerData::~CNpcResServerData()
{
}

CNpcResServerData* CNpcResServerData::Create(const TCHAR* sNpcName, float fBaseSize, float fBottomSize, bool bAdhereTerrain, const TCHAR* sAniFileName)
{
	if (sNpcName == NULL)
	{
		ostringstream strm;
		strm<<"【NpcRes_Common】表中【NpcName】项不能为空！请查实！";
		GenErr(strm.str());
	}

	if (fBaseSize <= 0)
	{
		ostringstream strm;
		strm<<"名字为：【"<< sNpcName<<"】的Npc在【NpcRes_Common】表中【Scaling】【BaseSize】两项必须都大于0！请查实！";
		GenErr(strm.str());
	}

	CNpcResServerData* pData = new CNpcResServerData(sNpcName, CNpcResServerDataMgr::GetInst()->GetSize());
	if (!CNpcResServerDataMgr::GetInst()->AddEntity(pData))
	{
		ostringstream strm;
		strm<< "在表【NpcRes_Common】中叫【" << sNpcName <<"】的Npc重复！请查实！" ;
		GenErr(strm.str());
	}
	pData->m_fBaseSize = fBaseSize;
	pData->m_fBottomSize = fBottomSize;
	pData->m_bAdhereTerrain = bAdhereTerrain;
	CAniKeyFrameCfg::SetNpcName2AinMap(sNpcName, sAniFileName);		//设置Npc名称对应的动作名称
	return pData;
}


