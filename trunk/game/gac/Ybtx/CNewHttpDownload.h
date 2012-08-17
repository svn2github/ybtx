#pragma once
#include <string>
#include <tchar.h>
#include <fstream>
#include "HttpDownloadHandler.h"
#pragma comment(lib, "wininet.lib")


class CNewHTTPDownloader
{
public:
	CNewHTTPDownloader(IHttpDownloadHandler* pHandler)
		:m_pHander(pHandler)
	{
	}
	void DownloadFile(const wstring& strHostName,
			const wstring& strFileName,
			const wstring& strDistPath,
			int nPriority = THREAD_PRIORITY_IDLE);
	HANDLE GetCurThreadHandle();
	DWORD GetCurThreadId();

private:
	static unsigned int __stdcall __uis_DownloadFile(void* pParam);
private:
	std::wstring m_strHostname;
	std::wstring m_strFileName;
	std::wstring m_strDistPath;
	IHttpDownloadHandler* m_pHander;
	HANDLE m_handleCurThread;
	unsigned int m_uCurThreadId;
};
