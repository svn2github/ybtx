#include "stdafx.h"
#include "CRBQuadScene.h"
#include "CMirrorBuffer.h"
#include "CTrMirrorBuffer.h"
#include "CMirrorPatchCmd.h"
#include "CMirrorPatch.h"
#include "CRectangle.h"
#include "CRBQuadObject.h"
#include "CCoreQuadScene.h"

namespace sqr
{

	class CRBQuadSceneCreatedCmd : public CMirrorRBPatchCmd
	{
	public:
		CRBQuadSceneCreatedCmd(uint32 uSlot, uint32 uWidth, uint32 uHeight, uint32 uPrecison);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	private:
		uint32	m_uWidth;
		uint32	m_uHeight;
		uint32	m_uPrecision;
	};

}

CRBQuadSceneCreatedCmd::CRBQuadSceneCreatedCmd(uint32 uSlot, uint32 uWidth, uint32 uHeight, uint32 uPrecision)
: CMirrorRBPatchCmd(uSlot)
, m_uWidth(uWidth)
, m_uHeight(uHeight)
, m_uPrecision(uPrecision)
{

}

void CRBQuadSceneCreatedCmd::Release()
{
	delete this;
}

void CRBQuadSceneCreatedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	pBuffer->RemoveIdleSlot(m_uSlot);

	new (pBuffer, m_uSlot)CRBQuadScene
		(m_uWidth, m_uHeight, m_uPrecision);
}

//----------------------------------------------------------------------

CRBQuadScene::CRBQuadScene(uint32 uWidth, uint32 uHeight, uint32 uPrecision)
: CMirrorQuadScene(uWidth, uHeight, uPrecision)
{

}

CRBQuadScene::~CRBQuadScene()
{

}

CRBQuadScene* CRBQuadScene::Create(uint32 uWidth, uint32 uHeight, uint32 uPrecision, 
											   uint32 &uSlot, CTrMirrorBuffer *pMgr)
{
	CMirrorBuffer* pBuffer = pMgr->GetReadBuffer();

	uSlot = pBuffer->WithdrawIdleSlot();
	
	CRBQuadScene* pData = new(pBuffer, uSlot)CRBQuadScene(uWidth, uHeight, uPrecision);

	CMirrorPatch* pPatch = pMgr->GetCurRBPatch();
	pPatch->AddCmd(new CRBQuadSceneCreatedCmd(uSlot, uWidth, uHeight,uPrecision));


	return pData;
}

void CRBQuadScene::Release(uint32 uSlot, CTrMirrorBuffer* pMgr)
{
}

void CRBQuadScene::QueryObject(void (*pFunc)(void*, uint32), void* pVecObject,const CFPos& pos,
									 float fGridRadius)const
{
	m_pScene->QueryObject(pFunc, pVecObject, pos, fGridRadius);
}

void CRBQuadScene::QueryObject(void (*pFunc)(void*, uint32), void* pVecObject, CRBQuadObject* pObject,
									 const CFPos& OffsetPos, float fRadius) const
{

	m_pScene->QueryObject(pFunc, pVecObject, pObject->GetQuadObject(), OffsetPos, fRadius);
}
