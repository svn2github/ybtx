#pragma once

#include <string>

class IHttpDownloadHandler
{
public:
	// 实现抽象类虚函数
	virtual void OnStartDownload(const wstring& url) {};
	virtual void OnError(const wstring& url, const wchar_t* szMsg) {};
	virtual void OnProcess(const wstring& url, int nNow, int nTotal) {};
	virtual void OnFinished(const wstring& sPath) {};
};
