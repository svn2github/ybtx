#include "stdafx.h"
#include "CSyncCoreQuadScene.h"
#include "TMirrorBaseData.inl"
#include "CWBQuadScene.h"

CSyncCoreQuadScene::CSyncCoreQuadScene(uint32 uWidth, uint32 uHeight, uint32 uPrecision, 
						   uint32 uSlot, sqr::CTrMirrorBuffer *pMgr)
						   : Parent_t(uSlot, pMgr)
{
	CWBQuadScene::Create(uWidth, uHeight, uPrecision, uSlot, pMgr);
}

CSyncCoreQuadScene::~CSyncCoreQuadScene()
{
}

void CSyncCoreQuadScene::Release()
{
	CWBQuadScene* pScene = GetData();
	pScene->Release(m_uSlot, m_pMgr);

	delete this;
}
