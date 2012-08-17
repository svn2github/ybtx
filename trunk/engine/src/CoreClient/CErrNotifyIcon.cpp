#include "stdafx.h"
#include <shellapi.h>

#include "CErrNotifyIcon.h"
#include "TTickHandler.h"
#include "CTimeSystemClient.h"

CErrNotifyIcon::CErrNotifyIcon(HWND hWnd, HINSTANCE hInstance)
: m_pTickShowErrIcon(NULL)
{
	m_hErrIcon	= LoadIcon(NULL, MAKEINTRESOURCE(IDI_ERROR));
	m_hAppIcon	= LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));

	m_pData = new NOTIFYICONDATA;
	
	m_pData->cbSize		= sizeof(NOTIFYICONDATA);
	m_pData->hWnd		= hWnd;
	m_pData->uID		= 0;
	m_pData->uFlags		= NIF_ICON | NIF_TIP | NIF_INFO;	
	m_pData->hIcon		= m_hAppIcon;
	
	m_pData->szTip[0] = '\0';
	m_pData->szInfo[0] = '\0';
	m_pData->szInfoTitle[0] = '\0';

	m_pTickShowErrIcon = new TickFuncHandler_t(this);

	AddNotifyIcon();
}

CErrNotifyIcon::~CErrNotifyIcon()
{
	DelNotifyIcon();

	delete m_pData;

	delete m_pTickShowErrIcon;
}

void CErrNotifyIcon::Init(HWND hWnd, HINSTANCE hInstance)
{
	Inst() = new CErrNotifyIcon(hWnd, hInstance);
}

void CErrNotifyIcon::Unit()
{
	delete Inst();
	Inst() = NULL;
}

CErrNotifyIcon*& CErrNotifyIcon::Inst()
{
	static CErrNotifyIcon* ls_pInst = NULL;
	return ls_pInst;
}

void CErrNotifyIcon::ShowAppIcon()
{
	CTimeSystemClient::Inst()->UnRegister(m_pTickShowErrIcon);

	m_pData->uFlags		= NIF_ICON | NIF_TIP;
	m_pData->hIcon = m_hAppIcon;

	ModifyNotifyIcon();
}

//如果来了一个err，我们会让err icon持续显示10秒，然后在变成app icon
void CErrNotifyIcon::ShowErrIcon(const char *szMsg)
{
	m_pData->uFlags		= NIF_ICON | NIF_TIP | NIF_INFO;
	m_pData->hIcon = m_hErrIcon;
	
	strncpy(m_pData->szTip, szMsg, sizeof(m_pData->szTip));
	strncpy(m_pData->szInfo, szMsg, sizeof(m_pData->szInfo));

	ModifyNotifyIcon();

	//首先如果先前有一个tick还没有开始运行的话，注销这个tick
	//因为在CAppClient的构造函数抛出异常之后，会造成timemgr被析够
	//如果再次使用就会导致整个程序当掉，所以这里要判断是否为空
	if(CTimeSystemClient::Inst())
	{
		CTimeSystemClient::Inst()->UnRegister(m_pTickShowErrIcon);
	
		CTimeSystemClient::Inst()->Register(m_pTickShowErrIcon, 10000);
	}
}


void CErrNotifyIcon::AddNotifyIcon()
{
	Shell_NotifyIcon(NIM_ADD, m_pData);
}

void CErrNotifyIcon::DelNotifyIcon()
{
	Shell_NotifyIcon(NIM_DELETE, m_pData);
}

void CErrNotifyIcon::ModifyNotifyIcon()
{
	Shell_NotifyIcon(NIM_MODIFY, m_pData);
}

