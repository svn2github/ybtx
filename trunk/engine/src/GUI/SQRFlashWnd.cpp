#include "stdafx.h"
#include "SQRFlashWnd.h"
#include "CFlashPlayer.h"

DYNAMIC_CREATE_WND( SQRFlashWnd, SQRWnd )

SQRFlashWnd::SQRFlashWnd()
{
	m_FlashPlayer = NULL;
}

SQRFlashWnd::~SQRFlashWnd(void)
{
	SafeDelete(m_FlashPlayer);
}

int32 SQRFlashWnd::Create(const WndCreateParam& param )
{
	if ( FAILED( SQRWnd::Create( param ) ) )
		return GERROR;
	m_FlashPlayer=new CFlashPlayer((int)(param.width), (int)(param.height) );

	wstring wsrc;
	string src=m_hWnd->m_szSource.c_str();
	utf8_to_utf16(wsrc,src);
	m_FlashPlayer->SetFlashFile(wsrc.c_str());
	
	return true;
}

void SQRFlashWnd::OnProcess()
{
	if(m_FlashPlayer)
		m_FlashPlayer->UpdateTexture();
}

void SQRFlashWnd::SetszSourceName( const char* szDesc )
{
	if(m_FlashPlayer)
	{
		wstring wsrc;
		string src=szDesc;
		utf8_to_utf16(wsrc,src);
		m_FlashPlayer->SetFlashFile(wsrc.c_str());
		m_FlashPlayer->Play();
	}

	SQRWnd::SetszSourceName( szDesc );
}

void SQRFlashWnd::SetStyle( uint Style )
{
	SQRWnd::SetStyle(Style);
}

void SQRFlashWnd::PlayFlash()
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->Play();
}

void SQRFlashWnd::StopFlash()
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->Stop();
}

void SQRFlashWnd::UpdateFlash()
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->UpdateTexture();
}

void SQRFlashWnd::DrawWndBackground()
{
	m_beBGDrawed = true;
	if(!m_FlashPlayer)
		return ;
	OnProcess();
	ITexture* flashTex=m_FlashPlayer->GetTexture();
	DrawRect(flashTex, CFRect(0, 0, GetWndWidth(), GetWndHeight()),0xffffffff, &CFRect(0, 0, m_FlashPlayer->GetTexWidth(), m_FlashPlayer->GetTexHeight()), false);
}

void SQRFlashWnd::ResizeRect( const CFRect& rt ,bool refreshbak)
{
	SQRWnd::ResizeRect(rt,refreshbak);
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ResizeRect(rt.right-rt.left,rt.bottom-rt.top);
}
//MSG

LRESULT SQRFlashWnd::WndProc( uint32 message, WPARAM wParam, LPARAM lParam )
{
	if(message>WM_USER)
	{
		if(m_FlashPlayer)
			m_FlashPlayer->ProcKeyBoardMsg(message,wParam,lParam);
		return 0;
	}

	switch(message)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR:
	case WM_IME_CHAR:
	case WM_IME_COMPOSITION:
	case WM_IME_COMPOSITIONFULL:
	case WM_IME_CONTROL:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_KEYDOWN:
	case WM_IME_KEYUP:
	case WM_IME_NOTIFY:
	case WM_IME_REQUEST:
	case WM_IME_SELECT:
	case WM_IME_SETCONTEXT:
	case WM_IME_STARTCOMPOSITION:
	case WM_CANCELMODE:
		{
			if(m_FlashPlayer)
				m_FlashPlayer->ProcKeyBoardMsg(message,wParam,lParam);
			return 0;
		}
	}
	return SQRControl::WndProc(message,wParam,lParam);
}

//左键双击消息
void SQRFlashWnd::OnLButtonDblClk( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_LBUTTONDOWN,nFlags,MAKELPARAM(x,y));
	m_FlashPlayer->ProcMouseMsg(WM_LBUTTONUP,nFlags,MAKELPARAM(x,y));
}
//左键点击消息
void SQRFlashWnd::OnLButtonDown( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_LBUTTONDOWN,nFlags,MAKELPARAM(x,y));
}
//左键释放消息
void SQRFlashWnd::OnLButtonUp( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_LBUTTONUP,nFlags,MAKELPARAM(x,y));
}
//一次鼠标左键按下抬起操作
void SQRFlashWnd::OnLButtonClick( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_LBUTTONDOWN,nFlags,MAKELPARAM(x,y));
	m_FlashPlayer->ProcMouseMsg(WM_LBUTTONUP,nFlags,MAKELPARAM(x,y));
}
//中键双击消息
void SQRFlashWnd::OnMButtonDblClk( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_MBUTTONDBLCLK,nFlags,MAKELPARAM(x,y));
}
//中键点击消息
void SQRFlashWnd::OnMButtonDown( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_MBUTTONDOWN,nFlags,MAKELPARAM(x,y));
}
//中键弹起消息
void SQRFlashWnd::OnMButtonUp( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_MBUTTONUP,nFlags,MAKELPARAM(x,y));
}
//鼠标移动消息
void SQRFlashWnd::OnMouseMove( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_MOUSEMOVE,nFlags,MAKELPARAM(x,y));
}
//鼠标滚轮消息
void SQRFlashWnd::OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_MOUSEWHEEL,nFlags,MAKELPARAM(x,y));
}
//右键双击消息
void SQRFlashWnd::OnRButtonDblClk( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_RBUTTONDBLCLK,nFlags,MAKELPARAM(x,y));
}
//右键点击消息
void SQRFlashWnd::OnRButtonDown( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_RBUTTONDOWN,nFlags,MAKELPARAM(x,y));
}
//右键释放消息
void SQRFlashWnd::OnRButtonUp( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_RBUTTONUP,nFlags,MAKELPARAM(x,y));
}
//一次鼠标右键按下抬起操作
void SQRFlashWnd::OnRButtonClick( uint32 nFlags, int32 x, int32 y )
{
	if(!m_FlashPlayer)
		return ;
	m_FlashPlayer->ProcMouseMsg(WM_RBUTTONDOWN,nFlags,MAKELPARAM(x,y));
	m_FlashPlayer->ProcMouseMsg(WM_RBUTTONUP,nFlags,MAKELPARAM(x,y));
}
