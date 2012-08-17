#include "stdafx.h"
#include "CCharacterFollowerVariantCallback.h"
#include "ICharacterFollowerCallbackHandler.h"
#include "ICharacterDirectorCallbackHandler.h"
#include "CSyncVariantClient.h"
#include "CCharacterDirector.h"
#include "CFighterFollower.h"
#include "CEntityClientManager.h"

int SetNpcTypeVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());
	ENpcType eNpcType = (ENpcType)GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("NpcType")->GetNumber<uint8>();
	GetCharacter()->SetNpcType(eNpcType);

	return 0;
}

int LevelChangeVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());
	uint32 uLevel = GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("Level")->GetNumber<uint8>();
	uint32 uEntityID = GetCharacter()->GetEntityID();
	if (GetCharacter()->CastToCharacterDirector())
	{
		CCharacterDirector* pDirector = GetCharacter()->CastToCharacterDirector();
		uint32 CurLevel = pDirector->CppGetLevel();
		pDirector->CppSetLevel(uLevel);
		ICharacterDirectorCallbackHandler* pHandler = pDirector->GetCallbackHandler()->CastToDirectorHandler();
		if(pHandler == NULL)
		{
			GenErr("CallBackHandler == NULL");
		}
		pHandler->OnRelationChanged(uEntityID);
		if (GetCharacter() && GetCharacter()->GetAgileValueBeCare())
		{
			pHandler->OnAgileValueChanged(uEntityID);
		}
		uint32 DesLevel = pDirector->CppGetLevel();
		pDirector->RefreshRelationList();
		pHandler->OnLevelUp(DesLevel-CurLevel);
	}
	else
	{
		GetCharacter()->CppSetLevel(uLevel);
		ICharacterFollowerCallbackHandler* pHandler = GetCharacter()->GetCallbackHandler();
		if(pHandler == NULL)
		{
			GenErr("CallBackHandler == NULL");
		}
		pHandler->OnRelationChanged(uEntityID);
		if (GetCharacter() && GetCharacter()->GetAgileValueBeCare())
		{
			pHandler->OnAgileValueChanged(uEntityID);
		}
	}

	return 0;
}

int SetClassVariantCallback::exec(CCypherVariant* pVariant)
{
	Ast (GetCharacter());
	EClass eClass = (EClass)GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("Class")->GetNumber<uint8>();
	GetCharacter()->CppSetClass(eClass);

	return 0;
}

int SetCampVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());
	ECamp eCamp = (ECamp)GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("Camp")->GetNumber<uint8>();
	uint32 uEntityID = GetCharacter()->GetEntityID();
	GetCharacter()->CppSetCamp(eCamp);
	if (GetCharacter()->CastToCharacterDirector())
	{
		GetCharacter()->CastToCharacterDirector()->RefreshRelationList();
	}
	GetCharacter()->GetCallbackHandler()->OnRelationChanged(uEntityID);

	return 0;
}

int SetNpcAITypeVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());
	ENpcAIType eNpcAIType = (ENpcAIType)GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("NpcAIType")->GetNumber<uint8>();
	GetCharacter()->SetAITypeID(eNpcAIType);
	
	return 0;
}

int SetNpcActiveTypeVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());
	bool bActive = GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("BeActiveNpc")->GetNumber<bool>();
	GetCharacter()->SetActiveNpc(bActive);

	return 0;
}

int SetAttackTypeVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());
	EAttackType eAttackType = (EAttackType)GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("AttackType")->GetNumber<uint8>();
	GetCharacter()->SetNpcAttackType(eAttackType);

	return 0;
}

int SetNpcGameCampVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());
	int32 iGameCamp = GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("GameCamp")->GetNumber<int32>();
	GetCharacter()->CppSetGameCamp(iGameCamp);
	uint32 uEntityID = GetCharacter()->GetEntityID();
	if (GetCharacter()->CastToCharacterDirector())
	{
		GetCharacter()->CastToCharacterDirector()->RefreshRelationList();
	}
	GetCharacter()->GetCallbackHandler()->OnRelationChanged(uEntityID);

	return 0;
}

int SetNpcRealNameVariantCallback::exec(CCypherVariant* pVariant)
{
	Ast (GetCharacter());
	const TCHAR* szRealName = (const TCHAR*)GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("RealName")->GetString();
	GetCharacter()->SetRealName(szRealName);

	return 0;
}

int SetBeShowInClientVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());
	bool bShow = (bool)GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("ShowInClient")->GetNumber<bool>();
	GetCharacter()->SetShowInClient(bShow);

	return 0;
}

int OnSpeedChangeVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());
	float fSpeed = (float)GetCharacter()->GetSyncVariant(eCTI_Agile)->GetMember("Attribute1")->GetMember("Speed")->GetNumber<float>();
	GetCharacter()->SetSpeed(fSpeed);
	return 0;
}

int SetAlertTargetVariantCallback::exec(CCypherVariant* pVariant)
{
	Ast (GetCharacter());
	uint32 uAlertTargetID = (uint32)GetCharacter()->GetSyncVariantHolder(eCTI_Agile)->GetNumber<uint32>("Alert/AlertTargetID");
	GetCharacter()->SetNpcAlertState(uAlertTargetID);

	return 0;
}

int SetPKSwitchVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());
	bool bPKState = GetCharacter()->GetViewVariantHolder(eCTI_Agile)->GetNumber<bool>("PKSwitch/PKSwitchType");
	GetCharacter()->CppSetPKState(bPKState);
	//EPKState ePKState = (EPKState)GetCharacter()->GetViewVariantHolder()->GetNumber<uint8>("PKSwitch/PKSwitchType");
	//GetCharacter()->CppSetPKState(ePKState);
	uint32 uEntityID = GetCharacter()->GetEntityID();
	if (GetCharacter()->CastToCharacterDirector())
	{
		GetCharacter()->CastToCharacterDirector()->RefreshRelationList();
	}
	GetCharacter()->GetCallbackHandler()->OnRelationChanged(uEntityID);
	
	return 0;
}

//int SetZoneTypeVariantCallback::exec(CCypherVariant *pVariant)
//{
//	Ast (GetCharacter());
//	EZoneType eZoneType = (EZoneType)GetCharacter()->GetViewVariantHolder()->GetNumber<uint8>("PKSwitch/ZoneType");
//	GetCharacter()->GetFighter()->SetZoneType(eZoneType);
//
//	return 0;
//}

int SetFighteDirVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());
	bool bFaceEnemy = (bool) GetCharacter()->GetViewVariantHolder()->GetNumber<bool>("Attribute/FightDir");
	
	if (bFaceEnemy)
		GetCharacter()->CreateNpcDirTick();
	else
		GetCharacter()->DestroyNpcDirTick();
	return 0;
}

int AddMasterVariantCallback::exec(CCypherVariant *pVariant)
{
	Ast (GetCharacter());

	const CSyncVariantClientHolder* pViewVarHolder = GetCharacter()->GetViewVariantHolder();
	ECharacterType eType = (ECharacterType) pViewVarHolder->GetNumber<uint8>("Attribute/Type");
	
	if (eType == eCT_Npc)
	{
		uint32 uOldMasterID = GetCharacter()->GetMasterID();
		pViewVarHolder = GetCharacter()->GetViewVariantHolder(eCTI_Agile);
		uint32 uNewMasterID = pViewVarHolder->GetNumber<uint32>("Master/MasterID");
		uint32 uDirectorID = CEntityClientManager::GetInst()->GetDirectorID();
		if (uOldMasterID != 0)
		{
			if (uOldMasterID == uDirectorID)
			{
				CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(uDirectorID);
				CCharacterDirector* pDirector = pCharacter->CastToCharacterDirector();
				pDirector->RemoveServant(GetCharacter()->GetNpcType(), GetCharacter()->GetEntityID());
			}
			else
				GetCharacter()->SetMaster(0);
		}
		if (uNewMasterID != 0)
		{
			if (uNewMasterID == uDirectorID)
			{
				CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(uDirectorID);
				CCharacterDirector* pDirector = pCharacter->CastToCharacterDirector();
				pDirector->AddServant(GetCharacter()->GetNpcType(), GetCharacter()->GetEntityID());
			}
			else
				GetCharacter()->SetMaster(uNewMasterID);
		}
	}

	return 0;
}

int BirthCampChangeVariantCallback::exec(CCypherVariant* pVariant)
{
	Ast (GetCharacter());
	const CSyncVariantClientHolder* pViewVarHolder = GetCharacter()->GetViewVariantHolder();
	ECamp eBirthCamp = (ECamp)pViewVarHolder->GetNumber<uint8>("Attribute/BirthCamp");
	GetCharacter()->CppSetBirthCamp(eBirthCamp);

	return 0;
}

int ShadowVariantVariantCallback::exec(CCypherVariant* pVariant)
{
	Ast (GetCharacter());
	GetCharacter()->InitIsShadowVariant();
	return 0;
}

int StealthVariantCallback::exec(CCypherVariant* pVariant)
{
	Ast (GetCharacter());
	bool bStealth = GetCharacter()->GetSyncVariantHolder(eCTI_Agile)->GetNumber<bool>("Attribute1/Stealth");
	GetCharacter()->SetStealthState(bStealth);
	return 0;
}

int MaterialNumVariantCallback::exec(CCypherVariant* pVariant)
{
	Ast (GetCharacter());
	uint32 uMaterialNum = GetCharacter()->GetSyncVariantHolder(eCTI_Agile)->GetNumber<uint32>("Attribute1/MaterialNum");
	GetCharacter()->OnMaterialNumChange(uMaterialNum);
	return 0;
}

int ExpOwnerIDVariantCallback::exec(CCypherVariant* pVariant)
{
	Ast (GetCharacter());
	GetCharacter()->OnExpOwnerChange();

	return 0;
}

