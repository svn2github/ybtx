#include "stdafx.h"
#include "SQRSpin.h"

DYNAMIC_CREATE_WND( SQRSpin, SQRWnd )

SQRSpin::SQRSpin(void)
{
}

SQRSpin::~SQRSpin(void)
{
}

int32 SQRSpin::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.uStyle |= WS_CHILD;
	if( FAILED( SQRWnd::Create( newParam ) ) )
		return GERROR;

	CFRect rt;
    CFRect rt1,rt2;
	GetWndRect( rt );
	float fWidth = rt.right - rt.left;
	float fHeight = rt.bottom - rt.top;

	if( GetStyle()&PS_HSCROLL )
	{
		rt1 = CFRect( 0, 0, fWidth/2, fHeight );
		rt2 = CFRect( fWidth/2, 0, fWidth, fHeight );
	}
	else
	{
		rt1 = CFRect( 0, 0, fWidth, fHeight/2 );
		rt2 = CFRect( 0, fHeight/2, fWidth, fHeight );
	}

	WndCreateParam childParam;
	childParam.x = rt1.left;
	childParam.y = rt1.top;
	childParam.width = rt1.Width();
	childParam.height = rt1.Height();
	childParam.pParentWnd = this;
	childParam.font_size = 12;
	m_PressButton1.Create( childParam );
	childParam.x = rt2.left;
	childParam.y = rt2.top;
	childParam.width = rt2.Width();
	childParam.height = rt2.Height();
	m_PressButton2.Create( childParam );

	return 0;
}

WND_IMAGE_LIST* SQRSpin::GetWndBkImage( const IMAGE_PARAM& ImageFlag )
{
	//#define SM_SP_P1					0x0002		//按键1
	//#define SM_SP_P2			        0x0003		//按键2

	IMAGE_PARAM Flag = ImageFlag;
	if( SM_SP_P1 == Flag.CtrlMask )
	{
		Flag.CtrlMask -= SM_SP_P1 - SM_BS_BK;
		return m_PressButton1.GetWndBkImage( Flag );
	}
	if( SM_SP_P2 == Flag.CtrlMask )
	{
		Flag.CtrlMask -= SM_SP_P2 - SM_BS_BK;
		return m_PressButton2.GetWndBkImage( Flag );
	}
	return NULL;
}

bool SQRSpin::IsHasEventState(EventStateMask Mask)
{
	return SQRWnd::IsHasEventState(Mask);
}

bool SQRSpin::ObjHasEventState(EventStateMask Mask)
{
	return SQRSpin::IsHasEventState(Mask);
}


uint32 SQRSpin::GetWndTextColor( const IMAGE_PARAM& ImageFlag )
{
	//#define SM_SP_P1					0x0002		//按键1
	//#define SM_SP_P2			        0x0003		//按键2

	IMAGE_PARAM Flag = ImageFlag;
	if( SM_SP_P1 == Flag.CtrlMask )
	{
		Flag.CtrlMask -= SM_SP_P1 - SM_BS_BK;
		return m_PressButton1.GetWndTextColor( Flag );
	}
	if( SM_SP_P2 == Flag.CtrlMask )
	{
		Flag.CtrlMask -= SM_SP_P2 - SM_BS_BK;
		return m_PressButton2.GetWndTextColor( Flag );
	}
	return NULL;
}

uint32 SQRSpin::GetWndTextBcColor( const IMAGE_PARAM& ImageFlag )
{
	//#define SM_SP_P1					0x0002		//按键1
	//#define SM_SP_P2			        0x0003		//按键2

	IMAGE_PARAM Flag = ImageFlag;
	if( SM_SP_P1 == Flag.CtrlMask )
	{
		Flag.CtrlMask -= SM_SP_P1 - SM_BS_BK;
		return m_PressButton1.GetWndTextBcColor( Flag );
	}
	if( SM_SP_P2 == Flag.CtrlMask )
	{
		Flag.CtrlMask -= SM_SP_P2 - SM_BS_BK;
		return m_PressButton2.GetWndTextBcColor( Flag );
	}
	return NULL;
}

void SQRSpin::SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	//#define SM_SP_P1					0x0002		//按键1
	//#define SM_SP_P2			        0x0003		//按键2

	IMAGE_PARAM Flag = ImageFlag;
	if( SM_SP_P1 == Flag.CtrlMask )
	{
		Flag.CtrlMask -= SM_SP_P1 - SM_BS_BK;
		m_PressButton1.SetWndTextColor( Flag, uColor );
	}
	if( SM_SP_P2 == Flag.CtrlMask )
	{
		Flag.CtrlMask -= SM_SP_P2 - SM_BS_BK;
		m_PressButton2.SetWndTextColor( Flag, uColor );
	}
}

void SQRSpin::SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	//#define SM_SP_P1					0x0002		//按键1
	//#define SM_SP_P2			        0x0003		//按键2

	IMAGE_PARAM Flag = ImageFlag;
	if( SM_SP_P1 == Flag.CtrlMask )
	{
		Flag.CtrlMask -= SM_SP_P1 - SM_BS_BK;
		m_PressButton1.SetWndTextBcColor( Flag, uColor );
	}
	if( SM_SP_P2 == Flag.CtrlMask )
	{
		Flag.CtrlMask -= SM_SP_P2 - SM_BS_BK;
		m_PressButton2.SetWndTextBcColor( Flag, uColor );
	}
}

SQRWnd* SQRSpin::GetResWnd( int32 nCtrlMask )
{
	//#define SM_SP_P1					0x0002		//按键1
	//#define SM_SP_P2			        0x0003		//按键2

	if( SM_SP_P1 == nCtrlMask )
	{
		nCtrlMask -= SM_SP_P1 - SM_BS_BK;
		return m_PressButton1.GetResWnd( nCtrlMask );
	}
	if( SM_SP_P2 == nCtrlMask )
	{
		nCtrlMask -= SM_SP_P2 - SM_BS_BK;
		return m_PressButton2.GetResWnd( nCtrlMask );
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

void SQRSpin::OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 )
{
	if( ( pChild == &m_PressButton1 || pChild == &m_PressButton2 ) &&
		( uMsgID == BUTTON_CLICKUP || uMsgID == BUTTON_CLICKCONT ) )
	{
		SendCtrlMsg( SPIN_DELTA, GetStyle()&PS_HSCROLL, pChild == &m_PressButton1 ? -1 : 1 );
	}
}
