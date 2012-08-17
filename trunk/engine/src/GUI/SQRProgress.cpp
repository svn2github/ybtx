#include "stdafx.h"
#include "SQRProgress.h"
#include "SQRDialog.h"
#include "CColor.inl"

DYNAMIC_CREATE_WND( SQRProgress, SQRWnd )

SQRProgress::SQRProgress()
: m_nProgressMode(0)
, m_pPreDifWnd(new SQRWnd)
, m_pBakDifWnd(new SQRWnd)
, m_pDiffWnd(NULL)
, m_Diftime(0)
, m_Lasttime((DWORD)(-1))
, m_beDifPre(false)
{
}

SQRProgress::~SQRProgress()
{
    DestroyWnd();

	delete m_pPreDifWnd;
	delete m_pBakDifWnd;
}

int32 SQRProgress::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.uStyle |= WS_CHILD|WS_IGNOREWHEEL;
	int32 re = SQRWnd::Create( newParam );

	float nWidth  = GetWndWidth();
	float nHeight = GetWndHeight();
	CFRect rt( 0.0f, 0.0f, nWidth, nHeight );
	uint32 dwColor;
	dwColor = CColor::RandDark();
	m_FillImage.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), dwColor );
	dwColor = 0x0;
	m_FillHeadImage.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), dwColor );
    m_nPos          = 0;
    m_nRange        = 100;

	// 两个显示块
	WndCreateParam processParam = param;
	processParam.szTitle = "";
	processParam.uStyle |= WS_CHILD | WS_MSGTRANSPT;
	processParam.pParentWnd = this;
	m_pPreDifWnd->Create( processParam );
	m_pBakDifWnd->Create( processParam );
	m_pPreDifWnd->ShowWnd(false);
	m_pBakDifWnd->ShowWnd(false);

	return re;
}

void SQRProgress::SetPos( int32 nPos )					
{ 
	if ( min( nPos, m_nRange ) == m_nRange && m_nPos != m_nRange )
		SetEventStateMask(IP_FILL_FULL);

	m_nPos = min( nPos, m_nRange ); 
}

void SQRProgress::SetRange( int32 nRange )				
{ 
	m_nRange = nRange; SetPos( m_nPos ); 
}

void SQRProgress::SetProgressMode( int32 nProgressMode )
{
	m_nProgressMode = nProgressMode;
}

void SQRProgress::DrawWndBackground()
{
	m_beBGDrawed = true;
	if( !( GetStyle()&PRG_FILLFIRST ) )
		SQRWnd::DrawWndBackground();

	CFRect rt;

	if( IsEnable() && m_FillImage.GetImageCount() )
	{
		rt.left			=	0;
		rt.top			=	0;
		rt.right		=	(float)m_hWnd->m_Width;
		rt.bottom		=	(float)m_hWnd->m_Height;

		if ( m_nRange != 0 )
		{
			if( 0 == m_nProgressMode)        //左端固定的进度条
				rt.right = rt.left + m_nPos/(float)m_nRange*rt.Width() + 0.5f;
			else if( 1 == m_nProgressMode ) //右端固定的进度条
				rt.left = rt.right - m_nPos/(float)m_nRange*rt.Width() + 0.5f;
			else if( 2 == m_nProgressMode )   //顶部固定的进度条
				rt.bottom = rt.top + m_nPos/(float)m_nRange*rt.Height() + 0.5f;
			else if( 3 == m_nProgressMode )//底部固定的进度条
				rt.top = rt.bottom - m_nPos/(float)m_nRange*rt.Height() + 0.5f;

			SetEventStateMask(IP_FILL);
			DrawRect( m_FillImage, &rt );
		}
		else
			SetEventStateMask(IP_FILL);
		
	}
	if ( IsEnable() && m_FillHeadImage.GetImageCount() )//进度条尖端
	{
		IMAGE& ImageFillHead = m_FillHeadImage.GetImage(0);
		if( 0 == m_nProgressMode)        //左端固定的进度条
			ImageFillHead.SetPosWndX(rt.right - ImageFillHead.rtTexture.Width());
		else if( 1 == m_nProgressMode ) //右端固定的进度条
			ImageFillHead.SetPosWndX(rt.left);
		else if( 2 == m_nProgressMode )   //顶部固定的进度条
			ImageFillHead.SetPosWndY(rt.bottom - ImageFillHead.rtTexture.Height());
		else if( 3 == m_nProgressMode )//底部固定的进度条
			ImageFillHead.SetPosWndY(rt.top);

		SetEventStateMask(IP_CLICKDOWN);
		DrawRect( m_FillHeadImage );
	}
}

void SQRProgress::DrawAfterChildren()
{
	if( GetStyle()&PRG_FILLFIRST )
		SQRWnd::DrawWndBackground();
	SQRWnd::DrawAfterChildren();
}

void SQRProgress::OnProcess()
{
	// 处理子窗体是否正常显示
	SQRWnd::OnProcess();
	if ( m_Diftime == .0f)
		return;

	DWORD cur_time = static_cast<DWORD>(GetProcessTime());
	if ( cur_time < m_Diftime || m_Lasttime == 0.0f)
		CalculatePos();
	else
		EndChangeDiff();
}

void SQRProgress::ReleaseWndTex()
{
	// 释放可以释放的UI贴图资源
	m_FillImage.ReleaseTex();
	m_FillHeadImage.ReleaseTex();
	SQRControl::ReleaseWndTex();
}
void SQRProgress::CreatWndTex()
{
	// 显示之前申请资源
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	m_FillImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_FillHeadImage.CreatTex( GetGraphic(), m_FlushDlg );
	SQRControl::CreatWndTex();
}

int32	SQRProgress::DestroyWnd()
{
    m_FillImage.Release();
	m_FillHeadImage.Release();
    return SQRWnd::DestroyWnd();
}


SQRWnd* SQRProgress::GetResWnd( int32 nCtrlMask )
{
	//#define SM_PG_BK					0x0001		//SQRProgress背景贴图
	//#define SM_PG_PD1					0x0002		//显示块1背景贴图
	//#define SM_PG_BD2					0x0003		//显示块2背景贴图
	if( SM_PG_BK == nCtrlMask )
	{
		nCtrlMask -= SM_PG_BK - SM_BS_BK;
		return SQRWnd::GetResWnd( nCtrlMask );
	}
	else if (SM_PG_PD1 == nCtrlMask)
	{
		nCtrlMask -= SM_PG_PD1 - SM_BS_BK;
		return m_pPreDifWnd->GetResWnd( nCtrlMask );
	}
	else if (SM_PG_BD2 == nCtrlMask)
	{
		nCtrlMask -= SM_PG_BD2 - SM_BS_BK;
		return m_pBakDifWnd->GetResWnd( nCtrlMask );
	}
	else
		return NULL;
}

WND_IMAGE_LIST*  SQRProgress::GetWndBkImage( const IMAGE_PARAM& ImageFlag )
{
	IMAGE_PARAM Flag = ImageFlag;
    if( Flag.CtrlMask != SM_BS_BK )
	{
		if( Flag.CtrlMask == SM_PG_PD1 )
		{
			Flag.CtrlMask -= SM_PG_PD1 - SM_BS_BK;
			return m_pPreDifWnd->GetWndBkImage( Flag );
		}
		else if( Flag.CtrlMask == SM_PG_BD2 )
		{
			Flag.CtrlMask -= SM_PG_BD2 - SM_BS_BK;
			return m_pBakDifWnd->GetWndBkImage( Flag );
		}
		else
			return NULL;
	}
    else if( Flag.StateMask < IP_MOUSEOVER )
        return SQRWnd::GetWndBkImage( Flag );
    else if( Flag.StateMask == IP_FILL )
        return &m_FillImage;
	else if( Flag.StateMask == IP_FILL_FULL )
		return NULL;
	else if( Flag.StateMask == IP_CLICKDOWN )//用于进度条尖端贴图
		return &m_FillHeadImage;
    else
        return NULL;
}

bool SQRProgress::ObjHasEventState(EventStateMask Mask)
{
	if ( IP_FILL_FULL == Mask )
		return true;
	return SQRProgress::IsHasEventState(Mask);
}

bool SQRProgress::IsHasEventState(EventStateMask Mask)
{
	if(	IP_FILL			== Mask ||
		IP_CLICKDOWN	== Mask )
		return true;
	return SQRWnd::IsHasEventState(Mask);
}

void SQRProgress::SetFillColor( uint32 dwFillColor )    
{
	IMAGE_PARAM Flag;
	Flag.CtrlMask = SM_BS_BK;
	Flag.StateMask = IP_FILL;

	WND_IMAGE_LIST* pList = GetWndBkImage( Flag );
	for ( int32 i = 0; i < pList->GetImageCount(); i++ )
	{
		pList->GetImage( i ).dwColor = dwFillColor;
	};

	Flag.StateMask = IP_ENABLE;
	pList = GetWndBkImage( Flag );
	for ( int32 i = 0; i < pList->GetImageCount(); i++ )
	{
		pList->GetImage( i ).dwColor = dwFillColor;
	};
}

// 完成位置计算，长度计算，时间和显示窗口初始化
SQRWnd*	SQRProgress::SetChangeDiff(int32 dif, uint32 time)
{
	EndChangeDiff();
	// 时间
	m_Diftime = static_cast<DWORD>(GetProcessTime() + time);
	m_Lasttime = static_cast<DWORD>(time);

	m_RePosRect.left = .0f; m_RePosRect.right = .0f; 
	m_RePosRect.top = .0f; m_RePosRect.bottom = .0f;
	// 长度
	CFRect wndrt;
	if ( dif >= 0 )
	{
		m_beDifPre = false;
		m_pDiffWnd = m_pBakDifWnd;	// 增加
		if( m_FillImage.GetImageCount() )
		{
			if( 0 == m_nProgressMode)        //左端固定的进度条
				m_RePosRect.left = -(dif/(float)m_nRange*m_hWnd->m_Width + 0.5f);
			else if( 1 == m_nProgressMode ) //右端固定的进度条
				m_RePosRect.right = dif/(float)m_nRange*m_hWnd->m_Width + 0.5f;
			else if( 2 == m_nProgressMode )   //顶部固定的进度条
				m_RePosRect.top = -(dif/(float)m_nRange*m_hWnd->m_Height + 0.5f);
			else if( 3 == m_nProgressMode )//底部固定的进度条
				m_RePosRect.bottom = dif/(float)m_nRange*m_hWnd->m_Height + 0.5f;
		}
	}
	else
	{
		m_beDifPre = true;
		m_pDiffWnd = m_pPreDifWnd;	// 减少
		if( m_FillImage.GetImageCount() )
		{
			if( 0 == m_nProgressMode)        //左端固定的进度条
				m_RePosRect.right = -(dif/(float)m_nRange*m_hWnd->m_Width + 0.5f);
			else if( 1 == m_nProgressMode ) //右端固定的进度条
				m_RePosRect.left = dif/(float)m_nRange*m_hWnd->m_Width + 0.5f;
			else if( 2 == m_nProgressMode )   //顶部固定的进度条
				m_RePosRect.bottom = -(dif/(float)m_nRange*m_hWnd->m_Height + 0.5f);
			else if( 3 == m_nProgressMode )//底部固定的进度条
				m_RePosRect.top = dif/(float)m_nRange*m_hWnd->m_Height + 0.5f;
		}
	}
	// 显示
	if (m_pDiffWnd)
		m_pDiffWnd->ShowWnd(true);
	// 计算一次位置
	CalculatePos();

	return m_pDiffWnd;
}

void SQRProgress::CalculatePos()
{
	if ( m_pDiffWnd != NULL && m_pDiffWnd->IsShow() && (!m_pDiffWnd->m_bInShape) )
	{
		// 在显示更改信息中
		CFRect rt, childrt;
		GetWndRect(rt);

		if ( m_beDifPre )	// 减少
		{
			float dis;
			if( 0 == m_nProgressMode)        //左端固定的进度条
			{
				if (m_RePosRect.right > m_hWnd->m_Width)
					m_RePosRect.right = m_hWnd->m_Width;
				dis = m_nPos/(float)m_nRange*m_hWnd->m_Width + 0.5f;
				childrt.top = rt.top;	
				childrt.bottom = rt.bottom;
				childrt.left = rt.left + dis; 
				childrt.right = childrt.left + m_RePosRect.right;
			}
			else if( 1 == m_nProgressMode ) //右端固定的进度条
			{
				if ((-m_RePosRect.left) > m_hWnd->m_Width)
					m_RePosRect.left = -m_hWnd->m_Width;
				dis = m_nPos/(float)m_nRange*m_hWnd->m_Width + 0.5f;
				childrt.top = rt.top;	
				childrt.bottom = rt.bottom;
				childrt.right = rt.right - dis;
				childrt.left = childrt.right + m_RePosRect.left;
			}
			else if( 2 == m_nProgressMode )   //顶部固定的进度条
			{
				if (m_RePosRect.bottom > m_hWnd->m_Height)
					m_RePosRect.bottom = m_hWnd->m_Height;
				dis = m_nPos/(float)m_nRange*m_hWnd->m_Height + 0.5f;
				childrt.left = rt.left;
				childrt.right = rt.right;
				childrt.top = rt.top + dis;
				childrt.bottom = childrt.top + m_RePosRect.bottom;
			}
			else if( 3 == m_nProgressMode )//底部固定的进度条
			{
				if ((-m_RePosRect.top) > m_hWnd->m_Height)
					m_RePosRect.top = m_hWnd->m_Height;
				dis = m_nPos/(float)m_nRange*m_hWnd->m_Height + 0.5f;
				childrt.left = rt.left;
				childrt.right = rt.right;
				childrt.bottom = rt.bottom - dis;
				childrt.top = childrt.bottom + m_RePosRect.top;
			}
		}
		else	// 增加
		{
			float dis;
			if( 0 == m_nProgressMode)        //左端固定的进度条
			{
				dis = m_nPos/(float)m_nRange*m_hWnd->m_Width + 0.5f;
				if ((-m_RePosRect.left) > dis)
					m_RePosRect.left = -dis;
				childrt.top = rt.top;	
				childrt.bottom = rt.bottom;
				childrt.right = rt.left + dis; 
				childrt.left = childrt.right + m_RePosRect.left;
			}
			else if( 1 == m_nProgressMode ) //右端固定的进度条
			{
				dis = m_nPos/(float)m_nRange*m_hWnd->m_Width + 0.5f;
				if ( m_RePosRect.right > dis )
					m_RePosRect.right = dis;
				childrt.top = rt.top;	
				childrt.bottom = rt.bottom;
				childrt.left = rt.right - dis;
				childrt.right = childrt.left + m_RePosRect.right;
			}
			else if( 2 == m_nProgressMode )   //顶部固定的进度条
			{
				dis = m_nPos/(float)m_nRange*m_hWnd->m_Height + 0.5f;
				if ((-m_RePosRect.top) > dis)
					m_RePosRect.top = -dis;
				childrt.left = rt.left;
				childrt.right = rt.right;
				childrt.bottom = rt.top + dis;
				childrt.top = childrt.bottom + m_RePosRect.top;
			}
			else if( 3 == m_nProgressMode )//底部固定的进度条
			{
				dis = m_nPos/(float)m_nRange*m_hWnd->m_Height + 0.5f;
				if (m_RePosRect.bottom > dis)
					m_RePosRect.bottom = dis;
				childrt.left = rt.left;
				childrt.right = rt.right;
				childrt.top = rt.bottom - dis;
				childrt.bottom = childrt.top + m_RePosRect.bottom;
			}
		}
		m_pDiffWnd->SetWndRect(childrt);
	}
}

void SQRProgress::EndChangeDiff()
{
	m_pPreDifWnd->ShowWnd(false);
	m_pBakDifWnd->ShowWnd(false);
	m_pDiffWnd = NULL;

	m_Diftime = 0;
	m_Lasttime = (DWORD)(-1);
}



