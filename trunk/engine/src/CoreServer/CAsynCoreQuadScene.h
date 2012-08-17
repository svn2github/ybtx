#pragma once
#include "TMirrorBaseData.h"

namespace sqr
{
	template<typename T> struct TRect;
	typedef TRect<float> CFRect;

	template<typename T> struct TPos;
	typedef TPos<float> CFPos;

	class CTrMirrorBuffer;
	class CRBQuadScene;
	class CAsynCoreQuadObject;

	class CAsynCoreQuadScene : public TBaseRBData<CRBQuadScene>
	{
	public:
		friend class CTestSuiteTripleBuffer;
		friend class CAsynCoreQuadObject;
		typedef TBaseRBData<CRBQuadScene>	Parent_t;

		CAsynCoreQuadScene(uint32 uWidth, uint32 uHeight, uint32 uPrecision, CTrMirrorBuffer* pMgr);
		
		void Release();

		void QueryObject(void (*pFunc)(void*, uint32), void* pVecObject,
			const CFPos& pos, float fGridRadius)const;

		void QueryObject(void (*pFunc)(void*, uint32), void* pVecObject, 
			CAsynCoreQuadObject* pObj, const CFPos& OffsetPos, float fRadius) const;

	private:
		~CAsynCoreQuadScene();
	};
	
}
