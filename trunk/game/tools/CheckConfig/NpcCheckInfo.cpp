#include "stdafx.h"
#include "NpcCheckInfo.h"
#include "CNpcFightBaseData.h"
#include "CNpcAIBaseData.h"
#include "NpcInfoDefs.h"
#include "CNpcResServerData.h"

bool NpcCheckInfo::CheckNpcFightInfo(const char* szName)
{
	const CNpcFightBaseData* pData= CNpcFightBaseDataMgr::GetInst()->GetEntity( szName );
	if (pData)
		return true;
	return false;
}

bool NpcCheckInfo::CheckNpcAITypeInfo(const char* szAIType)
{
	AITypeMap::mapNpcAIType_itr iter3 = sNpcAIType.mapNpcAIType.find( szAIType);
	if (iter3 != sNpcAIType.mapNpcAIType.end())
		return true;
	return false;
}

bool NpcCheckInfo::CheckNpcAIDataInfo(const char* szAIData)
{
	const CNpcAIBaseData* pData = CNpcAIBaseDataMgr::GetInst()->GetEntity( szAIData );
	if (pData)
		return true;
	return false;
}

bool NpcCheckInfo::CheckNpcResInfo(const char* szNpcName)
{
	const CNpcResServerData* pData = CNpcResServerDataMgr::GetInst()->GetEntity( szNpcName );
	if (pData)
		return true;
	return false;
}

bool NpcCheckInfo::CheckNpcTypeInfo(const char* szNpcType)
{
	TypeMapRgst::mapNpcType_itr iter = NpcType.mapNpcTypeMap.find(szNpcType);
	if (iter != NpcType.mapNpcTypeMap.end())
		return true;
	return false;
}

bool NpcCheckInfo::CheckNpcAoiTypeInfo(const char* szNpcAIType)
{
	AITypeAoiRgst::mapObjectAoi_itr iter = sObjectAoiType.mapObjectAoiType.find(szNpcAIType);
	if (iter != sObjectAoiType.mapObjectAoiType.end())
		return true;
	return false;
}