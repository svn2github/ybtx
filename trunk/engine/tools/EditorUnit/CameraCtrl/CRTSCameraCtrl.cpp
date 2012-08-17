#include "stdafx.h"
#include "CameraCtrl\CRTSCameraCtrl.h"
#include "CameraCtrl\CRTSCamDelegate.h"
#include "CEditContext.h"

DEF_UI_IMP(CRTSCameraCtrl);

CRTSCameraCtrl::CRTSCameraCtrl(void* param)
{
	m_pRTSDele = new CRTSCamDelegate;
}

CRTSCameraCtrl::~CRTSCameraCtrl()
{
	GfkSafeDelete(m_pRTSDele);
}

void	CRTSCameraCtrl::SetContext(CEditContext* pContext)
{
	if(m_pOwnContext == pContext)
		return;
	
	m_pOwnContext = pContext;
	if(m_pOwnContext)
		m_pOwnContext->RegisterDelegate(m_pRTSDele);
}