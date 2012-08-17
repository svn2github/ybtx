#pragma once
#include "CCtrlBase.h"

namespace sqr_tools
{
	class CRTSCamDelegate;
	class CRTSCameraCtrl : public CCtrlBase
	{
		REG_UI_IMP(CRTSCameraCtrl);
	public:
		CRTSCameraCtrl(void* param);
		~CRTSCameraCtrl();

		void	SetContext(CEditContext* pContext);
	protected:
		CRTSCamDelegate*	m_pRTSDele;
	};
}