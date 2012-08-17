#include "stdafx.h"
#include "SQRWebBrowser.h"
#include "CWebBrowser.h"
#include "CWebBrowserATL.h"
#include "CWebBrowserCallBack.h"
#include "CWindowTarget.h"

DYNAMIC_CREATE_WND( SQRWebBrowser, SQRWnd )

SQRWebBrowser::SQRWebBrowser(void)
{
	m_WebWinAtl=NULL;
	m_WebWin=NULL;
	m_IsATL=false;
	m_Show=false;
}

SQRWebBrowser::~SQRWebBrowser(void)
{
	SafeDelete(m_WebWinAtl);
	SafeDelete(m_WebWin);
}

char* SQRWebBrowser::GetSrcCode()
{
	wchar_t* code = NULL;
	if(!m_IsATL)
	{
		if(m_WebWin)
			code = m_WebWin->GetSourceCode();
	}
	else
	{
		if(m_WebWinAtl)
			code = m_WebWinAtl->GetSourceCode();
	}

	if (code != NULL)
	{
		int nLen = WideCharToMultiByte( CP_ACP, 0, code, -1, NULL, 0, NULL, NULL );
		if (nLen == 0)
			return NULL;

		char* pResult = new char[nLen];
		WideCharToMultiByte( CP_ACP, 0, code, -1, pResult, nLen, NULL, NULL );
		return pResult;
	}
	return NULL;
}

int32 SQRWebBrowser::Create(const WndCreateParam& param )
{
	if ( FAILED( SQRWnd::Create( param ) ) )
		return GERROR;	

	if(!m_IsATL)
	{
		m_WebWin=new CWebBrowser((int)(param.width), (int)(param.height),CMainWindowTarget::GetInst()->GetHwnd());
		if(!m_WebWin->Create())
		{
			SafeDelete(m_WebWin);
			m_WebWin=NULL;
			return false;
		}
		m_WebWin->SetVisible(false);
		OnScreenPosChanged();
		//构建回调
		CWebBrowserCallBack* callBack=new CWebBrowserCallBack(this);
		m_WebWin->SetWebEventCallBack(callBack);
		SetszSourceName(m_hWnd->m_szSource.c_str());
	}
	else
	{
		m_WebWinAtl=new CWebBrowserATL((int)(param.width), (int)(param.height),CMainWindowTarget::GetInst()->GetHwnd());
		if(!m_WebWinAtl->Create())
		{
			SafeDelete(m_WebWinAtl);
			m_WebWinAtl=NULL;
			return false;
		}
		m_WebWinAtl->SetVisible(false);
		OnScreenPosChanged();
		//构建回调
		CWebBrowserCallBack* callBack=new CWebBrowserCallBack(this);
		m_WebWinAtl->SetWebEventCallBack(callBack);
		SetszSourceName(m_hWnd->m_szSource.c_str());
	}

	return true;
}

void SQRWebBrowser::SetszSourceName( const char* szDesc )
{
	if(!m_IsATL)
	{
		if(m_WebWin)
			m_WebWin->OpenURL(szDesc);
	}
	else
	{
		if(m_WebWinAtl)
			m_WebWinAtl->OpenURL(szDesc);
	}
	SQRWnd::SetszSourceName( szDesc );
}

void SQRWebBrowser::ShowWnd( bool bShow )
{
	SQRWnd::ShowWnd(bShow);
	m_Show=bShow;
}

void SQRWebBrowser::OnWndMoved( float dx, float dy )
{
	SQRWnd::OnWndMoved(dx,dy);
}

void SQRWebBrowser::OnScreenPosChanged()
{
	if(!m_IsATL)
	{
		if(m_WebWin)
		{
			HWND mainHand=CMainWindowTarget::GetInst()->GetHwnd();
			m_WebWin->SetPostion((int)GetWndLeft(),(int)GetWndTop());
		}
	}
	else
	{
		if(m_WebWinAtl)
		{
			HWND mainHand=CMainWindowTarget::GetInst()->GetHwnd();
			m_WebWinAtl->SetPostion((int)GetWndLeft(),(int)GetWndTop());
		}
	}
}

void SQRWebBrowser::DrawWndBackground()
{
	m_beBGDrawed = true;
	OnProcess();
	//ITexture* ieTex=m_WebWin->GetTexture();
	//DrawRect(ieTex, CFRect(0, 0, GetWndWidth(), GetWndHeight()),0xffffffff, &CFRect(0, 0, float(m_WebWin->GetTexWidth()),float(m_WebWin->GetTexHeight()) ), false);
}

void SQRWebBrowser::OnProcess()
{
	if(!m_IsATL)
	{
		if(m_WebWin)
		{
			m_WebWin->SetVisible(m_Show);
			m_WebWin->UpdateTexture();
		}
	}
	else
	{
		if(m_WebWinAtl)
		{
			m_WebWinAtl->SetVisible(m_Show);
			m_WebWinAtl->UpdateTexture();
		}
	}
}

LRESULT SQRWebBrowser::WndProc( uint32 message, WPARAM wParam, LPARAM lParam )
{
	return SQRControl::WndProc(message,wParam,lParam);
}
