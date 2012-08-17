#include "stdafx.h"
#include "CAsynCoreQuadScene.h"
#include "CTrMirrorQuadBuffer.h"
#include "CRBQuadScene.h"
#include "TMirrorBaseData.inl"
#include "CRBQuadObject.h"
#include "CAsynCoreQuadObject.h"

CAsynCoreQuadScene::CAsynCoreQuadScene(uint32 uWidth, uint32 uHeight,  
						   uint32 uPrecision, sqr::CTrMirrorBuffer *pMgr)
						   : Parent_t(pMgr)
{
	CRBQuadScene::Create(uWidth, uHeight, uPrecision, m_uSlot, pMgr);
}

CAsynCoreQuadScene::~CAsynCoreQuadScene()
{
	
}

void CAsynCoreQuadScene::QueryObject(void (*pFunc)(void*, uint32), void *pVecObject, 
							   const CFPos &pos, float fGridRadius) const
{
	CRBQuadScene* pScene = GetData();
	pScene->QueryObject(pFunc, pVecObject, pos, fGridRadius);
}

void CAsynCoreQuadScene::QueryObject(void (*pFunc)(void*, uint32), void *pVecObject, 
							   CAsynCoreQuadObject *pObj, const CFPos &OffsetPos, 
							   float fRadius) const
{
	CRBQuadScene* pScene = GetData();
	CRBQuadObject* pObject = pObj->GetData();
	pScene->QueryObject(pFunc, pVecObject, pObject, OffsetPos, fRadius);
}

void CAsynCoreQuadScene::Release()
{	
	CRBQuadScene* pScene = GetData();
	pScene->Release(m_uSlot, m_pMgr);

	delete this;
}
