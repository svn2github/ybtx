#include "stdafx.h"
#include "CQueryJobQueue.h"
#include "CQueryJobSubQueue.h"
#include "ThreadHelper.inl"
#include "ExpHelper.h"

CQueryJobQueue::CQueryJobQueue(size_t stSlotNum)
:m_stSlotNum( stSlotNum )
{
	m_aryJobSubQueue= new CQueryJobSubQueue[ m_stSlotNum ];

	m_nCurInSlotIndex=m_nCurOutSlotIndex=0;
}

CQueryJobQueue::~CQueryJobQueue(void)
{
	delete [] m_aryJobSubQueue;
}


void CQueryJobQueue::Push(CQueryJob* pJob)
{
	const int32 nIndex= m_nCurInSlotIndex++ % m_stSlotNum;

	CQueryJobSubQueue* pSubQueue = m_aryJobSubQueue+nIndex;

	pSubQueue->Push( pJob );
}

CQueryJob* CQueryJobQueue::Pop()
{
	const int32 nIndex= Atomic_FetchAndAdd( &m_nCurOutSlotIndex,1 ) % m_stSlotNum;

	CQueryJobSubQueue* pSubQueue = m_aryJobSubQueue+nIndex;

	CQueryJob* pJob=pSubQueue->Pop();

	Ast(pJob);

	return pJob;
}
