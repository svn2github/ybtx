#pragma once
#include "PatternRef.h"

namespace sqr
{
	class CSyncCoreObjectServer;

	class CAsyAoiViewObj;
	class CAoiViewObjHandler
	{
	public:
		friend class CAsyAoiViewObj;
		friend class CAoiSyncerObjHandler;
		friend class CAoiSynceeObjHandler;

		CAoiViewObjHandler(CSyncCoreObjectServer* pCoreObject);
		virtual ~CAoiViewObjHandler();

		CSyncCoreObjectServer* GetCoreObj() const;
		uint32 GetCoreObjID();

		virtual void OnViewRelationChanged(CAsyAoiViewObj* pObjServer,bool bSee, uint32 uDimension);
		virtual void OnMsgFromViewer(const void* pContext);
		virtual void OnMsgFromViewee(const void* pContext);
		virtual void OnMsgToVieweeHandled(const void* pContext);
		virtual void OnMsgToViewerHandled(const void* pContext);
		virtual void OnMsgFromSelf(const void* pContext);
		virtual void OnHalted();

	protected:
		TPtRefer<CAoiViewObjHandler, CSyncCoreObjectServer>	m_CoreObj;
		TPtRefee<CAoiViewObjHandler, CAsyAoiViewObj>		m_RefsByAoiObj;
	};

}

