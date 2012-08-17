#pragma once
#include "CMirrorQuadObject.h"

namespace sqr
{

	class CTrMirrorBuffer;
	class CCoreQuadScene;

	class CWBQuadObject : public CMirrorQuadObject
	{
	public:
		friend class CWBQuadObjectCreatedCmd;
		friend class CWBQuadObjectReleasedCmd;
		friend class CWBQuadInsertToSceneCmd;
		friend class CWBQuadRemoveFrSceneCmd;
		friend class CWBQuadPosChangedCmd;
		friend class CWBQuadPosForceChangedCmd;
		friend class CWBQuadSizeChangedCmd;
		friend class CWBQuadRawPosChangedCmd;

		static CWBQuadObject* Create(uint32 uObjId, const CFPos& pos, uint32 uSlot, CTrMirrorBuffer* pMgr);
		void Release(uint32 uSlot, CTrMirrorBuffer* pMgr);

		void InsertToScene(uint32 uSlot, uint32 uSceneSlot, CTrMirrorBuffer* pMgr);
		void RemoveFrScene(uint32 uSlot, uint32 uSceneSlot, CTrMirrorBuffer* pMgr);

		void RawSetPos(const CFPos& pos, uint32 uSlot, CTrMirrorBuffer* pMgr);
		void SetPos(const CFPos& pos, uint32 uSlot, uint32 uSceneSlot, CTrMirrorBuffer* pMgr);
		void ForceSetPos(const CFPos& pos, uint32 uSlot, uint32 uSceneSlot, CTrMirrorBuffer* pMgr);
		const CFPos& GetPosition() const;

		void SetSize(float fSize, uint32 uSlot, uint32 uSceneSlot, CTrMirrorBuffer* pMgr);
		float GetSize();

	private:
		CCoreQuadScene* GetScene(uint32 uSlot, CTrMirrorBuffer* pMgr) const;
		
		void DestroyObject();

	private:
		CWBQuadObject(uint32 uObjId, const CFPos& pos);
		~CWBQuadObject();

	};

}
