
#include "stdafx.h"
#include "Ybtx.h"
#include "CDownloadHandler.h"
#include "ShareStack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CDownloadHandler::CDownloadHandler()
{
}

CDownloadHandler::~CDownloadHandler()
{
}

void CDownloadHandler::OnStartDownload(const wstring& url)
{
}

void CDownloadHandler::OnError(const wstring& url, const wchar_t* szMsg)
{
	ShareStack::GetInst()->PushMsg(szMsg);
	theApp.GetMainWnd()->PostMessage(WM_USER_MESSAGE_CALLBACK, 0, 0);
}

void CDownloadHandler::OnProcess(const wstring& url, int nNow, int nTotal)
{
	theApp.GetMainWnd()->PostMessage(WM_USER_PROGRESS_DOWN, (WPARAM)((double)nNow/nTotal*100), 0);
}

void CDownloadHandler::OnFinished(const wstring& sPath)
{
	theApp.GetMainWnd()->PostMessage(WM_USER_DOWN_FINISH, 0, 0);
}

