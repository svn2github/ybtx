#pragma once

#include "CEventDelegate.h"
#include "CFreeCameraCtrller.h"

namespace sqr_tools
{

class CEditContext;
class CFREECamDelegate: public CEventDelegate, public CFreeCameraCtrller
{
public:
	CFREECamDelegate(void);
	~CFREECamDelegate(void);
	
	bool MsgProc(const CMsg& msg);
	CtrlState _DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery );

protected:
	void _Init( CEventOwner* pOwner );
	void _InitCameraInfo(void);

	CEditContext* m_pContext;
	unsigned short m_mouseX;
	unsigned short m_mouseY;
};

//!namespace
}