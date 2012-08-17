#pragma once
#include "CMirrorQuadScene.h"

namespace sqr
{
	template<typename T> struct TRect;
	typedef TRect<float> CFRect;

	template<typename T> struct TPos;
	typedef TPos<float> CFPos;

	class CTrMirrorBuffer;
	class CRBQuadObject;

	class CRBQuadScene : public CMirrorQuadScene
	{
	public:
		friend class CRBQuadSceneCreatedCmd;

		static CRBQuadScene* Create(uint32 uWidth, uint32 uHeight, 
			uint32 uPrecision, uint32& uSlot, CTrMirrorBuffer* pMgr);
		void Release(uint32 uSlot, CTrMirrorBuffer* pMgr);
		

		void QueryObject(void (*pFunc)(void*, uint32), void* pVecObject,const CFPos& pos, float fGridRadius)const;
		void QueryObject(void (*pFunc)(void*, uint32), void* pVecObject, CRBQuadObject* pObject, const CFPos& OffsetPos, float fRadius) const;


	private:
		CRBQuadScene(uint32 uWidth, uint32 uHeight, uint32 uPrecision);
		~CRBQuadScene();

	};
}
