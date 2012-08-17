#include "StdAfx.h"
#include "CameraCtrl\CFPSCameraCtrl.h"
#include "CameraCtrl\CFPSCamDelegate.h"
#include "CEditContext.h"

DEF_UI_IMP(CFPSCameraCtrl);

CFPSCameraCtrl::CFPSCameraCtrl(void* param)
{
	m_pFPSDele = new CFPSCamDelegate;
}

CFPSCameraCtrl::~CFPSCameraCtrl(void)
{
	GfkSafeDelete(m_pFPSDele);
}

void CFPSCameraCtrl::SetContext(CEditContext* pContext)
{
	if(m_pOwnContext == pContext)
		return;
	
	m_pOwnContext = pContext;
	if(m_pOwnContext)
		m_pOwnContext->RegisterDelegate(m_pFPSDele);
}
