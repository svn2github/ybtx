#include "stdafx.h"
#include "CSyncCorePos.h"
#include "TMirrorBaseData.inl"
#include "CTrMirrorBuffer.h"
#include "CMirrorBuffer.h"
#include "CWBPosData.h"

CSyncCorePos::CSyncCorePos(const CFPos& pos, uint32 uSlot, CTrMirrorBuffer* pMgr)
: Parent_t(uSlot, pMgr)
{
	CWBPosData::Create(pos, uSlot, pMgr);
}

CSyncCorePos::~CSyncCorePos()
{
	
}


const CFPos& CSyncCorePos::GetPos() const
{
	CWBPosData* pData = GetData();
	return pData->GetPos();
}

void CSyncCorePos::SetPos(const CFPos& pos)
{
	CWBPosData* pData = GetData();
	pData->SetPos(pos, m_uSlot, m_pMgr);
}

void CSyncCorePos::Release()
{
	CWBPosData* pData = GetData();
	pData->Release(m_uSlot, m_pMgr);

	delete this;
}
