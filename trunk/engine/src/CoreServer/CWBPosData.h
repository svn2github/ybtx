#pragma once
#include "CMirrorPosData.h"

namespace sqr
{

	class CTrMirrorBuffer;
	class CMirrorPatch;

	class CWBPosData : public CMirrorPosData
	{
	public:
		friend class CWBPosDataCreatedCmd;
		friend class CWBPosDataChangedCmd;
		friend class CWBPosDataReleasedCmd;

		static CWBPosData* Create(const CFPos& pos, uint32 uSlot, CTrMirrorBuffer* pMgr);
		void Release(uint32 uSlot, CTrMirrorBuffer* pMgr);
		void SetPos(const CFPos& pos, uint32 uSlot, CTrMirrorBuffer* pMgr);

	private:
		CWBPosData(const CFPos& pos);
		~CWBPosData();
	};

}
