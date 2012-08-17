#pragma once
#include "TGameEntityManager.h"
#include "TIDPtrMap.inl"
#include "TSqrAllocator.inl"


template < class GameEntityType >
void TGameEntityManager<GameEntityType>::DeleteAll()
{
	typename GameEntityMapType::Iterator it = m_GameEntityMap.Begin();
	for(; it != m_GameEntityMap.End(); ++it)
	{
		it->Release();	//此处假设CEntity会自己把自己从map中删除掉
	}
}

template < class GameEntityType >
bool TGameEntityManager<GameEntityType>::AddGameEntity(GameEntityType *pGameEntity, uint32& uGameEntityID)
{
	uGameEntityID = m_GameEntityMap.Add(pGameEntity);

	return true;
}

template < class GameEntityType >
void TGameEntityManager<GameEntityType>::RemoveGameEntity(uint32 uObjID)
{
	if (!m_GameEntityMap.Del(uObjID))
		return;
}

template < class GameEntityType >
void TGameEntityManager<GameEntityType>::RemoveGameEntity(GameEntityType* pGameEntity)
{
	if (!m_GameEntityMap.Del(pGameEntity->GetEntityID()))
		GenErr("Del CEntityServer Err!");
}

template < class GameEntityType >
inline GameEntityType* TGameEntityManager<GameEntityType>::GetGameEntity(uint32 uObjID)
{
	GameEntityType* pGameEntity;
	if(!m_GameEntityMap.Get(pGameEntity, uObjID))
		return NULL;
	return pGameEntity;
}

