#pragma once

#include "CFPSCameraCtrller.h"
#include "CEventDelegate.h"

namespace sqr_tools
{
	class CEditContext;

class CFPSCamDelegate: public CEventDelegate, public CFPSCameraCtrller
{
public:
	CFPSCamDelegate(void);
	~CFPSCamDelegate(void);

	bool MsgProc(const CMsg& msg);
	CtrlState _DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery );

protected:
	CEditContext* m_pContext;
	unsigned short m_mouseX;
	unsigned short m_mouseY;

	void _Init( CEventOwner* pOwner );
	void _InitCameraInfo(void);
};

}