#pragma once
#include "CMirrorQuadObject.h"

namespace sqr
{
	class CTrMirrorBuffer;

	class CRBQuadObject : public CMirrorQuadObject
	{
	public:
		friend class CRBQuadObjectCreatedCmd;
		friend class CRBQuadSizeChangedCmd;

		static CRBQuadObject* Create(uint32 uObjId, const CFPos& pos, uint32& uSlot, CTrMirrorBuffer* pMgr);
		void Release(uint32 uSlot, CTrMirrorBuffer* pMgr);

		void SetSize(float fSize, uint32 uSlot, CTrMirrorBuffer* pMgr);
		float GetSize();

		const CFPos& GetPosition() const;

	private:
		CRBQuadObject(uint32 uObjId, const CFPos& pos);
		~CRBQuadObject();
	};
}
