#pragma once
#include "CMirrorPosData.h"

namespace sqr
{

	class CTrMirrorBuffer;

	class CRBPosData : public CMirrorPosData
	{
	public:
		friend class CRBPosDataCreatedCmd;

		static CRBPosData* Create(const CFPos& pos, uint32& uSlot, CTrMirrorBuffer* pMgr);
		void Release(uint32 uSlot, CTrMirrorBuffer* pMgr);

	private:
		CRBPosData(const CFPos& pos);
		~CRBPosData();
	};

}
