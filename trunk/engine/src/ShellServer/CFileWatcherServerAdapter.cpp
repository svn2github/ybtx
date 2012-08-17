#include "stdafx.h"
#ifdef _WIN32

#include "CFileWatcherServerAdapter.h"
#include "CAppServer.h"
#include "IAppServerHandler.h"

void CFileWatcherServerAdapter::NotityFileChanged(CFileWatcherData* pChangeFileDataVec)
{
	CAppServer::Inst()->GetHandler()->OnFileChanged(pChangeFileDataVec);
}

void CFileWatcherServerAdapter::PostThreadEvent(CAppThreadEvent* pEvent)
{
	CAppServer::Inst()->PostThreadAppEvent(pEvent);
}

#endif
