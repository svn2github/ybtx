#include "stdafx.h"
#include "CNpcServerCreator.h"
#include "CPos.h"
#include "CCharacterDictator.h"
#include "CNpcServerBaseData.h"
#include "CNpcStateTransit.h"
#include "CNpcFightBaseData.h"
#include "CNpcAI.h"
#include "CTotemAI.h"
#include "CDummyAI.h"
#include "CMemberAI.h"
#include "CServantAI.h"
#include "CMagicBuildingAI.h"
#include "CTruckAI.h"
#include "CLittlePetAI.h"
#include "CFighterDictator.h"
#include "NpcInfoDefs.h"
#include "MagicOps_ChangeValue.h"


void CNpcServerCreator::CreateServerNpcAI( CCharacterDictator* pCharacter, const TCHAR* szNpcName, uint32 uLevel ,const TCHAR* AITypeName, const TCHAR* szNpcType, EClass eClass, ECamp eCamp, const TCHAR* szServantRealName, uint32 uMasterObjID)
{
	pCharacter->CppInit(eClass, uLevel, eCamp);
	CreateNpcAI(pCharacter, szNpcName, AITypeName, szNpcType, szServantRealName, uMasterObjID);
}

void CNpcServerCreator::CreateNpcAI(CCharacterDictator *pCharacter, const TCHAR* szNpcName, const TCHAR* AITypeName, const TCHAR* szNpcType, const TCHAR* szServantRealName, uint32 uMasterObjID)
{
	const CNpcServerBaseData* pNpcBaseData = CNpcServerBaseDataMgr::GetInst()->GetEntity( szNpcName );
	if( !pNpcBaseData)
	{
		ostringstream strm;
		strm<<"名字为："<< szNpcName <<" 的Npc在Npc_Common表中不存在";
		GenErr(strm.str());
	}

	const CNpcStateTransit* pStateTransit = CNpcStateTransitMgr::GetInst()->GetEntity( AITypeName );
	if (!pStateTransit)
	{
		ostringstream strm;
		strm<<"Npc名为："<<szNpcName<<"的NpcAI： "<< AITypeName <<" 的AI类型不存在！";
		GenErr(strm.str());
	}
	
	if (uMasterObjID && uMasterObjID != pCharacter->GetEntityID())	//防止把自己设为自己的master
	{
		CCharacterDictator* pMaster = CCharacterDictator::GetCharacterByID(uMasterObjID);
		if (pMaster)
		{
			pCharacter->SetMaster(pMaster);
			pCharacter->CppSetCamp(pMaster->CppGetCamp());	//召唤兽的阵营和主人一致
		}
	}
	if(szServantRealName)
		pCharacter->SetRealName(szServantRealName);
	else
		pCharacter->SetRealName("");
	pCharacter->SetNpcName(szNpcName);

	const CNpcFightBaseData* pNpcFightData = pNpcBaseData->m_pFightBaseData;
	SetMainHandSpeed(pCharacter->GetFighter(),pNpcFightData->m_fAttackSpeed);
	pCharacter->GetFighter()->CppInitNPCNormalAttack(pNpcFightData->m_sNormalAttack.c_str());

	CNpcAI* pAI = NULL;
	//真实的AI类型，Npc类型以及Aoi类型（可能和Npc_Common表的类型不一样）
	ENpcType eNpcType;
	ENpcAIType eNpcAIType;
	EObjectAoiType eObjAoiType;
	eNpcType = NpcType.mapNpcTypeMap[szNpcType];
	eNpcAIType = sNpcAIType.mapNpcAIType[AITypeName];
	eObjAoiType = sObjectAoiType.mapObjectAoiType[AITypeName];
	
	switch(eNpcType)
	{
	case ENpcType_Normal:
	case ENpcType_AttackNearest:
		pAI = new CNpcAI( pCharacter, pNpcBaseData, pStateTransit, eNpcType, eNpcAIType, eObjAoiType); 
		break;
		case ENpcType_Totem:
			pAI = new CTotemAI( pCharacter, pNpcBaseData, pStateTransit, eNpcType, eNpcAIType, eObjAoiType); 
			break;
		case ENpcType_Pet: 
		case ENpcType_BattleHorse:
		case ENpcType_Summon:
		case ENpcType_MonsterCard:
		case ENpcType_OrdnanceMonster:
		case ENpcType_Cannon:
		case ENpcType_Shadow:
		case ENpcType_QuestBeckonNpc:	
		case ENpcType_BossCortege:
		case ENpcType_NotCtrlSummon:
			pAI = new CServantAI( pCharacter, pNpcBaseData, pStateTransit, eNpcType, eNpcAIType, eObjAoiType); 
			break;
		case ENpcType_Dummy:
			pAI = new CDummyAI( pCharacter, pNpcBaseData,  pStateTransit, eNpcType, eNpcAIType, eObjAoiType);
			break;
		case ENpcType_Member:
			pAI = new CMemberAI( pCharacter, pNpcBaseData, pStateTransit, eNpcType, eNpcAIType, eObjAoiType);
			break;
		case ENpcType_MagicBuilding:
			pAI = new CMagicBuildingAI( pCharacter, pNpcBaseData, pStateTransit, eNpcType, eNpcAIType, eObjAoiType);
			break;
		case ENpcType_Truck:
			pAI = new CTruckAI( pCharacter, pNpcBaseData, pStateTransit, eNpcType, eNpcAIType, eObjAoiType); 
			break;
		case ENpcType_LittlePet:
			pAI = new CLittlePetAI( pCharacter, pNpcBaseData, pStateTransit, eNpcType, eNpcAIType, eObjAoiType);
			break;

		default :
			ostringstream strm;
			strm<<"Npc名为："<<szNpcName<<"的Npc类型不正确！请查实！";
			GenErr(strm.str());
			break;
	}
}

