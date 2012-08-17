#include "stdafx.h"
#include "CMirrorWBPatch.h"
#include "CMirrorPatchCmd.h"
#include "TMemPoolAllocator.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"

CMirrorWBPatch::CMirrorWBPatch(uint64 uVersion, MemPool_t* pMemPool)
: m_pMemPool(pMemPool)
, m_uVersion(uVersion)
{
	m_pQueCmd = new (pMemPool->AllocMem(sizeof(DequeCmd_t))) DequeCmd_t(*pMemPool);
}

CMirrorWBPatch::~CMirrorWBPatch()
{
}


void CMirrorWBPatch::Patch(CMirrorBuffer* pBuffer)
{
	size_t i = 0;
	size_t stSize = m_pQueCmd->size();
	for (;;)
	{
		SQR_TRY
		{
			while(i < stSize)
			{
				CMirrorPatchCmd* pCmd = m_pQueCmd->at(i++);
				pCmd->HandleCmd(pBuffer);
			}
			break;
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
	}
}

void CMirrorWBPatch::AddCmd(CMirrorPatchCmd* pCmd)
{
	m_pQueCmd->push_back(pCmd);
}

void* CMirrorWBPatch::AllocCmdMem(size_t stSize)
{
	return m_pMemPool->AllocMem(stSize);
}
