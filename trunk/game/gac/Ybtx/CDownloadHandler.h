
#include "HttpDownloadHandler.h"


class CDownloadHandler
	: public IHttpDownloadHandler
{
public:
	CDownloadHandler();
	virtual ~CDownloadHandler();

	virtual void OnStartDownload(const std::wstring& url);
	virtual void OnError(const std::wstring& url, const wchar_t* szMsg);
	virtual void OnProcess(const std::wstring& url, int nNow, int nTotal);
	virtual void OnFinished(const std::wstring& sPath);
};

