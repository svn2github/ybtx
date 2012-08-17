#include "stdafx.h"
#include "CEntityServerManager.h"
#include "CCharacterDictator.h"
#include "CIntObjServer.h"
#include "CObjIDPool.h"
#include "BaseHelper.h"
#include "TGameEntityManager.inl"

template class TGameEntityManager<CEntityServer>;

CEntityServerManager* CEntityServerManager::ms_pInst = NULL;

CEntityServerManager* CEntityServerManager::GetInst()
{
	return ms_pInst;
}

void CEntityServerManager::CreateInst()
{
	Ast(ms_pInst == NULL);
	ms_pInst = new CEntityServerManager;
}

void CEntityServerManager::DestroyInst()
{
	Ast( ms_pInst );
	delete ms_pInst;
	ms_pInst = NULL;
}

CEntityServerManager::CEntityServerManager()
{
	m_pIDPool = new CGameEntityIDPool;
}


CEntityServerManager::~CEntityServerManager()
{
	SafeDelete(m_pIDPool);
	DeleteAll();
}

CCharacterDictator* CEntityServerManager::GetCharacter(uint32 uObjID)
{
	CEntityServer* pEntity = GetGameEntity(uObjID);
	if( !pEntity )
		return NULL;
	
	EGameEntityType eType=pEntity->GetGameEntityType();

	if ( (eType != eGET_CharacterDictator) && (eType != eGET_CharacterMediator ) )
		return NULL;
	
	return class_cast<CCharacterDictator*>(pEntity);
}

CIntObjServer* CEntityServerManager::GetIntObject(uint32 uObjID)
{
	CEntityServer* pEntity = GetGameEntity(uObjID);
	if(pEntity == NULL || pEntity->GetGameEntityType() != eGET_IntObject )
	{
		return NULL;
	}
	return class_cast<CIntObjServer*>(pEntity);
}

uint32 CEntityServerManager::GenerateEntityID()const
{
	return m_pIDPool->GenerateID();
}

