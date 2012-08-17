#pragma once
#include "CTickMallocObject.h"
#include "CTickMallocAllocator.h"

namespace sqr
{
	class CTickImp;
	class CTickQueue;
	class CTickQueueStack;
	template<typename, template<typename>class>class TObjectBank;
	
	class CTickSlot
		: public CTickMallocObject
	{
	public:
		typedef std::vector< pair<uint32,CTickQueue*>, CTickMallocAllocator< pair<uint32, CTickQueue*> > >	MapTickQueue_t;
		
		void AddTick( CTickImp* pTickImp );
		void DelTick( CTickImp* pTickImp );
		void ClearTick();

		CTickQueue* GetTickQueue(uint32 uRing);

		typedef TObjectBank<CTickQueue, CTickMallocAllocator>	QueueBank_t;

		void DelTickQueue(CTickQueue* pTickQueue, QueueBank_t* pBank);
	private:

		MapTickQueue_t	m_TickQueueMap;
	};
}
