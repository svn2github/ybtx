#pragma once
#include "TRootVariantPoolMgr.h"
#include "BaseHelper.h"
#include "TObjectBank.inl"
#include "TSqrAllocator.inl"

template<typename VariantType>
typename TRootVariantPoolMgr<VariantType>::VariantPool* TRootVariantPoolMgr<VariantType>::GetPool(uint32 uVarDefID, bool bForSync, ECodeTableIndex eCodeTableIndex)
{
	MapVariantPool* pMapPool;
	if (bForSync)
	{
		if (m_vecPoolForSync.size() <= (uint32)eCodeTableIndex || !m_vecPoolForSync[eCodeTableIndex])
		{
			for (uint32 i=m_vecPoolForSync.size(); i<=(uint32)eCodeTableIndex; i++)
			{
				m_vecPoolForSync.push_back(new MapVariantPool);
			}
		}
		pMapPool = m_vecPoolForSync[eCodeTableIndex];
	}
	else
	{
		if (m_vecPoolForView.size() <= (uint32)eCodeTableIndex || !m_vecPoolForView[eCodeTableIndex])
		{
			for (uint32 i=m_vecPoolForView.size(); i<=(uint32)eCodeTableIndex; i++)
			{
				m_vecPoolForView.push_back(new MapVariantPool);
			}
		}
		pMapPool = m_vecPoolForView[eCodeTableIndex];
	}

	MapVariantPool& mapPool = *pMapPool;

	typename MapVariantPool::iterator it = mapPool.find(uVarDefID);
	if(it != mapPool.end())
		return it->second;

	VariantPool* pPool = new VariantPool(5000);
	mapPool.insert(make_pair(uVarDefID, pPool));
	return pPool;
}

template<typename VariantType>
typename TRootVariantPoolMgr<VariantType>::VariantPool* TRootVariantPoolMgr<VariantType>::GetSubTreePool(uint32 uSubTreeType)
{
	typename MapVariantPool::iterator it = m_mapSubTreePool.find(uSubTreeType);
	if(it != m_mapSubTreePool.end())
		return it->second;

	VariantPool* pPool = new VariantPool(5000);
	m_mapSubTreePool.insert(make_pair(uSubTreeType, pPool));
	return pPool;
}

template<typename VariantType>
TRootVariantPoolMgr<VariantType>::TRootVariantPoolMgr()
{
}

template<typename VariantType>
TRootVariantPoolMgr<VariantType>::~TRootVariantPoolMgr()
{
	for(size_t i = 0; i < m_vecPoolForSync.size(); ++i)
	{
		ClearMap(*m_vecPoolForSync[i]);
		SafeDelete(m_vecPoolForSync[i]);
	}
	m_vecPoolForSync.clear();
	for(size_t i = 0; i < m_vecPoolForView.size(); ++i)
	{
		ClearMap(*m_vecPoolForView[i]);
		SafeDelete(m_vecPoolForView[i]);
	}
	m_vecPoolForView.clear();
	ClearMap(m_mapSubTreePool);
}
