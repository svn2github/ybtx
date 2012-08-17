#include "stdafx.h"
#include "CQueryResultQueue.h"
#include "CQueryJob.h"
#include "ThreadHelper.inl"
#include "ExpHelper.h"


CQueryResultQueue::CQueryResultQueue(void)
{
	m_pIn=m_pOut=new CQueryDummyJob;
}

CQueryResultQueue::~CQueryResultQueue(void)
{
	Ast( m_pIn == m_pOut );
	delete m_pIn;
	m_pIn=m_pOut=NULL;
}

bool CQueryResultQueue::Empty()const
{
	return !m_pOut->m_pNext;
}

void CQueryResultQueue::Push( CQueryJob* pJob )
{
	pJob->m_pNext=NULL;

	CQueryJob* pPrev=static_cast<CQueryJob*>
		( Atomic_SwapPtr( reinterpret_cast<void* volatile*>( &m_pIn ), pJob ) );

	Ast( pPrev );

	pPrev->m_pNext = pJob;
}


CDbContext* CQueryResultQueue::Get(CDbThreadMgr* pMgr)
{
	CQueryJob* pNext=m_pOut->m_pNext;
	
	if( !pNext )
		return NULL;	

	return pNext->GetContext();
}

bool CQueryResultQueue::Pop(CDbThreadMgr* pMgr)
{
	CQueryJob* pNext=m_pOut->m_pNext;
	
	if( !pNext )
		return false;	

	m_pOut->Unit(pMgr);
	m_pOut = pNext;

	return true;
}

