#pragma once

namespace sqr
{
class IWebBrowserEventProc
{
public:
	IWebBrowserEventProc(void);
	~IWebBrowserEventProc(void);

	virtual void OnForward(){};
	virtual void OnLoadComplete(){};
	virtual void OnDocumentComplete(){};
};

//!namespace
}