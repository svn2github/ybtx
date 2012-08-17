
#include "stdafx.h"
#include "SQRStatic.h"

DYNAMIC_CREATE_WND( SQRStatic, SQRWnd )


SQRStatic::SQRStatic()
{

}

SQRStatic::~SQRStatic()
{

}

void SQRStatic::DrawText( const wstring& text, uint32 textColor, uint32 textBcColor )
{
	if( (STA_VERTICAL&GetStyle()) == STA_VERTICAL )
	{
		float x = 0, y = 0;
		CVector2f size	= GetFont()->CalculateSize(text.c_str());
		float Width		= size.y;
		float Height	= size.x;

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

		SQRWnd::DrawText( text, x, y, textColor, textBcColor, true, NULL, true);
	}
	else
		SQRWnd::DrawText(text, textColor, textBcColor);
}

