#include "stdafx.h"
#include "CEntityClientManager.h"
#include "CEntityClient.h"
#include "CCharacterFollower.h"
#include "CIntObjClient.h"
#include "TIDPtrMap.inl"

CEntityClientManager* CEntityClientManager::ms_pInst = NULL;

void CEntityClientManager::DeleteAll()
{
	for(uint32 i = 0; i < m_vecEntity.size(); ++i)
	{
		delete m_vecEntity[i];
		m_vecEntity[i] = NULL;
	}
}

bool CEntityClientManager::AddGameEntity(CEntityClient* pGameEntity, uint32 uGameEntityID)
{
	const uint32 uIndex = TIDPtrMap<CEntityClient>::GetIndexByID(uGameEntityID);
	if( m_vecEntity.size() <= uIndex )
		m_vecEntity.resize( uIndex+1 );
	m_vecEntity[uIndex] = pGameEntity;
	return true;
}

void CEntityClientManager::RemoveGameEntity(uint32 uObjID)
{
	const uint32 uIndex = TIDPtrMap<CEntityClient>::GetIndexByID(uObjID);
	m_vecEntity[uIndex] = NULL;
}

void CEntityClientManager::RemoveGameEntity(CEntityClient* pGameEntity)
{
	RemoveGameEntity(pGameEntity->GetEntityID());
}

CEntityClient* CEntityClientManager::GetGameEntity(uint32 uObjID) const
{
	const uint32 uIndex = TIDPtrMap<CEntityClient>::GetIndexByID(uObjID);
	if(m_vecEntity.size() <= uIndex)
		return NULL;
	return m_vecEntity[uIndex];
}

CEntityClient* CEntityClientManager::GetEntityByID(uint32 uEntityID)
{
	return GetInst()->GetGameEntity(uEntityID);
}

const CEntityClientManager* CEntityClientManager::GetInst()
{
	return ms_pInst;
}

void CEntityClientManager::CreateInst()
{
	Ast(ms_pInst == NULL);
	ms_pInst = new CEntityClientManager;
}

void CEntityClientManager::DestroyInst()
{
	delete ms_pInst;
	ms_pInst = NULL;
}

void CEntityClientManager::SetEntityCallbackHandler(CEntityClient* pEntity)
{
	Ast(GetHandler());
	GetHandler()->OnGameEntityAdded(pEntity);
}

void CEntityClientManager::SetDirectorID(uint32 uDirectorID)
{
	m_uDirectorID = uDirectorID;
}

uint32	CEntityClientManager::GetDirectorID() const 
{
	return m_uDirectorID;
}

CCharacterFollower* CEntityClientManager::GetCharacter(uint32 uObjID)const
{
	CEntityClient* pEntity = GetGameEntity(uObjID);
	if(pEntity == NULL || pEntity->GetGameEntityType() != eGET_CharacterDirector && pEntity->GetGameEntityType() != eGET_CharacterFollower)
	{
		return NULL;
	}
	return class_cast<CCharacterFollower*>(pEntity);
}

CIntObjClient* CEntityClientManager::GetIntObject(uint32 uObjID)const
{
	CEntityClient* pEntity = GetGameEntity(uObjID);
	if(pEntity == NULL || pEntity->GetGameEntityType() != eGET_IntObject)
	{
		return NULL;
	}
	return class_cast<CIntObjClient*>(pEntity);
}
