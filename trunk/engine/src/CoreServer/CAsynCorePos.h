#pragma once
#include "TMirrorBaseData.h"

namespace sqr
{
	template<typename T> struct TPos;
	typedef TPos<float> CFPos;

	class CTrMirrorBuffer;
	class CRBPosData;

	class CAsynCorePos : public TBaseRBData<CRBPosData>
	{
	public:
		friend class CTestSuiteTripleBuffer;
		typedef TBaseRBData<CRBPosData>	Parent_t;

		CAsynCorePos(const CFPos& pos, CTrMirrorBuffer* pMgr);
		void Release();
	
		const CFPos& GetPos() const;
	
	private:
		~CAsynCorePos();

	};

}
