#include "stdafx.h"
#include "CRBQuadObject.h"
#include "CMirrorPatchCmd.h"
#include "CMirrorPatch.h"
#include "CTrMirrorQuadBuffer.h"
#include "CMirrorBuffer.h"
#include "CCoreQuadObject.h"
#include "ExpHelper.h"

namespace sqr
{
	
	class CRBQuadObjectCreatedCmd : public CMirrorRBPatchCmd
	{
	public:
		CRBQuadObjectCreatedCmd(uint32 uSlot, uint32 uObjId, const CFPos& pos);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	private:
		uint32	m_uObjId;
		CFPos	m_Pos;
	};

	class CRBQuadSizeChangedCmd : public CMirrorRBPatchCmd
	{
	public:
		CRBQuadSizeChangedCmd(uint32 uSlot, float fSize);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
		bool PatchEnabled(CMirrorBuffer* pBuffer);
	private:
		float	m_fSize;
	};
}

CRBQuadObjectCreatedCmd::CRBQuadObjectCreatedCmd(uint32 uSlot, uint32 uObjId, const CFPos &pos)
: CMirrorRBPatchCmd(uSlot)
, m_uObjId(uObjId)
, m_Pos(pos)
{

}

void CRBQuadObjectCreatedCmd::Release()
{
	delete this;
}

void CRBQuadObjectCreatedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	pBuffer->RemoveIdleSlot(m_uSlot);

	new (pBuffer, m_uSlot)CRBQuadObject(m_uObjId, m_Pos);
}


CRBQuadSizeChangedCmd::CRBQuadSizeChangedCmd(uint32 uSlot, float fSize)
: CMirrorRBPatchCmd(uSlot)
, m_fSize(fSize)
{

}

void CRBQuadSizeChangedCmd::Release()
{
	delete this;
}

bool CRBQuadSizeChangedCmd::PatchEnabled(CMirrorBuffer* pBuffer)
{
	if(pBuffer->IsIdleSlot(m_uSlot))
	{
		GenErr("Set Size Could not Patch On an Idle Slot");		
	}

	//如果有数据，并且size不相等，那么可以上patch
	CRBQuadObject* pData = static_cast<CRBQuadObject*>(pBuffer->GetBuffer(m_uSlot));
	if(pData->GetSize() != m_fSize)
	{
		return true;		
	}
	return false;
}

void CRBQuadSizeChangedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	CRBQuadObject* pData = static_cast<CRBQuadObject*>(pBuffer->GetBuffer(m_uSlot));
	pData->m_pQuadObject->RawSetSize(m_fSize);
}

//----------------------------------------------------------------------------

CRBQuadObject::CRBQuadObject(uint32 uObjId, const CFPos& pos)
: CMirrorQuadObject(uObjId, pos)
{

}

CRBQuadObject::~CRBQuadObject()
{

}

void CRBQuadObject::SetSize(float fSize, uint32 uSlot, CTrMirrorBuffer* pMgr)
{
	if(m_pQuadObject->GetSize() == fSize)
		return;

	m_pQuadObject->RawSetSize(fSize);

	CMirrorPatch* pPatch = pMgr->GetCurRBPatch();
	pPatch->AddCmd(new CRBQuadSizeChangedCmd(uSlot, fSize));
}

float CRBQuadObject::GetSize()
{
	return m_pQuadObject->GetSize();
}

const CFPos& CRBQuadObject::GetPosition() const
{
	return m_pQuadObject->GetPosition();
}

CRBQuadObject* CRBQuadObject::Create(uint32 uObjId, const CFPos& pos, 
											 uint32& uSlot, CTrMirrorBuffer* pMgr)
{
	CMirrorBuffer* pBuffer = pMgr->GetReadBuffer();

	uSlot = pBuffer->WithdrawIdleSlot();
	
	CRBQuadObject* pData = new(pBuffer, uSlot)CRBQuadObject(uObjId, pos);

	CMirrorPatch* pPatch = pMgr->GetCurRBPatch();
	pPatch->AddCmd(new CRBQuadObjectCreatedCmd(uSlot, uObjId, pos));

	return pData;
}

void CRBQuadObject::Release(uint32 uSlot, CTrMirrorBuffer* pMgr)
{
	
}
