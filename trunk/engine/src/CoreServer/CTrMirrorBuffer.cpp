#include "stdafx.h"
#include "CTrMirrorBuffer.h"
#include "CMirrorBuffer.h"
#include "CMirrorRBPatch.h"
#include "CMirrorWBPatch.h"
#include "CMirrorWBPatchMgr.h"
#include "CMirrorRBPatchMgr.h"
#include "CTrDiscardedCmdMgr.h"
#include "ThreadHelper.h"
#include "ExpHelper.h"


CTrMirrorBuffer::CTrMirrorBuffer(uint32 uDataSize, uint32 uDataNum)
{
	CreateLock(&m_Lock);

	m_pRBPatchMgr = new CMirrorRBPatchMgr;
	m_pWBPatchMgr = new CMirrorWBPatchMgr;

	m_pDiscardedCmdMgr = new CTrDiscardedCmdMgr;

	CreateTripleBuffer(uDataSize, uDataNum);

	m_pWriteBuffer->SetVersion(1);  //首先就把write的版本设置为1
}


CTrMirrorBuffer::~CTrMirrorBuffer()
{
	/*
		在删除buffer之前，我们需要清空buffer上面的所有数据
	*/
	for(int i = 0; i < 5; i++)
	{
		SwapWIBuffer();
		SwapRIBuffer();
	}
	

	DeleteTripleBuffer();

	delete m_pDiscardedCmdMgr;

	delete m_pRBPatchMgr;
	delete m_pWBPatchMgr;

	DestroyLock(&m_Lock);
}

void CTrMirrorBuffer::Release()
{
	delete this;
}

void CTrMirrorBuffer::CreateTripleBuffer(uint32 uDataSize, uint32 uDataNum)
{
	m_pReadBuffer = new CMirrorBuffer(uDataSize, uDataNum, this);
	m_pWriteBuffer = new CMirrorBuffer(uDataSize, uDataNum, this);
	m_pIdleBuffer = new CMirrorBuffer(uDataSize, uDataNum, this);
}

void CTrMirrorBuffer::DeleteTripleBuffer()
{
	delete m_pReadBuffer;
	delete m_pWriteBuffer;
	delete m_pIdleBuffer;
}

CMirrorBuffer* CTrMirrorBuffer::GetWriteBuffer()
{
	return m_pWriteBuffer;
}


CMirrorBuffer* CTrMirrorBuffer::GetReadBuffer()
{
	return m_pReadBuffer;
}

CMirrorPatch* CTrMirrorBuffer::GetCurWBPatch()
{
	return m_pWBPatchMgr->GetCurPatch(m_pWriteBuffer->GetVersion());
}

CMirrorPatch* CTrMirrorBuffer::GetCurRBPatch()
{
	return m_pRBPatchMgr->GetCurPatch();
}

void CTrMirrorBuffer::OnRBUpdate()
{
	SwapRIBuffer();
}

void CTrMirrorBuffer::OnWBUpdate()
{
	SwapWIBuffer();
}

void CTrMirrorBuffer::SwapRIBuffer()
{
	uint64 uReadVersion = m_pReadBuffer->GetVersion();
	//Lock(&m_Lock);
	
	uint64 uIdleVersion = m_pIdleBuffer->GetVersion();

	if(uReadVersion == uIdleVersion)
	{
		//Unlock(&m_Lock);
		return;
	}

	m_pReadBuffer->Swap(m_pIdleBuffer);

	m_pDiscardedCmdMgr->SwapRIDiscardedCmd();

	//Unlock(&m_Lock);

	m_pRBPatchMgr->Patch(m_pReadBuffer);	

	m_pDiscardedCmdMgr->ClearDiscardedCmd();
}


void CTrMirrorBuffer::SwapWIBuffer()
{
	uint64 uWriteVersion = m_pWriteBuffer->GetVersion();

	//Lock(&m_Lock);

	uint64 uIdleVersion = m_pIdleBuffer->GetVersion();
	
	Ast(uWriteVersion >= uIdleVersion + 1);

	m_pWriteBuffer->Swap(m_pIdleBuffer);	

	m_pDiscardedCmdMgr->SwapWIDiscardedCmd();

	//Unlock(&m_Lock);

	//wb的version还必须是先前的
	m_pWriteBuffer->SetVersion(uWriteVersion);

	m_pWBPatchMgr->Patch(uIdleVersion + 1, uWriteVersion, m_pWriteBuffer);

	//如果两个值相差大于1，那么证明先前的ib是rb交换过来的，这时候我们就可以把一些delta删除了
	if(uWriteVersion - uIdleVersion > 1)
	{
		m_pWBPatchMgr->DeletePatch(uIdleVersion + 1, uWriteVersion - 1);
	}

	m_pWriteBuffer->IncVersion();
}

void CTrMirrorBuffer::AddDiscardedCmd(uint32 uSlot)
{
	m_pDiscardedCmdMgr->AddDiscardedCmd(uSlot);	
}

bool CTrMirrorBuffer::IsDiscardedCmd(uint32 uSlot)
{
	return m_pDiscardedCmdMgr->IsDiscardedCmd(uSlot);
}
