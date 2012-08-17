#pragma once
#include "SQRControl.h"

namespace sqr
{
	class CWebBrowser;
	class CWebBrowserATL;
	class GUI_API SQRWebBrowser : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRWebBrowser )
	public:
		SQRWebBrowser(void);
		~SQRWebBrowser(void);

		virtual int32	Create(const WndCreateParam& param );
		virtual void	DrawWndBackground();
		virtual void	SetszSourceName( const char* szDesc );
		virtual void    OnProcess();
		virtual void	OnWndMoved( float dx, float dy );
		virtual void	OnScreenPosChanged();
		virtual void	ShowWnd( bool bShow );

		//本窗口处理消息的函数
		virtual LRESULT WndProc( uint32 message, WPARAM wParam, LPARAM lParam );
		char*	GetSrcCode();

	private:
		CWebBrowser*	m_WebWin;
		CWebBrowserATL* m_WebWinAtl;
		bool			m_IsATL;
		bool			m_Show;
	};

//!namespace
}
