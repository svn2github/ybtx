#include "stdafx.h"
#include "CMirrorRBPatch.h"
#include "CMirrorPatchCmd.h"
#include "CMirrorBuffer.h"
#include "TSqrAllocator.inl"
#include "ExpHelper.h"
#include "ErrLogHelper.h"


CMirrorRBPatch::CMirrorRBPatch()
{
	m_pQueCmd = new DequeCmd_t;
	m_pSwapQueCmd = new DequeCmd_t;
}

CMirrorRBPatch::~CMirrorRBPatch()
{
	m_pQueCmd->clear();
	m_pSwapQueCmd->clear();

	delete m_pQueCmd;
	delete m_pSwapQueCmd;
}

void CMirrorRBPatch::AddCmd(CMirrorPatchCmd *pCmd)
{
	m_pQueCmd->push_back(pCmd);
}


void CMirrorRBPatch::Patch(CMirrorBuffer* pBuffer)
{
	if(m_pQueCmd->empty())
		return;

	DequeCmd_t::iterator iter = m_pQueCmd->begin();

	size_t stSize = m_pQueCmd->size();
	size_t i = 0;

	for (;;)
	{
		SQR_TRY
		{
			while(i < stSize)
			{
				CMirrorRBPatchCmd* pCmd = static_cast<CMirrorRBPatchCmd*>(m_pQueCmd->at(i++));
				
				if(!pCmd->PatchDiscarded(pBuffer))
				{
					if(pCmd->PatchEnabled(pBuffer))
					{
						pCmd->HandleCmd(pBuffer);	
					}
					m_pSwapQueCmd->push_back(pCmd);
				}
				else
				{
					pCmd->Release();
				}
			}
			break;
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
	}

	m_pQueCmd->clear();

	if(m_pSwapQueCmd->empty())
	{	
		return;
	}

	DequeCmd_t* pTemp = m_pQueCmd;
	m_pQueCmd = m_pSwapQueCmd;
	m_pSwapQueCmd = pTemp;
}
