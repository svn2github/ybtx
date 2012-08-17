#pragma once
#include "SQRControl.h"

namespace sqr
{
	class CFlashPlayer;
	class GUI_API SQRFlashWnd : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRFlashWnd )
	public:
		SQRFlashWnd();  //主窗口句柄
		~SQRFlashWnd(void);

		virtual int32	Create(const WndCreateParam& param );
		virtual void	DrawWndBackground();
		virtual void	SetszSourceName( const char* szDesc );
		virtual void    SetStyle( uint Style );
		virtual void	ResizeRect( const CFRect& rt ,bool refreshbak = true);
		virtual void    OnProcess();
		
		void PlayFlash();
		void StopFlash();
		void UpdateFlash();

		//左键双击消息
		 void	OnLButtonDblClk( uint32 nFlags, int32 x, int32 y );
		//左键点击消息
		 void	OnLButtonDown( uint32 nFlags, int32 x, int32 y ); 
		//左键释放消息
		 void	OnLButtonUp( uint32 nFlags, int32 x, int32 y ); 
		//一次鼠标左键按下抬起操作
		 void	OnLButtonClick( uint32 nFlags, int32 x, int32 y ); 
		//中键双击消息
		 void	OnMButtonDblClk( uint32 nFlags, int32 x, int32 y );
		//中键点击消息
		 void	OnMButtonDown( uint32 nFlags, int32 x, int32 y );
		//中键弹起消息
		 void	OnMButtonUp( uint32 nFlags, int32 x, int32 y );
		//鼠标移动消息
		 void	OnMouseMove( uint32 nFlags, int32 x, int32 y );
		//鼠标滚轮消息
		 void	OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y );
		//右键双击消息
		 void	OnRButtonDblClk( uint32 nFlags, int32 x, int32 y );
		//右键点击消息
		 void	OnRButtonDown( uint32 nFlags, int32 x, int32 y );
		//右键释放消息
		 void	OnRButtonUp( uint32 nFlags, int32 x, int32 y ); 
		//一次鼠标右键按下抬起操作
		 void	OnRButtonClick( uint32 nFlags, int32 x, int32 y );

		//本窗口处理消息的函数
		virtual LRESULT WndProc( uint32 message, WPARAM wParam, LPARAM lParam );

	private:
		void*			m_WndHandle;
		CFlashPlayer*	m_FlashPlayer;
	};

}