#pragma once
#include "CAsyAoiSyncerObj.h"

namespace sqr
{
	class CAsyAoiScene;
	class CAoiSynceeObjHandler;

	class CAsyAoiSynceeObj
		:public CAsyAoiSyncerObj
	{
		friend class CAsyAoiScene;
		friend class CAsyAoiViewObj;
		friend class CAsyAoiSyncerObj;
		friend class CAoiSyncChangedResult;
		friend class CAoiSyncSightEnabledResult;
		friend class CAoiSetLastSyncJobIDResult;
		friend class CAoiConnectionDisabledResult;
		friend class CAoiMsgFromSyncerHandledResult;
		friend class CAoiMsgToSyncerHandledEndedResult;

		friend class CAsyAoiDimObj;

	public:
		EAoiObjType GetType()const;
		
		void PostMsgToSyncer(const void* pContext);

	private:
		CAsyAoiSynceeObj(CAsyAoiScene *pScene, const CFPos& pos,CAoiSynceeObjHandler *pHandler);

		void OnMsgFromSyncer(const void* pContext);
		void OnMsgToSyncerHandled(const void* pContext);

		static uint32 ms_uSyncJobID;
		uint32 m_uCurSyncJobID;
		uint32 m_uLastSyncJobID;
	};


}

