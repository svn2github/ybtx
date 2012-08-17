#ifdef _WIN32

#pragma once
#include "CAppThreadEvent.h"
#include "CFileWatcherData.h"

namespace sqr
{
	class CFileWatcherAdapter
	{
	public:
		CFileWatcherAdapter(){};
		~CFileWatcherAdapter(){};
		virtual void NotityFileChanged(CFileWatcherData* pChangeFileDataVec) {};
		virtual void PostThreadEvent(CAppThreadEvent* pEvent) {};
	};
}
#endif
