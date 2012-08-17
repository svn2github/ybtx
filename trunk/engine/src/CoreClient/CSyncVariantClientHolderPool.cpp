#include "stdafx.h"
#include "CSyncVariantClientHolderPool.h"
#include "TSqrAllocator.inl"
#include "BaseHelper.h"
#include "CTimeSystemClient.h"

CSyncVariantClientHolderPackage::CSyncVariantClientHolderPackage(uint32 uGlobalID)
:m_uGlobalID(uGlobalID)
{
	CTimeSystemClient::Inst()->Register(this, 2*60*1000);
}

CSyncVariantClientHolderPackage::~CSyncVariantClientHolderPackage()
{
	m_vecSyncVariantHolder.clear();
	m_vecViewVariantHolder.clear();	
}

void CSyncVariantClientHolderPackage::Release()
{
	ClearVector(m_vecSyncVariantHolder);
	ClearVector(m_vecViewVariantHolder);
}

void CSyncVariantClientHolderPackage::OnTick()
{
	CSyncVariantClientHolderPool::GetInst()->GetSyncVariantClientHolderById(m_uGlobalID);
	Release();
	delete this;
}

CSyncVariantClientHolderPool::CSyncVariantClientHolderPool()
{
}

CSyncVariantClientHolderPool::~CSyncVariantClientHolderPool()
{
	Clear();	
}

CSyncVariantClientHolderPackage* CSyncVariantClientHolderPool::GetSyncVariantClientHolderById(uint32 uId)
{
	MapSyncVariantClientHolder::iterator it = m_mapSyncVariantClientHolder.find(uId);
	if (it == m_mapSyncVariantClientHolder.end())
		return NULL;
	m_mapSyncVariantClientHolder.erase(it);
	CTimeSystemClient::Inst()->UnRegister(it->second);
	return it->second;
}

void CSyncVariantClientHolderPool::PushSyncVariantClientHolder(uint32 uId, CSyncVariantClientHolderPackage* pData)
{
	MapSyncVariantClientHolder::iterator it = m_mapSyncVariantClientHolder.find(uId);
	if (it != m_mapSyncVariantClientHolder.end())
		GenErr("出现ID重复的对象删除VariantHolder");
	m_mapSyncVariantClientHolder.insert(make_pair(uId, pData));
}

void CSyncVariantClientHolderPool::Clear()
{
	MapSyncVariantClientHolder::iterator it = m_mapSyncVariantClientHolder.begin();
	for (; it!=m_mapSyncVariantClientHolder.end(); it++)
	{
		it->second->Release();
	}
	ClearMap(m_mapSyncVariantClientHolder);
}
