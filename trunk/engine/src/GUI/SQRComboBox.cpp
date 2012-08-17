#include "stdafx.h"
#include "SQRComboBox.h"

/////////////////////////////////////////////////////////
// class CListBox
/////////////////////////////////////////////////////////
DYNAMIC_CREATE_WND( SQRComboBox, SQRWnd )

SQRComboBox::SQRComboBox()
{
	
}

SQRComboBox::~SQRComboBox()
{
}

int32	SQRComboBox::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.uStyle |= WS_CHILD;
	int32 re = SQREdit::Create( newParam );
    
	WndCreateParam buttonParam = param;
	buttonParam.szTitle = "";
	buttonParam.uStyle = 0;
	buttonParam.x = param.width;
	buttonParam.y = 0;
	buttonParam.width = param.height;
	buttonParam.pParentWnd = this;
    m_Button.Create( buttonParam );

	WndCreateParam listBoxParam = param;
	listBoxParam.szTitle = "listBox";
	listBoxParam.uStyle = WS_VERSCROLL;
	listBoxParam.x = 0;
	listBoxParam.y = param.height;
	listBoxParam.height = param.height * 5;
	listBoxParam.pParentWnd = this;
	m_ListBox.Create( listBoxParam );
	m_ListBox.ShowWnd( false );
    return 0;
}

void SQRComboBox::OnCreatedAfter()
{
	m_ListBox.InsertColumn( 0, m_ListBox.GetWorkAreaWidth() );
}


void SQRComboBox::DrawWndBackground()
{
    return SQRWnd::DrawWndBackground();
}

int32 SQRComboBox::InsertItem( const char* sText, int32 nItem, float fHeight )
{
    nItem = m_ListBox.InsertItem( nItem, fHeight );
	m_ListBox.GetSubItem( nItem, 0 )->SetWndText( sText );
	return nItem;
}

void SQRComboBox::DrawWndText()
{
	FlushDraw();
	// CEdit::DrawWndText();
	CFRect drawArea = getTextDrawArea();

	// »­ÎÄ×Ö		
	drawTextLines(drawArea);

	// »­¹â±ê
	if ( !( GetStyle() & COMBO_DISEDIT ) )
	{
		if ( IsActive() && ( GetProcessTime() - m_ShowTime )%1000 < 500 )
			drawCarat(drawArea);
	}
}

void SQRComboBox::DrawAfterChildren()
{
	FlushDraw();
	SQREdit::DrawAfterChildren();
}

void SQRComboBox::InsertDefaultColumn()
{
	m_ListBox.InsertColumn( 0, m_ListBox.GetWorkAreaWidth()/GetRootZoomSize() );
}

int32 SQRComboBox::InsertColumn(int32 nCol, float fWidth)
{
	return m_ListBox.InsertColumn( nCol, fWidth );
}

SQRWnd* SQRComboBox::GetItem( int32 nItem )
{
	return m_ListBox.GetSubItem( nItem, 0 );
}

void SQRComboBox::RemoveAllItem()
{
    m_ListBox.DeleteAllItem();
}

void SQRComboBox::RemoveItem( int32 nItem )
{
    m_ListBox.DeleteItem( nItem );
}

void SQRComboBox::RemoveItemByText( const char* sText )
{
	for( int32 i = 0; i < m_ListBox.GetItemCount(); i++ )
	{
		if( m_ListBox.GetSubItem( i, 0 )->GetWndText() == UIString(sText) )
		{
            m_ListBox.DeleteItem( i );
			return;
		}
	}
}

void SQRComboBox::SetItemHeight( float fItemHeight, int32 nItem )
{
	m_ListBox.SetItemHeight( fItemHeight, nItem );
}

void SQRComboBox::SetSel( int32 nItem )
{
	m_ListBox.SelectItem( nItem );
	nItem = m_ListBox.GetSelectItem();
	SetWndText( m_ListBox.GetSubItem( nItem, 0 )->GetWndText() );
}

int32 SQRComboBox::GetCurrentSel()
{
    return m_ListBox.GetSelectItem();
}

const char* SQRComboBox::GetSelText()
{
	int32 nSelItem = m_ListBox.GetSelectItem();
	return nSelItem < 0 ? "" : m_ListBox.GetSubItem( nSelItem, 0 )->GetWndText();
}

const char* SQRComboBox::GetItemText( int32 nItem )
{
	return nItem >= 0 && nItem < m_ListBox.GetItemCount() ? m_ListBox.GetSubItem( nItem, 0 )->GetWndText() : "";
}

//ÏìÓ¦×Ó´°¿Ú
void SQRComboBox::OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 )
{
    switch( uMsgID )
    {
    case ITEM_LBUTTONCLICK:
		if( pChild == &m_ListBox )
        {
            m_ListBox.SelectItem( uParam1 );
            SetWndText( GetSelText() );
			OnSelChanged();
			m_ListBox.ShowWnd( false );

			//ÔÚÑ¡ÔñµÄÊ±ºòÐèÒª·¢ËÍÒ»¸öÏûÏ¢¸ø¸¸´°¿Ú
			SendCtrlMsg( ITEM_LBUTTONCLICK, uParam1, uParam2 );
        }        
		break;

    case BUTTON_LCLICK:
		if( pChild == &m_Button )
            m_ListBox.ShowWnd(!m_ListBox.IsShow());
        break;

    default:
        break;
    };
}

void SQRComboBox::OnActive( bool bActive, SQRWnd* Wnd )
{
    if( !bActive )
        m_ListBox.ShowWnd(false);
}


WND_IMAGE_LIST* SQRComboBox::GetWndBkImage( const IMAGE_PARAM& ImageFlag )
{
//#define SM_CB_BK					0x0001		//Edit±³¾°ÌùÍ¼
//#define SM_CB_BT					0x0002		//Button±³¾°ÌùÍ¼
//#define SM_CB_LB_BK				0x0003		//ListBox±³¾°ÌùÍ¼
//#define SM_CB_SC_BK				0x0004		//±³¾°ÌùÍ¼
//#define SM_CB_SC_P1				0x0005		//°´¼ü1
//#define SM_CB_SC_P2			    0x0006		//°´¼ü2
//#define SM_CB_SC_BAR			    0x0007		//»¬¿é

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_CB_BK )
	{
		Flag.CtrlMask -= SM_CB_BK - SM_BS_BK;
		return SQRWnd::GetWndBkImage( Flag );
	}
	else if( Flag.CtrlMask == SM_CB_BT )
	{
		Flag.CtrlMask -= SM_CB_BT - SM_BS_BK;
		return m_Button.GetWndBkImage( Flag );
	}
	else
	{
		Flag.CtrlMask -= SM_CB_LB_BK - SM_BS_BK;
		return m_ListBox.GetWndBkImage( Flag );
	}
}

bool SQRComboBox::IsHasEventState(EventStateMask Mask)
{
	return SQRButton::IsHasEventState(Mask);
}

bool SQRComboBox::ObjHasEventState(EventStateMask Mask)
{
	return SQRComboBox::IsHasEventState(Mask);
}

uint32 SQRComboBox::GetWndTextColor( const IMAGE_PARAM& ImageFlag )
{
	//#define SM_CB_BK					0x0001		//Edit±³¾°ÌùÍ¼
	//#define SM_CB_BT					0x0002		//Button±³¾°ÌùÍ¼
	//#define SM_CB_LB_BK				0x0003		//ListBox±³¾°ÌùÍ¼
	//#define SM_CB_SC_BK				0x0004		//±³¾°ÌùÍ¼
	//#define SM_CB_SC_P1				0x0005		//°´¼ü1
	//#define SM_CB_SC_P2			    0x0006		//°´¼ü2
	//#define SM_CB_SC_BAR			    0x0007		//»¬¿é

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_CB_BK )
	{
		Flag.CtrlMask -= SM_CB_BK - SM_BS_BK;
		return SQRWnd::GetWndTextColor( Flag );
	}
	else if( Flag.CtrlMask == SM_CB_BT )
	{
		Flag.CtrlMask -= SM_CB_BT - SM_BS_BK;
		return m_Button.GetWndTextColor( Flag );
	}
	else
	{
		Flag.CtrlMask -= SM_CB_LB_BK - SM_BS_BK;
		return m_ListBox.GetWndTextColor( Flag );
	}
}

uint32 SQRComboBox::GetWndTextBcColor( const IMAGE_PARAM& ImageFlag )
{
	//#define SM_CB_BK					0x0001		//Edit±³¾°ÌùÍ¼
	//#define SM_CB_BT					0x0002		//Button±³¾°ÌùÍ¼
	//#define SM_CB_LB_BK				0x0003		//ListBox±³¾°ÌùÍ¼
	//#define SM_CB_SC_BK				0x0004		//±³¾°ÌùÍ¼
	//#define SM_CB_SC_P1				0x0005		//°´¼ü1
	//#define SM_CB_SC_P2			    0x0006		//°´¼ü2
	//#define SM_CB_SC_BAR			    0x0007		//»¬¿é

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_CB_BK )
	{
		Flag.CtrlMask -= SM_CB_BK - SM_BS_BK;
		return SQRWnd::GetWndTextBcColor( Flag );
	}
	else if( Flag.CtrlMask == SM_CB_BT )
	{
		Flag.CtrlMask -= SM_CB_BT - SM_BS_BK;
		return m_Button.GetWndTextBcColor( Flag );
	}
	else
	{
		Flag.CtrlMask -= SM_CB_LB_BK - SM_BS_BK;
		return m_ListBox.GetWndTextBcColor( Flag );
	}
}

void SQRComboBox::SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	//#define SM_CB_BK					0x0001		//Edit±³¾°ÌùÍ¼
	//#define SM_CB_BT					0x0002		//Button±³¾°ÌùÍ¼
	//#define SM_CB_LB_BK				0x0003		//ListBox±³¾°ÌùÍ¼
	//#define SM_CB_SC_BK				0x0004		//±³¾°ÌùÍ¼
	//#define SM_CB_SC_P1				0x0005		//°´¼ü1
	//#define SM_CB_SC_P2			    0x0006		//°´¼ü2
	//#define SM_CB_SC_BAR			    0x0007		//»¬¿é

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_CB_BK )
	{
		Flag.CtrlMask -= SM_CB_BK - SM_BS_BK;
		return SQRWnd::SetWndTextColor( Flag, uColor );
	}
	else if( Flag.CtrlMask == SM_CB_BT )
	{
		Flag.CtrlMask -= SM_CB_BT - SM_BS_BK;
		return m_Button.SetWndTextColor( Flag, uColor );
	}
	else
	{
		Flag.CtrlMask -= SM_CB_LB_BK - SM_BS_BK;
		return m_ListBox.SetWndTextColor( Flag, uColor );
	}
}

void SQRComboBox::SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	//#define SM_CB_BK					0x0001		//Edit±³¾°ÌùÍ¼
	//#define SM_CB_BT					0x0002		//Button±³¾°ÌùÍ¼
	//#define SM_CB_LB_BK				0x0003		//ListBox±³¾°ÌùÍ¼
	//#define SM_CB_SC_BK				0x0004		//±³¾°ÌùÍ¼
	//#define SM_CB_SC_P1				0x0005		//°´¼ü1
	//#define SM_CB_SC_P2			    0x0006		//°´¼ü2
	//#define SM_CB_SC_BAR			    0x0007		//»¬¿é

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_CB_BK )
	{
		Flag.CtrlMask -= SM_CB_BK - SM_BS_BK;
		return SQRWnd::SetWndTextBcColor( Flag, uColor );
	}
	else if( Flag.CtrlMask == SM_CB_BT )
	{
		Flag.CtrlMask -= SM_CB_BT - SM_BS_BK;
		return m_Button.SetWndTextBcColor( Flag, uColor );
	}
	else
	{
		Flag.CtrlMask -= SM_CB_LB_BK - SM_BS_BK;
		return m_ListBox.SetWndTextBcColor( Flag, uColor );
	}
}

SQRWnd* SQRComboBox::GetResWnd( int32 nCtrlMask )
{
	//#define SM_CB_BK					0x0001		//Edit±³¾°ÌùÍ¼
	//#define SM_CB_BT					0x0002		//Button±³¾°ÌùÍ¼
	//#define SM_CB_LB_BK				0x0003		//ListBox±³¾°ÌùÍ¼
	//#define SM_CB_SC_BK				0x0004		//±³¾°ÌùÍ¼
	//#define SM_CB_SC_P1				0x0005		//°´¼ü1
	//#define SM_CB_SC_P2			    0x0006		//°´¼ü2
	//#define SM_CB_SC_BAR			    0x0007		//»¬¿é

	if( nCtrlMask == SM_CB_BK )
	{
		nCtrlMask -= SM_CB_BK - SM_BS_BK;
		return SQRWnd::GetResWnd( nCtrlMask );
	}
	else if( nCtrlMask == SM_CB_BT )
	{
		nCtrlMask -= SM_CB_BT - SM_BS_BK;
		return m_Button.GetResWnd( nCtrlMask );
	}
	else
	{
		nCtrlMask -= SM_CB_LB_BK - SM_BS_BK;
		return m_ListBox.GetResWnd( nCtrlMask );
	}
}

LRESULT SQRComboBox::WndProc( uint32 message, WPARAM wParam, LPARAM lParam )
{
	if ( GetStyle() & COMBO_DISEDIT )
	{
		switch(message)
		{
		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDBLCLK:
			return 0;
		case WM_LBUTTONDOWN:
			m_ListBox.ShowWnd(!m_ListBox.IsShow());
		default:
			return SQRWnd::WndProc( message, wParam, lParam );
		}
	}
	else
		return SQREdit::WndProc( message, wParam, lParam );
}
int32 SQRComboBox::GetItemCount()
{
	return m_ListBox.GetItemCount();
}
