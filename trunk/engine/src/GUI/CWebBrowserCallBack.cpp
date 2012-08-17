#include "stdafx.h"
#include "CWebBrowserCallBack.h"
#include "SQRWebBrowser.h"

//Webä¯ÀÀÆ÷»Øµ÷

CWebBrowserCallBack::CWebBrowserCallBack(SQRWebBrowser* _webCtrl)
{
	m_WebBrowser=_webCtrl;
}

CWebBrowserCallBack::~CWebBrowserCallBack(void)
{
	m_WebBrowser=NULL;
}

void CWebBrowserCallBack::OnForward()
{
	if(m_WebBrowser)
	{
		uint32 uMsgID = WEBBROWSER_FORWARD;
		m_WebBrowser->SendCtrlMsg(uMsgID,0,0);
	}
}

void CWebBrowserCallBack::OnLoadComplete()
{
	if(m_WebBrowser)
	{
		uint32 uMsgID = WEBBROWSER_LOADCOMPLETE;
		m_WebBrowser->SendCtrlMsg(uMsgID,0,0);
	}
}

void CWebBrowserCallBack::OnDocumentComplete()
{
	if(m_WebBrowser)
	{
		uint32 uMsgID = WEBBROWSER_DOCCOMPLETE;
		m_WebBrowser->SendCtrlMsg(uMsgID,0,0);
	}
}
