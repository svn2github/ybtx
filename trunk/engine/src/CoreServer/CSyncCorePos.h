#pragma once
#include "TMirrorBaseData.h"

namespace sqr
{
	template<typename T> struct TPos;
	typedef TPos<float> CFPos;

	class CTrMirrorBuffer;
	class CWBPosData;

	class CSyncCorePos : public TBaseWBData<CWBPosData>
	{
	public:
		friend class CTestSuiteTripleBuffer;
		typedef TBaseWBData<CWBPosData> Parent_t;

		CSyncCorePos(const CFPos& pos, uint32 uSlot, CTrMirrorBuffer* pMgr);
	
		void Release();

		const CFPos& GetPos() const;
		void SetPos(const CFPos& pos);
	

	private:
		~CSyncCorePos();
	};
}
