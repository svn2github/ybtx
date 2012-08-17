#pragma once

#include "CTPSCameraCtrller.h"
#include "CEventDelegate.h"

namespace sqr_tools
{

class CEditContext;
class CTPSCamDelegate : public CEventDelegate, public CTPSCameraCtrller
{
public:
	CTPSCamDelegate(void);
	~CTPSCamDelegate(void);

	bool MsgProc(const CMsg& msg);
	CtrlState _DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery );
protected:
	CEditContext* m_pContext;
	void _Init( CEventOwner* pOwner );
	void _InitCameraInfo(void);

};

//!namespace
}
