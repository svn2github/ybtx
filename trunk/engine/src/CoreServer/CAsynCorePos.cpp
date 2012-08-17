#include "stdafx.h"
#include "CAsynCorePos.h"
#include "TMirrorBaseData.inl"
#include "CRBPosData.h"
#include "CTrMirrorBuffer.h"
#include "CMirrorBuffer.h"

CAsynCorePos::CAsynCorePos(const CFPos& pos, CTrMirrorBuffer* pMgr)
: Parent_t(pMgr)
{
	CRBPosData::Create(pos, m_uSlot, pMgr);
}

CAsynCorePos::~CAsynCorePos()
{
	
}


const CFPos& CAsynCorePos::GetPos() const
{
	CRBPosData* pData = GetData();
	return pData->GetPos();
}

void CAsynCorePos::Release()
{
	CRBPosData* pData = GetData();
	pData->Release(m_uSlot, m_pMgr);

	delete this;
}
