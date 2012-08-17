#pragma once
#include "SQRButton.h"

namespace sqr
{
	class GUI_API SQRSpin :	public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRSpin )

			SQRButton	m_PressButton1;
		SQRButton	m_PressButton2;
	public:
		SQRSpin(void);
		virtual ~SQRSpin(void);

		virtual int32	Create( const WndCreateParam& param );
		virtual WND_IMAGE_LIST* GetWndBkImage( const IMAGE_PARAM& ImageFlag );

		virtual uint32	GetWndTextColor( const IMAGE_PARAM& ImageFlag );
		virtual void	SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );

		virtual uint32	GetWndTextBcColor( const IMAGE_PARAM& ImageFlag );
		virtual void	SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );

		virtual SQRWnd*	GetResWnd( int32 nCtrlMask );

		virtual void	DrawWndBackground() {};


		static bool		IsHasEventState(EventStateMask Mask);
		virtual bool	ObjHasEventState(EventStateMask Mask);
	protected:
		virtual void	OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 );
	};
}