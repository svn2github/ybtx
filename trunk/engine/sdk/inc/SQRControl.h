#pragma once
#include "SQRWnd.h"

namespace sqr
{
	class GUI_API SQRControl : public SQRWnd
	{
		DYNAMIC_DECLARE_WND( SQRControl )
	public:
		SQRControl(void);
		virtual ~SQRControl(void);
		//本窗口处理消息的函数
		virtual LRESULT WndProc( uint32 message, WPARAM wParam, LPARAM lParam );
		//发送消息到父窗口
		LRESULT	SendCtrlMsg( uint32 uMsgID, uint32 uParam1, uint32 uParam2 );
	};
}