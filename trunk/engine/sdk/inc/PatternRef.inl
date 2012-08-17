#pragma once
#include "PatternRef.h"
#include "ExpHelper.h"
#include "PtRefProxy.inl"

/*
对于一个对象(refee)，可以被多个对象(refer)引用
refer与refee之间，通过一个引用代理类proxy联系，
refer通过proxy访问refee
当refer每attach refee的时候，proxy上面的引用计数加一
当refer每detach refee的时候，proxy上面的引用计数减一
当proxy的引用计数为0的时候，表明没有refer去引用refee了
当refee对象删除的时候，如果有对应的proxy，那么把proxy上面指向自己的指针清空
这样refer对象通过proxy得到的指针就为null，防止野指针
*/

template < class HolderType, class PointeeHolderType >
TPtRefBase<HolderType,PointeeHolderType>::TPtRefBase()
{
	m_pHolder = NULL;    
}

template < class HolderType, class PointeeHolderType >
inline HolderType* TPtRefBase<HolderType,PointeeHolderType>::GetHolder() const
{
	return m_pHolder;
}

template < class HolderType, class PointeeHolderType >
void TPtRefBase<HolderType,PointeeHolderType>::SetHolder(HolderType* pHolder)
{
	m_pHolder = pHolder;
}



template < class HolderType, class PointeeHolderType >
TPtRefer<HolderType,PointeeHolderType>::TPtRefer( HolderType* pHolder )
:m_pProxy(NULL)
{
	this->SetHolder(pHolder);
}

template < class HolderType, class PointeeHolderType >
TPtRefer<HolderType,PointeeHolderType>::TPtRefer( HolderType* pHolder, RefeeType& Refee)
:m_pProxy(NULL)  
{
	this->SetHolder(pHolder);
	this->Attach(Refee);
}

template < class HolderType, class PointeeHolderType >
TPtRefer<HolderType,PointeeHolderType>::TPtRefer( const TPtRefer& Other)
:m_pProxy(Other.m_pProxy)  
{
	if(m_pProxy)m_pProxy->m_uRefNum++;
}

template < class HolderType, class PointeeHolderType >
TPtRefer<HolderType,PointeeHolderType>::~TPtRefer()
{
	Detach();
}

template < class HolderType, class PointeeHolderType >
inline PointeeHolderType* TPtRefer<HolderType,PointeeHolderType>::Get() const
{
	return 	m_pProxy ? m_pProxy->GetHolder() : NULL;
}

template < class HolderType, class PointeeHolderType >
void TPtRefer<HolderType,PointeeHolderType>::Attach(RefeeType& Refee)
{
	//首先判断是不是去引用了另一个对象
	if(m_pProxy)
	{
		if(m_pProxy == Refee.m_pProxy)
		{
			return;
		}
		//如果是，则先detach原先的引用对象
		if(!Refee.m_pProxy || m_pProxy != Refee.m_pProxy)
			Detach();
	}

	if(!Refee.m_pProxy)
	{
		Refee.m_pProxy = TPtProxyBankMgr<PointeeHolderType,HolderType>::Inst()->PopOrNew();
		Refee.m_pProxy->SetHolder(&Refee);
	}

	m_pProxy = Refee.m_pProxy;
	m_pProxy->m_uRefNum++;
}	

template < class HolderType, class PointeeHolderType >
void TPtRefer<HolderType,PointeeHolderType>::Detach()
{
	if(!m_pProxy)
		return;

	//如果ref num为0，表明没有refee没有被引用了
	if( --m_pProxy->m_uRefNum == 0)
		TPtProxyBankMgr<PointeeHolderType,HolderType>::Inst()->Push(m_pProxy);

	m_pProxy = NULL;
}

template < class HolderType, class PointeeHolderType >
const bool TPtRefer<HolderType,PointeeHolderType>::operator < ( const ReferType& Other ) const
{
	return Get() < Other.Get();
}

template < class HolderType, class PointerHolderType >
TPtRefee<HolderType,PointerHolderType>::TPtRefee(HolderType *pHolder)
: m_pProxy(NULL)              
{
	this->SetHolder(pHolder);
}

template < class HolderType, class PointerHolderType >
TPtRefee<HolderType,PointerHolderType>::~TPtRefee()
{
	if(m_pProxy)
		m_pProxy->m_pRefee = NULL;
	m_pProxy = NULL;
}



template < class HolderType, class PointeeHolderType >
TPtBiRef<HolderType,PointeeHolderType>::TPtBiRef( HolderType* pHolder )
:m_pPointee(NULL)      
{
	this->SetHolder(pHolder);
}

template < class HolderType, class PointeeHolderType >
TPtBiRef<HolderType,PointeeHolderType>::TPtBiRef( HolderType* pHolder, PointeeType& Pointee )
:m_pPointee(NULL)
{
	this->SetHolder(pHolder);
	this->Attach(Pointee);
}

template < class HolderType, class PointeeHolderType >
TPtBiRef<HolderType,PointeeHolderType>::~TPtBiRef()
{
	Detach();
}

template < class HolderType, class PointeeHolderType >
inline PointeeHolderType* TPtBiRef<HolderType,PointeeHolderType>::Get() const
{
	return 	m_pPointee ? m_pPointee->GetHolder() : NULL;
}

template < class HolderType, class PointeeHolderType >
void TPtBiRef<HolderType,PointeeHolderType>::Attach(PointeeType& Pointee)
{
	Ast((void*)&Pointee != (void*)this);
	this->UniAttach(Pointee);
	Pointee.UniAttach(*this);
}

template < class HolderType, class PointeeHolderType >
void TPtBiRef<HolderType,PointeeHolderType>::Detach()
{
	if (m_pPointee != NULL)
		m_pPointee->Detach(*this);
}

template < class HolderType, class PointeeHolderType >
const bool TPtBiRef<HolderType,PointeeHolderType>::operator < ( const ThisType& Other ) const
{
	return Get() < Other.Get();
}

template < class HolderType, class PointeeHolderType >
void TPtBiRef<HolderType,PointeeHolderType>::Detach(PointeeType& Pointee)
{
	this->UniDetach(Pointee);
	Pointee.UniDetach(*this);
}

template < class HolderType, class PointeeHolderType >
void TPtBiRef<HolderType,PointeeHolderType>::UniAttach(PointeeType& Pointee)
{
	if (m_pPointee)
		Detach(*m_pPointee);
	m_pPointee = &Pointee;
}

template < class HolderType, class PointeeHolderType >
void TPtBiRef<HolderType,PointeeHolderType>::UniDetach(PointeeType& Pointee)
{
	if (&Pointee == m_pPointee)
		m_pPointee = NULL;
}
