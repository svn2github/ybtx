#include "stdafx.h"
#include "SQRClock.h"
#include "SQRDialog.h"

DYNAMIC_CREATE_WND( SQRClockPart, SQRWnd )
SQRClockPart::SQRClockPart()
: flength( 0.0f )
, mflag( -1 )
, bclock( TRUE )
, bfrontcl( FALSE )
{

}

SQRClockPart::~SQRClockPart()
{
}

void SQRClockPart::SetBeFrontColor(bool b)
{
	bfrontcl = b;
	InitClockPart();
}

void SQRClockPart::InitClockPart()
{
	offset[0]	 = 0.0f;
	offset[1]	 = 0.0f;
	offset[2]	 = 0.0f;
	offset[3]	 = 0.0f;
	offset[4]	 = 0.0f;
	offset[5]	 = 0.0f;
	offset[6]	 = -1.0f;
	if(bfrontcl)
		ShowWnd(true);
}


void SQRClockPart::OnProcess()
{
	offset[0] = 0.0f;
	offset[1] = 0.0f;
	CFRect rt;
	GetWndRect( rt );
	float fvalue = (float)( rt.bottom - rt.top ); 
	float ffvalue = (float)( rt.right - rt.left );
	float alllong = fvalue + ffvalue;
	
	switch( (INT32)offset[4] )
	{
	case 0:
	case 2:
		if (bclock)
		{
			if( flength <= ffvalue )
				offset[0] = flength;
			else
			{
				offset[0] = ffvalue;
				ffvalue    = flength - ffvalue;
				offset[1] = ffvalue;
			}
			break;
		}
		else
		{
			if (flength <= fvalue)
				offset[1] = fvalue - flength;
			else
			{
				offset[1] = 0;
				fvalue  = alllong - flength;
				offset[0] = fvalue;
			}
			break;
		}

	case 1:
	case 3:
		if (bclock)
		{
			if( flength <= fvalue )
				offset[0] = flength;
			else
			{
				offset[0] = fvalue;
				fvalue    = flength - fvalue;
				offset[1] = fvalue;
			}
			break;
		}
		else
		{
			if (flength <= ffvalue)
				offset[1] =  ffvalue - flength;
			else
			{
				offset[1] = 0;
				fvalue = alllong - flength;
				offset[0] = fvalue;
			}
			break;
		}
	}
	
}


DYNAMIC_CREATE_WND( SQRClock, SQRWnd )

SQRClock::SQRClock()
: fNowlength( 0.0f )
, mBegintime( 0 )
, falllength( 0.0f )
, mRunedtime( 0 )
, m_pTime(NULL)
, m_bFrontColor( FALSE )
{
	
}

SQRClock::~SQRClock()
{
	SafeRelease(m_pTime);
}

void SQRClock::OnProcess()
{	
	float fvalue;
	uint64 uCurTime = m_pTime!=NULL?m_pTime->GetCurTime():GetProcessTime();
	if( Funcflag == 1 )
	{
		if( lastRunedTime == 0 )
			lastRunedTime = static_cast<DWORD>(uCurTime + mRRunedtime - mBegintime);
		if( 0 == mRRunedtime )
			return ;
		UINT64 iivalue = lastRunedTime;
		fvalue = (float)mEndtiem > 0.0f ? (float)( iivalue ) / (float)mEndtiem : 1.1f;
	}
	else
	{
		if( 0 == mBegintime )
			return ;
		UINT64 ivalue = uCurTime + mRunedtime - mBegintime;
		fvalue =(float)mEndtiem > 0.0f ? (float)( ivalue ) / (float)mEndtiem : 1.1f;
	}
	if ( m_bFrontColor )
	{
		if ( fvalue <= 1.05f )
		{
			fvalue *= falllength;
			if (pProgress[0]->bclock)	// Ë³Ê±Õë£¬ÓÉÁÁµ½°µ
			{
				for ( int32 i = 0; i < 4; ++i )
				{
					pProgress[i]->offset[6] = (float)i;
					pProgress[i]->offset[4] = (float)i;
					if ( fvalue <= flength[i] )
					{
						pProgress[i]->flength = fvalue;
						pProgress[i]->ShowWnd( TRUE );

						for ( int32 j = i+1; j < 4; ++j )
							pProgress[j]->ShowWnd( FALSE );
						break;
					}
					else
					{
						fvalue -= flength[i];
						pProgress[i]->flength = flength[i];
						pProgress[i]->ShowWnd( TRUE );
					}
				}
			}
			else						// ÄæÊ±Õë£¬ÓÉ°µµ½ÁÁ
			{
				for (int32 i = 3; i >= 0; --i )
				{
					pProgress[i]->offset[6] = (float)i;
					pProgress[i]->offset[4] = (float)i;
					if ( fvalue <= flength[i])
					{
						pProgress[i]->flength = fvalue;
						pProgress[i]->ShowWnd( TRUE );

						for ( int32 j = i-1; j >=0; --j )
						{
							pProgress[j]->flength = flength[i];
							pProgress[j]->ShowWnd( TRUE );
						}
						break;
					}
					else
					{
						fvalue -= flength[i];
						pProgress[i]->ShowWnd(FALSE);
					}
				}
			}
		}
		else
		{
			if ( !(pProgress[0]->bclock) )
				ShowWnd( FALSE );
			mBegintime = 0;
			// ´¥·¢×´Ì¬¸Ä±ßÊÂ¼þ
			//SetEventStateMask(IP_FILL_FULL);
		}
	}
	else
	{
		if( fvalue <= 1.0f ) 
		{
			fvalue *= falllength;
			if (pProgress[0]->bclock) 
			{
				for( int32 i = 0; i < 4; ++i )
				{
					if( fvalue <= flength[i] )
					{
						pProgress[i]->offset[4] = (float)i;
						pProgress[i]->flength = fvalue;
						break;
					}
					else 
					{
						fvalue -= flength[i];
						pProgress[i]->ShowWnd( FALSE );
					}
				}
			}
			else
			{
				for (int32 i = 3; i >= 0; --i )
				{
					if ( fvalue <= flength[i])
					{
						pProgress[i]->offset[4] = (float)i;
						pProgress[i]->flength = fvalue;
						break;
					}
					else
					{

						fvalue -= flength[i];
						pProgress[i]->ShowWnd(TRUE);
					}
				}
			}
		}
		else
		{
			if (pProgress[0]->bclock)
				ShowWnd( FALSE );
			mBegintime = 0;
			// ´¥·¢×´Ì¬¸Ä±ßÊÂ¼þ
			//SetEventStateMask(IP_FILL_FULL);
		}
	}
	
}

void SQRClock::ReleaseWndTex()
{
	// ÊÍ·Å¿ÉÒÔÊÍ·ÅµÄUIÌùÍ¼×ÊÔ´
	m_BackImage.ReleaseTex();
	SQRControl::ReleaseWndTex();
}
void SQRClock::CreatWndTex()
{
	// ÏÔÊ¾Ö®Ç°ÉêÇë×ÊÔ´
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	m_BackImage.CreatTex( GetGraphic(), m_FlushDlg );
	SQRControl::CreatWndTex();
}

void SQRClock::OnDestroy()
{
	GfkSafeDelete(pProgress[0]);
	GfkSafeDelete(pProgress[1]);
	GfkSafeDelete(pProgress[2]);
	GfkSafeDelete(pProgress[3]);
}

int32 SQRClock::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.uStyle |= WS_CHILD | WS_IGNOREWHEEL;
	int32 re = SQRWnd::Create( newParam );
	float height = param.height / 2.0f; 
	float width  = param.width / 2.0f;
	m_Rect.left			= 0;
	m_Rect.top			= 0;
	m_Rect.right		= (float)width;
	m_Rect.bottom		= (float)height;

	WndCreateParam processParam = param;
	processParam.szTitle = "";
	processParam.uStyle |= WS_CHILD | WS_IGNOREWHEEL;
	processParam.x		 = width;
	processParam.y		 = 0;
	processParam.width   = width;
	processParam.height  = height;
	processParam.pParentWnd = this;
	pProgress[0] = new SQRClockPart;
	pProgress[0]->Create( processParam );
	
	processParam.x = width;
	processParam.y = height;
	processParam.width = width;
	processParam.height = height;
	pProgress[1] = new SQRClockPart;
	pProgress[1]->Create( processParam );
	
	processParam.x = 0;
	processParam.y = height;
	processParam.width = width;
	processParam.height = height;
	pProgress[2] = new SQRClockPart;
	pProgress[2]->Create( processParam );
	
	processParam.x = 0;
	processParam.y = 0;
	processParam.width = width;
	processParam.height = height;
	pProgress[3] = new SQRClockPart;
	pProgress[3]->Create( processParam );
	
	return re;
}

SQRWnd* SQRClock::GetResWnd( int32 nCtrlMask )
{
	//#define SM_CK_BK					0x0001      //SQRClock±³¾°ÌùÍ¼
	//#define SM_CK_PG0_Bk				0x0002		//Progress[0]±³¾°ÌùÍ¼
	//#define SM_CK_PG1_Bk				0x0003		//Progress[1]±³¾°ÌùÍ¼
	//#define SM_CK_PG2_Bk				0x0004		//Progress[2]±³¾°ÌùÍ¼
	//#define SM_CK_PG3_Bk				0x0005		//Progress[3]±³¾°ÌùÍ¼

	if( SM_CK_BK == nCtrlMask )
	{
		nCtrlMask -= SM_CK_BK - SM_BS_BK;
		return SQRWnd::GetResWnd( nCtrlMask );
	}
	else if (SM_CK_PG0_Bk == nCtrlMask)
	{
		nCtrlMask -= SM_CK_PG0_Bk - SM_BS_BK;
		return pProgress[0]->GetResWnd( nCtrlMask );
	}
	else if (SM_CK_PG1_Bk == nCtrlMask)
	{
		nCtrlMask -= SM_CK_PG1_Bk - SM_BS_BK;
		return pProgress[1]->GetResWnd( nCtrlMask );
	}
	else if (SM_CK_PG2_Bk == nCtrlMask)
	{
		nCtrlMask -= SM_CK_PG2_Bk - SM_BS_BK;
		return pProgress[2]->GetResWnd( nCtrlMask );
	}
	else if (SM_CK_PG3_Bk == nCtrlMask)
	{
		nCtrlMask -= SM_CK_PG3_Bk - SM_BS_BK;
		return pProgress[3]->GetResWnd( nCtrlMask );
	}
	else
		return NULL;
	
	
}

WND_IMAGE_LIST* SQRClock::GetWndBkImage( const IMAGE_PARAM& ImageFlag )
{
	//#define SM_CK_BK					0x0001      //SQRClock±³¾°ÌùÍ¼
	//#define SM_CK_PG0_Bk				0x0002		//Progress[0]±³¾°ÌùÍ¼
	//#define SM_CK_PG1_Bk				0x0003		//Progress[1]±³¾°ÌùÍ¼
	//#define SM_CK_PG2_Bk				0x0004		//Progress[2]±³¾°ÌùÍ¼
	//#define SM_CK_PG3_Bk				0x0005		//Progress[3]±³¾°ÌùÍ¼

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_CK_BK )
	{
		Flag.CtrlMask -= SM_CK_BK - SM_BS_BK;
		return SQRWnd::GetWndBkImage( Flag );
	}
	else if( Flag.CtrlMask == SM_CK_PG0_Bk )
	{
		Flag.CtrlMask -= SM_CK_PG0_Bk - SM_BS_BK;
		return pProgress[0]->GetWndBkImage( Flag );
	}
	else if( Flag.CtrlMask == SM_CK_PG1_Bk )
	{
		Flag.CtrlMask -= SM_CK_PG1_Bk - SM_BS_BK;
		return pProgress[1]->GetWndBkImage( Flag );
	}
	else if (Flag.CtrlMask == SM_CK_PG2_Bk)
	{
		Flag.CtrlMask -= SM_CK_PG2_Bk - SM_BS_BK;
		return pProgress[2]->GetWndBkImage( Flag );
	}
	else if (Flag.CtrlMask == SM_CK_PG3_Bk)
	{
		Flag.CtrlMask -= SM_CK_PG3_Bk - SM_BS_BK;
		return pProgress[3]->GetWndBkImage( Flag );
	}
	else
		return NULL;
}

bool SQRClock::IsHasEventState(EventStateMask Mask)
{
	if ( IP_FILL_FULL == Mask )
		return true;
	return SQRWnd::IsHasEventState(Mask);
}

bool SQRClock::ObjHasEventState(EventStateMask Mask)
{
	return SQRClock::IsHasEventState(Mask);
}

void SQRClock::SetBackImageColor(uint32 uColor)
{
	m_hWnd->m_Enable.ClearImage();
	for (int i = 0; i < 4; i++)
	{
		pProgress[i]->m_hWnd->m_Enable.ClearImage();
		pProgress[i]->m_hWnd->m_Enable.AddImage( GetGraphic(), 0, "", &m_Rect, CFPos(0.0f, 0.0f), uColor );
	}
}

void SQRClock::SetBeFrontImageColor(bool b)
{
	m_bFrontColor = b;
	for ( int32 i = 0; i < 4; ++i )
		pProgress[i]->SetBeFrontColor( b );
}


void SQRClock::setTime( int32 dtime )
{
	ShowWnd( TRUE );
	SetEventStateMask(IP_ENABLE);
	CFRect rt;
	mBegintime = static_cast<DWORD>( m_pTime!=NULL?m_pTime->GetCurTime():GetProcessTime() );
	mEndtiem   = dtime;
	fNowlength = 0.0f;
	Funcflag   = 0;
	flagRect   = 0;
	falllength = 0;
	for( int32 i = 0; i < 4; ++i )
	{
		pProgress[i]->GetWndRect( rt );
		pProgress[i]->ShowWnd( TRUE );
		pProgress[i]->InitClockPart();
		pProgress[i]->flength   = 0.0f;
		ZeroMemory( pProgress[i]->offset, sizeof(float)*6 );
		flength[i]  = (rt.bottom - rt.top) + (rt.right - rt.left);
		falllength += flength[i];	
	}
	fNowlength = flength[0];
}
void SQRClock::setRunedTime(int32 runedtime)
{
	mRunedtime = runedtime;
}

void SQRClock::SetDirection(bool Direction)
{
	ShowWnd(TRUE);
	SetEventStateMask(IP_ENABLE);
	for ( int32 i = 0; i < 4; ++i )
	{
		pProgress[i]->ShowWnd( TRUE );
		pProgress[i]->InitClockPart();
		pProgress[i]->bclock = Direction;
		pProgress[i]->flength   = 0.0f;
		ZeroMemory( pProgress[i]->offset, sizeof(float)*6 );
	}
}

void SQRClock::setTimeEx(int32 dtime, IRenderTime* pTime)
{
	if(pTime != m_pTime)
	{
		SafeRelease(m_pTime);
		m_pTime = pTime;
		if (pTime)
		{
			m_pTime->AddRef();
		}
	}
	setTime(dtime);
}

void SQRClock::SetPosEx(int32 nPos, IRenderTime* pTime)
{
	if(pTime != m_pTime)
	{
		SafeRelease(m_pTime);
		m_pTime = pTime;
		m_pTime->AddRef();
	}
	SetPos(nPos);
}

void SQRClock::SetPos(int32 nPos)
{
	ShowWnd( TRUE );
	SetEventStateMask(IP_ENABLE);
	CFRect rt;
	mBegintime		= static_cast<DWORD>( m_pTime!=NULL?m_pTime->GetCurTime():GetProcessTime() );
	//mEndtiem		= 10000;
	lastRunedTime	= 0;
	fNowlength		= 0.0f;
	flagRect		= 0;
	Funcflag		= 1;
	falllength		= 0;
	for( int32 i = 0; i < 4; ++i )
	{
		pProgress[i]->GetWndRect( rt );
		pProgress[i]->ShowWnd( TRUE );
		pProgress[i]->InitClockPart();
		pProgress[i]->flength   = 0.0f;
		ZeroMemory( pProgress[i]->offset, sizeof(float)*6 );
		flength[i]  = (rt.bottom - rt.top ) + ( rt.right - rt.left );
		falllength += flength[i];	
	}
	fNowlength = flength[0];

	Funcflag =1;
	mRRunedtime = nPos;
}
void SQRClock::SetRange(int32 nRange)
{
	Funcflag = 1;
	mEndtiem   = nRange;
}

void SQRClock::_ResizeRect( const CFRect& rt ,bool refreshbak)
{
	SQRWnd::_ResizeRect( rt ,refreshbak);
	CFRect rtt;
	falllength = 0;
	for( int32 i = 0; i < 4; ++i )
	{
		pProgress[i]->GetWndRect( rtt );
		flength[i]  = (rtt.bottom - rtt.top ) + ( rtt.right - rtt.left );
		falllength += flength[i];	
	}
}

void SQRClock::_SetWndRect( const CFRect& rt )
{
	SQRWnd::_SetWndRect( rt );
	CFRect rtt;
	falllength = 0;
	for( int32 i = 0; i < 4; ++i )
	{
		pProgress[i]->GetWndRect( rtt );
		flength[i]  = (rtt.bottom - rtt.top ) + ( rtt.right - rtt.left );
		falllength += flength[i];
	}
}


