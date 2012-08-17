//////////////////////////////////////////////////////////////////////////////////
// UtilCtrl.cpp : 小控件

#include "stdafx.h"
#include "UtilCtrl.h"
#include "Resource.h"

//一下代码段用于查找内存泄露
#if defined(DEBUG_NEW)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

map<float,DWORD> CtrlCV;
/////////////////////////////////////////////////////////////////////////

LRESULT CCtrlWnd::SendCtrlNotifyMsg( DWORD Notify, UINT message, WPARAM wParam, LPARAM lParam )
{
	NotifyMessage NotifyMsg;
	NotifyMsg.hrd.hwndFrom = m_hWnd;
	NotifyMsg.hrd.idFrom = GetDlgCtrlID();
	NotifyMsg.hrd.code = Notify;
	NotifyMsg.pWnd = this;
	NotifyMsg.Message = message;
	NotifyMsg.wParam = wParam;
	NotifyMsg.lParam = lParam;
	return GetParent()->SendMessage( WM_NOTIFY, NotifyMsg.hrd.idFrom, (LPARAM) (LPNMHDR)&NotifyMsg ); 
}

LRESULT CCtrlWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	//将鼠标信息传给父窗口
	if( message >= WM_MOUSEFIRST && message <= WM_MOUSELAST || message == WM_SETCURSOR )
	{
		LRESULT Re = SendCtrlNotifyMsg( NM_CHILDETOPARENT, message, wParam, lParam ); 
		if( !m_IsNeedMsg )
			return Re;
	}
	if( WM_LBUTTONDOWN < message && WM_MBUTTONDBLCLK > message )
		SetFocus();

	return CStatic::WindowProc( message, wParam, lParam );
}


// CFrameCtrl

#define		SLIDERHEIGHT 18
#define		SLIDERLENGHT 100
#define		GRIDSHORT    8
#define		GRIDLONG     18

IMPLEMENT_DYNAMIC(CFrameCtrl, CWnd)
BEGIN_MESSAGE_MAP(CFrameCtrl, CWnd)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CFrameCtrl::CFrameCtrl():CCtrlWnd( TRUE )
{
	m_FrameStart = 0;
	m_FrameEnd = 100;
	m_FrameCur = 0;
	m_Font.CreateFont( 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	m_State = enumPS_STOP;
}

void CFrameCtrl::OnPaint()
{
	CalFramePerGrid();
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	CRect ClienRect,SliderRect,InfoRect;
	GetClientRect( &ClienRect );
	dc.FillSolidRect( &ClienRect, 0x80c0c0c0 );

	SliderRect = ClienRect;
	SliderRect.bottom = SliderRect.top + SLIDERHEIGHT;
	dc.FillSolidRect( &SliderRect, 0xa0a0a0a0 );
	OnDraw( &dc );
}

void CFrameCtrl::OnDraw( CDC* pDC )
{
	//	CCtrlWnd::OnDraw( pDC );

	CRect ClienRect,SliderRect,InfoRect;
	GetClientRect( &ClienRect );

	static CRect Erase( 0, 0, SLIDERLENGHT, 0 );
	Erase.right++;
	pDC->FillSolidRect( &Erase, 0xa0a0a0a0 );
	Erase.left += SLIDERLENGHT/2 - 10;
	Erase.right -= SLIDERLENGHT/2 - 10;
	Erase.top = Erase.bottom;
	Erase.bottom = ClienRect.bottom;
	pDC->FillSolidRect( &Erase, 0x80c0c0c0 );

	SliderRect = ClienRect;
	SliderRect.bottom = SliderRect.top + SLIDERHEIGHT;
	pDC->Draw3dRect( &ClienRect, 0xcc000000, 0xcc000000 );
	pDC->Draw3dRect( &SliderRect, 0xcc000000, 0xcc000000 );

	//画关键帧
	float PixPerFrame = (float)m_PixPerGrid/m_FramesPerGrid;
	for( map< int, int >::iterator it = m_KeyFrame.begin(); it != m_KeyFrame.end(); ++it )
	{
		CRect rt( (int)( it->first*PixPerFrame ) + SLIDERLENGHT/2, SliderRect.bottom, 
			(int)( ( it->first + 1 )*PixPerFrame ) + SLIDERLENGHT/2, SliderRect.bottom + GRIDLONG);
		pDC->FillSolidRect( &rt, 0x8000ff00 );
	}

	//画刻度
	for( int i = 0; i < ( ClienRect.Width() - SLIDERLENGHT )/m_PixPerGrid + 1; i++ )
	{
		int Length = GRIDSHORT;
		if( i%10 == 0 )
			Length = GRIDLONG;
		pDC->MoveTo( i*m_PixPerGrid + SLIDERLENGHT/2, SliderRect.bottom );
		pDC->LineTo( i*m_PixPerGrid + SLIDERLENGHT/2, SliderRect.bottom + Length );
	}

	//标示帧号
	pDC->SetBkMode( TRANSPARENT );
	pDC->SelectObject( &m_Font );
	for( int i = 0; i < ( ClienRect.Width() - SLIDERLENGHT )/m_PixPerGrid + 1; i++ )
	{
		if( i%10 == 0  )
		{
			char Buf[32];
			sprintf( Buf, "%d", i*m_FramesPerGrid );
			pDC->TextOut( i*m_PixPerGrid + SLIDERLENGHT/2+2, SliderRect.bottom + 5, Buf );
		}
	}

	//画指针
	int PosX = (long)( ( m_FrameCur / m_FramesPerGrid )*m_PixPerGrid ) + SLIDERLENGHT/2;
	int PosY = SliderRect.bottom-1;
	pDC->Draw3dRect( &CRect( PosX, PosY, PosX + 1, PosY + SLIDERHEIGHT - 6 ), 0x0000ff, 0x0000ff );

	CRect PoitRect = SliderRect;
	PoitRect.left   = PosX - 3;
	PoitRect.right  = PoitRect.left + 7;
	PoitRect.top   += SLIDERHEIGHT - 2;
	PoitRect.bottom = PoitRect.top + 16;
	pDC->Draw3dRect( &PoitRect, 0xfefefe, 0xfefefe );

	//画滑块
	CRect BlockRect = SliderRect;
	BlockRect.left = (long)( ( m_FrameCur / m_FramesPerGrid )*m_PixPerGrid ) + 1;
	BlockRect.right = BlockRect.left + SLIDERLENGHT-2;
	BlockRect.top++;
	BlockRect.bottom--;
	Erase = BlockRect;
	pDC->FillSolidRect( &BlockRect, 0xaaaaaa );
	pDC->Draw3dRect( &BlockRect, 0xCED3D6, 0x424242 );
	BlockRect.left++;
	BlockRect.right--;
	BlockRect.top++;
	BlockRect.bottom--;
	pDC->Draw3dRect( &BlockRect, 0xCED3D6, 0x848484 );
	char Buf[32];
	sprintf( Buf, "%d/%d", (int)m_FrameCur, m_FrameEnd - m_FrameStart );
	pDC->TextOut( BlockRect.left + 3 +(int)( ( BlockRect.Width() - strlen( Buf )*6.5f )/2 ), BlockRect.top, Buf ); 
}

void CFrameCtrl::CalFramePerGrid()
{
	CRect ClienRect;
	GetClientRect( &ClienRect );

	int FrameNum = m_FrameEnd - m_FrameStart + 1;

	//计算合适的每帧象素
	int FramesPerGrid[3] = { 1, 2, 5 };
	int Start = 1;
	int n = 0;
	m_PixPerGrid = 0;
	while( 1 )
	{
		m_PixPerGrid = ( ClienRect.Width() - SLIDERLENGHT )/( FrameNum/( FramesPerGrid[n%3]*Start ) );
		if( m_PixPerGrid > 5 )
			break;
		n++;
		if( n == 3 )
		{
			Start *= 10; 
			n = 0;
		}
	}
	m_FramesPerGrid = FramesPerGrid[n%3]*Start;
}

void CFrameCtrl::PreSubclassWindow()
{
	SetTimer( 0, 20, NULL );
}

void CFrameCtrl::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	//当鼠标移出客户区继续跟踪鼠标
	if( m_BeginDrag && DragDetect( CPoint( GetSystemMetrics( SM_CXSCREEN ), 100 ) ) )
	{
		POINT point;
		::GetCursorPos( &point );
		ScreenToClient( &point );
		CRect ClienRect;
		GetClientRect( &ClienRect );
		ClienRect.bottom = ClienRect.top + GRIDLONG + 16;

		m_FrameCur = ( ( point.x - SLIDERLENGHT/2.0f )/m_PixPerGrid ) * m_FramesPerGrid;
		if( m_FrameCur < 0 )
			m_FrameCur = 0;
		if( m_FrameCur > (float)m_FrameEnd )
			m_FrameCur = (float)m_FrameEnd;
		m_IsInDrag = TRUE;
		CDC* pDC = GetDC();
		OnDraw( pDC );
		ReleaseDC( pDC );
	}
	else
	{
		m_IsInDrag = FALSE;
		m_BeginDrag = FALSE;
	}

	CWnd::OnTimer(nIDEvent);
}

void CFrameCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect ClienRect;
	GetClientRect( &ClienRect );
	ClienRect.bottom = ClienRect.top + GRIDLONG + 16;
	if( point.y < 16 )
	{
		m_FrameCur = ( ( point.x - SLIDERLENGHT/2.0f )/m_PixPerGrid ) * m_FramesPerGrid;
		if( m_FrameCur < 0 )
			m_FrameCur = 0;
		if( m_FrameCur > (float)m_FrameEnd )
			m_FrameCur = (float)m_FrameEnd;
		CDC* pDC = GetDC();
		OnDraw( pDC );
		ReleaseDC( pDC );
		m_BeginDrag = TRUE;
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CFrameCtrl::SetRange( int FrameEnd, int FrameStart )
{ 
	if( m_FrameEnd != FrameEnd || m_FrameStart != FrameStart )
	{
		m_FrameEnd = FrameEnd; 
		m_FrameStart = FrameStart;
		CalFramePerGrid();
		Invalidate();
	}
}

float CFrameCtrl::SetFrameNum( float Frame )
{ 
	if( !m_IsInDrag && m_State == enumPS_PLAY  )
	{
		static float CurFrame = -1;
		m_FrameCur = Frame; 

		if( abs( CurFrame - Frame ) > 1 )
		{
			CDC* pDC = GetDC();
			OnDraw( pDC );
			ReleaseDC( pDC );
			CurFrame = Frame;
		}
	}

	return m_FrameCur;
}


// CColorBlock

IMPLEMENT_DYNAMIC(CColorBlock, CWnd)
BEGIN_MESSAGE_MAP(CColorBlock, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CColorBlock message handlers
void CColorBlock::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CCtrlWnd::OnPaint() for painting messages
	OnDraw( &dc );
}

void CColorBlock::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	m_Color = map<float,DWORD>();
	SetColorList( m_Color );
	CCtrlWnd::PreSubclassWindow();
}

//设置指定百分比的颜色
void CColorBlock::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default	
	SetCurPos( point );
	CCtrlWnd::OnLButtonDown(nFlags, point);
}

void CColorBlock::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect Rect;
	GetClientRect( &Rect );

	float Percent = ( point.x - 3 )/( (float)Rect.Width() - 6 );
	CColorDialog Dlg( GetGDIColor( Percent ), CC_FULLOPEN );
	if( Dlg.DoModal() == IDCANCEL )
		return;
	DWORD Color = Dlg.GetColor();
	char* CBuf  = (char*)&Color;
	char temp = CBuf[0];
	CBuf[0] = CBuf[2];
	CBuf[2] = temp;
	m_Color[ m_CurColorPercent ] = ( Color&0xffffff )|GetAlpha( Percent );
	ResetPic();

	Invalidate();

	CCtrlWnd::OnLButtonDblClk(nFlags, point);
}

void CColorBlock::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if( nFlags&MK_LBUTTON )
	{
		CRect Rect;
		GetClientRect( &Rect );

		DWORD Color = GetColor( m_CurColorPercent );
		if( m_CurColorPercent > 0.01f && m_CurColorPercent < 0.99f && !( GetKeyState( VK_CONTROL )&0x8000 ) )
		{
			m_Color.erase( m_CurColorPercent );
			m_CurColorPercent = ( point.x - 3 )/( (float)Rect.Width() - 6 );
		}
		if( m_CurColorPercent <= 0.01f )
			m_CurColorPercent = 0.0f;
		if( m_CurColorPercent >= 0.99f )
			m_CurColorPercent = 1.0f;

		int Alpha = 255 - (int)( ( point.y - 3.0f )*255/( Rect.Height() - 6 ) );
		Alpha = max( Alpha, 0 );
		Alpha = min( Alpha, 255 );
		m_Color[m_CurColorPercent] = ( Color&0xffffff )|( Alpha<<24 );
		ResetPic();

		CDC* pDC = GetDC();
		OnDraw( pDC );
		ReleaseDC( pDC );
	}

	CCtrlWnd::OnMouseMove(nFlags, point);
}

void CColorBlock::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if( GetKeyState(VK_CONTROL)&0x8000 )
	{
		if( nChar == 'C' )
			GetColorList( CtrlCV );
		else if( nChar == 'V' )
		{
			m_Color = CtrlCV;
			SetColorList( m_Color );
		}
	}

	if( nChar == VK_DELETE )
	{		
		if( m_CurColorPercent > 0.01f && m_CurColorPercent < 0.99f )
			m_Color.erase( m_CurColorPercent );
		ResetPic();

		CDC* pDC = GetDC();
		OnDraw( pDC );
		ReleaseDC( pDC );
	}

	CCtrlWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CColorBlock::OnDraw( CDC* pDC )
{
//	CCtrlWnd::OnDraw( pDC );

	CRect Rect;
	GetClientRect( &Rect );

	CDC MemDC;
	MemDC.CreateCompatibleDC( pDC );
	MemDC.SelectObject( &m_Bitmap );
	pDC->BitBlt( 0, 0, Rect.Width(), Rect.Height(), &MemDC, 0, 0, SRCCOPY );

	DWORD Color;
	CGdiObject* pOldObj = pDC->SelectObject( &m_Font );
	pDC->SetBkMode( TRANSPARENT );

	for( map<float,DWORD>::iterator it = m_Color.begin(); it != m_Color.end(); ++it )
	{
		DWORD Alpha = it->second>>24;
		int PosX = (int)( (*it).first*( Rect.Width() - 6 ) + 3 );
		int PosY = (int)( ( Rect.Height() - 6 )*( 255.0f - Alpha )/255 + 3 );

		if( it == m_Color.begin() )
			pDC->MoveTo( PosX, PosY );
		else
		{
			CPen Pen( PS_SOLID, 1, Color );
			CGdiObject* OldObj = pDC->SelectObject( &Pen );
			pDC->LineTo( PosX, PosY );
			pDC->SelectObject( OldObj );
		}

		Color = GetGDIColor( (*it).first );
		Color =  ~Color;
		Color &= 0xffffff;
		Color |= 0xcc000000;
		if( (*it).first == m_CurColorPercent )
			pDC->FillSolidRect( PosX-3, PosY-3, 6, 6, Color );
		else
			pDC->Draw3dRect( PosX-3, PosY-3, 6, 6, Color, Color );
		CRect rt = Rect;
		if( PosX > Rect.right - 20 )
			rt.right = PosX;
		else
			rt.left = PosX;
		rt.top  = PosY;
		if( rt.top > Rect.bottom - 14 )
			rt.top -= 14;
		char Buf[256];
		sprintf( Buf, "%d", Alpha );
		pDC->SetTextColor( Color );
		if( PosX > Rect.right - 20 )
			pDC->DrawText( Buf, &rt, DT_RIGHT );
		else
			pDC->DrawText( Buf, &rt, DT_LEFT );
	}
	pDC->SelectObject( pOldObj );
}

void  CColorBlock::SetColorList( map<float,DWORD>& Color )
{
	if( &Color != &m_Color )
	{
		Update();
		m_EditColor = &Color;
	}

	m_Color = Color;
	if( m_Color.size() < 2 )
	{
		m_Color.clear();
		m_Color[0.0f] = 0;
		m_Color[1.0f] = 0xffffffff;
	}

	if( m_Color.find( 1.0 ) == m_Color.end() )
	{
		m_Color[1.0f] = 0xffffffff;
	}

	CRect Rect;
	GetClientRect( &Rect );

	BITMAPINFO bmi;
	ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth		= Rect.Width();
	bmi.bmiHeader.biHeight		= Rect.Height();
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biBitCount	= 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	if( !m_MemDC.GetSafeHdc() )
		m_MemDC.CreateCompatibleDC( NULL );
	HBITMAP hBmp = CreateDIBSection( m_MemDC.m_hDC, &bmi, 
		DIB_RGB_COLORS, (void **)&m_pBit, NULL, 0 );

	m_Bitmap.DeleteObject();
	m_Bitmap.Attach( hBmp );
	ResetPic();
	CDC* pDC = GetDC();
	OnDraw( pDC );
	ReleaseDC( pDC );
};

void  CColorBlock::GetColorList( map<float,DWORD>& Color )
{
	Color = m_Color;
};

//填写颜色缓冲区
void CColorBlock::ResetPic()
{
	if( m_Color.size() >= 2 )
	{
		CRect Rect;
		GetClientRect( &Rect );
		map<float,DWORD>::iterator itend = m_Color.end();
		itend--;

		for( int j = 0; j < 3; j++ )
		{
			for( int i = 0; i< Rect.Height(); i++ )
			{
				BYTE* Pix;
				BYTE* SC;

				Pix = (BYTE*)&m_pBit[ i*Rect.Width() + j ];
				SC  = (BYTE*)&( m_Color.begin()->second );
				for( int k = 0; k < 4; k ++ )
					Pix[k] = SC[k]; 

				Pix = (BYTE*)&m_pBit[ i*Rect.Width() + Rect.Width() - j - 1 ];
				SC  = (BYTE*)&( itend->second );
				for( int k = 0; k < 4; k ++ )
					Pix[k] = SC[k]; 
			}
		}

		for( map<float,DWORD>::iterator it = m_Color.begin(); it != itend; ++it )
		{
			map<float,DWORD>::iterator Next = it;
			Next++;
			int Start = (int)( (*it).first*Rect.Width() );
			int End   = (int)( (*Next).first*Rect.Width() );
			BYTE* SC  = (BYTE*)&((*it).second);
			BYTE* EC  = (BYTE*)&((*Next).second);
			for( int j = Start; j < End; j++ )
			{
				float W = ( (float)End - j )/( End - Start );
				for( int i = 0; i< Rect.Height(); i++ )
				{
					BYTE* Pix = (BYTE*)&m_pBit[ i*Rect.Width() + j ];
					for( int k = 0; k < 4; k ++ )
						Pix[k] = (BYTE)( min( SC[k]*W + EC[k]*( 1 - W ), 255 ) ); 
				}
			}
		}
	}
}

//得到指定百分比的颜色
DWORD CColorBlock::GetGDIColor( float Percent )
{
	map<float,DWORD>::iterator it = m_Color.end();
	it--;
	DWORD Color = GetColor( Percent );
	char* CBuf  = (char*)&Color;
	char temp = CBuf[0];
	CBuf[0] = CBuf[2];
	CBuf[2] = temp;

	return Color;
}

DWORD CColorBlock::GetColor( float Percent )
{
	map<float,DWORD>::iterator it = m_Color.end();
	it--;
	DWORD Color = (*it).second;
	for( it = m_Color.begin(); it != m_Color.end(); ++it )
	{
		if( Percent < it->first )
		{
			map<float,DWORD>::iterator pre = it;
			pre--;
			BYTE* SC  = (BYTE*)&((*pre).second);
			BYTE* EC  = (BYTE*)&((*it).second);
			float W = ( it->first - Percent )/( it->first - pre->first );

			BYTE* Pix = (BYTE*)&Color;
			for( int i = 0; i<4; i++ )
				Pix[i] = (BYTE)( min( SC[i]*W + EC[i]*( 1 - W ), 255 ) ); 
			break;
		}
	}

	return Color;
}

DWORD CColorBlock::GetAlpha( float Percent )
{
	map<float,DWORD>::iterator it = m_Color.end();
	it--;
	DWORD Alpha = (*it).second;
	for( it = m_Color.begin(); it != m_Color.end(); ++it )
	{
		if( Percent < it->first )
		{
			map<float,DWORD>::iterator pre = it;
			pre--;
			float W = ( it->first - Percent )/( it->first - pre->first );

			Alpha = (DWORD)( ( (*pre).second&0xff000000 )*W + ( (*it).second&0xff000000 )*( 1 - W ) ); 
			break;
		}
	}

	return Alpha&0xff000000;
}

void CColorBlock::SetCurPos( CPoint& point )
{
	CRect Rect;
	GetClientRect( &Rect );

	m_ColorValid = TRUE;
	m_CurColorPercent = ( point.x - 3 )/( (float)Rect.Width() - 6 );
	if( m_CurColorPercent < 0.01f )
		m_CurColorPercent = 0.0f;
	if( m_CurColorPercent > 0.99f )
		m_CurColorPercent = 1.0f;
	for( map<float,DWORD>::iterator it = m_Color.begin(); it != m_Color.end(); ++it )
	{
		if( abs( it->first - m_CurColorPercent ) < 3.1f/( (float)Rect.Width() - 6 ) )
		{
			m_CurColorPercent = it->first;
			break;
		}
	}

	Invalidate();
}


// CBlockCombo

IMPLEMENT_DYNAMIC(CBlockCombo, CComboBox)
BEGIN_MESSAGE_MAP(CBlockCombo, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnCbnSelendok)
END_MESSAGE_MAP()

void CBlockCombo::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	SetItemHeight( -1, 14 );

	CComboBox::PreSubclassWindow();
}

void CBlockCombo::OnCbnSelendok()
{
	// TODO: Add your control notification handler code here
	if( m_BlockEdit )
	{
		int Index = GetCurSel();
		if( Index >= 0 )
            m_BlockEdit->SetCurState( (DWORD)GetItemData( Index ) );
	}
}

void CBlockCombo::SetCurSelByDataValue( DWORD Data )
{
	for( int i = 0; i < GetCount(); i++ )
	{
		if( GetItemData( i ) == Data )
		{
			SetCurSel( i );
			if( m_BlockEdit )
				m_BlockEdit->SetCurState( Data );
			return;
		}
	}
	SetCurSel(0);
	if( m_BlockEdit )
        m_BlockEdit->SetCurState( (DWORD)GetItemData( 0 ) );
}

// CCompBlock

IMPLEMENT_DYNAMIC(CCompBlock, CWnd)
BEGIN_MESSAGE_MAP(CCompBlock, CWnd)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CCompBlock message handlers

void CCompBlock::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	m_Font.CreateFont( 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );

	m_State = map<float,DWORD>();
	SetStateList( m_State, NULL );
	SetRange( 100 );
	m_BlockCursor = ::LoadCursor( NULL, IDC_SIZEWE );

	CCtrlWnd::PreSubclassWindow();
}

void CCompBlock::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CCtrlWnd::OnPaint() for painting messages
	OnDraw( &dc );
}

void CCompBlock::OnDraw( CDC* pDC )
{
//	CCtrlWnd::OnDraw( pDC );

	CRect ClienRect;
	for( map<float,DWORD>::iterator it = m_State.begin(); it != m_State.end(); ++it )
	{
		map<float,DWORD>::iterator next = it;
		next++;
		GetClientRect( &ClienRect );
		int Width = ClienRect.Width();
		ClienRect.left = (int)( (*it).first*Width );
		if( next != m_State.end() )
			ClienRect.right = (int)( (*next).first*Width );
		pDC->FillSolidRect( &ClienRect, m_CurPercent == (*it).first ? 0xcc808080 : 0xcccccccc );
		pDC->Draw3dRect( &ClienRect, 0xccffffff, 0xcc222222 );
		ClienRect.bottom--;
		ClienRect.right--;
		pDC->Draw3dRect( &ClienRect, 0xccffffff, 0xcc505050 );

		CString str;
		str.Format( "%.1f", (*it).first*m_Range );
		ClienRect.left += 3;
		ClienRect.top += 1;
		pDC->SelectObject( &m_Font );
		pDC->SetBkMode( TRANSPARENT );
		pDC->DrawText( str, &ClienRect, DT_LEFT );
	}
}

void  CCompBlock::SetStateList( map<float,DWORD>& State, CBlockCombo* StateOption )
{
	if( &State != &m_State )
	{
		Update();
		m_BlockOption = StateOption;
		m_EditState = &State;
		if( m_BlockOption )
			m_BlockOption->SetCompBlock( this );
	}

	m_State = State;
	if( m_State.size() < 1 )
	{
		m_State.clear();
		m_State[0.0f] = 0;
	}

	if( m_BlockOption )
	{
		if( m_State.find( m_CurPercent ) == m_State.end() )
			m_CurPercent = 0.0f;
		m_BlockOption->SetCurSelByDataValue( m_State[m_CurPercent] );
	}

	CDC* pDC = GetDC();
	OnDraw( pDC );
	ReleaseDC( pDC );
}

void CCompBlock::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect Rect;
	GetClientRect( &Rect );

	m_CurPercent = point.x/(float)Rect.Width();

	m_PreMoveBlock = -1;
	for( map<float,DWORD>::iterator it = m_State.begin(); it != m_State.end(); ++it )
	{
		if( abs( m_CurPercent - it->first )*Rect.Width() < 4.0f )
		{
			if( it->first > 0.01f && it->first < 0.99f )
				m_PreMoveBlock = it->first;
			::SetCursor( m_BlockCursor );
			m_SetBlockCur = TRUE;
			break;
		}
	}

	if( m_CurPercent < 0.01f )
		m_CurPercent = 0.0f;
	if( m_CurPercent > 0.99f )
		m_CurPercent = 1.0f;
	for( map<float,DWORD>::iterator it = m_State.begin(); it != m_State.end(); ++it )
		if( m_CurPercent < it->first )
			break;
	it--;
	m_CurPercent = it->first;
	if( m_BlockOption )
		m_BlockOption->SetCurSelByDataValue( it->second );

	CDC* pDC = GetDC();
	OnDraw( pDC );
	ReleaseDC( pDC );

	CCtrlWnd::OnLButtonDown(nFlags, point);
}

void CCompBlock::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect Rect;
	GetClientRect( &Rect );

	m_CurPercent = point.x/(float)Rect.Width();
	m_State[m_CurPercent] = 0;
	CDC* pDC = GetDC();
	OnDraw( pDC );
	ReleaseDC( pDC );

	CCtrlWnd::OnLButtonDblClk(nFlags, point);
}

void CCompBlock::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect Rect;
	GetClientRect( &Rect );

	float CurPercent = point.x/(float)Rect.Width();
	if( CurPercent < 0.01f )
		CurPercent = 0.0f;
	if( CurPercent > 0.99f )
		CurPercent = 1.0f;

	m_SetBlockCur = FALSE;

	if( m_PreMoveBlock > 0.01f && nFlags&MK_LBUTTON )
	{
		DWORD State = m_State[m_PreMoveBlock];
		m_State.erase( m_PreMoveBlock );
		m_State[CurPercent] = State;
		m_PreMoveBlock = CurPercent;
		m_CurPercent = m_PreMoveBlock;
		if( m_BlockOption )
			m_BlockOption->SetCurSelByDataValue( m_State[m_CurPercent] );

		CDC* pDC = GetDC();
		OnDraw( pDC );
		ReleaseDC( pDC );
		m_SetBlockCur = TRUE;
	}
	else
	{
		m_PreMoveBlock = -1;
		for( map<float,DWORD>::iterator it = m_State.begin(); it != m_State.end(); ++it )
		{
			if( abs( CurPercent - it->first )*Rect.Width() < 4.0f )
			{
				if( nFlags&MK_LBUTTON )
				{
					if( it->first > 0.01f && it->first < 0.99f )
					{
						DWORD State = it->second;
						m_State.erase( it );
						m_State[CurPercent] = State;
						m_PreMoveBlock = CurPercent;
						m_CurPercent = m_PreMoveBlock;

						CDC* pDC = GetDC();
						OnDraw( pDC );
						ReleaseDC( pDC );
					}
				}
				m_SetBlockCur = TRUE;
				break;
			}
		}
	}

	CCtrlWnd::OnMouseMove(nFlags, point);
}

BOOL CCompBlock::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if( m_SetBlockCur )
	{
		::SetCursor( m_BlockCursor );
		return 1;
	}
	return CCtrlWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CCompBlock::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if( GetKeyState(VK_CONTROL)&0x8000 )
	{
		if( nChar == 'C' )
			CtrlCV = m_State;
		else if( nChar == 'V' )
		{
			m_State = CtrlCV;
			SetStateList( m_State, NULL );
		}
	}

	if( nChar == VK_DELETE )
	{
		if( m_CurPercent > 0.01f && m_CurPercent < 0.99f )
			m_State.erase( m_CurPercent );

		for( map<float,DWORD>::iterator it = m_State.begin(); it != m_State.end(); ++it )
			if( m_CurPercent < it->first )
				break;
		it--;
		m_CurPercent = it->first;

		CDC* pDC = GetDC();
		OnDraw( pDC );
		ReleaseDC( pDC );
	}

	CCtrlWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


// CMsgWnd

IMPLEMENT_DYNAMIC(CMsgWnd, CWnd)
BEGIN_MESSAGE_MAP(CMsgWnd, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CMsgWnd message handlers

void CMsgWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	CFont Font;
	Font.CreateFont( 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "宋体" );
	dc.SelectObject( &Font );
	CString str;
	CRect rt;
	GetWindowText( str );
	GetClientRect( &rt );
	dc.FillSolidRect( rt, 0xccccffff );
	dc.DrawText( str, &rt, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
	dc.Draw3dRect( rt, 0, 0 );
}

// CFileDialog

IMPLEMENT_DYNAMIC(CFileDialog, CFileDialog)
CFileDialog::CFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
							   DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}
BEGIN_MESSAGE_MAP(CFileDialog, CFileDialog)
END_MESSAGE_MAP()

// CFileDialog message handlers

INT_PTR CFileDialog::DoModal( CString& RootPath )
{
	// TODO: Add your specialized code here and/or call the base class
	INT_PTR re = CFileDialog::DoModal();
	if( re == IDOK )
		RootPath = m_FloderName;
	
	return re;
}

void CFileDialog::OnFolderChange()
{
	// TODO: Add your specialized code here and/or call the base class
	m_FloderName = GetFolderPath();

	CFileDialog::OnFolderChange();
}
