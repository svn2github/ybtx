#include "stdafx.h"
#include "CWBPosData.h"
#include "CTrMirrorPosBuffer.h"
#include "CMirrorBuffer.h"
#include "CMirrorPatchCmd.h"
#include "CMirrorPatch.h"

namespace sqr
{

	class CWBPosDataCreatedCmd : public CMirrorWBPatchCmd
	{
	public:
		CWBPosDataCreatedCmd(uint32 uSlot, const CFPos& pos);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	private:
		CFPos	m_Pos;
	};

	class CWBPosDataChangedCmd : public CMirrorWBPatchCmd
	{
	public:
		CWBPosDataChangedCmd(uint32 uSlot, const CFPos& pos);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	private:
		CFPos	m_Pos;
	};

	class CWBPosDataReleasedCmd : public CMirrorWBPatchCmd
	{
	public:
		CWBPosDataReleasedCmd(uint32 uSlot);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	};
}

CWBPosDataCreatedCmd::CWBPosDataCreatedCmd(uint32 uSlot, const CFPos& pos)
: CMirrorWBPatchCmd(uSlot)
, m_Pos(pos)
{

}

void CWBPosDataCreatedCmd::Release()
{
	delete this;
}

void CWBPosDataCreatedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	if(!pBuffer->IsIdleSlot(m_uSlot))
		return;

	pBuffer->RemoveIdleSlot(m_uSlot);

	new(pBuffer, m_uSlot)CWBPosData(m_Pos);
}

////////////////////////////////////////////////////////////////////////////////

CWBPosDataChangedCmd::CWBPosDataChangedCmd(uint32 uSlot, const CFPos& pos)
: CMirrorWBPatchCmd(uSlot)
, m_Pos(pos)
{

}

void CWBPosDataChangedCmd::Release()
{
	delete this;
}

void CWBPosDataChangedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	CWBPosData* pData = (CWBPosData*)pBuffer->GetBuffer(m_uSlot);
	pData->RawSetPos(m_Pos);
}

//////////////////////////////////////////////////////////////////////

CWBPosDataReleasedCmd::CWBPosDataReleasedCmd(uint32 uSlot)
: CMirrorWBPatchCmd(uSlot)
{

}

void CWBPosDataReleasedCmd::Release()
{
	delete this;
}

void CWBPosDataReleasedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	CWBPosData* pData = static_cast<CWBPosData*>(pBuffer->GetBuffer(m_uSlot));
	pBuffer->RestoreIdleSlot(m_uSlot);

	delete pData;
}

//////////////////////////////////////////////////////////////////////

CWBPosData::CWBPosData(const CFPos &pos)
: CMirrorPosData(pos)
{
	
}

CWBPosData::~CWBPosData()
{

}

CWBPosData* CWBPosData::Create(const CFPos& pos, uint32 uSlot,
										   CTrMirrorBuffer* pMgr)
{
	CMirrorBuffer* pBuffer = pMgr->GetWriteBuffer();

	CWBPosData* pData = NULL;
	if(pBuffer->IsIdleSlot(uSlot))
	{
		pData = new(pBuffer, uSlot)CWBPosData(pos);
	
		pBuffer->RemoveIdleSlot(uSlot);
	}
	else
	{
		pData = static_cast<CWBPosData*>(pBuffer->GetBuffer(uSlot));
	}
	
	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBPosDataCreatedCmd(uSlot, pos));

	pMgr->AddDiscardedCmd(uSlot);

	return pData;
}

void CWBPosData::SetPos(const CFPos& pos, uint32 uSlot, CTrMirrorBuffer* pMgr)
{
	m_Pos = pos;

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBPosDataChangedCmd(uSlot, pos));
}

void CWBPosData::Release(uint32 uSlot, CTrMirrorBuffer* pMgr)
{
	CMirrorBuffer* pBuffer = pMgr->GetWriteBuffer();
	pBuffer->RestoreIdleSlot(uSlot);

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBPosDataReleasedCmd(uSlot));

	pMgr->AddDiscardedCmd(uSlot);

	delete this;
}
