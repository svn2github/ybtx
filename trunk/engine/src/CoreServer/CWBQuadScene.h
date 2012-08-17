#pragma once 
#include "CMirrorQuadScene.h"

namespace sqr
{
	template<typename T> struct TRect;
	typedef TRect<float> CFRect;

	class CTrMirrorBuffer;

	class CWBQuadScene : public CMirrorQuadScene
	{
	public:
		friend class CWBQuadSceneCreatedCmd;
		friend class CWBQuadSceneReleasedCmd;
			
		static CWBQuadScene* Create(uint32 uWidth, uint32 uHeight, uint32 uPrecision,
			uint32 uSlot, CTrMirrorBuffer* pMgr);
		void Release(uint32 uSlot, CTrMirrorBuffer* pMgr);

	private:
		CWBQuadScene(uint32 uWidth, uint32 uHeight, uint32 uPrecision);
		~CWBQuadScene();
		
		/*
			因为真正的quad scene不是在buffer上面创建的，所以我们在调用release的时候
			需要手动把quad scene的内存回收
		*/
		void DestroyScene();
	};

}
