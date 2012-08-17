#include "stdafx.h"
#include "TServantMgr.h"
#include "ServantType.h"
#include "TSqrAllocator.inl"

template < class GameEntityType >
bool TServantMgr<GameEntityType>::AddServant( ENpcType eNpcType, uint32 uEntityID, GameEntityType *pServant )
{
	bool bServant = ServantType::BeServantType(eNpcType);
	if (!bServant)
		return false;

	if (!m_mapServantByID.insert(make_pair(uEntityID, pServant)).second)
		return false;

	if (!m_mapServantByType[eNpcType].insert(uEntityID).second)
	{
		m_mapServantByID.erase(uEntityID);
		return false;
	}
	OnServantChanged();
	return true;
}

template < class GameEntityType >
void TServantMgr<GameEntityType>::RemoveServant( ENpcType pNpcType, uint32 uEntityID, GameEntityType* pServant )
{
	if (m_mapServantByID.find(uEntityID) == m_mapServantByID.end())
		return;
	if (m_mapServantByID.erase(uEntityID))
	{
		ServantTypeMapType::iterator it = m_mapServantByType.find( pNpcType );
		if (it == m_mapServantByType.end())
			return;
		it->second.erase(uEntityID);
		this->OnRemoveServant(pServant);
	}
}

template < class GameEntityType >
GameEntityType* TServantMgr<GameEntityType>::GetServantByID( uint32 uGlobalID ) const
{
	typename ServantIDMapType::const_iterator it = m_mapServantByID.find( uGlobalID );
	if (it == m_mapServantByID.end())
		return NULL;
	return it->second;
}

template < class GameEntityType >
const typename TServantMgr<GameEntityType>::ServantSetType& TServantMgr<GameEntityType>::GetServantSetByType( ENpcType eType ) const
{
	static typename TServantMgr<GameEntityType>::ServantSetType ServantSet;
	ServantTypeMapType::const_iterator it = m_mapServantByType.find(eType);
	if (it == m_mapServantByType.end())
		return ServantSet;

	return it->second;
}

template < class GameEntityType >
typename TServantMgr<GameEntityType>::ServantIDMapType& TServantMgr<GameEntityType>::GetServantIDMapType()
{
	return m_mapServantByID;
}

template <class GameEntityType>
uint32 TServantMgr<GameEntityType>::GetFirstServantID()
{
	typename ServantIDMapType::iterator iter = m_mapServantByID.begin();
	return iter->first;
}

template < class GameEntityType >
uint32  TServantMgr<GameEntityType>::GetServantIDByType( ENpcType eType ) const
{
	const ServantSetType& ServantSet = GetServantSetByType(eType);
	return ServantSet.empty() ? 0 : *ServantSet.begin();
}

template < class GameEntityType >
int32 TServantMgr<GameEntityType>::GetServantNumByID() const
{
	return int32(m_mapServantByID.size());
}

template <class GameEntityType>
void TServantMgr<GameEntityType>::ClearAllServant()
{
	m_mapServantByID.clear();
	m_mapServantByType.clear();
}

template < class GameEntityType >
TServantMgr<GameEntityType>::~TServantMgr()
{
	m_mapServantByID.clear();
	m_mapServantByType.clear();
}
