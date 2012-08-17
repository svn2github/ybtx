#include "stdafx.h"
#include "CAsynCoreQuadObject.h"
#include "CTrMirrorQuadBuffer.h"
#include "CMirrorBuffer.h"
#include "CRBQuadObject.h"
#include "TMirrorBaseData.inl"

CAsynCoreQuadObject::CAsynCoreQuadObject(uint32 uObjId, const CFPos& pos, CTrMirrorBuffer* pMgr)
: Parent_t(pMgr)
{
	CRBQuadObject::Create(uObjId, pos, m_uSlot, pMgr);
}

CAsynCoreQuadObject::~CAsynCoreQuadObject()
{
	
}

void CAsynCoreQuadObject::SetSize(float fSize)
{
	CRBQuadObject* pData = GetData();
	pData->SetSize(fSize, m_uSlot, m_pMgr);
}

float CAsynCoreQuadObject::GetSize()
{
	CRBQuadObject* pData = GetData();
	return pData->GetSize();
}

const CFPos& CAsynCoreQuadObject::GetPosition() const
{
	CRBQuadObject* pData = GetData();
	return pData->GetPosition();
}

void CAsynCoreQuadObject::Release()
{
	CRBQuadObject* pData = GetData();
	pData->Release(m_uSlot, m_pMgr);

	delete this;
}
