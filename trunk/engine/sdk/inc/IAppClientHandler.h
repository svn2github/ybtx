#pragma once

#include "TAppHandler.h"
#include "CTraitsClient.h"
#include "IProcessCallBack.h"
#include "CallBackX.h"

namespace sqr
{
	class CFileWatcherData;

	class IAppClientHandler
		:	public		TAppHandler<CTraitsClient>
		,	public		IProcessCallBack
	{
	public:
		IAppClientHandler();

				void Clear(void)				{ OnLoading(0.0f);			}
				void DoProcess( float process ) { OnLoading(process);		}

		virtual void OnCloseMainWnd() { CALL_CLASS_CALLBACK() };

		virtual void OnLoading(float fPercent) { CALL_CLASS_CALLBACK_1(fPercent) };
		virtual void OnMainSceneCreated(CCoreSceneClient* pCoreScene);

		virtual void OnDestroyMainScene() { CALL_CLASS_CALLBACK() };
		virtual void OnMainSceneDestroyed() { CALL_CLASS_CALLBACK() };

		virtual void OnBreak(){ CALL_CLASS_CALLBACK() }
		virtual void OnFileChanged(CFileWatcherData* pFileDataVec);
	};
}