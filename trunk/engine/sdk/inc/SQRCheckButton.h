#pragma once
#include "SQRButton.h"

namespace sqr
{
	class GUI_API SQRCheckButton : public SQRButton
	{
		DYNAMIC_DECLARE_WND( SQRCheckButton )

			bool                m_bCheck;

	public:
		SQRCheckButton(void);
		virtual ~SQRCheckButton(void);

		virtual int32		DestroyWnd();
		virtual int32		Create( const WndCreateParam& param );
		virtual WND_IMAGE_LIST* GetWndBkImage( const IMAGE_PARAM& ImageFlag );

		virtual uint32		GetWndTextColor( const IMAGE_PARAM& ImageFlag );
		virtual void		SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );

		virtual uint32		GetWndTextBcColor( const IMAGE_PARAM& ImageFlag );
		virtual void		SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );


		static bool			IsHasEventState(EventStateMask Mask);
		virtual bool		ObjHasEventState(EventStateMask Mask);

		virtual void		ReleaseWndTex();		// 节省资源模式下，窗口关闭使用
		virtual void		CreatWndTex();			// 节省资源模式下，窗口打开使用

	protected:
		virtual void		DrawWndBackground();
		virtual void		DrawWndText();
		virtual void		DrawText( const wstring& text, uint32 textColor, uint32 textBcColor );
		virtual void		OnCheck( bool bCheck ) { CALL_CLASS_CALLBACK_1(bCheck) }
		virtual void		OnLButtonClick( uint32 nFlags, int32 x, int32 y );
		virtual void		OnRButtonClick( uint32 nFlags, int32 x, int32 y );
		virtual void		OnLButtonUp( uint32 nFlags, int32 x, int32 y );
		virtual void		OnRButtonUp( uint32 nFlags, int32 x, int32 y );
		virtual void		PrepareDefaultDesc();

	public:
		void				SetCheck( bool bCheck );
		bool				GetCheck() const
		{
			return m_bCheck;
		}

		SQRWnd*				GetFirstBrother();
		SQRWnd*				GetLastBrother();

	protected:
		WND_IMAGE_LIST		m_UncheckMouseOverImage;
		WND_IMAGE_LIST		m_UncheckClickDownImage;
		WND_IMAGE_LIST		m_UncheckEnableImage;
		WND_IMAGE_LIST		m_UncheckDisableImage;

		uint32				m_UncheckMouseOverColor;
		uint32				m_UncheckClickDownColor;
		uint32				m_UncheckEnableColor;
		uint32				m_UncheckDisableColor;

		//----------------------------------------- 下面的是字的边框在
		uint32				m_UncheckMouseOverBcColor;
		uint32				m_UncheckClickDownBcColor;
		uint32				m_UncheckEnableBcColor;
		uint32				m_UncheckDisableBcColor;
	};

}