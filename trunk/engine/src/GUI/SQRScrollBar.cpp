#include "stdafx.h"
#include "SQRScrollBar.h"
#include "SQRRootWnd.h"
#include "SQRGUIManager.h"
#define SB_MIN_SIZE 32

class CScrollBarButton : public SQRButton
{
	void CScrollBarButton::OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y )
	{
		SQRScrollBar* pSrollBar = (SQRScrollBar*)GetParent();
		zDelta = static_cast<short>( ( zDelta < 0 ? 1 : -1 )*( max( pSrollBar->GetPage()/10, 1 ) ) );
		pSrollBar->SetPos( pSrollBar->GetPos() + zDelta,true );	
	}
};

DYNAMIC_CREATE_WND( SQRScrollBar, SQRWnd )

SQRScrollBar::SQRScrollBar(void)
: m_pPressButton1( new CScrollBarButton )
, m_pPressButton2( new CScrollBarButton )
, m_pScrollBar( new CScrollBarButton )
{
}

SQRScrollBar::~SQRScrollBar(void)
{
	DestroyWnd();

	delete m_pPressButton1;
	delete m_pPressButton2;
	delete m_pScrollBar;
}

int32 SQRScrollBar::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.uStyle |= WS_CHILD;
	if( FAILED( SQRWnd::Create( newParam ) ) )
		return GERROR;

	CFRect rt;
	CFRect rt1,rt2,rt3;
	GetWndRect( rt );
	CFRect rtWnd = rt;

	float PressW;
	if( GetStyle()&SS_HSCROLL )
	{
		PressW = min( rtWnd.Height(), rtWnd.Width() / 3 );
		rt1 = CFRect( 0, 0, PressW, rtWnd.Height() );
		rt2 = CFRect( rtWnd.Width() - PressW, 0, rtWnd.Width(), rtWnd.Height() );
		rt3 = rt1;
		rt3.left = rt1.right;
		rt3.right = rt3.left + rt1.Width();	
	}
	else
	{
		PressW = min( rtWnd.Width(), rtWnd.Height() / 3 );
		rt1 = CFRect( 0, 0, rtWnd.Width(), PressW );
		rt2 = CFRect( 0, rtWnd.Height() - PressW, rtWnd.Width(), rtWnd.Height() );
		rt3 = rt1;
		rt3.top = rt1.bottom;
		rt3.bottom = rt3.top + rt1.Height();	
	}

	WndCreateParam childParam;
	childParam.x = rt1.left;
	childParam.y = rt1.top;
	childParam.width = rt1.Width();
	childParam.height = rt1.Height();
	childParam.pParentWnd = this;
	childParam.font_size = 12;
	m_pPressButton1->Create( childParam );
	m_pPressButton1->SetMouseWheel(true);
	childParam.x = rt2.left;
	childParam.y = rt2.top;
	childParam.width = rt2.Width();
	childParam.height = rt2.Height();
	m_pPressButton2->Create( childParam );
	m_pPressButton2->SetMouseWheel(true);
	childParam.x = rt3.left;
	childParam.y = rt3.top;
	childParam.width = rt3.Width();
	childParam.height = rt3.Height();
	m_pScrollBar->Create( childParam );
	m_pScrollBar->SetMouseWheel(true);
	//m_pScrollBar->ShowWnd(false);
	if( GetStyle()&SS_HSCROLL )
	{
		m_pPressButton1->SetLockFlag( LOCK_LEFT|LOCK_TOP|LOCK_BOTTOM );
		m_pPressButton2->SetLockFlag( LOCK_RIGHT|LOCK_TOP|LOCK_BOTTOM);
		m_pScrollBar->SetLockFlag(LOCK_LEFT|LOCK_RIGHT|LOCK_TOP|LOCK_BOTTOM);
	}
	else
	{
		m_pPressButton1->SetLockFlag( LOCK_TOP|LOCK_LEFT|LOCK_RIGHT );
		m_pPressButton2->SetLockFlag( LOCK_BOTTOM|LOCK_LEFT|LOCK_RIGHT );
		m_pScrollBar->SetLockFlag(LOCK_TOP|LOCK_BOTTOM|LOCK_LEFT|LOCK_RIGHT);
	}

	m_fRange		= 100;
	m_fPageSize = 10;
	m_fCurPos	= 0;

	RefreshThumbState();
	SetMouseWheel(true);
	return 0;
}


WND_IMAGE_LIST* SQRScrollBar::GetWndBkImage( const IMAGE_PARAM& ImageFlag )
{
	//ScrollBar
	//#define SM_SC_BK				0x0001		//±³¾°ÌùÍ¼
	//#define SM_SC_P1				0x0002		//°´¼ü1
	//#define SM_SC_P2			    0x0003		//°´¼ü2
	//#define SM_SC_BAR			    0x0004		//»¬¿é

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_SC_BK )
	{
		Flag.CtrlMask -= SM_SC_BK - SM_BS_BK;
		return SQRWnd::GetWndBkImage( Flag );
	}
	else if( Flag.CtrlMask == SM_SC_P1 )
	{
		Flag.CtrlMask -= SM_SC_P1 - SM_BS_BK;
		return m_pPressButton1->GetWndBkImage( Flag );
	}
	else if( Flag.CtrlMask == SM_SC_P2 )
	{
		Flag.CtrlMask -= SM_SC_P2 - SM_BS_BK;
		return m_pPressButton2->GetWndBkImage( Flag );
	}
	else if( Flag.CtrlMask == SM_SC_BAR )
	{
		Flag.CtrlMask -= SM_SC_BAR - SM_BS_BK;
		return m_pScrollBar->GetWndBkImage( Flag );
	}
	else 
		return NULL;
}

uint32 SQRScrollBar::GetWndTextColor( const IMAGE_PARAM& ImageFlag )
{
	//ScrollBar
	//#define SM_SC_BK				0x0001		//±³¾°ÌùÍ¼
	//#define SM_SC_P1				0x0002		//°´¼ü1
	//#define SM_SC_P2			    0x0003		//°´¼ü2
	//#define SM_SC_BAR			    0x0004		//»¬¿é

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_SC_BK )
	{
		Flag.CtrlMask -= SM_SC_BK - SM_BS_BK;
		return SQRWnd::GetWndTextColor( Flag );
	}
	else if( Flag.CtrlMask == SM_SC_P1 )
	{
		Flag.CtrlMask -= SM_SC_P1 - SM_BS_BK;
		return m_pPressButton1->GetWndTextColor( Flag );
	}
	else if( Flag.CtrlMask == SM_SC_P2 )
	{
		Flag.CtrlMask -= SM_SC_P2 - SM_BS_BK;
		return m_pPressButton2->GetWndTextColor( Flag );
	}
	else if( Flag.CtrlMask == SM_SC_BAR )
	{
		Flag.CtrlMask -= SM_SC_BAR - SM_BS_BK;
		return m_pScrollBar->GetWndTextColor( Flag );
	}
	else 
		return NULL;
}

uint32 SQRScrollBar::GetWndTextBcColor( const IMAGE_PARAM& ImageFlag )
{
	//ScrollBar
	//#define SM_SC_BK				0x0001		//±³¾°ÌùÍ¼
	//#define SM_SC_P1				0x0002		//°´¼ü1
	//#define SM_SC_P2			    0x0003		//°´¼ü2
	//#define SM_SC_BAR			    0x0004		//»¬¿é

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_SC_BK )
	{
		Flag.CtrlMask -= SM_SC_BK - SM_BS_BK;
		return SQRWnd::GetWndTextBcColor( Flag );
	}
	else if( Flag.CtrlMask == SM_SC_P1 )
	{
		Flag.CtrlMask -= SM_SC_P1 - SM_BS_BK;
		return m_pPressButton1->GetWndTextBcColor( Flag );
	}
	else if( Flag.CtrlMask == SM_SC_P2 )
	{
		Flag.CtrlMask -= SM_SC_P2 - SM_BS_BK;
		return m_pPressButton2->GetWndTextBcColor( Flag );
	}
	else if( Flag.CtrlMask == SM_SC_BAR )
	{
		Flag.CtrlMask -= SM_SC_BAR - SM_BS_BK;
		return m_pScrollBar->GetWndTextBcColor( Flag );
	}
	else 
		return NULL;
}

void SQRScrollBar::SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	//ScrollBar
	//#define SM_SC_BK				0x0001		//±³¾°ÌùÍ¼
	//#define SM_SC_P1				0x0002		//°´¼ü1
	//#define SM_SC_P2			    0x0003		//°´¼ü2
	//#define SM_SC_BAR			    0x0004		//»¬¿é

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_SC_BK )
	{
		Flag.CtrlMask -= SM_SC_BK - SM_BS_BK;
		SQRWnd::SetWndTextColor( Flag, uColor );
	}
	else if( Flag.CtrlMask == SM_SC_P1 )
	{
		Flag.CtrlMask -= SM_SC_P1 - SM_BS_BK;
		m_pPressButton1->SetWndTextColor( Flag, uColor );
	}
	else if( Flag.CtrlMask == SM_SC_P2 )
	{
		Flag.CtrlMask -= SM_SC_P2 - SM_BS_BK;
		m_pPressButton2->SetWndTextColor( Flag, uColor );
	}
	else if( Flag.CtrlMask == SM_SC_BAR )
	{
		Flag.CtrlMask -= SM_SC_BAR - SM_BS_BK;
		m_pScrollBar->SetWndTextColor( Flag, uColor );
	}
}

void SQRScrollBar::SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	//ScrollBar
	//#define SM_SC_BK				0x0001		//±³¾°ÌùÍ¼
	//#define SM_SC_P1				0x0002		//°´¼ü1
	//#define SM_SC_P2			    0x0003		//°´¼ü2
	//#define SM_SC_BAR			    0x0004		//»¬¿é

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_SC_BK )
	{
		Flag.CtrlMask -= SM_SC_BK - SM_BS_BK;
		SQRWnd::SetWndTextBcColor( Flag, uColor );
	}
	else if( Flag.CtrlMask == SM_SC_P1 )
	{
		Flag.CtrlMask -= SM_SC_P1 - SM_BS_BK;
		m_pPressButton1->SetWndTextBcColor( Flag, uColor );
	}
	else if( Flag.CtrlMask == SM_SC_P2 )
	{
		Flag.CtrlMask -= SM_SC_P2 - SM_BS_BK;
		m_pPressButton2->SetWndTextBcColor( Flag, uColor );
	}
	else if( Flag.CtrlMask == SM_SC_BAR )
	{
		Flag.CtrlMask -= SM_SC_BAR - SM_BS_BK;
		m_pScrollBar->SetWndTextBcColor( Flag, uColor );
	}
}

SQRWnd* SQRScrollBar::GetResWnd( int32 nCtrlMask )
{
	//ScrollBar
	//#define SM_SC_BK				0x0001		//±³¾°ÌùÍ¼
	//#define SM_SC_P1				0x0002		//°´¼ü1
	//#define SM_SC_P2			    0x0003		//°´¼ü2
	//#define SM_SC_BAR			    0x0004		//»¬¿é

	if( nCtrlMask == SM_SC_BK )
	{
		nCtrlMask -= SM_SC_BK - SM_BS_BK;
		return SQRWnd::GetResWnd( nCtrlMask );
	}
	else if( nCtrlMask == SM_SC_P1 )
	{
		nCtrlMask -= SM_SC_P1 - SM_BS_BK;
		return m_pPressButton1->GetResWnd( nCtrlMask );
	}
	else if( nCtrlMask == SM_SC_P2 )
	{
		nCtrlMask -= SM_SC_P2 - SM_BS_BK;
		return m_pPressButton2->GetResWnd( nCtrlMask );
	}
	else if( nCtrlMask == SM_SC_BAR )
	{
		nCtrlMask -= SM_SC_BAR - SM_BS_BK;
		return m_pScrollBar->GetResWnd( nCtrlMask );
	}
	else 
		return NULL;
}

void SQRScrollBar::SetWndRect( const CFRect& rt )
{
	SQRWnd::SetWndRect( rt );
	ProcessMove( false );
}

void SQRScrollBar::ResizeRect( const CFRect& rt, bool refreshbak  )
{
	SQRWnd::ResizeRect( rt, refreshbak );
	ProcessMove( false );
}

void SQRScrollBar::_SetWndRect( const CFRect& rt )
{
	SQRWnd::_SetWndRect( rt );
	ProcessMove( false );
}

void SQRScrollBar::_ResizeRect( const CFRect& rt, bool refreshbak  )
{
	SQRWnd::_ResizeRect( rt, refreshbak );
	ProcessMove( false );
}


//////////////////////////////////////////////////////////////////////////
void SQRScrollBar::ProcessMove( bool bSendMsg ,bool bDRAG /*= false*/)
{
	m_fRange = max( 1, m_fRange ); 
	m_fPageSize = min( m_fPageSize, m_fRange ); 
	m_fPageSize = max( m_fPageSize, 1 ); 

	m_fCurPos = min( m_fCurPos, (float)( m_fRange - m_fPageSize ) ); 
	m_fCurPos = max( (float)m_fCurPos, 0 ); 

	CFRect rt1, rt2, rt3, rt4;
	m_pPressButton1->GetWndRect( rt1 );
	m_pPressButton2->GetWndRect( rt2 );
	m_pScrollBar->GetWndRect( rt3 );

	SQRRootWnd* rootWnd = SQRWnd::GetRootWnd();
	float zoom;
	if (rootWnd)
		zoom = rootWnd->GetZoomSize();
	else
		zoom = 1.0f;
	float _min		= zoom * m_WndZoom * SB_MIN_SIZE;
	float _min_wnd	= ( (GetStyle()&SS_HSCROLL) != 0 ) ? (rt2.right - rt1.left) : (rt2.bottom - rt1.top);
	_min_wnd  = (_min_wnd > 0 ? _min_wnd : _min) * 2 / 3;
	m_sbMinSize = (int32)(min( _min, _min_wnd ));

	CFPos posStart, posEnd;
	if( GetStyle()&SS_HSCROLL )
	{
		if (m_fRange > m_fPageSize )
		{
			rt3.right  = rt3.left +  (rt2.left - rt1.right ) * (LONG)m_fPageSize/ (LONG)m_fRange;
			if (rt3.right > rt2.left)
			{
				rt3.right = rt2.left;
				rt3.left = rt2.left - (rt2.left - rt1.right ) * (LONG)m_fPageSize/ (LONG)m_fRange;

			}
			if ( Isbottom() ) 
			{
				rt3.left = rt3.left + rt2.left - rt3.right;
				rt3.right = rt2.left;
			}
			if (rt3.right - rt3.left <= m_sbMinSize)
				rt3.left   = rt3.right - m_sbMinSize;
		}
		posStart.x = rt1.right;
		posEnd.x   = rt2.left - ( rt3.right - rt3.left );
		posStart.y = rt1.top;
		posEnd.y   = rt2.top;
	}
	else
	{
		if ( m_fRange > m_fPageSize )	//ÏÔÊ¾ÄÚÈÝÂúÒ»ÆÁ£¬»¬¿éshow trueÊ±£¬¼ÆËã»¬¿éµÄ´óÐ¡
		{
			rt3.bottom  =  rt3.top  + (rt2.top - rt1.bottom ) * (LONG)m_fPageSize/ (LONG)m_fRange;
			if ( rt3.bottom > rt2.top )
			{
				rt3.bottom = rt2.top;
				rt3.top = rt2.top - (rt2.top - rt1.bottom ) * (LONG)m_fPageSize/ (LONG)m_fRange;
			}
			if ( Isbottom() ) 
			{
				rt3.top = rt3.top + (rt2.top - rt3.bottom); 
				rt3.bottom = rt2.top;
			}
			if (rt3.bottom - rt3.top <= m_sbMinSize)
				rt3.top = rt3.bottom - m_sbMinSize;
		}
		posStart.x = rt1.left;
		posEnd.x   = rt2.left;
		posStart.y = rt1.bottom;
		posEnd.y   = rt2.top - ( rt3.bottom - rt3.top );

	}	
	float fPos	= ( m_fRange > m_fPageSize ) ? m_fCurPos/( m_fRange - m_fPageSize ) : 0;
	rt4.left	= posEnd.x*fPos + posStart.x*( 1 - fPos );
	rt4.right	= rt4.left + ( rt3.right - rt3.left );
	rt4.top		= posEnd.y*fPos + posStart.y*( 1 - fPos );
	rt4.bottom	= rt4.top + ( rt3.bottom - rt3.top );
	
	if (bDRAG)
		m_pScrollBar->ResizeRect( rt4, false );
	else
	{
		if (Isbottom())
		{
			m_pScrollBar->ResizeRect( rt3, false );
		}
		else
			m_pScrollBar->ResizeRect( rt4, false );
	}
	
	if( bSendMsg )
	{
		SQRControl::SendCtrlMsg( SCROLL_MOVE, GetStyle()&SS_HSCROLL, (uint32)m_fCurPos );
		OnPosChanged();
	}
}

void SQRScrollBar::SetPos( int32 nPos, bool bMouseWheel )
{ 
	if(abs(m_fCurPos - (float)nPos) < 0.00001)
		return;
	m_fCurPos = (float)nPos; 
	ProcessMove(true, bMouseWheel);
}

void SQRScrollBar::SetRange( int32 fRange )
{
	if(m_fRange == (float)fRange)
		return;
	m_fRange = (float)fRange; 
	ProcessMove( true );
	RefreshThumbState();
}

void SQRScrollBar::SetPage( int32 fPage )
{
	if(m_fPageSize == (float)fPage)
		return;
	m_fPageSize = (float)fPage; 
	ProcessMove( true );
	RefreshThumbState();
}

void SQRScrollBar::OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 )
{
	int32 nPrePos = (int32)m_fCurPos;
	int32 nCtrl	  = SM_SC_BAR;	
	if( uMsgID == BUTTON_CLICKDOWN || uMsgID == BUTTON_CLICKCONT )
	{
		if( pChild == m_pPressButton1 )
			m_fCurPos--;
		if( pChild == m_pPressButton2 )
			m_fCurPos++;

		nCtrl = pChild == m_pPressButton1 ? SM_SC_P1 : SM_SC_P2;
		ProcessMove( true , true);
	}
	else if( uMsgID == BUTTON_DRAG && pChild == m_pScrollBar )
	{
		CPos HeldPoint( ( (short*)&uParam1 )[0], ( (short*)&uParam1 )[1] );
		CPos CurPoint( ( (short*)&uParam2 )[0], ( (short*)&uParam2 )[1] );
		CFRect rt1, rt2, rt3;
		m_pPressButton1->GetWndRect( rt1 );
		m_pPressButton2->GetWndRect( rt2 );
		m_pScrollBar->GetWndRect( rt3 );
		if( GetStyle()&SS_HSCROLL )
		{ 
			float fStart = rt1.right;
			float fEnd = rt2.left - ( rt3.right - rt3.left );
			m_fCurPos += ( CurPoint.x - HeldPoint.x )*( m_fRange - m_fPageSize ) / (fEnd - fStart);
		}
		else
		{
			float fStart = rt1.bottom;
			float fEnd = rt2.top - ( rt3.bottom - rt3.top );
			m_fCurPos += ( CurPoint.y - HeldPoint.y )*( m_fRange - m_fPageSize ) / (fEnd - fStart);
		}
		ProcessMove( true , true);
	}
	else if( uMsgID == BUTTON_CLICKUP )
	{
		SQRControl::SendCtrlMsg( SCROLL_RELEASE, nCtrl, nPrePos );
	}
}

void SQRScrollBar::OnLButtonDown( uint32 nFlags, int32 x, int32 y )
{
	CPos Point( x, y );

	CFRect rt1, rt2, rt3;
	m_pPressButton1->GetWndRect( rt1 );
	m_pPressButton2->GetWndRect( rt2 );
	m_pScrollBar->GetWndRect( rt3 );
	ScreenToWnd( rt1 );
	ScreenToWnd( rt2 );

	float fPos = -1;
	if( GetStyle()&SS_HSCROLL )
	{
		float fStart = rt1.right;
		float fEnd = rt2.left - ( rt3.right - rt3.left );
		if( Point.x > rt1.right && Point.x < rt2.left )
			fPos = ( Point.x - rt1.right )*( m_fRange - m_fPageSize ) / ( fEnd - fStart );
	}
	else
	{
		float fStart = rt1.bottom;
		float fEnd = rt2.top - ( rt3.bottom - rt3.top );
		if( Point.y > rt1.bottom && Point.y < rt2.top )
			fPos = ( Point.y - rt1.bottom )*( m_fRange - m_fPageSize ) / ( fEnd - fStart );
	}	

	if( fPos >= 0 )
	{
		if( fPos > m_fCurPos )
		{
			fPos = min( fPos, m_fRange );
			m_fCurPos += m_fPageSize;
			if( m_fCurPos > fPos )
				m_fCurPos = fPos;
		}
		else
		{
			fPos = max( fPos, 0 );
			m_fCurPos -= m_fPageSize;
			if( m_fCurPos < fPos )
				m_fCurPos = fPos;
		}
		ProcessMove( true, true );
	}
}

void SQRScrollBar::OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y )
{
	zDelta = static_cast<short>( ( zDelta < 0 ? 1 : -1 )*( max( GetPage()/10, 1 ) ) );
	SetPos( GetPos() + zDelta,true );
}

bool SQRScrollBar::Isbottom()const
{
	// m_pPressButton2.rect.top == m_pScrollBar.rect.bottom
	float bottom_button_pos = m_pPressButton2->GetWndHandle()->m_yScreen;
	float scrollbar_pos	    = m_pScrollBar->GetWndHandle()->m_yScreen;
	float scrollbar_height  = m_pScrollBar->GetWndHandle()->m_Height;
	/*return m_pPressButton2->GetWndHandle()->m_yScreen == m_pScrollBar->GetWndHandle()->m_yScreen + m_pScrollBar->GetWndHandle()->m_Height;*/
	bool bBottom	= abs((scrollbar_pos + scrollbar_height) - bottom_button_pos) < 1.0f;
	bool bShowBar	= GetRange() <= GetPage();
	return bBottom || bShowBar;
}

bool SQRScrollBar::Istop() const
{
//	int32 apos = m_pPressButton2->GetWndTop();
//	int32 bpos = m_pScrollBar->GetWndTop();
//	int32 height = m_pScrollBar->m_hWnd->m_Height;
	int32 pos = GetPos();
//	bool val = m_pPressButton2->GetWndTop() == m_pScrollBar->GetWndTop();
	return pos == 0;
}

void SQRScrollBar::RefreshThumbState()
{
	bool enable = m_fRange > m_fPageSize;
	m_pScrollBar->GetWndHandle()->m_bShow = enable;
	m_pPressButton1->GetWndHandle()->m_bEnable = enable;
	m_pPressButton2->GetWndHandle()->m_bEnable = enable;
}

void SQRScrollBar::SetCutWnd(SQRWnd* wnd)
{
	SQRWnd::SetCutWnd(wnd);
	m_pScrollBar->SetCutWnd(wnd);
	m_pPressButton1->SetCutWnd(wnd);
	m_pPressButton2->SetCutWnd(wnd);
}