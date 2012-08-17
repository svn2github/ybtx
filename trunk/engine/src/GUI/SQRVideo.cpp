
#include "stdafx.h"
#include "SQRVideo.h"
#include "Cmovie.h"

DYNAMIC_CREATE_WND( SQRVideo, SQRWnd )

class CMemStream;

SQRVideo::SQRVideo()
:m_pMovie(NULL)
,m_bCycle(true)
{

}

SQRVideo::~SQRVideo()
{
	SafeDelete(m_pMovie);
}

int32 SQRVideo::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.uStyle |= WS_CHILD;
	if ( FAILED( SQRWnd::Create( newParam ) ) )
		return GERROR;

	//创建播放、暂停、回放按钮
	WndCreateParam buttonParam = param;
	buttonParam.szTitle = "Play";
	buttonParam.uStyle |= WS_CHILD;
	buttonParam.x = 0;
	buttonParam.y = param.height;
	buttonParam.width = param.width/3;
	buttonParam.height = 20;
	buttonParam.pParentWnd = this;
	m_playButton.Create( buttonParam );

	buttonParam.szTitle = "Pause";
	buttonParam.x = param.width/3;
	m_pauseButton.Create( buttonParam );

	buttonParam.szTitle = "Replay";
	buttonParam.x = param.width*2/3;
	m_replayButton.Create( buttonParam );

	m_playButton.ShowWnd( (GetStyle()&WS_NSITEM) == 0 );
	m_pauseButton.ShowWnd( (GetStyle()&WS_NSITEM) == 0 );
	m_replayButton.ShowWnd( (GetStyle()&WS_NSITEM) == 0 );

	m_pMovie = new CMovie();

	SetszSourceName(m_hWnd->m_szSource.c_str());

	return 0;
}

void SQRVideo::DrawWndBackground()
{
	m_beBGDrawed = true;
	if(m_pMovie)
	{
		if (GetWndWidth() == 0.0f || GetWndHeight() == 0.0f)
			return;

		if (m_pMovie->IsPlayCompleted())
		{
			CompleteCallBack();
			if (m_bCycle)
				Rewind();
		}

		m_pMovie->UpdateMovieTexture();

		DrawRect(m_pMovie->GetMovieTexture(), CFRect(0, 0, GetWndWidth(), GetWndHeight()), 0xffffffff, &CFRect(0, 0, m_pMovie->GetMovieWidth(), m_pMovie->GetMovieHeight()), false);
	}
}

bool SQRVideo::IsHasEventState(EventStateMask Mask)
{
	return SQRWnd::IsHasEventState(Mask);
}

bool SQRVideo::ObjHasEventState(EventStateMask Mask)
{
	return SQRVideo::IsHasEventState(Mask);
}

SQRWnd* SQRVideo::GetResWnd( int32 nCtrlMask )
{
	//#define SM_VO_B1					0x0001		//按键1
	//#define SM_VO_B2			        0x0002		//按键2
	//#define SM_VO_B3			        0x0003		//按键3
	if( SM_VO_BK == nCtrlMask )
	{
		nCtrlMask -= SM_VO_BK - SM_BS_BK;
		return SQRWnd::GetResWnd( nCtrlMask );
	}
	else if( SM_VO_B1 == nCtrlMask )
	{
		nCtrlMask -= SM_VO_B1 - SM_BS_BK;
		return m_playButton.GetResWnd( nCtrlMask );
	}
	else if( SM_VO_B2 == nCtrlMask )
	{
		nCtrlMask -= SM_VO_B2 - SM_BS_BK;
		return m_pauseButton.GetResWnd( nCtrlMask );
	}
	else if( SM_VO_B3 == nCtrlMask )
	{
		nCtrlMask -= SM_VO_B3 - SM_BS_BK;
		return m_replayButton.GetResWnd( nCtrlMask );
	}
	else
		return NULL;
}

void SQRVideo::SetszSourceName( const char* szDesc )
{
	Load(szDesc);

	//if (!(m_playButton.IsShow()
	//	&&m_pauseButton.IsShow()
	//	&&m_replayButton.IsShow()))
	//{
	//	PlayMovie();
	//}

	SQRWnd::SetszSourceName( szDesc );
}

void SQRVideo::SetStyle( uint32 Style )
{
	m_playButton.ShowWnd( (GetStyle()&WS_NSITEM) == 0 );
	m_pauseButton.ShowWnd( (GetStyle()&WS_NSITEM) == 0 );
	m_replayButton.ShowWnd( (GetStyle()&WS_NSITEM) == 0 );
	SQRWnd::SetStyle(Style);
}

void SQRVideo::Load(const char* fullPathName)
{
	m_pMovie->Load(fullPathName);
}

void SQRVideo::Play()
{
	m_pMovie->Play();
}

void SQRVideo::Pause()
{
	m_pMovie->Pause();
}

void SQRVideo::Rewind()
{
	m_pMovie->Replay();
}

void SQRVideo::OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 )
{
	if (uMsgID == BUTTON_CLICKDOWN)
	{
		if (pChild == &m_playButton)
		{
			Play();
		}
		else if (pChild == &m_pauseButton)
		{
			Pause();
		} 
		else if (pChild == &m_replayButton)
		{
			Rewind();
		}
	}
}

int SQRVideo::CompleteCallBack(void)
{
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_RET(int) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	return 0;
}

float SQRVideo::GetProcess( void )
{
	if (m_pMovie)
		return m_pMovie->GetProgress();
	else
		return 0.0f;
}

void sqr::SQRVideo::SetCycle( bool value )
{
	m_bCycle = value;
}