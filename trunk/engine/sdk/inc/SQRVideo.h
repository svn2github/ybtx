#pragma once
#include "SQRButton.h"
#include "SQRControl.h"

namespace sqr
{
	class CMovie;

	class GUI_API SQRVideo : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRVideo )
	public:
		SQRVideo(void);
		virtual ~SQRVideo(void);

		virtual int32 Create( const WndCreateParam& param );

		virtual void DrawWndBackground();

		static bool		IsHasEventState(EventStateMask Mask);
		virtual bool	ObjHasEventState(EventStateMask Mask);

		virtual SQRWnd*	GetResWnd( int32 nCtrlMask );

		virtual void	SetszSourceName( const char* szDesc );
		virtual void    SetStyle( uint Style );

		virtual int		CompleteCallBack(void);

	public:
		void Load(const char* fullPathName);

		void Play(void);

		void Pause(void);

		void Rewind(void);

		float GetProcess(void);

		void SetCycle(bool value);

	protected:
		virtual void OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 );

	private:
		CMovie *m_pMovie;
		CFRect rtImage;
		CFRect rtText;
		SQRButton m_playButton;
		SQRButton m_pauseButton;
		SQRButton m_replayButton;
		bool m_bCycle;
	};
}