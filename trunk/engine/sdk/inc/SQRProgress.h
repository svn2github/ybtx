#pragma once

#include "SQRControl.h"

namespace sqr
{
	class GUI_API SQRProgress:public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRProgress )

	public:
		SQRProgress(void);
		virtual ~SQRProgress(void);

	public:
		virtual int32		DestroyWnd();
		virtual int32		Create( const WndCreateParam& param );
		virtual void		OnProcess();

		virtual SQRWnd*				GetResWnd( int32 nCtrlMask );
		virtual WND_IMAGE_LIST*		GetWndBkImage( const IMAGE_PARAM& ImageFlag );
		virtual void				DrawWndBackground();
		virtual void				DrawAfterChildren();

		static bool				IsHasEventState(EventStateMask Mask);
		virtual bool			ObjHasEventState(EventStateMask Mask);

		virtual void			ReleaseWndTex();		// 节省资源模式下，窗口关闭使用
		virtual void			CreatWndTex();			// 节省资源模式下，窗口打开使用

	public:
		int32			GetPos() const
		{
			return m_nPos;
		}
		int32			GetRange() const
		{
			return m_nRange;
		}
		int32           GetProgressMode() const
		{
			return m_nProgressMode;
		}
		void			SetPos( int32 nPos );	//				{ throw(1); m_nPos = min( nPos, m_nRange ); }
		void			SetRange( int32 nRange );//				{ m_nRange = nRange; SetPos( m_nPos ); }
		void			SetFillColor( uint32 dwFillColor );
		void            SetProgressMode( int32 nProgressMode ); //设置进度条延伸方向

		SQRWnd*			SetChangeDiff(int32 dif, uint32 time);
		void			EndChangeDiff();
		void			CalculatePos();
		SQRWnd*			GetPreDifWnd();
		SQRWnd*			GetBakDifWnd();
		SQRWnd*			GetCurDifWnd();
	protected:
		int32           m_nPos;
		int32           m_nRange;
		int32           m_nProgressMode;
		WND_IMAGE_LIST	m_FillImage;
		WND_IMAGE_LIST	m_FillHeadImage;

		SQRWnd*			m_pPreDifWnd;
		SQRWnd*			m_pBakDifWnd;
		SQRWnd*			m_pDiffWnd;

		DWORD			m_Diftime;
		DWORD			m_Lasttime;
		CFRect			m_RePosRect;
		bool			m_beDifPre;
		
	};
	inline
		SQRWnd* SQRProgress::GetPreDifWnd()
	{
		return m_pPreDifWnd;
	}

	inline
		SQRWnd* SQRProgress::GetBakDifWnd()
	{
		return m_pBakDifWnd;
	}

	inline
		SQRWnd* SQRProgress::GetCurDifWnd()
	{
		return m_pDiffWnd;
	}
}