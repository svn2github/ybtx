#pragma once
#include "CNpcAI.h"

class CMagicBuildingAI 
	: public CNpcAI
{
public:
	CMagicBuildingAI(CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType);
private:
	virtual ~CMagicBuildingAI();
};

