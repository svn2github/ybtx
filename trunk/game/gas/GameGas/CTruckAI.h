#pragma once

#include "CNpcAI.h"

class CTruckFollowMaster;

enum ETruckState
{
	ETruckState_None,
	ETruckState_Disband,		//¶ªÆú
	ETruckState_Destory,		//Ïú»Ù	
};

class CTruckAI
	: public CNpcAI
{
friend class CTruckFollowMaster;
public:
	CTruckAI(CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType);
	~CTruckAI();
	virtual void OnMasterDestroyed();
	void SetTruckFollowState(ETruckState eState);
	void StopFollowMaster();
	bool CanBeTakeOver(CCharacterDictator* pNewMaster);
	void TakeOverByNewMaster(CCharacterDictator* pNewMaster);
private:
	CDistortedTick*				m_pTruckFollowMasterTick;
};
