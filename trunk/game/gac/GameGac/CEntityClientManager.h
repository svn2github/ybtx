#pragma once
#include "CStaticObject.h"
#include "IEntityClientManagerHandler.h"

class CEntityClient;
class CCharacterFollower;
class CIntObjClient;

class CEntityClientManager
	:public virtual CStaticObject
{
	CEntityClientManager() {}
	~CEntityClientManager()	{}

public:
	void DeleteAll();
	bool AddGameEntity(CEntityClient* pGameEntity, uint32 uGameEntityID);
	void RemoveGameEntity(uint32 uObjID) ;
	void RemoveGameEntity(CEntityClient* pGameEntity);
	CEntityClient* GetGameEntity(uint32 uObjID) const;

	CCharacterFollower* GetCharacter(uint32 uObjID)const;
	CIntObjClient* GetIntObject(uint32 uObjID)const;
	static CEntityClient* GetEntityByID(uint32 uEntityID);
	static const CEntityClientManager* GetInst();
	static void CreateInst();
	static void DestroyInst();
	void SetHandler( IEntityClientManagerHandler* pHandler) { m_pHandler = pHandler; }
	IEntityClientManagerHandler* GetHandler() const { return m_pHandler; }
	void SetEntityCallbackHandler(CEntityClient* pEntity);
	void SetDirectorID(uint32 uDirectorID);
	uint32	GetDirectorID() const;
private:
	typedef vector<CEntityClient*> VecEntity;
	VecEntity						m_vecEntity;
	IEntityClientManagerHandler*	m_pHandler;
	static CEntityClientManager*	ms_pInst;
	uint32							m_uDirectorID;
};

