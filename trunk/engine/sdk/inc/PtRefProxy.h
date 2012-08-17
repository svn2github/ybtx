#pragma once

#include "TSqrAllocObject.h"
#include "TSqrAllocator.h"
#include "ThreadTypes.h"

namespace sqr
{
	DeclareAllocObject(CPTREFMallocObject);
	DeclarePoolAllocator(CPTREFMallocAllocator, CPTREFMallocObject);

	template<typename,template<typename>class>
	class TObjectBank;

	template < class HolderType, class PointeeHolderType >
	class TPtProxy;

	template < class HolderType, class PointeeHolderType >
	class TPtRefee;

	template < class HolderType, class PointeeHolderType >
	class TPtRefer;

	template < class HolderType, class PointeeHolderType >
	class TPtProxyBankMgr;

	template < class HolderType, class PointeeHolderType >
	class TPtProxy 
		: public CPTREFMallocObject
	{
		typedef TPtRefee<HolderType, PointeeHolderType> RefeeType;
		friend class TPtRefer<PointeeHolderType, HolderType>;
		friend class TPtRefee<HolderType, PointeeHolderType>;
		friend class TPtProxyBankMgr<HolderType, PointeeHolderType>;
	public:
		TPtProxy();		
		~TPtProxy();
	private:
		RefeeType*	m_pRefee;
		uint32		m_uRefNum;
		HolderType*	GetHolder() const;
		void SetHolder(RefeeType* pRefee);
	};

	template < class HolderType, class PointeeHolderType >
	class TPtProxyBankMgr
	{	
		typedef TPtProxy<HolderType, PointeeHolderType>		Proxy_t;
		typedef TObjectBank<Proxy_t, CPTREFMallocAllocator> ProxyBank_t;
	public:
		TPtProxyBankMgr();
		~TPtProxyBankMgr();

		static TPtProxyBankMgr* Inst()
		{
			static TPtProxyBankMgr ProxyBankMgr;
			return &ProxyBankMgr;
		}

		void CreateProxyBank();
		void DestroyProxyBank();

		void Push(Proxy_t* pProxy);
		Proxy_t* PopOrNew();

	private:
		ProxyBank_t*			m_pProxyBank;
		static HTLSKEY			ms_tlsPtRefKey;
	};
}
