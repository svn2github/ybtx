#pragma once
#include "TMirrorBaseData.h"

namespace sqr
{
	template<typename T> struct TPos;
	typedef TPos<float> CFPos;

	class CTrMirrorBuffer;
	class CRBQuadObject;

	class CAsynCoreQuadObject : public TBaseRBData<CRBQuadObject>
	{
	public:
		friend class CTestSuiteTripleBuffer;
		friend class CAsynCoreQuadScene;
		typedef TBaseRBData<CRBQuadObject>	Parent_t;

		CAsynCoreQuadObject(uint32 uObjId, const CFPos& pos, CTrMirrorBuffer* pMgr);
		
		void Release();

		void SetSize(float fSize);
		float GetSize();
		const CFPos& GetPosition() const;

	private:
		~CAsynCoreQuadObject();
	
	};

}
