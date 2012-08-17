#pragma once
#include "IWebBrowserEventProc.h"

namespace sqr
{

class SQRWebBrowser;
class CWebBrowserCallBack :public IWebBrowserEventProc
{
public:
	CWebBrowserCallBack(SQRWebBrowser* _webCtrl);
	~CWebBrowserCallBack(void);

	virtual void OnForward();
	virtual void OnLoadComplete();
	virtual void OnDocumentComplete();

private:
	SQRWebBrowser* m_WebBrowser;
};

//!namespace
}
