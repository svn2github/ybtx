#pragma once
#include "SQRControl.h"

namespace sqr
{

	class GUI_API SQRButton : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRButton )
	protected:
		WND_IMAGE_LIST	  	m_MouseOverImage;
		WND_IMAGE_LIST		m_ClickDownImage;


		uint32	  			m_MouseOverColor;
		uint32				m_ClickDownColor;

		uint32	  			m_MouseOverBcColor;
		uint32				m_ClickDownBcColor;

		uint32				m_uFlashCircle;
		uint32				m_uFlashStartTime;
		uint32				m_uFlashEndTime;
		bool				m_bFirstMouseOver;
	private:
		float				m_fTextOffsetX;
		float				m_fTextOffsetY;

	public:
		SQRButton(void);
		virtual ~SQRButton(void);

		virtual int32			DestroyWnd();
		virtual int32	    	Create( const WndCreateParam& param );

		virtual WND_IMAGE_LIST*	GetWndBkImage( const IMAGE_PARAM& ImageFlag );

		virtual uint32			GetWndTextColor( const IMAGE_PARAM& ImageFlag );
		virtual void			SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );

		virtual uint32			GetWndTextBcColor( const IMAGE_PARAM& ImageFlag );
		virtual void			SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );

		virtual void			SetFlashTime( uint32 uFlashCircle, int32 nFlashTime = -1 );

		virtual void			DrawWndBackground();

		virtual void			DrawWndText();
		virtual void			DrawText( const wstring& text, uint32 textColor, uint32 textBcColor );
		virtual void			DrawText( const wstring& text, float x, float y, CColor color, CColor bkColor, bool multiline, CFRect* rect );

		static  bool			IsHasEventState(EventStateMask Mask);
		void					SetBackImageColor(uint32 uColor);
		virtual bool			ObjHasEventState(EventStateMask Mask);

		virtual void			ReleaseWndTex();		// 节省资源模式下，窗口关闭使用
		virtual void			CreatWndTex();			// 节省资源模式下，窗口打开使用

		void					SetTextOffsetX(float x);
		float					GetTextOffsetX();
		void					SetTextOffsetY(float y);
		float					GetTextOffsetY();

	protected:
		virtual void			OnLButtonDown( uint32 nFlags, int32 x, int32 y );
		virtual void			OnLButtonUp( uint32 nFlags, int32 x, int32 y );
		virtual void			OnLButtonClick( uint32 nFlags, int32 x, int32 y );
		virtual void			OnRButtonClick( uint32 nFlags, int32 x, int32 y );
		virtual void			OnRButtonUp( uint32 nFlags, int32 x, int32 y );
		virtual void			OnRButtonDown( uint32 nFlags, int32 x, int32 y );
		virtual void			OnMouseMove( uint32 nFlags, int32 x, int32 y );
		virtual void			OnDrag( const CFPos& HeldPoint, const CFPos& CurPoint );
		virtual void			OnHeld( float xScreen, float yScreen );
		virtual void			DrawBackImage( WND_IMAGE_LIST& Enable, WND_IMAGE_LIST& Disable, WND_IMAGE_LIST& MouseOver, WND_IMAGE_LIST& ClickDown, bool bClick );
	};

	//-----------------------------------------
	inline float
		SQRButton::GetTextOffsetX()
	{
		return m_fTextOffsetX;
	}

	inline void
		SQRButton::SetTextOffsetX(float x)
	{
		m_fTextOffsetX = x;
	}

	inline float
		SQRButton::GetTextOffsetY()
	{
		return m_fTextOffsetY;
	}

	inline void
		SQRButton::SetTextOffsetY(float y)
	{
		m_fTextOffsetY = y;
	}

}