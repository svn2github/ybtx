#include "stdafx.h"
#include "CNpcServerBaseData.h"
#include "CNpcStateTransit.h"
#include "CNpcFightBaseData.h"
#include "CNpcAIBaseData.h"
#include "NpcInfoDefs.h"
#include "CNpcResServerData.h"
#include "CNpcEnmityTargetFilterData.h"

CNpcServerBaseData* CNpcServerBaseData::Create(const TCHAR* szName, const TCHAR* szNpcType, const TCHAR* szAIType, const TCHAR* szAIData
											   , uint32 uCamp, uint32 uClass, uint32 uSex, uint32 uLevel)
{
	bool bSuccess = true;
	if (szNpcType == NULL)
	{
		bSuccess = false;
		cout << "Npc类型为空\n";
	}

	if (szName == NULL)
	{
		bSuccess = false;
		cout << "Npc名为空\n";
	}

	if (szAIType == NULL)
	{
		bSuccess = false;
		cout << "Npc " << szName << " AI类型为空\n";
	}

	if (szAIData == NULL)
	{
		bSuccess = false;
		cout << "Npc " << szName << " AI数据为空\n";
	}

	CNpcServerBaseData* pData = new CNpcServerBaseData(CNpcServerBaseDataMgr::GetInst()->GetSize(), szName);

	if (!CNpcServerBaseDataMgr::GetInst()->AddEntity(pData))
	{
		bSuccess = false;
		cout << "Npc总表中 " << szName << " 重复！\n";
	}

	pData->m_pFightBaseData = CNpcFightBaseDataMgr::GetInst()->GetEntity( pData->GetName() );

	if (pData->m_pFightBaseData == NULL)
	{
		bSuccess = false;
		cout << "Npc " << szName <<" 战斗信息不存在！\n";
	}

	pData->m_pStateTransitOne = CNpcStateTransitMgr::GetInst()->GetEntity( szAIType );
	if (pData->m_pStateTransitOne == NULL)
	{
		bSuccess = false;
		cout << "Npc " << szName << "  AI类型： " << szAIType << " 不存在！\n";
	}

	pData->m_pAIBaseDataOne = CNpcAIBaseDataMgr::GetInst()->GetEntity( szAIData );
	if (pData->m_pAIBaseDataOne == NULL)
	{
		bSuccess = false;
		cout << "Npc " << szName << " AI数据 " << szAIData << " 不存在！\n";
	}

	pData->m_pResBaseData = CNpcResServerDataMgr::GetInst()->GetEntity( pData->GetName());
	if (pData->m_pResBaseData == NULL)
	{
		bSuccess = false;
		cout << "Npc  " << szName << "在【NpcRes_Common】表中不存在！请查实！\n";
	}

	//这个指针可能是为空的
	pData->m_pEnmityTargetFilter = CNpcEnmityTargetFilterDataMgr::GetInst()->GetEntity(pData->GetName());

	//索引Npc类型
	TypeMapRgst::mapNpcType_itr iter1 = NpcType.mapNpcTypeMap.find(szNpcType);
	if (iter1 == NpcType.mapNpcTypeMap.end())
	{
		bSuccess = false;
		cout << "Npc " << szName << "  Type： " << szNpcType << " 尚未注册\n";
	}
	pData->m_eType = NpcType.mapNpcTypeMap[szNpcType];

	//索引Aoi类型
	AITypeAoiRgst::mapObjectAoi_itr iter2 = sObjectAoiType.mapObjectAoiType.find(szAIType);
	if (iter2 == sObjectAoiType.mapObjectAoiType.end())
	{
		bSuccess = false;
		cout << "Npc " << szName << "  AIType： " << szAIType << " 尚未注册AOI类型！\n";
	}
	pData->m_eAoiType = sObjectAoiType.mapObjectAoiType[szAIType];

	//索引AIType类型
	AITypeMap::mapNpcAIType_itr iter3 = sNpcAIType.mapNpcAIType.find( szAIType);
	if (iter3 == sNpcAIType.mapNpcAIType.end())
	{
		bSuccess = false;
		cout << "Npc " << szName << "  AIType：" << szAIType << " 尚未注册AI类型！\n";
	}
	pData->m_eAIType = sNpcAIType.mapNpcAIType[szAIType];

	pData->m_uCamp = uCamp;
	pData->m_uClass = uClass;
	pData->m_uSex = uSex;
	pData->m_uLevel = uLevel;

	if (!bSuccess)
	{
		delete pData;
		string str1;
		str1 = "构建Npc的基础性息失败！";
		GenErr(str1, szName);
	}
	return pData;
}

void CNpcServerBaseData::SetCanBeChangeAI(bool bCanChangeAI)
{
	m_bCanBeChangeAI = bCanChangeAI;
}

void CNpcServerBaseData::SetBeSleep(bool bSleep)
{
	m_bSleep = bSleep;
}

void CNpcServerBaseData::SetBeSynToClient(bool bSynToClient)
{
	m_bSynToClient = bSynToClient;
}
void CNpcServerBaseData::SetCanBeSelected(bool bCanBeSelected)
{
	m_bCanBeSelected = bCanBeSelected;
}
void CNpcServerBaseData::SetCanBeRavin(bool bCanBeRavin)
{
	m_bCanBeRavin = bCanBeRavin;
}

CNpcServerBaseData::CNpcServerBaseData(uint32 uID, const string& sName)
: m_uID( uID )
, m_sName( sName )
{
}

CNpcServerBaseDataMgr::CNpcServerBaseDataMgr()
{

}

CNpcServerBaseDataMgr::~CNpcServerBaseDataMgr()
{
	DeleteAll();
}

