#pragma once

#include "CCtrlBase.h"

namespace sqr_tools
{
class CFPSCamDelegate;
class CFPSCameraCtrl :public CCtrlBase
{
	REG_UI_IMP(CFPSCameraCtrl);
public:
	CFPSCameraCtrl(void* param);
	~CFPSCameraCtrl(void);

	void	SetContext(CEditContext* pContext);
	CFPSCamDelegate* m_pFPSDele;
};

}
