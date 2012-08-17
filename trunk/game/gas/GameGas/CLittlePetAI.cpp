#include "stdafx.h"
#include "CLittlePetAI.h"
#include "CAppServer.h"
#include "CCharacterDictator.h"
#include "CFighterDictator.h"
#include "CoreCommon.h"

CLittlePetAI::CLittlePetAI(CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType)
:CNpcAI(pCharacter, pBaseData, pStateTransit, eRealNpcType, eRealAIType, eObjAoiType)
{
	pCharacter->CppSetCamp(eCamp_Passerby);
	CAppServer::Inst()->RegisterTick(this, 2000);
}

CLittlePetAI::~CLittlePetAI()
{
	ClearAllEnmity();
	DeleteNpcAIStateMachine();
	CAppServer::Inst()->UnRegisterTick(this);
}

void CLittlePetAI::OnTick()
{
	FollowMaster();
}

void CLittlePetAI::OnMasterDestroyed()
{
	CAppServer::Inst()->UnRegisterTick(this);
	GetCharacter()->SetMaster(NULL);
}

void CLittlePetAI::FollowMaster()
{
	CCharacterDictator* pServant = GetCharacter();
	Ast(pServant);
	Ast(GetCharacter()->GetFighter());
	CCharacterDictator* pMaster = pServant->GetMaster();
	if (pMaster)
	{
		CFighterDictator* pMasterFighter = pMaster->GetFighter();
		Ast(pMasterFighter);
		CFPos servantpos;
		CFPos masterPos;
		pMaster->GetPixelPos(masterPos);
		pServant->GetPixelPos(servantpos);
		float fDis = pServant->GetEntityDist(pMaster);

		if ( fDis <= 2.5*eGridSpanForObj) 
			return;
		if ( fDis > 15*eGridSpanForObj)
		{
			pServant->StopMoving();
			pServant->SetPixelPos(masterPos);
			return;
		}

		float fSpeed;
		bool bWallState = pMaster->CppGetCtrlState(eFCS_InWalkState);
		fSpeed = bWallState ? pMasterFighter->m_WalkSpeed.Get(pMasterFighter) : pMasterFighter->m_RunSpeed.Get(pMasterFighter);
		MoveToPixel(masterPos, fSpeed, bWallState, 0.5f);
	}
}

