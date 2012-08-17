#include "stdafx.h"

#include "CAsynMulticast.h"
#include "CAsynPipe.h"
#include "CPipeThreadMgr.h"
#include "CAsynPipeReactor.h"

#include "ExpHelper.h"
#include "CIdMaker.h"
#include "CPipeJob.inl"
#include "TSqrAllocator.inl"

CAsynMulticast::VecMulticast		CAsynMulticast::m_vecId2Multicast;
CAsynMulticast::VecAsynMulticast_t	CAsynMulticast::m_vecId2AsynMulticast;
IdMaker* CAsynMulticast::m_idMaker;
const uint32 CAsynMulticast::m_uResizeUint = 1024;


void CAsynMulticast::InitStaticInfo(uint32 uThreadNum)
{
	m_vecId2Multicast.resize(uThreadNum);
	m_vecId2AsynMulticast.resize(m_uResizeUint);
	m_idMaker = new IdMaker(m_uResizeUint);
}

void CAsynMulticast::ReleaseIdMaker()
{
	delete m_idMaker;
}

CAsynMulticast::CAsynMulticast()
{
	bool flag = m_idMaker->GetId(m_uLocalID);
	if (!flag){
		uint32 newSize = m_idMaker->GetCurrentMax()+m_uResizeUint;
		m_idMaker->Resize(newSize);
		m_vecId2AsynMulticast.resize(newSize);
		m_idMaker->GetId(m_uLocalID);
	}
	m_vecId2AsynMulticast[m_uLocalID] = this;
	m_vecThreadInfo.resize(CAsynPipeReactor::m_suThreadNum);
}

CAsynMulticast::~CAsynMulticast()
{
	m_idMaker->ReleaseId((uint32)m_uLocalID);
	m_vecId2AsynMulticast[m_uLocalID] = NULL;
}

bool CAsynMulticast::AddPipe(IPipe* pPipe)
{
	Ast(pPipe != NULL);
	pair<SetPipe_t::iterator,bool> p = m_setPipeWithin.insert(pPipe);
	CAsynPipe* pAsynPipe = class_cast<CAsynPipe*>(pPipe);
	if(p.second)
		++m_vecThreadInfo[pAsynPipe->GetThreadId()];
	pAsynPipe->AddToMulticast(this);
	CPipeThread* pPool = CPipeThreadMgr::Inst()->GetThread(pAsynPipe->GetThreadId());
	(new(pPool) CMulticastAddPipe(GetLocalID(), pAsynPipe->GetLocalID(), pPool))->Add(pPool);
	return p.second;
}

bool CAsynMulticast::DelPipe(IPipe* pPipe)
{
	if (!pPipe)
		return false;
	CAsynPipe* pAsynPipe = class_cast<CAsynPipe*>(pPipe);
	SetPipe_t::iterator it = m_setPipeWithin.find(pPipe);
	bool flag = false;
	if (it != m_setPipeWithin.end()){
		m_setPipeWithin.erase(it);
		--m_vecThreadInfo[pAsynPipe->GetThreadId()];
		flag = true;
	}

	pPipe->DelFrMulticast(this);

	CPipeThread* pPool = CPipeThreadMgr::Inst()->GetThread(pAsynPipe->GetThreadId());
	(new(pPool) CMulticastDelPipe(GetLocalID(), pAsynPipe->GetLocalID(), pPool))->Add(pPool);
	return flag;
}

void CAsynMulticast::Release()
{
	for (SetPipe_t::iterator it = m_setPipeWithin.begin(); it != m_setPipeWithin.end();++it)
	{
		(*it)->DelFrMulticast(this);
	}
	m_setPipeWithin.clear();

	size_t stSize = CPipeThreadMgr::Inst()->GetThreadSize();
	for(size_t i = 0; i < stSize; i++)
	{
		CPipeThread* pPool = CPipeThreadMgr::Inst()->GetThread(i);
		(new(pPool) CDelMulticastJob(GetLocalID(), pPool))->Add(pPool);
	}
	delete this;
}


void CAsynMulticast::SendCmd(const void* pData,size_t stSize)
{
	if(!pData || stSize <= 0)
		return;

	for (uint32 index = 0; index < m_vecThreadInfo.size(); ++index){
		if (m_vecThreadInfo[index] > 0)
		{
			CPipeThread* pPool = CPipeThreadMgr::Inst()->GetThread(index);
			(new(pPool) CMulticastMsgJob(GetLocalID(), pData, stSize, pPool))->Add(pPool);
		}
	}
}


size_t CAsynMulticast::GetLocalID()const
{
	return m_uLocalID;
}

bool CAsynMulticast::Empty()const
{
	return m_setPipeWithin.empty();
}
