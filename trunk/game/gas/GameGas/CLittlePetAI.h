#pragma once

#include "CTick.h"
#include "CNpcAI.h"

class CLittlePetAI
	: public CNpcAI
	, public CTick
{
public:
	CLittlePetAI( CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType);
	~CLittlePetAI();

	void OnTick();
	void FollowMaster();
	void OnMasterDestroyed();
};
