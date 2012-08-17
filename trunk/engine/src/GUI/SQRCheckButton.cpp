#include "stdafx.h"
#include "SQRCheckButton.h"
#include "SQRGUIManager.h"
#include "CodeCvs.h"
#include "SQRDialog.h"

DYNAMIC_CREATE_WND( SQRCheckButton, SQRButton )

SQRCheckButton::SQRCheckButton(void)
{
}

SQRCheckButton::~SQRCheckButton(void)
{
    DestroyWnd();
}

int32 SQRCheckButton::Create( const WndCreateParam& param )
{
	int32 re = SQRButton::Create( param );

	float nWidth  = GetWndWidth();
	float nHeight = GetWndWidth();
	CFRect rt( 0.0f, 0.0f, nWidth, nHeight );
    m_UncheckMouseOverImage.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), 0xff567890 );
    m_UncheckClickDownImage.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), 0xff5678ff );
    m_UncheckEnableImage.   AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), 0xff456789 );
	m_UncheckDisableImage.  AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), 0xff555555 );
	
	m_UncheckEnableColor		= 0xffffffff;
	m_UncheckDisableColor		= 0xffffffff;
	m_UncheckMouseOverColor		= 0xffffffff;
	m_UncheckClickDownColor		= 0xffffffff;
	
	m_UncheckMouseOverBcColor	= 0;
	m_UncheckClickDownBcColor	= 0;
	m_UncheckEnableBcColor		= 0;
	m_UncheckDisableBcColor		= 0;
 
    m_bCheck					= false;

	return re;
}

WND_IMAGE_LIST* SQRCheckButton::GetWndBkImage( const IMAGE_PARAM& ImageFlag )
{
	//#define IP_ENABLE					0x0001		//背景贴图
	//#define IP_DISABLE					0x0002		//背景贴图
	//#define IP_MOUSEOVER		        0x0003		//BUTTON控件MOUSEOVER贴图
	//#define IP_CLICKDOWN		        0x0004		//BUTTON控件CLICKDOWN贴图
	//#define IP_UNCHECK_ENABLE			0x0005		//BUTTON控件Uncheck Normal贴图
	//#define IP_UNCHECK_ENABLE			0x0006		//BUTTON控件Uncheck Disable贴图
	//#define IP_UNCHECK_MOUSEOVER	    0x0007		//BUTTON控件Uncheck MOUSEOVER贴图
	//#define IP_UNCHECK_CLICKDOWN	    0x0008		//BUTTON控件Uncheck CLICKDOWN贴图

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask != SM_BS_BK )
		return NULL;
	else if( Flag.StateMask < IP_UNCHECK_ENABLE )
		return SQRButton::GetWndBkImage( Flag );
	else if( Flag.StateMask == IP_UNCHECK_ENABLE )
		return &m_UncheckEnableImage;
	else if( Flag.StateMask == IP_UNCHECK_DISABLE )
        return &m_UncheckDisableImage;
    else if( Flag.StateMask == IP_UNCHECK_MOUSEOVER )
        return &m_UncheckMouseOverImage;
    else if( Flag.StateMask == IP_UNCHECK_CLICKDOWN )
        return &m_UncheckClickDownImage;
	else
		return NULL;
}

bool	SQRCheckButton::IsHasEventState(EventStateMask Mask)
{
	if(	IP_UNCHECK_ENABLE		== Mask ||
		IP_UNCHECK_DISABLE		== Mask ||
		IP_UNCHECK_MOUSEOVER	== Mask ||
		IP_UNCHECK_CLICKDOWN	== Mask )
		return true;
	return SQRButton::IsHasEventState(Mask);
}

bool SQRCheckButton::ObjHasEventState(EventStateMask Mask)
{
	return SQRCheckButton::IsHasEventState(Mask);
}

uint32 SQRCheckButton::GetWndTextColor( const IMAGE_PARAM& ImageFlag )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask != SM_BS_BK )
		return 0;
	else if( Flag.StateMask < IP_UNCHECK_ENABLE )
		return SQRButton::GetWndTextColor( Flag );
	else if( Flag.StateMask == IP_UNCHECK_ENABLE )
		return m_UncheckEnableColor;
	else if( Flag.StateMask == IP_UNCHECK_DISABLE )
		return m_UncheckDisableColor;
	else if( Flag.StateMask == IP_UNCHECK_MOUSEOVER )
		return m_UncheckMouseOverColor;
	else if( Flag.StateMask == IP_UNCHECK_CLICKDOWN )
		return m_UncheckClickDownColor;
	else
		return 0;
}

uint32 SQRCheckButton::GetWndTextBcColor( const IMAGE_PARAM& ImageFlag )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask != SM_BS_BK )
		return 0;
	else if( Flag.StateMask < IP_UNCHECK_ENABLE )
		return SQRButton::GetWndTextBcColor( Flag );
	else if( Flag.StateMask == IP_UNCHECK_ENABLE )
		return m_UncheckEnableBcColor;
	else if( Flag.StateMask == IP_UNCHECK_DISABLE )
		return m_UncheckDisableBcColor;
	else if( Flag.StateMask == IP_UNCHECK_MOUSEOVER )
		return m_UncheckMouseOverBcColor;
	else if( Flag.StateMask == IP_UNCHECK_CLICKDOWN )
		return m_UncheckClickDownBcColor;
	else
		return 0;
}

void SQRCheckButton::SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.StateMask < IP_UNCHECK_ENABLE )
		SQRButton::SetWndTextColor( Flag, uColor );
	else if( Flag.StateMask == IP_UNCHECK_ENABLE )
		m_UncheckEnableColor = uColor;
	else if( Flag.StateMask == IP_UNCHECK_DISABLE )
		m_UncheckDisableColor = uColor;
	else if( Flag.StateMask == IP_UNCHECK_MOUSEOVER )
		m_UncheckMouseOverColor = uColor;
	else if( Flag.StateMask == IP_UNCHECK_CLICKDOWN )
		m_UncheckClickDownColor = uColor;
}

void SQRCheckButton::SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.StateMask < IP_UNCHECK_ENABLE )
		SQRButton::SetWndTextBcColor( Flag, uColor );
	else if( Flag.StateMask == IP_UNCHECK_ENABLE )
		m_UncheckEnableBcColor = uColor;
	else if( Flag.StateMask == IP_UNCHECK_DISABLE )
		m_UncheckDisableBcColor = uColor;
	else if( Flag.StateMask == IP_UNCHECK_MOUSEOVER )
		m_UncheckMouseOverBcColor = uColor;
	else if( Flag.StateMask == IP_UNCHECK_CLICKDOWN )
		m_UncheckClickDownBcColor = uColor;
}

void SQRCheckButton::ReleaseWndTex()
{
	// 释放可以释放的UI贴图资源
	m_UncheckMouseOverImage.ReleaseTex();
	m_UncheckClickDownImage.ReleaseTex();
	m_UncheckEnableImage.ReleaseTex();
	m_UncheckDisableImage.ReleaseTex();
	SQRButton::ReleaseWndTex();
}
void SQRCheckButton::CreatWndTex()
{
	// 显示之前申请资源
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	m_UncheckMouseOverImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_UncheckClickDownImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_UncheckEnableImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_UncheckDisableImage.CreatTex( GetGraphic(), m_FlushDlg );
	SQRButton::CreatWndTex();
}


void SQRCheckButton::DrawWndBackground()
{
	m_beBGDrawed = true;
	//处于选中状态
	if( m_bCheck )
		DrawBackImage( m_hWnd->m_Enable, m_hWnd->m_Disable, m_MouseOverImage, m_ClickDownImage, IsHeld() );
	else
	{
		CFPos pt = GetCursorPos();
		if( IsHeld() )
		{
			SetEventStateMask(IP_UNCHECK_CLICKDOWN);
			DrawRect( m_UncheckClickDownImage );
		}
		else if( IsInWndArea( pt.x, pt.y ) && IsEnable() && IsLastMouseMsgWnd() )
		{
			SetEventStateMask(IP_UNCHECK_MOUSEOVER);
			DrawRect( m_UncheckMouseOverImage );
		}
		else if( !IsEnable() )
		{
			SetEventStateMask(IP_UNCHECK_DISABLE);
			DrawRect( m_UncheckDisableImage );
		}
		else if( m_uFlashCircle && m_uFlashEndTime - m_uFlashStartTime > uint32(GetProcessTime()) - m_uFlashStartTime )
		{
			float fAlpha  = GetTransparent();
			float fWeight = abs( (float)( ( uint32(GetProcessTime()) - m_uFlashStartTime )%( m_uFlashCircle*2 ) - m_uFlashCircle ) )/m_uFlashCircle;
			SetTransparent( fAlpha*fWeight );
			SetEventStateMask(IP_UNCHECK_ENABLE);
			DrawRect( m_UncheckEnableImage );
			SetTransparent( fAlpha*( 1.0f - fWeight ) );
			SetEventStateMask(IP_UNCHECK_MOUSEOVER);
			DrawRect( m_UncheckMouseOverImage );
			SetTransparent( fAlpha );
		}
		else
		{
			SetEventStateMask(IP_UNCHECK_ENABLE);
			DrawRect( m_UncheckEnableImage );
		}
	}
}

void SQRCheckButton::DrawWndText()
{
	UIWString str = GetWndTextW();
	int32 nSize = str.size();
	if( !nSize )
		return;

	CFPos pt = GetCursorPos();
	CFRect rt;
	GetWndRect(rt);
	rt.right	-= rt.left;
	rt.bottom	-= rt.top;
	rt.left		 = rt.top = 0;
 
	int32 i = 0;
	const wchar_t* szText = str.c_str();
	for( ; i < nSize - 1; i++ )
	{
		if( str[i] == '\\' && str[ i + 1 ] == 'n' )
			str[i] = str[ i + 1 ] = 0;
		if( !str[i] && !str[ i + 1 ] )
		{
			if( !m_bCheck )
				szText += i + 2;
			break;
		}
	}

	//处于选中状态
	if( m_bCheck )
	{
		if( IsHeld() )
		{
			if( IsInWndArea( pt.x, pt.y ) )
				DrawText( szText, m_ClickDownColor, m_ClickDownBcColor);
			else if( IsEnable() )
				DrawText( szText, m_hWnd->m_EnableColor, m_hWnd->m_EnableBcColor );
			else
				DrawText( szText, m_hWnd->m_DisableColor, m_hWnd->m_DisableBcColor );

		}
		else
		{
			if( IsInWndArea( pt.x, pt.y ) && IsEnable() )
				DrawText( szText, m_MouseOverColor, m_MouseOverBcColor );
			else if( IsEnable() )
				DrawText( szText, m_hWnd->m_EnableColor, m_hWnd->m_EnableBcColor );
			else
				DrawText( szText, m_hWnd->m_DisableColor, m_hWnd->m_DisableBcColor );
		}
	}
	//非选中状态
	else if( !m_bCheck )
	{
		if( IsHeld() )
		{
			if( IsInWndArea( pt.x, pt.y ) )
				DrawText( szText, m_UncheckClickDownColor, m_UncheckClickDownBcColor );
			else
				DrawText( szText, m_UncheckEnableColor, m_UncheckEnableBcColor );
		}
		else
		{
			if( IsInWndArea( pt.x, pt.y ) && IsEnable() )
				DrawText( szText, m_UncheckMouseOverColor, m_UncheckMouseOverBcColor );
			else if( IsEnable() )
				DrawText( szText, m_UncheckEnableColor, m_UncheckEnableBcColor );
			else
				DrawText( szText, m_UncheckDisableColor, m_UncheckDisableBcColor );
		}
	}
}

void SQRCheckButton::DrawText( const wstring& text, uint32 textColor, uint32 textBcColor )
{
	if( (WS_MULLINETEXT&GetStyle()) == WS_MULLINETEXT )
		SQRButton::DrawText( text, 0, 0, textColor, textBcColor, true, NULL );
	else
	{
		float x = 0, y = 0;
		CVector2f size	= GetFont()->CalculateSize(text.c_str());
		float Width		= size.x;
		float Height	= size.y;

		if( GetStyle() & WS_TEXTXC )
			x += m_hWnd->m_Width/2.0f - Width/2.0f;
		else if( GetStyle() & WS_TEXTXR )
			x += m_hWnd->m_Width - Width;

		if( GetStyle() & WS_TEXTYC )
			y += m_hWnd->m_Height/2.0f - Height/2.0f;
		else if( GetStyle() & WS_TEXTYB )
			y += m_hWnd->m_Height - Height;

		SQRButton::DrawText( text, x, y, textColor, textBcColor, false, NULL );
	}

}

void SQRCheckButton::PrepareDefaultDesc()
{
	if ( GetRichToolTip() )
	{
		SQRWnd::PrepareDefaultDesc();
	}
	const UIString& str = GetMouseOverDesc();
	int32 nSize = str.size();
	if( !nSize )
		return;

	int32 i = 0;
	for( ; i < nSize - 1; i++ )
		if( str[i] == '\\' && str[ i + 1 ] == 'n' )
			break;

	if( i == nSize - 1 )
		SQRWnd::PrepareDefaultDesc();
	else
	{
		UIString strTemp = GetMouseOverDesc();
		if( m_bCheck )
			m_hWnd->m_DescRichText->m_szDesc.assign( str.c_str(), i );
		else if( nSize > i + 2 )
			m_hWnd->m_DescRichText->m_szDesc.assign( str.c_str() + i + 2, nSize - i - 2 );
		SQRWnd::PrepareDefaultDesc();
		m_hWnd->m_DescRichText->m_szDesc = strTemp;
		m_hWnd->m_DescRichText->m_szDescW = utf8_to_utf16(m_hWnd->m_DescRichText->m_szDesc.c_str()).c_str();
	}
}

void SQRCheckButton::OnLButtonClick( uint32 nFlags, int32 x, int32 y )
{
	//处理分组状态
	//复选框,可同时选多个
	if( GetStyle() & BTN_CHECK )
	{
		//切换状态
		m_bCheck = !m_bCheck;
		OnCheck( m_bCheck );
	}
	//单选框，同时只有一个可被选中
	else if( GetStyle() & BTN_RADIO )
		SetCheck( true );

	SQRButton::OnLButtonClick( nFlags, x, y );
}

void SQRCheckButton::OnRButtonClick( uint32 nFlags, int32 x, int32 y )
{
	SQRButton::OnRButtonClick( nFlags, x, y );
}

void SQRCheckButton::OnLButtonUp( uint32 nFlags, int32 x, int32 y )
{
	SQRButton::OnLButtonUp( nFlags, x, y );
}

void SQRCheckButton::OnRButtonUp( uint32 nFlags, int32 x, int32 y )
{
	SQRButton::OnRButtonUp( nFlags, x, y );
}


SQRWnd* SQRCheckButton::GetFirstBrother()
{
    return GetParent()->FindFirstChild();
}

SQRWnd* SQRCheckButton::GetLastBrother()
{
    return GetParent()->FindLastChild();
}

int32	SQRCheckButton::DestroyWnd()
{
   m_UncheckMouseOverImage.Release();
   m_UncheckClickDownImage.Release();
   m_UncheckEnableImage.Release();
   m_UncheckDisableImage.Release();

    return SQRButton::DestroyWnd();
}

void SQRCheckButton::SetCheck( bool bCheck )
{ 
	m_bCheck = bCheck; 
	
	//单选框，同时只有一个可被选中
	if( m_bCheck  && (GetStyle() & BTN_RADIO) && GetGroupID() >= 0 )
	{
		SQRWnd* pBrother = GetFirstBrother();
		while( pBrother )
		{
			if( GetClassName() == pBrother->GetClassName() && pBrother != this &&
                GetGroupID() == static_cast<SQRCheckButton*>(pBrother)->GetGroupID() )
					static_cast<SQRCheckButton*>(pBrother)->SetCheck( false );

			pBrother = pBrother->FindNextWnd();
		}
	}
	OnCheck( m_bCheck );
}
