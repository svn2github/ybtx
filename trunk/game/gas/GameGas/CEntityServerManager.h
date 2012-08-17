#pragma once
#include "CEntityServer.h"
#include "TGameEntityManager.h"

class CCharacterDictator;
class CIntObjServer;
class IObjIDPool;

class CEntityServerManager 
	: public TGameEntityManager<CEntityServer>
{
	CEntityServerManager();
	~CEntityServerManager();
public:
	static CEntityServerManager* GetInst();
	static void CreateInst();
	static void DestroyInst();
	static CEntityServer* GetEntityByID(uint32 uEntityID);

	CCharacterDictator* GetCharacter(uint32 uObjID);
	
	CIntObjServer* GetIntObject(uint32 uObjID);

	uint32 GenerateEntityID()const;
private:
	IObjIDPool* m_pIDPool;

	static CEntityServerManager* ms_pInst;
};


inline CEntityServer* CEntityServerManager::GetEntityByID(uint32 uEntityID)
{
	return GetInst()->GetGameEntity(uEntityID);
}
