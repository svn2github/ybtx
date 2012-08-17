#include "stdafx.h"
#include "ExpHelper.h"
#include "CTickSlot.h"
#include "CTickImp.h"
#include "CTickMgr.h"
#include "CTickQueue.h"
#include "TObjectBank.inl"
#include "TSqrAllocator.inl"

void CTickSlot::AddTick( CTickImp* pTickImp )
{
	CTickQueue* pTickQueue = GetTickQueue( pTickImp->m_uRing );
	
	if( !pTickQueue )
	{
		QueueBank_t* pBank=pTickImp->m_pTickMgr->m_pTickQueueBank;

		pTickQueue= pBank->PopOrNew();
		
		m_TickQueueMap.push_back( make_pair( pTickImp->m_uRing,pTickQueue ) );
		pTickQueue->Init((uint32)m_TickQueueMap.size() - 1);
	}

	pTickQueue->push_back( pTickImp );
	pTickQueue->m_uTickCount++;  //该queue的tick个数增一
	pTickImp->m_uQueuePos = uint32(pTickQueue->size()-1);
}

void CTickSlot::DelTick( CTickImp* pTickImp )
{
	CTickQueue* pTickQueue = GetTickQueue( pTickImp->m_uRing );
	pTickQueue->at( pTickImp->m_uQueuePos ) = NULL;
	pTickQueue->m_uTickCount--;
	
	if (pTickQueue->m_uTickCount == 0)
	{
		//如果等于last tick queue，表明这个tick的删除是在当前的queue里面进行的
		//这时候我们不能删除这个queue，因为这轮tick之后自动会删除
		if (pTickQueue == pTickImp->m_pTickMgr->m_pLastTickQueue)
		{
			return;
		}

		//如果tick的计数为0了，那么我们不再需要这个queue了
		DelTickQueue(pTickQueue, pTickImp->m_pTickMgr->m_pTickQueueBank);
	}
}

void CTickSlot::ClearTick()
{
	const size_t stSize = m_TickQueueMap.size();

	for( size_t i=0;i<stSize;++i )
	{
		CTickQueue* pTickQueue= m_TickQueueMap[i].second;
		
		CTickQueue::iterator itEnd=pTickQueue->end();

		for( CTickQueue::iterator it=pTickQueue->begin() ; it!=itEnd; ++it )
		{
			CTickImp* pTickImp=*it;

			if( !pTickImp )
				continue;

			DelTick( pTickImp );
		}

		delete pTickQueue;
	}

	m_TickQueueMap.clear();
}

CTickQueue* CTickSlot::GetTickQueue( uint32 uRing )
{
	const size_t stSize=m_TickQueueMap.size();
	
	for( size_t i=0; i< stSize; ++i )
	{
		pair< uint32,CTickQueue*> Pair=m_TickQueueMap[i];
		if( Pair.first == uRing )
		{
			Pair.second->m_uRingSlot = (uint32)i;
			return Pair.second;
		}
	}
	return NULL;
}

void CTickSlot::DelTickQueue( CTickQueue* pTickQueue, QueueBank_t* pBank )
{
	uint32 uRingSlot = pTickQueue->m_uRingSlot;

	//因为在删除该slot上面的这个queue的时候，可能以前的某个操作已经把该queue前面的queue删除了
	//所以这个queue的位置就不是原先设置的ringslot上面的，所以我们需要从该ringslot向前遍历

	uint32 uQueueMapSize = (uint32)m_TickQueueMap.size();
	Ast(uQueueMapSize > 0); 

	if (uRingSlot >= uQueueMapSize)
	{
		uRingSlot = uQueueMapSize - 1;
	}

	for (uint32 i = uRingSlot; i >= 0; i--)
	{
		pair< uint32,CTickQueue*> Pair=m_TickQueueMap[i];
		if (Pair.second == pTickQueue)
		{
			m_TickQueueMap.erase( m_TickQueueMap.begin()+ i);
			pBank->Push( pTickQueue );
			return;
		}
	}

}

