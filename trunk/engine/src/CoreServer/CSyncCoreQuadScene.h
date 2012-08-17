#pragma once
#include "TMirrorBaseData.h"

namespace sqr
{
	template<typename T> struct TRect;
	typedef TRect<float> CFRect;

	class CTrMirrorBuffer;
	class CWBQuadScene;

	class CSyncCoreQuadScene
		: public TBaseWBData<CWBQuadScene>
	{
	public:
		friend class CTestSuiteTripleBuffer;
		friend class CSyncCoreQuadObject;
		typedef TBaseWBData<CWBQuadScene>	Parent_t;	

		CSyncCoreQuadScene(uint32 uWidth, uint32 uHeight, uint32 uPrecision, uint32 uSlot, CTrMirrorBuffer* pMgr);
	
		void Release();

	private:
		~CSyncCoreQuadScene();

	};

}
