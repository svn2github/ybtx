#include "stdafx.h"
#include "CWBQuadScene.h"
#include "CTrMirrorBuffer.h"
#include "CMirrorBuffer.h"
#include "CMirrorPatchCmd.h"
#include "CMirrorPatch.h"
#include "CRectangle.h"
#include "CCoreQuadScene.h"

namespace sqr
{
	
	class CWBQuadSceneCreatedCmd : public CMirrorWBPatchCmd
	{
	public:
		CWBQuadSceneCreatedCmd(uint32 uSlot, uint32 uWidth, uint32 uHeight, 
			uint32 uPrecision);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	private:
		uint32	m_uWidth;
		uint32	m_uHeight;
		uint32	m_uPrecision;
	};

	class CWBQuadSceneReleasedCmd : public CMirrorWBPatchCmd
	{
	public:
		CWBQuadSceneReleasedCmd(uint32 uSlot);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	};

}

CWBQuadSceneCreatedCmd::CWBQuadSceneCreatedCmd(uint32 uSlot, uint32 uWidth, uint32 uHeight, 
														   uint32 uPrecision)
: CMirrorWBPatchCmd(uSlot)
, m_uWidth(uWidth)
, m_uHeight(uHeight)
, m_uPrecision(uPrecision)
{

}

void CWBQuadSceneCreatedCmd::Release()
{
	delete this;
}

void CWBQuadSceneCreatedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	if(!pBuffer->IsIdleSlot(m_uSlot))
		return;

	pBuffer->RemoveIdleSlot(m_uSlot);

	new (pBuffer, m_uSlot) 
		CWBQuadScene(m_uWidth, m_uHeight, m_uPrecision);
}

CWBQuadSceneReleasedCmd::CWBQuadSceneReleasedCmd(uint32 uSlot)
: CMirrorWBPatchCmd(uSlot)
{

}

void CWBQuadSceneReleasedCmd::Release()
{
	delete this;
}

void CWBQuadSceneReleasedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	CWBQuadScene* pData = static_cast<CWBQuadScene*>(pBuffer->GetBuffer(m_uSlot));
		
	pBuffer->RestoreIdleSlot(m_uSlot);

	pData->DestroyScene();
}

//------------------------------------------------------------------------------

CWBQuadScene::CWBQuadScene(uint32 uWidth, uint32 uHeight, uint32 uPrecision)
: CMirrorQuadScene(uWidth, uHeight, uPrecision)
{

}

CWBQuadScene::~CWBQuadScene()
{

}

CWBQuadScene* CWBQuadScene::Create(uint32 uWidth, uint32 uHeight, uint32 uPrecision, uint32 uSlot, CTrMirrorBuffer *pMgr)
{
	CMirrorBuffer* pBuffer = pMgr->GetWriteBuffer();

	CWBQuadScene* pData = NULL;

	if(pBuffer->IsIdleSlot(uSlot))
	{
		pData = new(pBuffer, uSlot)CWBQuadScene(uWidth, uHeight, uPrecision);

		pBuffer->RemoveIdleSlot(uSlot);
	}
	else
	{
		pData = static_cast<CWBQuadScene*>(pBuffer->GetBuffer(uSlot));
	}

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBQuadSceneCreatedCmd(uSlot, uWidth, uHeight, uPrecision));

	pMgr->AddDiscardedCmd(uSlot);

	return pData;	
}

void CWBQuadScene::Release(uint32 uSlot, CTrMirrorBuffer* pMgr)
{
	CMirrorBuffer* pBuffer = pMgr->GetWriteBuffer();

	pBuffer->RestoreIdleSlot(uSlot);

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBQuadSceneReleasedCmd(uSlot));

	pMgr->AddDiscardedCmd(uSlot);

	DestroyScene();
}

void CWBQuadScene::DestroyScene()
{
	delete m_pScene;
}
