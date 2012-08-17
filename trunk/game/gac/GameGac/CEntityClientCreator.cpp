#include "stdafx.h"
#include "CEntityClientCreator.h"
#include "CEntityClientManager.h"
#include "CCharacterFollower.h"
#include "CCharacterDirector.h"
#include "CIntObjClient.h"
#include "CEntityClient.h"

CCharacterFollower* CEntityClientCreator::CreateCharacterFollower(CCoreObjectFollower* pCoreObj, uint32 uEntityID, bool bIsBattleHorse)
{
	Ast(pCoreObj);
	CCharacterFollower* pFollower = new CCharacterFollower(pCoreObj, uEntityID, bIsBattleHorse);
	Ast(pFollower);
	const_cast<CEntityClientManager*>(CEntityClientManager::GetInst())->SetEntityCallbackHandler(pFollower);
	return pFollower;
}

CCharacterDirector* CEntityClientCreator::CreateCharacterDirector(CCoreObjectDirector* pCoreObj, uint32 uEntityID)
{
	Ast (pCoreObj);
	CCharacterDirector* pDirector = new CCharacterDirector(pCoreObj, uEntityID);
	Ast (pDirector);
	const_cast<CEntityClientManager*>(CEntityClientManager::GetInst())->SetEntityCallbackHandler(pDirector);
	return pDirector;
}

CIntObjClient* CEntityClientCreator::CreateIntObjClient(CCoreObjectFollower *pCoreObj, uint32 uEntityID)
{
	Ast (pCoreObj);
	CIntObjClient* pIntObj = new CIntObjClient(pCoreObj, uEntityID);
	Ast (pIntObj);
	const_cast<CEntityClientManager*>(CEntityClientManager::GetInst())->SetEntityCallbackHandler(pIntObj);
	return pIntObj;
}

void CEntityClientCreator::DestroyEntityClient(CEntityClient* pEntity)
{
	pEntity->OnDestroy();
}
