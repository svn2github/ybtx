#pragma once

#include <windows.h>

#include "BittorrentDef.h"


class IBTDownloadHandler
{
public:
	virtual void AddFirewallException(char *AppPath, char *RuleName)=0;
	virtual void InitAgent(LPCSTR AgentName)=0;

	virtual HANDLE DownFromBuffer(LPBYTE TorrentContent, DWORD TorrentSize, const char *SavePath)=0;
	virtual int GetProgress(HANDLE DownHandler)=0;

	virtual void Shutdown()=0;
};

BITTORRENT_API IBTDownloadHandler* CreateDownloader();

