#pragma once
#include <set>
#include <map>
#include "GameDef.h"
#include "CDynamicObject.h"
#include "CServantMallocObject.h"
#include "TServantAllocator.h"

template < class GameEntityType >
class TServantMgr
	: public virtual CDynamicObject
	, public CServantMallocObject
{
public:
	typedef set< uint32, less<uint32>, TServantAllocator<uint32> > ServantSetType;
	typedef map<uint32, ServantSetType, less<uint32>, TServantAllocator<pair<uint32, ServantSetType > > > ServantTypeMapType;
	typedef map<uint32, GameEntityType*,  less<uint32>, TServantAllocator<pair<uint32, GameEntityType* > > > ServantIDMapType;
public:
	TServantMgr(GameEntityType* pOwner) : m_pOwner(pOwner){}
	~TServantMgr();
	GameEntityType* GetOwner() const {return m_pOwner;}
	bool AddServant( ENpcType eNpcType, uint32 uEntityID, GameEntityType *pServant );
	void RemoveServant( ENpcType pNpcType, uint32 uEntityID, GameEntityType* pServant );
	GameEntityType* GetServantByID( uint32 uGlobalID ) const;
	uint32 GetServantIDByType( ENpcType eType ) const;
	int32 GetServantNumByID() const;
	const ServantSetType& GetServantSetByType( ENpcType eType ) const;
	ServantIDMapType& GetServantIDMapType();
	void ClearAllServant();
	virtual void OnRemoveServant(GameEntityType* pServant ) {}
	uint32	GetFirstServantID();
	uint32 GetServantNum(){return m_mapServantByID.size();}

	virtual void OnServantChanged(){}

private:
	GameEntityType*				m_pOwner;
	ServantIDMapType			m_mapServantByID;
	ServantTypeMapType			m_mapServantByType;
};
