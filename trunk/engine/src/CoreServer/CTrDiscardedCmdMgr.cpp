#include "stdafx.h"
#include "CTrDiscardedCmdMgr.h"
#include "BaseHelper.h"
#include "ExpHelper.h"
#include "CMirrorMallocObject.h"
#include "TSqrAllocator.inl"

CTrDiscardedCmdMgr::CTrDiscardedCmdMgr()
: m_pIBAlloc(NULL)
, m_pRBAlloc(NULL)
, m_pIBDiscardedCmd(NULL)
, m_pRBDiscardedCmd(NULL)
{
	m_pIBAlloc = new SlotAlloc_t(1024, CMirrorMallocObject::GetPool());
	m_pIBDiscardedCmd = new (m_pIBAlloc->AllocMem(sizeof(SlotSet_t)))
		SlotSet_t(less<uint32>(), *m_pIBAlloc);

	m_pRBAlloc = new SlotAlloc_t(1024, CMirrorMallocObject::GetPool());
	m_pRBDiscardedCmd = new (m_pRBAlloc->AllocMem(sizeof(SlotSet_t)))
		SlotSet_t(less<uint32>(), *m_pRBAlloc);
}

CTrDiscardedCmdMgr::~CTrDiscardedCmdMgr()
{
	m_pIBAlloc->release();
	m_pIBAlloc = NULL;
	
	m_pRBAlloc->release();
	m_pRBAlloc = NULL;
}


inline void CTrDiscardedCmdMgr::CreateIBMem()
{
	
}

void CTrDiscardedCmdMgr::AddDiscardedCmd(uint32 uSlot)
{
	m_queWBDiscardedCmd.push_back(uSlot);
}

void CTrDiscardedCmdMgr::SwapWIDiscardedCmd()
{
	if(m_queWBDiscardedCmd.empty())
		return;

	QueCmd_t::iterator iter = m_queWBDiscardedCmd.begin();
	while(iter != m_queWBDiscardedCmd.end())
	{
		m_pIBDiscardedCmd->insert((*iter));
		++iter;
	}
	m_queWBDiscardedCmd.clear();

}

void CTrDiscardedCmdMgr::SwapRIDiscardedCmd()
{
	if(m_pIBDiscardedCmd->empty())
		return;

	std::swap(m_pRBDiscardedCmd, m_pIBDiscardedCmd);
	std::swap(m_pRBAlloc, m_pIBAlloc);

		
}

bool CTrDiscardedCmdMgr::IsDiscardedCmd(uint32 uSlot) 
{
	SlotSet_t::iterator iter = m_pRBDiscardedCmd->find(uSlot);
	if(iter == m_pRBDiscardedCmd->end())
		return false;
	return true;
}

void CTrDiscardedCmdMgr::ClearDiscardedCmd()
{
	m_pRBAlloc->reset();
	m_pRBDiscardedCmd = new (m_pRBAlloc->AllocMem(sizeof(SlotSet_t)))
		SlotSet_t(less<uint32>(), *m_pRBAlloc);
}
