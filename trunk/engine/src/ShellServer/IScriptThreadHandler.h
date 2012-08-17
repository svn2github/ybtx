#pragma once 
#include "CallBackX.h"
namespace sqr
{
	class CFileWatcherData;

	class IScriptThreadHandler
		: public virtual CDynamicObject
	{
	public:
		virtual void OnEndThread(){ CALL_CLASS_CALLBACK() }
		virtual void OnFileChanged(CFileWatcherData* pFileDataVec);

		virtual ~IScriptThreadHandler(){}
	};

}

