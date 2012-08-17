#include "stdafx.h"
#include "CQueryJobSubQueue.h"
#include "ThreadHelper.h"
#include "ExpHelper.h"
#include "CQueryJob.h"

CQueryJobSubQueue::CQueryJobSubQueue(void)
{
	m_pIn=m_pOut=NULL;
	CreateSpinLock( &m_hLock );
}

CQueryJobSubQueue::~CQueryJobSubQueue(void)
{
	DestroySpinLock( &m_hLock );
	Ast( !m_pIn );
	Ast( !m_pOut );
}

void CQueryJobSubQueue::Push( CQueryJob* pJob )
{
	pJob->m_pNext=NULL;

	SpinLock( &m_hLock );

	if( m_pIn )
		m_pIn->m_pNext=pJob;
	else
		m_pOut=pJob;

	m_pIn = pJob;

	SpinUnlock( &m_hLock );
}

CQueryJob* CQueryJobSubQueue::Pop()
{
	SpinLock( &m_hLock );

	CQueryJob* pJob = m_pOut;
	if( pJob )
	{
		m_pOut = pJob->m_pNext;

		if( !m_pOut )
			m_pIn = NULL;
	}

	SpinUnlock( &m_hLock );

	return pJob;
}
