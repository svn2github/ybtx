#include "stdafx.h"
#ifdef _WIN32

#include "CFileWatcherClientAdapter.h"
#include "CAppClient.h"
#include "IAppClientHandler.h"

void CFileWatcherClientAdapter::NotityFileChanged(CFileWatcherData* pChangeFileDataVec)
{
	CAppClient::Inst()->GetHandler()->OnFileChanged(pChangeFileDataVec);
}

void CFileWatcherClientAdapter::PostThreadEvent(CAppThreadEvent* pEvent)
{
	CAppClient::Inst()->PostThreadAppEvent(pEvent);
}

#endif
