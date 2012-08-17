#pragma once
#include "CSynAoiViewObj.h"

namespace sqr
{
	class CSynAoiSynceeObj;

	class CSynAoiSyncerObj
		:public CSynAoiViewObj
	{
		friend class CSynAoiScene;
		friend class CSynAoiSynceeObj;

	public:
		EAoiObjType GetType()const;

		void PostMsgToSyncee(const void* pContext);

	protected:
		CSynAoiSyncerObj(CSynAoiScene *pScene, const CFPos& pos, uint32 uId, uint32 uCoreObjId);
		
		void OnViewRelationChanged(CSynAoiViewObj* pViewObj,const CFPos& MyGridPos,
			const CFPos& OtGridPos,bool bSee, uint32 uDimension = 0);	

		virtual void OnSyncRelationChanged(CSynAoiSynceeObj* pSyncObj,bool bSee, uint32 uDimension = 0);
	};

}

