#include "stdafx.h"
#include "CTruckAI.h"
#include "CDistortedTick.h"
#include "CCharacterDictator.h"
#include "CFighterDictator.h"
#include "BaseHelper.h"
#include "CoreCommon.h"
#include "CCharacterMediator.h"

class CTruckFollowMaster
	: public CDistortedTick
	, public CNpcAIMallocObject
{
public:
	CTruckFollowMaster(CTruckAI* pOwner, uint32 uMasterID);
	~CTruckFollowMaster();
	void OnTick();
private:
	CTruckAI*				m_pAI;
	uint32					m_uMasterID;
};

CTruckAI::CTruckAI(CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType)
:CNpcAI(pCharacter, pBaseData, pStateTransit, eRealNpcType, eRealAIType, eObjAoiType)
{
	CCharacterDictator* pMaster = GetCharacter()->GetMaster();
	Ast (pMaster);
	CFighterDictator* pFighterMaster = pMaster->GetFighter();
	Ast (pFighterMaster);
	m_pTruckFollowMasterTick = new CTruckFollowMaster(this, pMaster->GetEntityID());
	GetCharacter()->RegistDistortedTick(m_pTruckFollowMasterTick, 2000);
}

CTruckAI::~CTruckAI()
{
	ClearAllEnmity();
	CCharacterDictator* pCharacter = GetCharacter();
	Ast (pCharacter);
	ClearAllEnmity();
	DeleteNpcAIStateMachine();
	if (m_pTruckFollowMasterTick)
	{
		pCharacter->UnRegistDistortedTick(m_pTruckFollowMasterTick);
		SafeDelete(m_pTruckFollowMasterTick);
	}
}

void CTruckAI::OnMasterDestroyed()
{
	SetTruckFollowState(ETruckState_Disband);
}

void CTruckAI::StopFollowMaster()
{
	OnMasterDestroyed();
}

bool CTruckAI::CanBeTakeOver(CCharacterDictator* pNewMaster)
{
	CCharacterDictator* pMaster = GetCharacter()->GetMaster();
	if (!pMaster)
		return true;
	else
	{
		ECamp eOldMasterCamp = pMaster->CppGetCamp();
		ECamp eNewMasterCamp = pNewMaster->CppGetCamp();
		if (pNewMaster != pMaster && eOldMasterCamp != eNewMasterCamp)
			return true;
	}
	return false;
}

void CTruckAI::SetTruckFollowState(ETruckState eState)
{
	CCharacterDictator* pMastser = GetCharacter()->GetMaster();
	if (!pMastser)
		return;
	if (eState == ETruckState_Disband)
	{
		if (pMastser->CastToCharacterMediator())
			pMastser->CastToCharacterMediator()->OnMasterDisbandTruck(GetCharacter());
		GetCharacter()->CppSetCamp(eCamp_Monster);
		pMastser->RemoveServantFromServantMgr(GetCharacter()->GetEntityID());
	}
	else
	{
		if (pMastser->CastToCharacterMediator())
			pMastser->CastToCharacterMediator()->OnMasterDestroyTruck(GetCharacter());
		pMastser->RemoveServant(GetCharacter());
	}
	GetCharacter()->SetMaster(NULL);
	if (m_pTruckFollowMasterTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pTruckFollowMasterTick);
		SafeDelete(m_pTruckFollowMasterTick);
	}
}

void CTruckAI::TakeOverByNewMaster(CCharacterDictator* pNewMaster)
{
	if (CanBeTakeOver(pNewMaster))
	{
		GetCharacter()->SetMaster(pNewMaster);
		m_pTruckFollowMasterTick = new CTruckFollowMaster(this, pNewMaster->GetEntityID());
		GetCharacter()->RegistDistortedTick(m_pTruckFollowMasterTick, 2000);
		GetCharacter()->CppSetCamp(pNewMaster->CppGetCamp());
		pNewMaster->AddServant(GetCharacter());
	}
}


CTruckFollowMaster::CTruckFollowMaster(CTruckAI* pOwner, uint32 uMasterID)
:m_pAI(pOwner)
,m_uMasterID(uMasterID)
{
	
}

CTruckFollowMaster::~CTruckFollowMaster()
{

}

void CTruckFollowMaster::OnTick()
{
	CCharacterDictator* pMaster = CCharacterDictator::GetCharacterByID(m_uMasterID);
	if (pMaster)
	{
		CFPos ownpos, masterpos;
		m_pAI->GetCharacter()->GetPixelPos(ownpos);
		pMaster->GetPixelPos(masterpos);
		float uDis = ownpos.Dist(masterpos);
		if (uDis < 25.0f*eGridSpanForObj)
		{
			float uLen = pMaster->GetBottomSize() + m_pAI->GetCharacter()->GetBottomSize() + 1.0f;
			if (uDis < (uLen + 1)*eGridSpanForObj)
				return;
			m_pAI->MoveToPixel(masterpos, false, uLen);
		}
		else
			m_pAI->SetTruckFollowState(ETruckState_Disband);
	}
	else
	{
		m_pAI->GetCharacter()->UnRegistDistortedTick(this);
	}
}
