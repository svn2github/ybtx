#include "stdafx.h"
#include "CGameSceneHandler.h"
#include "GameDef.h"
#include "CBulletMagicClient.h"
#include "CShockWaveMagicClient.h"
#include "CTransferableMagicClient.h"
#include "CCharacterFollower.h"
#include "CPositionMagicClient.h"
#include "CBattleArrayMagicClient.h"
#include "FXPlayer.h"
#include "CEntityClientManager.h"
#include "CBarrierMagicClient.h"
#include "CCoreObjectDirector.h"
#include "CSyncVariantClient.h"
#include "CEntityClientCreator.h"
#include "ICharacterFollowerCallbackHandler.h"
#include "CCharacterDirector.h"
#include "CFighterFollower.h"
#include "CIntObjClient.h"
#include "IIntObjCallbackHandler.h"
#include "StringHelper.h"


void CGameSceneHandler::OnObjectCreated(CCoreObjectFollower* pObject)
{
	if (!pObject)
		return;
	const CSyncVariantClient* pViewVariant = pObject->GetViewVariant(eCTI_Static);
	ECharacterType eType = (ECharacterType)pViewVariant->GetMember("Attribute")->GetMember("Type")->GetNumber<uint8>();

	switch (eType)
	{
	case eCT_Player:
		CreatePlayer(pObject);
		break;
	case eCT_Npc:
		CreateNpc(pObject);
		break;
	case eCT_IntObj:
		CreateIntObj(pObject);
		break;
	case eCT_Trap:
		CreateTrap(pObject);
		break;
	case eCT_Magic:
		CreateMagic(pObject);
		break;
	}
}

void CGameSceneHandler::OnDirectorCreated(CCoreObjectDirector* pObject)
{
	if (!pObject)
		return;
	if (pObject->GetHandler()) 
		GenErr("CoreObj already has handler");
	if(pObject->GetType() != eCOT_Director)
		GenErr("Is not CoreObjectDirector");
	uint32 uDirector = CEntityClientManager::GetInst()->GetDirectorID();
	CCharacterFollower* pCharacter = CEntityClientManager::GetInst()->GetCharacter(uDirector);
	CCharacterDirector* pDirector = NULL;
	if (pCharacter)							//×ª³¡¾°½áÊø
	{
		pDirector = pCharacter->CastToCharacterDirector();
		Ast (pDirector->BeInTransfer());
		pDirector->OnTransfered(pObject);

		CFighterFollower* pFighterFollower = pDirector->GetFighter();
		const CSyncVariantClientHolder* pSyncVarHolder = pCharacter->GetSyncVariantHolder(eCTI_Agile);
		pSyncVarHolder->EnumVariant("State", &CFighterFollower::OnSetStateForInit);

		const CSyncVariantClientHolder* pViewVarHolder = pCharacter->GetViewVariantHolder(eCTI_Agile);
		pViewVarHolder->EnumVariant("AgileValue", &CFighterFollower::OnSetStateForInit);
	}

}

void CGameSceneHandler::CreateNpc(CCoreObjectFollower* pObj)
{
	if (pObj->GetHandler())
		return;
	const CSyncVariantClient* pViewVariant = pObj->GetViewVariant(eCTI_Static);
	uint32 uEntityID = (uint32)pViewVariant->GetMember("Attribute")->GetMember("EntityID")->GetNumber<uint32>();
	bool bShowInClient = (bool)pViewVariant->GetMember("Attribute")->GetMember("ShowInClient")->GetNumber<bool>();
	if (!bShowInClient)
		return;
	CCharacterFollower* pFollower = CEntityClientCreator::CreateCharacterFollower(pObj, uEntityID, false);

	pFollower->RegistNpcVariantCallback();
	if (pFollower->GetCallbackHandler())
		pFollower->GetCallbackHandler()->OnCreated(pFollower, false);

	pFollower->GetFighter()->InitAgileValue();
	const CSyncVariantClientHolder* pViewVarHolder = pFollower->GetViewVariantHolder();
	pViewVarHolder->EnumVariant("Attribute", &CFighterFollower::OnSetStateForInit);
	pViewVarHolder = pFollower->GetViewVariantHolder(eCTI_Agile);
	pViewVarHolder->EnumVariant("AgileValue", &CFighterFollower::OnSetStateForInit);
	pViewVarHolder->EnumVariant("PKSwitch", &CFighterFollower::OnSetStateForInit);
	pFollower->InitRealName();
}

void CGameSceneHandler::CreatePlayer(CCoreObjectFollower* pObj)
{
	if (pObj->GetHandler())
		return;
	const CSyncVariantClient* pViewVariant = pObj->GetViewVariant(eCTI_Static);
	uint32 uEntityID = (uint32)pViewVariant->GetMember("Attribute")->GetMember("EntityID")->GetNumber<uint32>();

	CCharacterFollower* pFollower = CEntityClientCreator::CreateCharacterFollower(pObj, uEntityID, false);

	pFollower->RegistCommonVarriantCallback();
	if (pFollower->GetCallbackHandler())
		pFollower->GetCallbackHandler()->OnCreated(pFollower, false);

	pFollower->GetFighter()->InitAgileValue();
	const CSyncVariantClientHolder* pViewVarHolder = pFollower->GetViewVariantHolder();
	pViewVarHolder->EnumVariant("Attribute", &CFighterFollower::OnSetStateForInit);
	pViewVarHolder = pFollower->GetViewVariantHolder(eCTI_Agile);
	pViewVarHolder->EnumVariant("AgileValue", &CFighterFollower::OnSetStateForInit);
	pViewVarHolder->EnumVariant("PKSwitch", &CFighterFollower::OnSetStateForInit);
}

void CGameSceneHandler::CreateIntObj(CCoreObjectFollower* pObj)
{
	if (pObj->GetHandler())
	return;

	const CSyncVariantClient* pViewVariant = pObj->GetViewVariant(eCTI_Static);
	uint32 uEntityID = (uint32)pViewVariant->GetMember("Attribute")->GetMember("EntityID")->GetNumber<uint32>();
	CIntObjClient* pIntObj = CEntityClientCreator::CreateIntObjClient(pObj, uEntityID);

	if (pIntObj->GetCallbackHandler())
		pIntObj->GetCallbackHandler()->OnCreated(pIntObj);
}

void CGameSceneHandler::CreateTrap(CCoreObjectFollower* pObj)
{
	if (pObj->GetHandler())
		return;

	const CSyncVariantClient* pViewVariant = pObj->GetViewVariant(eCTI_Static);
	uint32 uEntityID = (uint32)pViewVariant->GetMember("Attribute")->GetMember("EntityID")->GetNumber<uint32>();
	CIntObjClient* pIntObj = CEntityClientCreator::CreateIntObjClient(pObj, uEntityID);

	if (pIntObj->GetCallbackHandler())
		pIntObj->GetCallbackHandler()->OnCreated(pIntObj);
}


void CGameSceneHandler::CreateMagic(CCoreObjectFollower* pObj)
{
	if (pObj->GetHandler())
		return;

	const CSyncVariantClientHolder* pViewVariant = pObj->GetViewVariantHolder(eCTI_Static);
	EMagicGlobalType eMagicType = (EMagicGlobalType)pViewVariant->GetNumber<uint8>("Attribute/MagicType");
	uint32 uCategoryID = pViewVariant->GetNumber<uint32>("Attribute/CategoryID");
	uint32 uSrcEntityID = pViewVariant->GetNumber<uint32>("Attribute/SrcEntityID");
	uint32 uTargetEntityID = pViewVariant->GetNumber<uint32>("Attribute/TargetEntityID");
	EBattleArrayState uArrayState = (EBattleArrayState)pViewVariant->GetNumber<uint8>("Attribute/ArrayState");
	switch(eMagicType)
	{
	case eMGT_Bullet:
		{
			const CBulletMagicCfgClientSharedPtr& pCfg = CBulletMagicCfgClient::GetByID(uCategoryID);
			CFPos pos(0.0f, 0.0f);
			CBulletMagicClient* pMagicClient = new CBulletMagicClient(pObj, uSrcEntityID, uTargetEntityID, pCfg, pos);
			pMagicClient->PlayMagicFx(pCfg->GetFX());
			pObj->SetHandler(pMagicClient);
		}
		break;
	case eMGT_ShockWave:
		{
			const CShockWaveMagicCfgClientSharedPtr& pCfg = CShockWaveMagicCfgClient::GetById(uCategoryID);
			CMagicClient* pMagicClient = new CShockWaveMagicClient(pObj);
			pMagicClient->PlayMagicFx(pCfg->GetFX());
			pObj->SetHandler(pMagicClient);
		}
		break;
	case eMGT_Transferable:
		{
			const CTransferableMagicCfgClientSharedPtr& pCfg = CTransferableMagicCfgClient::GetById(uCategoryID);
			CTransferableMagicClient* pMagicClient = new CTransferableMagicClient(pObj,uSrcEntityID);
			CCharacterFollower* pTargetj = CCharacterFollower::GetCharacterByID(uTargetEntityID);
			if (pTargetj)
			{
				pMagicClient->PlayMagicFx(pTargetj,pCfg->GetFX());
			}
			pObj->SetHandler(pMagicClient);
		}
		break;
	case eMGT_Position:
		{
			const CPositionMagicCfgClientSharedPtr& pCfg = CPositionMagicCfgClient::GetById(uCategoryID);
			CMagicClient* pMagicClient = new CMagicClient(pObj);
			pMagicClient->PlayMagicFx(pCfg->GetFX());
			pObj->SetHandler(pMagicClient);
		}
		break;
	case eMGT_Aure:
		break;
	case eMGT_BattleArray:
		{
			const CBattleArrayMagicCfgClientSharedPtr& pCfg = CBattleArrayMagicCfgClient::GetById(uCategoryID);
			CMagicClient* pMagicClient = new CMagicClient(pObj);	

			pObj->SetHandler(pMagicClient);
			switch (uArrayState)
			{
			case eBAS_InArray:
				{
					string strPosSucFx=pCfg->GetPosSucFx();
					strPosSucFx=sqr::erase(strPosSucFx,"(");
					strPosSucFx=sqr::erase(strPosSucFx,")");
					vector<string> sFXTable = CBaseFX::Split(strPosSucFx, ","); 
					pMagicClient->PlayMagicFx(sFXTable[(uTargetEntityID-1)*2]+","+sFXTable[(uTargetEntityID-1)*2+1]);
					break;
				}
			case eBAS_OutArray:
			case eBAS_ReturnArray:
				{
					string strPosFx=pCfg->GetPosFx();
					strPosFx=sqr::erase(strPosFx,"(");
					strPosFx=sqr::erase(strPosFx,")");
					vector<string> sFXTable = CBaseFX::Split(strPosFx, ","); 
					pMagicClient->PlayMagicFx(sFXTable[(uTargetEntityID-1)*2]+","+sFXTable[(uTargetEntityID-1)*2+1]);
				}
				break;
			case eBAS_FinishArray:
				{
					CCharacterFollower* pOwner = CEntityClientManager::GetInst()->GetCharacter(uSrcEntityID);
					vector<string> sFXTable = CBaseFX::Split(pCfg->GetAddUpSucFx(), ","); 
					CPlayerFX::PlayOnceFX(pOwner,sFXTable[0]+sFXTable[1]+".efx",sFXTable[1]);
				}
				break;
			}
		}
		break;
	case eMGT_Barrier:
		{ 
			const CBarrierMagicCfgClientSharedPtr& pCfg = CBarrierMagicCfgClient::GetById(uCategoryID);
			CMagicClient* pMagicClient = new CMagicClient(pObj);
			pMagicClient->PlayMagicFx(pCfg->GetFX());
			pObj->SetHandler(pMagicClient);
		}
		break;
	}
}
