#pragma once
#include "CCtrlBase.h"

namespace sqr_tools
{
	class CFREECamDelegate;

	class CFREECameraCtrl : public CCtrlBase
	{
		REG_UI_IMP(CRTSCameraCtrl);
	public:
		CFREECameraCtrl(void* param);
		~CFREECameraCtrl(void);

		void	SetContext(CEditContext* pContext);
		CFREECamDelegate* m_pFREEDele;
	};

//!namespace
}
