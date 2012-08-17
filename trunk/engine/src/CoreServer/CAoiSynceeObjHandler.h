#pragma once
#include "CAoiSyncerObjHandler.h"

namespace sqr
{
	class CSyncCoreObjectMediator;

	class CAoiSynceeObjHandler : public CAoiSyncerObjHandler
	{
	public:
		friend class CAoiViewObjHandler;
		friend class CAoiSyncerObjHandler;

		CAoiSynceeObjHandler(CSyncCoreObjectMediator* pCoreObject);

		CSyncCoreObjectMediator* GetCoreObj() const;

		virtual void OnMsgFromSyncer(const void* pContext);
		virtual void OnMsgToSyncerHandled(const void* pContext);
	};

}
