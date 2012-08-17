#pragma once

#include "TAppHandler.h"
#include "CSyncTraitsServer.h"
#include "CallBackX.h"

namespace sqr
{
	class CFileWatcherData;

	class IAppServerHandler
		:public TAppHandler<CSyncTraitsServer>
	{
	public:
		virtual void OnBreak(){ CALL_CLASS_CALLBACK() }
		virtual void OnClose(){ CALL_CLASS_CALLBACK() }
		virtual void OnFileChanged(CFileWatcherData* pFileDataVec);
		virtual ~IAppServerHandler(){}
	};
}

