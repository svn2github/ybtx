#pragma once
#include "CAoiViewObjHandler.h"

namespace sqr
{
	class CAsyAoiSynceeObj;
	class CSyncCoreObjectServer;
	class CSyncCoreObjectDictator;

	class CAoiSyncerObjHandler : public CAoiViewObjHandler
	{
	public:
		friend class CAoiViewObjHandler;
		friend class CAoiSynceeObjHandler;

		CAoiSyncerObjHandler(CSyncCoreObjectDictator* pCoreObject);

		CSyncCoreObjectDictator* GetCoreObj() const;

		virtual void OnSyncRelationChanged(CAsyAoiSynceeObj* pObjMediator,bool bSee, uint32 uDimension);
		virtual void OnMsgFromSyncee(const void* pContext);
		virtual void OnMsgToSynceeHandled(const void* pContext);
	};


}
