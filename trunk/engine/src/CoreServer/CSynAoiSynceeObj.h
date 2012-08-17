#pragma once
#include "CSynAoiSyncerObj.h"

namespace sqr
{
	class CSynAoiScene;

	class CSynAoiSynceeObj
		:public CSynAoiSyncerObj
	{
		friend class CSynAoiScene;
		friend class CSynAoiSyncerObj;
	public:
		EAoiObjType GetType()const;
		
		void PostMsgToSyncer(const void* pContext);

	private:
		CSynAoiSynceeObj(CSynAoiScene *pScene, const CFPos& pos,uint32 uId, uint32 uCoreObjId);
		
		void OnViewRelationKept(CSynAoiViewObj* pOtObj,
			const CFPos& MyOldGridPos,const CFPos& MyGridPos,
			const CFPos& OtOldGridPos,const CFPos& OtGridPos, uint32 uDimension = 0);	
	};


}

