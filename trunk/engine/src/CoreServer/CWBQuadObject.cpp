#include "stdafx.h"
#include "CWBQuadObject.h"
#include "CMirrorBuffer.h"
#include "CTrMirrorQuadBuffer.h"
#include "CMirrorPatchCmd.h"
#include "CMirrorPatch.h"
#include "CCoreQuadScene.h"
#include "CWBQuadScene.h"
#include "CCoreQuadObject.h"

namespace sqr
{

	class CWBQuadObjectCreatedCmd : public CMirrorWBPatchCmd
	{
	public:
		CWBQuadObjectCreatedCmd(uint32 uSlot, CMirrorBuffer* pBuffer, uint32 uObjId, const CFPos& pos);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	private:
		CMirrorBuffer*	m_pBuffer;
		uint32 m_uObjId;
		CFPos	m_Pos;
	};

	class CWBQuadObjectReleasedCmd : public CMirrorWBPatchCmd
	{
	public:
		CWBQuadObjectReleasedCmd(uint32 uSlot);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	};

	class CWBQuadObjectWithSceneSlotCmd : public CMirrorWBPatchCmd
	{
	public:
		CWBQuadObjectWithSceneSlotCmd(uint32 uSlot, uint32 uSceneSlot)
			: CMirrorWBPatchCmd(uSlot), m_uSceneSlot(uSceneSlot){}
		void Release() = 0;
		void HandleCmd(CMirrorBuffer* pBuffer) = 0;

	protected:
		inline CCoreQuadScene* GetScene(CMirrorBuffer* pBuffer) const
		{
			CWBQuadScene* pScene = static_cast<CWBQuadScene*>
				(pBuffer->GetBuffer(m_uSceneSlot));
			return pScene->GetScene();
		}

	protected:
		uint32	m_uSceneSlot;
	};

	class CWBQuadInsertToSceneCmd : public CWBQuadObjectWithSceneSlotCmd
	{
	public:
		CWBQuadInsertToSceneCmd(uint32 uSlot, uint32 uSceneSlot);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	};

	class CWBQuadRemoveFrSceneCmd : public CWBQuadObjectWithSceneSlotCmd
	{
	public:
		CWBQuadRemoveFrSceneCmd(uint32 uSlot, uint32 uSceneSlot);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	};

	class CWBQuadSizeChangedCmd : public CWBQuadObjectWithSceneSlotCmd
	{
	public:
		CWBQuadSizeChangedCmd(uint32 uSlot, float fSize, uint32 uSceneSlot);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	private:
		float m_fSize;
	};

	class CWBQuadPosChangedCmd : public CWBQuadObjectWithSceneSlotCmd
	{
	public:
		CWBQuadPosChangedCmd(uint32 uSlot, const CFPos& pos, uint32 uSceneSlot);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	private:
		CFPos m_Pos;
	};

	class CWBQuadPosForceChangedCmd : public CWBQuadObjectWithSceneSlotCmd
	{
	public:
		CWBQuadPosForceChangedCmd(uint32 uSlot, const CFPos& pos, uint32 uSceneSlot);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	private:
		CFPos m_Pos;
	};

	class CWBQuadRawPosChangedCmd : public CMirrorWBPatchCmd
	{
	public:
		CWBQuadRawPosChangedCmd(uint32 uSlot, const CFPos& pos);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);
	private:
		CFPos m_Pos;
	};
}

CWBQuadObjectCreatedCmd::CWBQuadObjectCreatedCmd(uint32 uSlot, CMirrorBuffer *pBuffer, uint32 uObjId, const CFPos &pos)
: CMirrorWBPatchCmd(uSlot)
, m_pBuffer(pBuffer)
, m_uObjId(uObjId)
, m_Pos(pos)
{

}

void CWBQuadObjectCreatedCmd::Release()
{
	delete this;
}

void CWBQuadObjectCreatedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	if(!pBuffer->IsIdleSlot(m_uSlot))
		return;

	pBuffer->RemoveIdleSlot(m_uSlot);

	CMirrorBuffer* pQuadBuffer = static_cast<CMirrorBuffer*>(pBuffer);
	

	new (pQuadBuffer, m_uSlot)
		CWBQuadObject(m_uObjId, m_Pos);
}

CWBQuadObjectReleasedCmd::CWBQuadObjectReleasedCmd(uint32 uSlot)
: CMirrorWBPatchCmd(uSlot)
{

}

void CWBQuadObjectReleasedCmd::Release()
{
	delete this;
}

void CWBQuadObjectReleasedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	CWBQuadObject* pData = static_cast<CWBQuadObject*>(pBuffer->GetBuffer(m_uSlot));
	pBuffer->RestoreIdleSlot(m_uSlot);

	pData->DestroyObject();
}

CWBQuadInsertToSceneCmd::CWBQuadInsertToSceneCmd(uint32 uSlot, uint32 uSceneSlot)
: CWBQuadObjectWithSceneSlotCmd(uSlot, uSceneSlot)
{

}

void CWBQuadInsertToSceneCmd::Release()
{
	delete this;
}

void CWBQuadInsertToSceneCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	CWBQuadObject* pData = static_cast<CWBQuadObject*>(pBuffer->GetBuffer(m_uSlot));	
	
	CCoreQuadScene* pScene = GetScene(pBuffer);

	pData->m_pQuadObject->InsertToScene(pScene);
}

CWBQuadRemoveFrSceneCmd::CWBQuadRemoveFrSceneCmd(uint32 uSlot, uint32 uSceneSlot)
: CWBQuadObjectWithSceneSlotCmd(uSlot, uSceneSlot)
{

}

void CWBQuadRemoveFrSceneCmd::Release()
{
	delete this;
}

void CWBQuadRemoveFrSceneCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	CWBQuadObject* pData = static_cast<CWBQuadObject*>(pBuffer->GetBuffer(m_uSlot));	
	
	CCoreQuadScene* pScene = GetScene(pBuffer);
	
	pData->m_pQuadObject->RemoveFrScene(pScene);
}

CWBQuadSizeChangedCmd::CWBQuadSizeChangedCmd(uint32 uSlot, float fSize, uint32 uSceneSlot)
: CWBQuadObjectWithSceneSlotCmd(uSlot, uSceneSlot)
, m_fSize(fSize)
{
	
}

void CWBQuadSizeChangedCmd::Release()
{
	delete this;
}

void CWBQuadSizeChangedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	CWBQuadObject* pData = static_cast<CWBQuadObject*>(pBuffer->GetBuffer(m_uSlot));	
	
	CCoreQuadScene* pScene = GetScene(pBuffer);
	
	pData->m_pQuadObject->SetSize(m_fSize, pScene);
}

CWBQuadPosChangedCmd::CWBQuadPosChangedCmd(uint32 uSlot, const CFPos& pos, uint32 uSceneSlot)
: CWBQuadObjectWithSceneSlotCmd(uSlot, uSceneSlot)
, m_Pos(pos)
{
	
}

void CWBQuadPosChangedCmd::Release()
{
	delete this;
}

void CWBQuadPosChangedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	CWBQuadObject* pData = static_cast<CWBQuadObject*>(pBuffer->GetBuffer(m_uSlot));	
	
	CCoreQuadScene* pScene = GetScene(pBuffer);
	
	pData->m_pQuadObject->SetPos(m_Pos, pScene);
}

CWBQuadPosForceChangedCmd::CWBQuadPosForceChangedCmd(uint32 uSlot, const CFPos& pos, uint32 uSceneSlot)
: CWBQuadObjectWithSceneSlotCmd(uSlot, uSceneSlot)
, m_Pos(pos)
{

}

void CWBQuadPosForceChangedCmd::Release()
{
	delete this;
}

void CWBQuadPosForceChangedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	CWBQuadObject* pData = static_cast<CWBQuadObject*>(pBuffer->GetBuffer(m_uSlot));	

	CCoreQuadScene* pScene = GetScene(pBuffer);

	pData->m_pQuadObject->ForceSetPos(m_Pos, pScene);
}

CWBQuadRawPosChangedCmd::CWBQuadRawPosChangedCmd(uint32 uSlot, const CFPos& pos)
: CMirrorWBPatchCmd(uSlot)
, m_Pos(pos)
{

}

void CWBQuadRawPosChangedCmd::Release()
{
	delete this;
}

void CWBQuadRawPosChangedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	CWBQuadObject* pData = static_cast<CWBQuadObject*>(pBuffer->GetBuffer(m_uSlot));	
	
	pData->m_pQuadObject->RawSetPos(m_Pos);
}

//--------------------------------------------------------------------------

CWBQuadObject::CWBQuadObject(uint32 uObjId, const CFPos& pos)
: CMirrorQuadObject(uObjId, pos)
{

}
		
CWBQuadObject::~CWBQuadObject()
{

}

inline CCoreQuadScene* CWBQuadObject::GetScene(uint32 uSlot, CTrMirrorBuffer* pMgr) const
{
	CMirrorBuffer* pBuffer = pMgr->GetWriteBuffer();
	CWBQuadScene* pScene = static_cast<CWBQuadScene*>(
		pBuffer->GetBuffer(uSlot));

	CCoreQuadScene* pQuadScene = pScene->GetScene();

	return pQuadScene;
}

void CWBQuadObject::InsertToScene(uint32 uSlot, uint32 uSceneSlot, CTrMirrorBuffer* pMgr)
{
	CCoreQuadScene* pScene = GetScene(uSceneSlot, pMgr);
	
	m_pQuadObject->InsertToScene(pScene);

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBQuadInsertToSceneCmd(uSlot, uSceneSlot));
}

void CWBQuadObject::RemoveFrScene(uint32 uSlot, uint32 uSceneSlot, CTrMirrorBuffer* pMgr)
{
	CCoreQuadScene* pScene = GetScene(uSceneSlot, pMgr);

	m_pQuadObject->RemoveFrScene(pScene);

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBQuadRemoveFrSceneCmd(uSlot, uSceneSlot));
}

void CWBQuadObject::RawSetPos(const CFPos& pos, uint32 uSlot, CTrMirrorBuffer* pMgr)
{
	m_pQuadObject->RawSetPos(pos);

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBQuadRawPosChangedCmd(uSlot, pos));
}

void CWBQuadObject::SetPos(const CFPos& pos, uint32 uSlot, uint32 uSceneSlot, CTrMirrorBuffer* pMgr)
{
	CCoreQuadScene* pScene = GetScene(uSceneSlot, pMgr);

	m_pQuadObject->SetPos(pos, pScene);

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBQuadPosChangedCmd(uSlot, pos, uSceneSlot));
}
	
void CWBQuadObject::ForceSetPos(const CFPos& pos, uint32 uSlot, uint32 uSceneSlot, CTrMirrorBuffer* pMgr)
{
	CCoreQuadScene* pScene = GetScene(uSceneSlot, pMgr);

	m_pQuadObject->ForceSetPos(pos, pScene);

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBQuadPosForceChangedCmd(uSlot, pos, uSceneSlot));
}

const CFPos& CWBQuadObject::GetPosition() const
{
	return m_pQuadObject->GetPosition();
}

void CWBQuadObject::SetSize(float fSize, uint32 uSlot, uint32 uSceneSlot, CTrMirrorBuffer* pMgr)
{
	CCoreQuadScene* pScene = GetScene(uSceneSlot, pMgr);

	m_pQuadObject->SetSize(fSize, pScene);

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBQuadSizeChangedCmd(uSlot, fSize, uSceneSlot));
}

float CWBQuadObject::GetSize()
{
	return m_pQuadObject->GetSize();
}


CWBQuadObject* CWBQuadObject::Create(uint32 uObjId, const CFPos& pos, uint32 uSlot, 
											 CTrMirrorBuffer* pMgr)
{
	CMirrorBuffer* pBuffer = pMgr->GetWriteBuffer();

	CWBQuadObject* pData = NULL;

	if(pBuffer->IsIdleSlot(uSlot))
	{
		pData = new(pBuffer, uSlot)CWBQuadObject(uObjId, pos);

		pBuffer->RemoveIdleSlot(uSlot);
	}
	else
	{
		pData = static_cast<CWBQuadObject*>(pBuffer->GetBuffer(uSlot));
	}

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBQuadObjectCreatedCmd(uSlot, pBuffer, uObjId, pos));

	pMgr->AddDiscardedCmd(uSlot);

	return pData;
}
	
void CWBQuadObject::Release(uint32 uSlot, CTrMirrorBuffer* pMgr)
{
	CMirrorBuffer* pBuffer = pMgr->GetWriteBuffer();
	
	pBuffer->RestoreIdleSlot(uSlot);

	CMirrorPatch* pPatch = pMgr->GetCurWBPatch();
	pPatch->AddCmd(new (pPatch)CWBQuadObjectReleasedCmd(uSlot));

	pMgr->AddDiscardedCmd(uSlot);
	
	DestroyObject();
}

void CWBQuadObject::DestroyObject()
{
	delete m_pQuadObject;
}
