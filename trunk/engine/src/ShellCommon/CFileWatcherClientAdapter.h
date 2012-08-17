#ifdef _WIN32

#pragma once
#include "CFileWatcherAdapter.h"

namespace sqr
{
	class CFileWatcherClientAdapter
		:public CFileWatcherAdapter
	{
	public:
		void NotityFileChanged(CFileWatcherData* pChangeFileDataVec);
		void PostThreadEvent(CAppThreadEvent* pEvent);
	};
}

#endif
