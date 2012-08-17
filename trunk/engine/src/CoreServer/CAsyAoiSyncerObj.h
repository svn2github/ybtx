#pragma once
#include "CAsyAoiViewObj.h"

namespace sqr
{
	class CAsyAoiSynceeObj;

	class CAoiSyncerObjHandler;
	class CAsyAoiSyncerObj
		:public CAsyAoiViewObj
	{
		friend class CAsyAoiViewObj;
		friend class CAsyAoiScene;
		friend class CAsyAoiSynceeObj;
		friend class CAoiSyncChangedResult;
		friend class CAoiDestroyObjJob;
		friend class CAoiMsgFromSynceeHandledResult;
		friend class CAoiMsgToSynceeHandledEndedResult;

		friend class CAsyAoiDimObj;

	public:
		EAoiObjType GetType()const;

		void PostMsgToSyncee(const void* pContext);

	protected:
		CAsyAoiSyncerObj(CAsyAoiScene *pScene, const CFPos& pos,CAoiSyncerObjHandler *pObj);

		void OnSyncRelationChanged(CAsyAoiSynceeObj* pSyncObj,bool bSee, uint32 uDimension);
	
		void OnMsgFromSyncee(const void* pContext);
		void OnMsgToSynceeHandled(const void* pContext);
	};

}

