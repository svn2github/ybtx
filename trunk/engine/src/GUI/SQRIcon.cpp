#include "stdafx.h"
#include "SQRIcon.h"

namespace sqr
{
	DYNAMIC_CREATE_WND( SQRIcon, SQRButton )

	SQRIcon::SQRIcon(void)
	{
	}

	SQRIcon::~SQRIcon(void)
	{
		DestroyWnd();
	}
}

void SQRIcon::OnLButtonDblClk( uint32 nFlags, int32 x, int32 y )
{
	uint32 uMsgID = ICON_LBUTTONDBLCLICK;
	SendCtrlMsg( uMsgID, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
}