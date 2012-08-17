#include "StdAfx.h"
#include "CameraCtrl\CTPSCamDelegate.h"
#include "CEventState.h"
#include "CMsg.h"
#include "CDataScene.h"
#include "CLogicDataMgr.h"
#include "CEditContext.h"
#include "CWindowTarget.h"

CTPSCamDelegate::CTPSCamDelegate(void)
{
}

CTPSCamDelegate::~CTPSCamDelegate(void)
{
}

void CTPSCamDelegate::_Init( CEventOwner* pOwner )
{
	m_pContext = dynamic_cast<CEditContext*>(pOwner);
	if(m_pContext)
	{
		_InitCameraInfo();
		m_pContext->SetCameraController(this);
		Active();
	}
}

void CTPSCamDelegate::_InitCameraInfo(void)
{
}

bool CTPSCamDelegate::MsgProc(const CMsg& msg)
{
	return CEventDelegate::MsgProc(msg);
}

CtrlState CTPSCamDelegate::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
{
	return CTPSCameraCtrller::_DoCameraCtrl( uCurTime, pCamera, pQuery );
}

