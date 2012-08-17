#include "stdafx.h"
#include "SQRButton.h"
#include "SQRGUIManager.h"
#include "SQRDialog.h"
#include "CColor.inl"

DYNAMIC_CREATE_WND( SQRButton, SQRControl )

SQRButton::SQRButton(void)
: m_fTextOffsetX(0)
, m_fTextOffsetY(0)
{
}

SQRButton::~SQRButton(void)
{
    DestroyWnd();
}

int32 SQRButton::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.uStyle |= WS_CHILD | WS_IGNOREWHEEL;
	int32 re = SQRWnd::Create( newParam );

	float nWidth  = GetWndWidth();
	float nHeight = GetWndHeight();
	CFRect rt( 0, 0, nWidth, nHeight );
	uint32 dwColor;
	dwColor = CColor::RandDark();
	m_MouseOverImage.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), dwColor );
	dwColor = CColor::RandDark();
	m_ClickDownImage.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), dwColor );

	m_ClickDownColor = 0xffffffff;
	m_MouseOverColor = 0xffffffff;

	m_MouseOverBcColor = 0;
	m_ClickDownBcColor = 0;

	m_uFlashCircle    = 0;
	m_uFlashStartTime = 0;
	m_uFlashEndTime   = 0;
    
	return re;
}

void SQRButton::SetBackImageColor(uint32 uColor)
{
	float nWidth  = GetWndOrgWidth();
	float nHeight = GetWndOrgHeight();
	CFRect rt( 0, 0, nWidth, nHeight );
	m_hWnd->m_Enable.ClearImage();
	m_hWnd->m_Disable.ClearImage();
	m_hWnd->m_Enable.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), uColor );
	m_hWnd->m_Disable.AddImage(GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), uColor );
}

void SQRButton::ReleaseWndTex()
{
	// 释放可以释放的UI贴图资源
	m_MouseOverImage.ReleaseTex();
	m_ClickDownImage.ReleaseTex();
	SQRWnd::ReleaseWndTex();
}
void SQRButton::CreatWndTex()
{
	// 显示之前申请资源
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	m_MouseOverImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_ClickDownImage.CreatTex( GetGraphic(), m_FlushDlg );
	SQRWnd::CreatWndTex();
}


WND_IMAGE_LIST* SQRButton::GetWndBkImage( const IMAGE_PARAM& ImageFlag )
{
	//#define IP_ENABLE					0x0001		//背景贴图
	//#define IP_DISABLE				0x0002		//背景贴图
	//#define IP_MOUSEOVER		        0x0003		//BUTTON控件MOUSEOVER贴图
	//#define IP_CLICKDOWN		        0x0004		//BUTTON控件CLICKDOWN贴图
	//#define IP_UNCHECK_ENABLE			0x0005		//BUTTON控件Uncheck Normal贴图
	//#define IP_UNCHECK_ENABLE			0x0006		//BUTTON控件Uncheck Disable贴图
	//#define IP_UNCHECK_MOUSEOVER	    0x0007		//BUTTON控件Uncheck MOUSEOVER贴图
	//#define IP_UNCHECK_CLICKDOWN	    0x0008		//BUTTON控件Uncheck CLICKDOWN贴图

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask != SM_BS_BK )//SM_BS_BK:背景贴图
		return NULL;
	else if( Flag.StateMask < IP_MOUSEOVER )//IP_MOUSEOVER:BUTTON控件CLICKDOWN贴图
		return SQRWnd::GetWndBkImage( Flag );
	else if( Flag.StateMask == IP_MOUSEOVER )
		return &m_MouseOverImage;
	else if( Flag.StateMask == IP_CLICKDOWN )
		return &m_ClickDownImage;
	else
		return NULL;
}

bool	SQRButton::IsHasEventState(EventStateMask Mask)
{
	if(	IP_MOUSEOVER		== Mask ||
		IP_CLICKDOWN		== Mask )
		return true;
	return SQRWnd::IsHasEventState(Mask);
}

bool SQRButton::ObjHasEventState(EventStateMask Mask)
{
	if ( IP_LBTNCLICK_ACTION == Mask ||
		IP_RBTNCLICK_ACTION	 == Mask )
		return true;
	return SQRButton::IsHasEventState(Mask);
}

uint32 SQRButton::GetWndTextColor( const IMAGE_PARAM& ImageFlag )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask != SM_BS_BK )
		return 0;
	else if( Flag.StateMask < IP_MOUSEOVER )
		return SQRWnd::GetWndTextColor( Flag );
	else if( Flag.StateMask == IP_MOUSEOVER )
		return m_MouseOverColor;
	else if( Flag.StateMask == IP_CLICKDOWN )
		return m_ClickDownColor;
	else
		return 0;
}

uint32 SQRButton::GetWndTextBcColor( const IMAGE_PARAM& ImageFlag )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask != SM_BS_BK )
		return 0;
	else if( Flag.StateMask < IP_MOUSEOVER )
		return SQRWnd::GetWndTextBcColor( Flag );
	else if( Flag.StateMask == IP_MOUSEOVER )
		return m_MouseOverBcColor;
	else if( Flag.StateMask == IP_CLICKDOWN )
		return m_ClickDownBcColor;
	else
		return 0;
}


void SQRButton::SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.StateMask < IP_MOUSEOVER )
		SQRWnd::SetWndTextColor( Flag, uColor );
	else if( Flag.StateMask == IP_MOUSEOVER )
		m_MouseOverColor = uColor;
	else if( Flag.StateMask == IP_CLICKDOWN )
		m_ClickDownColor = uColor;
}

void SQRButton::SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.StateMask < IP_MOUSEOVER )
		SQRWnd::SetWndTextBcColor( Flag, uColor );
	else if( Flag.StateMask == IP_MOUSEOVER )
		m_MouseOverBcColor = uColor;
	else if( Flag.StateMask == IP_CLICKDOWN )
		m_ClickDownBcColor = uColor;
}

void SQRButton::DrawWndText()
{
	CFPos pt = GetCursorPos();
	CFRect rt;
	GetWndRect( rt );
	rt.right -= rt.left;
	rt.bottom -= rt.top;
	rt.left = rt.top = 0.0f;
	if( IsHeld() )
		DrawText( GetWndTextW(), m_ClickDownColor, m_ClickDownBcColor );	
	else if( IsInWndArea( pt.x, pt.y ) && IsEnable() && IsLastMouseMsgWnd() )
		DrawText( GetWndTextW(), m_MouseOverColor, m_MouseOverBcColor );
	else
		SQRWnd::DrawWndText();
}

void SQRButton::DrawText( const wstring& text, uint32 textColor, uint32 textBcColor )
{
	if( (WS_MULLINETEXT&GetStyle()) == WS_MULLINETEXT )
		DrawText( text, 0, 0, textColor, textBcColor, true, NULL);
	else
	{
		float x = 0, y = 0;
		CVector2f size	= GetFont()->CalculateSize(text.c_str());
		float Width		= size.x;
		float Height	= size.y;
	
		if( GetStyle() & WS_TEXTXC )
			x += m_hWnd->m_Width/2 - (Width/2.0f);
		else if( GetStyle() & WS_TEXTXR )
			x += m_hWnd->m_Width - (Width);
		else
			x +=  m_itX;

		if( GetStyle() & WS_TEXTYC )
			y += m_hWnd->m_Height/2 - (Height/2.0f);
		else if( GetStyle() & WS_TEXTYB )
			y += m_hWnd->m_Height - (Height);
		else
			y += m_itY;

		DrawText( text, x, y, textColor, textBcColor, false, NULL );
	}
}

void SQRButton::DrawText( const wstring& text, float x, float y, CColor color, CColor bkColor, bool multiline, CFRect* rect)
{
	float zoomSize = GetRootZoomSize();
	if( IsHeld() )
	{
		int32 nStyle = GetStyle();
		float nXof = GetXOF(nStyle);
		float nYof = GetYOF(nStyle);

		x += nXof * zoomSize;
		y += nYof * zoomSize;
	}
	x += GetTextOffsetX() * zoomSize;
	y += GetTextOffsetY() * zoomSize;

	return SQRWnd::DrawText( text, x, y, color, bkColor, multiline, rect );

}

void SQRButton::DrawBackImage( WND_IMAGE_LIST& Enable, WND_IMAGE_LIST& Disable, WND_IMAGE_LIST& MouseOver, WND_IMAGE_LIST& ClickDown, bool bClick )
{
	if(!Enable.IsImageLoadValid()&&!Disable.IsImageLoadValid())
		return;
	CFPos pt = GetCursorPos();
	if( bClick )
	{
		SetEventStateMask(IP_CLICKDOWN);
		DrawRect( ClickDown );
	}
	else if( IsInWndArea( pt.x, pt.y ) && IsEnable() && IsLastMouseMsgWnd() )
	{
		SetEventStateMask(IP_MOUSEOVER);
		DrawRect( MouseOver );
	}
	else if( !IsEnable() )
	{
		SetEventStateMask(IP_DISABLE);
		DrawRect( Disable );
	}
	else if( m_uFlashCircle && m_uFlashEndTime - m_uFlashStartTime > uint32(GetProcessTime()) - m_uFlashStartTime )
	{
		float fAlpha  = GetTransparent();
		float fWeight = abs( (float)( ( uint32(GetProcessTime()) - m_uFlashStartTime )%( m_uFlashCircle*2 ) - m_uFlashCircle ) )/m_uFlashCircle;
		SetTransparent( fAlpha*fWeight );
		SetEventStateMask(IP_ENABLE);
		DrawRect( Enable );
		SetTransparent( fAlpha*( 1.0f - fWeight ) );
		DrawRect( MouseOver );
		SetTransparent( fAlpha );
	}
	else
	{
		SetEventStateMask(IP_ENABLE);
		DrawRect( Enable );
	}

	if( m_bFirstMouseOver && GetEventStateMask() == IP_MOUSEOVER )
		OnPlayWndSound( eMS_MouseOver );

	m_bFirstMouseOver = GetEventStateMask() == IP_MOUSEOVER ? false : true;
}

void SQRButton::DrawWndBackground()
{
	m_beBGDrawed = true;
	if(m_hWnd->m_bIsShowBackground)
		DrawBackImage( m_hWnd->m_Enable, m_hWnd->m_Disable, m_MouseOverImage, m_ClickDownImage, IsHeld() );
}

void SQRButton::OnLButtonDown( uint32 nFlags, int32 x, int32 y )
{
	uint32 uMsgID = BUTTON_CLICKDOWN;
	SendCtrlMsg( uMsgID, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
}

void SQRButton::OnRButtonDown(uint32 nFlags, int32 x, int32 y)
{
	uint32 uMsgID = BUTTON_RIGHTCLICKDOWN;
	SendCtrlMsg( uMsgID, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );	
}

void SQRButton::OnLButtonUp( uint32 nFlags, int32 x, int32 y )
{	
	uint32 uMsgID = BUTTON_CLICKUP;
	SendCtrlMsg( uMsgID, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
}

void SQRButton::OnLButtonClick( uint32 nFlags, int32 x, int32 )
{
	SetEventStateMask(IP_LBTNCLICK_ACTION);
	uint32 uMsgID = BUTTON_LCLICK;
	SendCtrlMsg( uMsgID, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
}

void SQRButton::OnRButtonUp( uint32 nFlags, int32 x, int32 y )
{
	uint32 uMsgID = BUTTON_RIGHTCLICKUP;
	SendCtrlMsg( uMsgID, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
}

void SQRButton::OnRButtonClick( uint32 nFlags, int32 x, int32 )
{
	SetEventStateMask(IP_RBTNCLICK_ACTION);
	uint32 uMsgID = BUTTON_RCLICK;
	SendCtrlMsg( uMsgID, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
}

void SQRButton::OnMouseMove( uint32 nFlags, int32 x, int32 y )
{
	uint32 uMsgID = BUTTON_MOUSEMOVE;
	SendCtrlMsg( uMsgID, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
}

void SQRButton::OnHeld( float xScreen, float yScreen )
{
	SendCtrlMsg( BUTTON_CLICKCONT, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
}

void SQRButton::OnDrag( const CFPos& HeldPoint, const CFPos& CurPoint )
{
	CFRect rt(HeldPoint.x, HeldPoint.y, CurPoint.x, CurPoint.y);
	WndToScreen( rt );
	//GetParent()->ScreenToWnd( rt );

	SendCtrlMsg( BUTTON_DRAG, MAKELONG( (short)rt.left, (short)rt.top ), MAKELONG( (short)rt.right, (short)rt.bottom ) );

	return SQRWnd::OnDrag( HeldPoint, CurPoint );
}


int32	SQRButton::DestroyWnd()
{
	// 清空
    m_MouseOverImage.Release();
    m_ClickDownImage.Release();

    return SQRWnd::DestroyWnd();
}


void SQRButton::SetFlashTime( uint32 uFlashCircle, int32 nFlashTime )
{
	m_uFlashCircle    = uFlashCircle;
	m_uFlashStartTime = uint32(GetProcessTime());
	m_uFlashEndTime   = m_uFlashStartTime + (uint32)nFlashTime;
}
