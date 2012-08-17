#pragma once
#include "TMirrorBaseData.h"

namespace sqr
{

	template<typename T> struct TPos;
	typedef TPos<float> CFPos;

	class CTrMirrorBuffer;
	class CWBQuadObject;

	class CSyncCoreQuadScene;

	class CSyncCoreQuadObject : public TBaseWBData<CWBQuadObject>
	{
	public:
		friend class CTestSuiteTripleBuffer;
		friend class CSyncCoreQuadScene;
		typedef TBaseWBData<CWBQuadObject>	Parent_t;

		CSyncCoreQuadObject(uint32 uObjId, const CFPos& pos, uint32 uSlot, CTrMirrorBuffer* pMgr);
		
		void Release();

		void InsertToScene(CSyncCoreQuadScene* pScene);
		void RemoveFrScene(CSyncCoreQuadScene* pScene);

		void SetSize(float fSize, CSyncCoreQuadScene* pScene);
		float GetSize();

		void RawSetPos(const CFPos& pos);
		void SetPos(const CFPos& pos, CSyncCoreQuadScene* pScene);
		void ForceSetPos(const CFPos& pos, CSyncCoreQuadScene* pScene);
		const CFPos& GetPosition() const;

	private:
		~CSyncCoreQuadObject();

	};
}
