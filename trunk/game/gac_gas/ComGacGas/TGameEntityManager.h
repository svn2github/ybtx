#pragma once

#include "TIDPtrMap.h"
#include "CEntityMallocObject.h"

template < class GameEntityType >
class TGameEntityManager : public CEntityMallocObject
{
	typedef TIDPtrMap<GameEntityType> GameEntityMapType;
protected:
	TGameEntityManager() {}
	~TGameEntityManager()
	{
		DeleteAll();
	}
	GameEntityMapType m_GameEntityMap;
public:
	void DeleteAll();
	bool AddGameEntity(GameEntityType *pGameEntity, uint32& uGameEntityID);
	void RemoveGameEntity(uint32 uObjID);
	void RemoveGameEntity(GameEntityType* pGameEntity);
	GameEntityType* GetGameEntity(uint32 uObjID);
};
