#include "StdAfx.h"
#include "CameraCtrl\CFREECameraCtrl.h"
#include "CameraCtrl\CFREECamDelegate.h"
#include "CEditContext.h"

DEF_UI_IMP(CFREECameraCtrl);

CFREECameraCtrl::CFREECameraCtrl(void* param)
{
	m_pFREEDele=new CFREECamDelegate();
}

CFREECameraCtrl::~CFREECameraCtrl(void)
{
	GfkSafeDelete(m_pFREEDele);
}

void CFREECameraCtrl::SetContext(CEditContext* pContext)
{
	if(m_pOwnContext == pContext)
		return;
	
	m_pOwnContext = pContext;
	if(m_pOwnContext)
		m_pOwnContext->RegisterDelegate(m_pFREEDele);
}
