#pragma once
#include "TSingleton.h"
#include "CMsg.h"


namespace sqr
{
	class CTPSCameraCtrller;
	class CCameraManager : public Singleton<CCameraManager>
	{
		enum CtrlState
		{
			CS_OUTCTRL,
			CS_MOVE,
			CS_ZOOM,
			CS_ROTATE,
		};

	private:
		CTPSCameraCtrller*	m_pTPSCtrller;
		CtrlState			m_eCtrlState;
		POINT				m_CurPoint;

	public:
		CCameraManager();
		~CCameraManager();
		void Init();
		void EventProcess(const CMsg& msg);

		CTPSCameraCtrller* GetTPSCameraCtrller();

		void Zoom(short zDelta);
		void LButtonDblClk();
		void Move(const POINT point);
	};
}