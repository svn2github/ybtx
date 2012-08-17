#include "stdafx.h"
#include "CMagicBuildingAI.h"
#include "CCharacterDictator.h"
#include "CServantData.h"

CMagicBuildingAI::CMagicBuildingAI(CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType)
:CNpcAI(pCharacter, pBaseData, pStateTransit, eRealNpcType, eRealAIType, eObjAoiType)
{
	CCharacterDictator* pServant = GetCharacter();
	Ast(pServant);
	CCharacterDictator* pMaster = pServant->GetMaster();
	Ast(pMaster)
	CFighterDictator* pFighterMaster =pMaster->GetFighter();
	Ast(pFighterMaster);
	CFighterDictator* pFighterServant = pServant->GetFighter();
	Ast(pFighterServant);

	const CServantData* pServantData = CServantDataMgr::GetInst()->GetEntity( pBaseData->GetName());
	if (pServantData)
	{
		pServantData->InitServantProperty(pFighterServant,pFighterMaster);
	}
}

CMagicBuildingAI::~CMagicBuildingAI()
{
	ClearAllEnmity();
}

