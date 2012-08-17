#include "stdafx.h"
#include "SQRGUIManager.h"
#include "SQRControl.h"

DYNAMIC_CREATE_WND( SQRControl, SQRWnd )

SQRControl::SQRControl()
{

}
SQRControl::~SQRControl()
{

}

LRESULT SQRControl::SendCtrlMsg( uint32 uMsgID, uint32 uParam1, uint32 uParam2 )
{
	if( m_hWnd->m_ParentWnd )
	{
		m_hWnd->m_ParentWnd->OnCtrlmsg( this, uMsgID, uParam1, uParam2 );
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////
// 功能：窗口处理的消息
// 参数：uint32 message, WPARAM wParam, LPARAM lParam 消息内容
// 返回：略
////////////////////////////////////////////////////////////////////////
LRESULT SQRControl::WndProc( uint32 message, WPARAM wParam, LPARAM lParam )
{
	SQRGUIManager* pGUI = SQRGUIManager::GetGUI( this );
	
	SQR_TRY{
		switch( message )
		{
		case WM_HOTKEY:
			break;
		case WM_CHAR:
			OnChar( (char)wParam, lParam );
			break;
		case WM_IME_CHAR:  
			OnImeChar( wParam, lParam );
			break;
		case WM_KEYDOWN:
			OnKeyDown( wParam, lParam );
			break;
		case WM_KEYUP:
			OnKeyUp( wParam, lParam );
			break;
		case WM_LBUTTONDBLCLK:
			OnLButtonDblClk( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_LBUTTONUP:
			if ( this == m_hWnd->m_pGUI->GetLastLButtonDownWnd() )
				OnLButtonClick( wParam, LOWORD(lParam), HIWORD(lParam) );
			else
				OnLButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_MBUTTONDBLCLK:
			OnMButtonDblClk( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_MBUTTONDOWN:
			OnMButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_MBUTTONUP:
			OnMButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_MOUSEMOVE:
			OnMouseMove( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_MOUSEWHEEL:
			OnMouseWheel( LOWORD(wParam), (short) HIWORD(wParam), LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_RBUTTONDBLCLK:
			OnRButtonDblClk( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_RBUTTONDOWN:
			OnRButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );	
			break;
		case WM_RBUTTONUP:
			if ( this == m_hWnd->m_pGUI->GetLastRButtonDownWnd() )
				OnRButtonClick( wParam, LOWORD(lParam), HIWORD(lParam) );
			else
				OnRButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_SETCURSOR:
			return OnSetCursor();
		default:
			return 0;
		}
		if( pGUI->FindWnd( this ) && GetParent() && (GetParent()->GetStyle()&WS_NOTIFY) )
			SendCtrlMsg( WND_NOTIFY, message, wParam );
		return 0;
	}
	SQR_CATCH(exp) {
		LogExp(exp);
	}
	SQR_TRY_END;
	
	return NULL;
}
