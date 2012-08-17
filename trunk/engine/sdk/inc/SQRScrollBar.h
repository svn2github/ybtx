#pragma once
#include "SQRButton.h"

namespace sqr
{
	class GUI_API SQRScrollBar : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRScrollBar )

	protected:
		float		m_fRange;
		float		m_fPageSize;
		float		m_fCurPos;
		int32		m_sbMinSize;	//滚动调中的滑块最小的size，对于水平滚动条来说是width，竖直的则是height
	public:
		SQRScrollBar(void);
		virtual ~SQRScrollBar(void);

		virtual int32			Create( const WndCreateParam& param );
		virtual WND_IMAGE_LIST* GetWndBkImage( const IMAGE_PARAM& ImageFlag );

		virtual uint32			GetWndTextColor( const IMAGE_PARAM& ImageFlag );
		virtual void			SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );

		virtual uint32			GetWndTextBcColor( const IMAGE_PARAM& ImageFlag );
		virtual void			SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );

		virtual SQRWnd*			GetResWnd( int32 nCtrlMask );
		virtual void			SetWndRect( const CFRect& rt );
		virtual void			ResizeRect( const CFRect& rt  ,bool refreshbak = true);
		virtual void			_ResizeRect( const CFRect& rt ,bool refreshbak = true);
		virtual void			_SetWndRect( const CFRect& rt );
	public:

		void					SetRange( int32 nMax );
		void					SetPage( int32 nPage );
		void					SetPos( int32 nPos, bool bMouseWheel);
		void					SetPos( int32 nPos ) { SetPos(nPos, false); }

		int32					GetRange()const
		{
			return (int32)m_fRange;
		}
		int32					GetPage()const
		{
			return (int32)m_fPageSize;
		}
		int32					GetPos()const
		{
			return (int32)m_fCurPos;
		}
		/// @brief 滚动条是否已经滚动到最下端
		bool					Isbottom()const;
		bool					Istop() const;

		void					SetCutWnd(SQRWnd* wnd);
	public:
		SQRButton*				m_pPressButton1;
		SQRButton*				m_pPressButton2;
		SQRButton*				m_pScrollBar;
		void					ProcessMove( bool bSendMsg , bool bDRAG = false);
	protected:
		virtual void			OnLButtonDown( uint32 nFlags, int32 x, int32 y );
		virtual void			OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y );
		virtual void			OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 );
		virtual void			OnPosChanged() {};
	protected:
		/// @brief 刷新滚动条中间滑块的可见状态
		void					RefreshThumbState();
	};
}