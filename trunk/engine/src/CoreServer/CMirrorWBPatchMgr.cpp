#include "stdafx.h"
#include "CMirrorWBPatchMgr.h"
#include "CMirrorWBPatch.h"
#include "CMirrorPatch.h"
#include "BaseHelper.h"
#include "ExpHelper.h"
#include "TMemPoolAllocator.h"
#include "TSqrAllocator.inl"
#include "ErrLogHelper.h"


CMirrorWBPatchMgr::CMirrorWBPatchMgr()
{

}


CMirrorWBPatchMgr::~CMirrorWBPatchMgr()
{
	DequePatch_t::iterator iter = m_quePatch.begin();
	while(iter != m_quePatch.end())	
	{
		IntDeletePatch(*iter);
		++iter;
	}

	while(!m_stkMemPool.empty())
	{
		MemPool_t* pMemPool = m_stkMemPool.top();
		m_stkMemPool.pop();
		pMemPool->release();
	}
}



CMirrorWBPatch* CMirrorWBPatchMgr::GetPatch(uint64 uVersion)
{
	DequePatch_t::reverse_iterator iter = m_quePatch.rbegin();
	while(iter != m_quePatch.rend())
	{
		CMirrorWBPatch* pPatch = (*iter);
		uint64 uVer = pPatch->GetVersion();
		if(uVer == uVersion)
			return pPatch;
		++iter;
	}
	return NULL;
}



CMirrorWBPatch* CMirrorWBPatchMgr::GetCurPatch(uint64 uVersion)
{
	if(m_quePatch.empty())
		return CreatePatch(uVersion);

	CMirrorWBPatch* pPatch = m_quePatch.back();
	if(pPatch->GetVersion() < uVersion)
		return CreatePatch(uVersion);

	pPatch = GetPatch(uVersion);
	Ast(pPatch);

	return pPatch;
}


void CMirrorWBPatchMgr::Patch(uint64 uVersion, CMirrorBuffer *pBuffer)
{
	Patch(uVersion, uVersion, pBuffer);
}


CMirrorWBPatch* CMirrorWBPatchMgr::CreatePatch(uint64 uVersion)
{
	if(!m_quePatch.empty())
	{
		Ast(m_quePatch.back()->GetVersion() < uVersion);
	}

	MemPool_t* pMemPool = NULL;
	if(m_stkMemPool.empty())
	{
		pMemPool = new MemPool_t(256, CMirrorMallocObject::GetPool());
	}
	else
	{
		pMemPool = m_stkMemPool.top();
		m_stkMemPool.pop();
	}

	CMirrorWBPatch* pPatch = new (pMemPool->AllocMem(sizeof(CMirrorWBPatch)))
		CMirrorWBPatch(uVersion, pMemPool);

	m_quePatch.push_back(pPatch);
	return pPatch;
}



void CMirrorWBPatchMgr::IntDeletePatch(CMirrorWBPatch* pPatch)
{
	MemPool_t* pMemPool = pPatch->GetMemPool();

	pMemPool->reset();
	m_stkMemPool.push(pMemPool);
}


void CMirrorWBPatchMgr::DeletePatch(uint64 uVersion)
{
	DeletePatch(uVersion, uVersion);
}


void CMirrorWBPatchMgr::DeletePatch(uint64 uMinVersion, uint64 uMaxVersion)
{
	if(m_quePatch.empty())
		return;

	//uint64 uMin = m_quePatch.front()->GetVersion();
	uint64 uMax = m_quePatch.back()->GetVersion();

	//Ast(uMinVersion <= uMin && uMinVersion <= uMaxVersion);

	if(uMax <= uMaxVersion)
	{
		//删除所有patch
		DequePatch_t::iterator iter = m_quePatch.begin();
		while(iter != m_quePatch.end())
		{
			SQR_TRY
			{
				while(iter != m_quePatch.end())
				{
					IntDeletePatch(*iter);
					++iter;
				}
			}
			SQR_CATCH(exp)
			{
				LogExp(exp);
				++iter;
			}
			SQR_TRY_END;
		}
		m_quePatch.clear();
	}
	else
	{
		//只删除到第一个patch版本号大于uMaxVersion的patch处
		DequePatch_t::iterator iter = m_quePatch.begin();

		while(iter != m_quePatch.end())
		{
			SQR_TRY
			{
				while(iter != m_quePatch.end())
				{
					CMirrorWBPatch* pPatch = (*iter);
					uint64 uVersion = pPatch->GetVersion();
					if(uVersion > uMaxVersion)
						goto out;

					IntDeletePatch(pPatch);
					++iter;
				}
			}
			SQR_CATCH(exp)
			{
				LogExp(exp);
				++iter;
			}
			SQR_TRY_END;
		}
out:
		Ast( iter != m_quePatch.end() );
		m_quePatch.erase(m_quePatch.begin(), iter);
	}
}


void CMirrorWBPatchMgr::Patch(uint64 uMinVersion, uint64 uMaxVersion,
							  CMirrorBuffer *pBuffer)
{
	if(m_quePatch.empty())
		return;

	DequePatch_t::iterator iter = m_quePatch.begin();


	while(iter != m_quePatch.end())
	{
		SQR_TRY
		{
			while(iter != m_quePatch.end())
			{
				CMirrorWBPatch* pPatch = (*iter);
				uint64 uVersion = pPatch->GetVersion();

				if(uVersion >= uMinVersion && uVersion <= uMaxVersion)
				{
					pPatch->Patch(pBuffer);
				}

				if(uVersion > uMaxVersion)
					return;

				++iter;
			}
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
			++iter;
		}
		SQR_TRY_END;
	}
}
