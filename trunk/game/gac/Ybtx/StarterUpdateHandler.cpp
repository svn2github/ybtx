
#include "stdafx.h"

#include "Ybtx.h"
#include "YbtxDlg.h"
#include "resource.h"
#include "ShareStack.h"


#include "StarterUpdateHandler.h"


CStarterUpdateHandler::CStarterUpdateHandler()
{}

CStarterUpdateHandler::~CStarterUpdateHandler()
{}

void CStarterUpdateHandler::SetDownState(int percent)
{
	theApp.GetMainWnd()->PostMessage(WM_USER_PROGRESS_DOWN, (WPARAM)percent, 0);
}

void CStarterUpdateHandler::SetSetupState(int percent)
{
	theApp.GetMainWnd()->PostMessage(WM_USER_PROGRESS_SETUP, (WPARAM)percent, 0);
}

void CStarterUpdateHandler::ShowMessage(const TCHAR* Msg)
{
	// 该处以后需要修改
	ShareStack::GetInst()->PushMsg(Msg);
	theApp.GetMainWnd()->PostMessage(WM_USER_MESSAGE_CALLBACK, 0, 0);
	//AfxMessageBox(Msg);
}

void CStarterUpdateHandler::CallBackFinishPatch(const TCHAR* szCmd)
{
	ShareStack::GetInst()->PushCmd(szCmd);
	theApp.GetMainWnd()->PostMessage(WM_USER_RESTART, 0, 0);
}

// 注意该回调函数不能被分线程回调，以后需要分离主线程与分线程的虚函数接口
void CStarterUpdateHandler::AlertExit(const TCHAR* Msg)
{
	AfxMessageBox(Msg);
	exit(0);
}

void CStarterUpdateHandler::UpdateVersion()
{
	theApp.GetMainWnd()->PostMessage(WM_USER_UPDATE_VERSION, 0, 0);
}


void CStarterUpdateHandler::PatchContinue()
{
	theApp.GetMainWnd()->PostMessage(WM_USER_PATCH_CONTINUE, 0, 0);
}

void CStarterUpdateHandler::VerifyFilesFinish(int bVerifySuccess)
{
	theApp.GetMainWnd()->PostMessage(WM_USER_VERIFY_FINISH, (WPARAM)bVerifySuccess, 0);
}

