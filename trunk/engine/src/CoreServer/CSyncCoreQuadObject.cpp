#include "stdafx.h"
#include "CSyncCoreQuadObject.h"
#include "TMirrorBaseData.inl"
#include "CWBQuadObject.h"
#include "CWBQuadScene.h"
#include "CSyncCoreQuadScene.h"

CSyncCoreQuadObject::CSyncCoreQuadObject(uint32 uObjId, const CFPos &pos, uint32 uSlot, CTrMirrorBuffer *pMgr)
: Parent_t(uSlot, pMgr)
{
	CWBQuadObject::Create(uObjId, pos, uSlot, pMgr);
}

CSyncCoreQuadObject::~CSyncCoreQuadObject()
{
	
}

void CSyncCoreQuadObject::InsertToScene(CSyncCoreQuadScene* pScene)
{
	uint32 uSceneSlot = pScene->GetSlot();

	CWBQuadObject* pObject = GetData();

	pObject->InsertToScene(m_uSlot, uSceneSlot, m_pMgr);
}

void CSyncCoreQuadObject::RemoveFrScene(CSyncCoreQuadScene* pScene)
{
	uint32 uSceneSlot = pScene->GetSlot();

	CWBQuadObject* pObject = GetData();

	pObject->RemoveFrScene(m_uSlot, uSceneSlot, m_pMgr);
}

void CSyncCoreQuadObject::SetSize(float fSize, CSyncCoreQuadScene* pScene)
{
	uint32 uSceneSlot = pScene->GetSlot();

	CWBQuadObject* pObject = GetData();

	pObject->SetSize(fSize, m_uSlot, uSceneSlot, m_pMgr);
}

float CSyncCoreQuadObject::GetSize()
{
	CWBQuadObject* pObject = GetData();
	return pObject->GetSize();
}
	
void CSyncCoreQuadObject::RawSetPos(const CFPos& pos)
{

	CWBQuadObject* pObject = GetData();

	pObject->RawSetPos(pos, m_uSlot, m_pMgr);
}

void CSyncCoreQuadObject::SetPos(const CFPos& pos, CSyncCoreQuadScene* pScene)
{
	uint32 uSceneSlot = pScene->GetSlot();

	CWBQuadObject* pObject = GetData();

	pObject->SetPos(pos, m_uSlot, uSceneSlot, m_pMgr);
}

void CSyncCoreQuadObject::ForceSetPos(const CFPos& pos, CSyncCoreQuadScene* pScene)
{
	uint32 uSceneSlot = pScene->GetSlot();

	CWBQuadObject* pObject = GetData();

	pObject->ForceSetPos(pos, m_uSlot, uSceneSlot, m_pMgr);
}

const CFPos& CSyncCoreQuadObject::GetPosition() const
{
	CWBQuadObject* pObject = GetData();
	return pObject->GetPosition();
}	

void CSyncCoreQuadObject::Release()
{
	CWBQuadObject* pObject = GetData();
	pObject->Release(m_uSlot, m_pMgr);

	delete this;
}
