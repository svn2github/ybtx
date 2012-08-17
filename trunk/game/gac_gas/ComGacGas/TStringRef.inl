#pragma once
#include "TStringRef.h"
#include "BaseDefs.h"


template<typename HolderType, typename RefeeType>
TStringRefer<HolderType, RefeeType>::TStringRefer(HolderType* pHolder)
:m_pHolder(pHolder)
{
}

template<typename HolderType, typename RefeeType>
TStringRefer<HolderType, RefeeType>::TStringRefer(HolderType* pHolder, const string& strKey)
:m_pHolder(pHolder)
{
	SetStringKey(strKey);
}

template<typename HolderType, typename RefeeType>
TStringRefer<HolderType, RefeeType>::~TStringRefer()
{
	ResetRefee();
}


template<typename HolderType, typename RefeeType>
void TStringRefer<HolderType, RefeeType>::OnRefeeChanged()const
{
	HolderType::UpdateCfg(m_pHolder->GetName());
}

template<typename HolderType, typename RefeeType>
void TStringRefer<HolderType, RefeeType>::ResetRefee()
{
	if(m_spRefee)
	{
		m_spRefee->Detach(m_itReferList);
		m_spRefee.reset();
	}
}

template<typename HolderType, typename RefeeType>
void TStringRefer<HolderType, RefeeType>::SetStringKey(const string& strKey)
{
	ResetRefee();

	typename RefeeType::MapImpClass& mapRefee = RefeeType::GetImpClassMap();
	typename RefeeType::MapImpClass::iterator it = mapRefee.find(strKey);
	if(it != mapRefee.end())
	{
		m_spRefee = *(it->second);
		m_itReferList = m_spRefee->Attach(this);
	}
}

template<typename HolderType, typename RefeeType>
void TStringRefer<HolderType, RefeeType>::GetStringKey(string& strKey)const
{
	if(!m_spRefee)
	{
		strKey = "";
		return;
	}

	strKey = m_spRefee->GetName();
}

template<typename HolderType, typename RefeeType>
const typename TStringRefer<HolderType, RefeeType>::RefeeSharedPtrType& TStringRefer<HolderType, RefeeType>::Get()const
{
	return m_spRefee;
}

template<typename ImpClass, typename ImpClassSharedPtr>
typename TStringRefee<ImpClass, ImpClassSharedPtr>::MapImpClass& TStringRefee<ImpClass, ImpClassSharedPtr>::GetImpClassMap()
{
	static MapImpClass ms_mapImpClass;
	return ms_mapImpClass;
}

template<typename ImpClass, typename ImpClassSharedPtr>
const string& TStringRefee<ImpClass, ImpClassSharedPtr>::GetName()const
{
	return static_cast<const ImpClass*>(this)->GetName();
}

template<typename ImpClass, typename ImpClassSharedPtr>
TStringRefee<ImpClass, ImpClassSharedPtr>::ListStringRefer::iterator TStringRefee<ImpClass, ImpClassSharedPtr>::Attach(IBaseStringRefer* pRefer)
{
	m_listRefer.push_front(pRefer);
	return m_listRefer.begin();
}

template<typename ImpClass, typename ImpClassSharedPtr>
void TStringRefee<ImpClass, ImpClassSharedPtr>::Detach(ListStringRefer::iterator& it)
{
	m_listRefer.erase(it);
}

template<typename ImpClass, typename ImpClassSharedPtr>
void TStringRefee<ImpClass, ImpClassSharedPtr>::NotifyRefer()const
{
	typename ListStringRefer::const_iterator it = m_listRefer.begin();
	for(; it != m_listRefer.end();)
	{
		ListStringRefer::const_iterator it_bak = it;
		++it_bak;
		(*it)->OnRefeeChanged();
		it = it_bak;
	}
}

template<typename ImpClass, typename ImpClassSharedPtr>
TStringRefee<ImpClass, ImpClassSharedPtr>::TStringRefee()
{
}

template<typename ImpClass, typename ImpClassSharedPtr>
TStringRefee<ImpClass, ImpClassSharedPtr>::~TStringRefee()
{
	//NotifyRefer();
}
