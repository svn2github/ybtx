#include "stdafx.h"
#include "SQRTipHolder.h"

SQRTipHolder::SQRTipHolder()
: m_pTipHolder(NULL)
{
}

SQRTipHolder::~SQRTipHolder()
{
	// need to delete m_pTipHolder??? 
	// maybe it will be delete by wnd, but seem not very well
	// i don't know, so have a try
	GfkSafeDelete(m_pTipHolder);
}

void SQRTipHolder::CreatHolderWnd(SQRWnd* wnd)
{
	if ( !wnd )
		return;

	// create the holder window
	m_pTipHolder = new SQRWnd();
	WndCreateParam param;
	WndHandle* wndHandle = wnd->GetWndHandle();
	param.uStyle = 0x60000000;

	param.x = wndHandle->m_xInitial;
	param.y = wndHandle->m_yInitial;
	param.width = wndHandle->m_WidthInitial;
	param.height = wndHandle->m_HeightInitial;
	param.szSource = "";//add by guo

	param.SetParentWnd(wnd);
	param.font_size = 15; // default

	m_pTipHolder->Create( param );
	m_pTipHolder->SetLockFlag( LOCK_LEFT|LOCK_RIGHT|LOCK_TOP|LOCK_BOTTOM );

	// it is a holder and never show
	m_pTipHolder->ShowWnd( false );
}
