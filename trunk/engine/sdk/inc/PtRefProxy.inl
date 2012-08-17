#pragma once
#include "PtRefProxy.h"
#include "TObjectBank.inl"
#include "PatternRef.inl"
#include "ThreadHelper.h"

template < class HolderType, class PointeeHolderType >
TPtProxy<HolderType,PointeeHolderType>::TPtProxy()
: m_pRefee(NULL)
, m_uRefNum(0) 
{
}

template < class HolderType, class PointeeHolderType >
TPtProxy<HolderType,PointeeHolderType>::~TPtProxy()
{
	m_pRefee = NULL;
}

template < class HolderType, class PointeeHolderType >
void TPtProxy<HolderType,PointeeHolderType>::SetHolder(RefeeType* pRefee)
{
	m_pRefee = pRefee;
}

template < class HolderType, class PointeeHolderType >
inline HolderType*	TPtProxy<HolderType,PointeeHolderType>::GetHolder() const
{
	if(m_pRefee)
		return m_pRefee->GetHolder();
	return NULL;
}




template < class HolderType, class PointeeHolderType > 
HTLSKEY TPtProxyBankMgr<HolderType,PointeeHolderType>::ms_tlsPtRefKey;

template < class HolderType, class PointeeHolderType >
TPtProxyBankMgr<HolderType,PointeeHolderType>::TPtProxyBankMgr()
{
	TLS_CreateKey(&ms_tlsPtRefKey);
}

template < class HolderType, class PointeeHolderType >
TPtProxyBankMgr<HolderType,PointeeHolderType>::~TPtProxyBankMgr()
{
	TLS_DestroyKey(ms_tlsPtRefKey);
}

template < class HolderType, class PointeeHolderType >
void TPtProxyBankMgr<HolderType,PointeeHolderType>::Push(Proxy_t* pProxy)
{
	if(pProxy->m_pRefee)
		pProxy->m_pRefee->m_pProxy = NULL;
	pProxy->m_pRefee = NULL;
	pProxy->m_uRefNum = 0;

	ProxyBank_t* pProxyBank = (ProxyBank_t*)TLS_GetValue(ms_tlsPtRefKey);
	pProxyBank->Push(pProxy);
}

template < class HolderType, class PointeeHolderType >
TPtProxy<HolderType, PointeeHolderType>* TPtProxyBankMgr<HolderType,PointeeHolderType>::PopOrNew()
{
	ProxyBank_t* pProxyBank = (ProxyBank_t*)TLS_GetValue(ms_tlsPtRefKey);
	return pProxyBank->PopOrNew();
}

template < class HolderType, class PointeeHolderType >
void TPtProxyBankMgr<HolderType,PointeeHolderType>::CreateProxyBank()
{
	ProxyBank_t* pProxyBank = new ProxyBank_t(5000);		//因为引用的对象非常多，所以默认开5000个
	TLS_SetValue(ms_tlsPtRefKey, pProxyBank);
}

template < class HolderType, class PointeeHolderType >
void TPtProxyBankMgr<HolderType,PointeeHolderType>::DestroyProxyBank()
{
	ProxyBank_t* pProxyBank = (ProxyBank_t*)TLS_GetValue(ms_tlsPtRefKey);
	TLS_SetValue(ms_tlsPtRefKey, NULL);
	delete pProxyBank;
}

