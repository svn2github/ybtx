#include "stdafx.h"
#include "CMirrorPatchCmd.h"
#include "CMirrorRBPatch.h"
#include "CMirrorWBPatch.h"
#include "CMirrorBuffer.h"
#include "CMirrorBufferData.h"
#include "CTrMirrorBuffer.h"
#include "TSqrAllocator.inl"

namespace sqr
{

	class CRBPatchCmdMemPool
	{
	public:
		static CRBPatchCmdMemPool& Inst()
		{
			static CRBPatchCmdMemPool ls_Inst;
			return ls_Inst;
		}

		inline void* Allocate(size_t stSize)
		{
			if(stSize > m_stObjSize)
				return SqrNew(stSize, CMirrorMallocObject::GetPool());

			if(m_stkObj.empty())
			{
				return SqrNew(m_stObjSize, CMirrorMallocObject::GetPool());
			}
			else
			{
				void* buf = m_stkObj.top();
				m_stkObj.pop();
				return buf;
			}			
		}

		inline void Deallocate(void* pMem, size_t stSize)
		{
			if(stSize > m_stObjSize)
			{
				SqrDelete(pMem, CMirrorMallocObject::GetPool());
				return;
			}

			m_stkObj.push(pMem);
		}

	private:
		CRBPatchCmdMemPool()
		{
			m_stObjSize = 20;

			for(int i = 0; i < 1024; i++)
			{
				m_stkObj.push(SqrNew(m_stObjSize, CMirrorMallocObject::GetPool()));
			}
		}
		~CRBPatchCmdMemPool()
		{
			while(!m_stkObj.empty())
			{
				void* pObj = m_stkObj.top();
				m_stkObj.pop();
				SqrDelete(pObj, CMirrorMallocObject::GetPool());
			}
		}

	private:
		typedef stack<void*, deque<void*, 
			CMirrorMallocAllocator<void*> > >	StackObj_t;

		StackObj_t	m_stkObj;

		size_t	m_stObjSize;
	};

}

		
//--------------------------------------------------------------

//因为rb的patch只可能在主线程里面运行，所以我们通过一个静态类来管理

void* CMirrorRBPatchCmd::operator new(size_t stSize)
{
	return CRBPatchCmdMemPool::Inst().Allocate(stSize);
}

//当调用delete rb cmd的指针时候，会掉用这个operator delete
void CMirrorRBPatchCmd::operator delete(void* pMem, size_t stSize)
{
	CRBPatchCmdMemPool::Inst().Deallocate(pMem, stSize);
}

CMirrorRBPatchCmd::CMirrorRBPatchCmd(uint32 uSlot)
: CMirrorPatchCmd(uSlot)
{

}

bool CMirrorRBPatchCmd::PatchDiscarded(CMirrorBuffer* pBuffer)
{
	CTrMirrorBuffer* pMgr = pBuffer->GetBufferMgr();
	return pMgr->IsDiscardedCmd(m_uSlot);
}

bool CMirrorRBPatchCmd::PatchEnabled(CMirrorBuffer* pBuffer)
{
	return pBuffer->IsIdleSlot(m_uSlot);
}

//---------------------------------------------------------------------

CMirrorWBPatchCmd::CMirrorWBPatchCmd(uint32 uSlot)
: CMirrorPatchCmd(uSlot)
{

}


void* CMirrorWBPatchCmd::operator new(size_t stSize, CMirrorPatch* pPatch)
{
	CMirrorWBPatch* pWBPatch = static_cast<CMirrorWBPatch*>(pPatch);
	return pWBPatch->AllocCmdMem(stSize);
}

