#include "stdafx.h"
#include "CMirrorBuffer.h"
#include "TIDStack.inl"
#include <algorithm>
#include "ExpHelper.h"
#include "TSqrAllocator.inl"

template class TIDStack<uint32, vector<uint32> >;

CMirrorBuffer::CMirrorBuffer(uint32 uDataSize, uint32 uDataNum, CTrMirrorBuffer* pMgr)
: m_uVersion(0)
, m_uDataSize(uDataSize)
, m_pMgr(pMgr)
{
	uint32 uBufSize = uDataSize * uDataNum;
	
	m_pVecBuffer = new VecBuffer_t;
	m_pVecBuffer->resize(uBufSize, 0);

	m_pIDStack = new TIDStack_t(uDataNum);

}

CMirrorBuffer::~CMirrorBuffer()
{
	delete m_pVecBuffer;
	delete m_pIDStack;
}

CTrMirrorBuffer* CMirrorBuffer::GetBufferMgr()
{
	return m_pMgr;
}

uint64 CMirrorBuffer::GetVersion()
{
	return m_uVersion;
}

void CMirrorBuffer::SetVersion(uint64 uVersion)
{
	m_uVersion = uVersion;
}

void CMirrorBuffer::IncVersion()
{
	++m_uVersion;
}

inline void CMirrorBuffer::AppendBuffer(size_t stSize)
{
	m_pVecBuffer->insert(m_pVecBuffer->end(), stSize, 0);
}

void* CMirrorBuffer::GetBuffer(uint32 uSlot)
{
	size_t stIndex = uSlot * m_uDataSize;
	size_t stSize = m_pVecBuffer->size();

	Ast(stIndex <= stSize);

	//如果buffer已经用完，那我们加入一块新数据
	if(stIndex == stSize)
	{
		AppendBuffer(m_uDataSize);
	}

	return &(m_pVecBuffer->at(stIndex));
}


void* CMirrorBuffer::AllocData(uint32 uSlot)
{
	return GetBuffer(uSlot);
}

void CMirrorBuffer::ReleaseData(uint32 uSlot)
{

}


uint32 CMirrorBuffer::WithdrawIdleSlot()
{
	return m_pIDStack->WithdrawIdleID();
}

void CMirrorBuffer::RestoreIdleSlot(uint32 uSlot)
{
	m_pIDStack->RestoreUsedID(uSlot);
}

void CMirrorBuffer::RemoveIdleSlot(uint32 uSlot)
{
	if(!m_pIDStack->IsStackID(uSlot))
	{
		m_pIDStack->ExpandIdleID(uSlot);
	}

	m_pIDStack->RemoveIdleID(uSlot);
}

bool CMirrorBuffer::IsIdleSlot(uint32 uSlot)
{
	return m_pIDStack->IsIdleID(uSlot);
}

uint32 CMirrorBuffer::GetIdleSlotNum()const
{
	return uint32(m_pIDStack->GetIdleIDSize());
}

void CMirrorBuffer::Swap(CMirrorBuffer* pOtBuf)
{
	size_t stMySize = m_pVecBuffer->size();
	size_t stOtSize = pOtBuf->m_pVecBuffer->size();

	size_t stMyIDSize = m_pIDStack->GetIDSize();
	size_t stOtIDSize = pOtBuf->m_pIDStack->GetIDSize();

	if(stMySize > stOtSize)
	{
		pOtBuf->AppendBuffer(stMySize - stOtSize);
		Ast(stMyIDSize > stOtIDSize);
		pOtBuf->m_pIDStack->ExpandIdleID(uint32(stMyIDSize - 1) );
	}
	else if(stMySize < stOtSize)
	{
		AppendBuffer(stOtSize - stMySize);
		Ast(stMyIDSize < stOtIDSize);
		m_pIDStack->ExpandIdleID(stOtIDSize - 1);
	}

	swap(m_pVecBuffer, pOtBuf->m_pVecBuffer);
	swap(m_pIDStack, pOtBuf->m_pIDStack);
	swap(m_uVersion, pOtBuf->m_uVersion);

	/*
	vector<char>* pBuf = m_pVecBuffer;
	m_pVecBuffer = pOtBuf->m_pVecBuffer;
	pOtBuf->m_pVecBuffer = pBuf;

	TIDStack<uint32>* pIDStack = m_pIDStack;
	m_pIDStack = pOtBuf->m_pIDStack;
	pOtBuf->m_pIDStack = pIDStack;

	uint64 uVersion = m_uVersion;
	m_uVersion = pOtBuf->m_uVersion;
	pOtBuf->m_uVersion = uVersion;
	*/
}


