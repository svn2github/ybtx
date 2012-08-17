#include "stdafx.h"
#include "CCameraManager.h"
#include "CEventState.h"
#include "CTPSCameraCtrller.h"

namespace sqr
{
	CCameraManager::CCameraManager()
	{
		m_pTPSCtrller = NULL;
		Init();
	}

	CCameraManager::~CCameraManager()
	{
	}

	void CCameraManager::Init()
	{
		m_pTPSCtrller = new CTPSCameraCtrller;
		m_eCtrlState = CS_OUTCTRL;

		m_pTPSCtrller->SetXRotateDegree(CMath::qtr_pi);
		m_pTPSCtrller->SetYRotateDegree(CMath::qtr_pi + CMath::half_pi);
		m_pTPSCtrller->SetTargetDist(640.0f);
		m_pTPSCtrller->SetResetCtrlTime(0);
	}

	void CCameraManager::EventProcess(const CMsg& msg)
	{
		POINT pt;
		MSG_MOUSE_INF inf = CEventState::GetInst()->GetMousePosition();
		pt.x = inf.X;
		pt.y = inf.Y;

		switch ( msg.msgSource )
		{
		case MGS_KEYMSG:
			{
				break;
			}
		case MGS_MOUSEMSG:
			{
				switch(msg.msgType.mouse_type)
				{
				case MGT_MOUSE_WHEEL:
					{
						m_eCtrlState = CS_ZOOM;
						this->Zoom(msg.msgInfo._iinfo*2);
					}
					break;
			
				case MGT_MOUSE_MOVE :
					{
						this->Move(pt);
					}
					break;

				case MGT_MOUSE_LBNDOWN:	
					{
						m_eCtrlState = CS_MOVE;
					}
					break;

				case MGT_MOUSE_LBNUP:	
					{
						m_eCtrlState = CS_OUTCTRL;
					}
					break;

				case MGT_MOUSE_RBNDOWN:	
					{
						m_eCtrlState = CS_ROTATE;
					}
					break;

				case MGT_MOUSE_RBNUP:	
					{
						m_eCtrlState = CS_OUTCTRL;
					}
					break;

				case MGT_MOUSE_LBNDCLK:	
					{
						m_eCtrlState = CS_OUTCTRL;
						this->LButtonDblClk();
					}
					break;
				}
			}	
		default:
			break;
		}

		m_CurPoint = pt;
	}

	CTPSCameraCtrller* CCameraManager::GetTPSCameraCtrller()
	{
		return m_pTPSCtrller;
	}

	void CCameraManager::Zoom( short zDelta )
	{
		float fTargetDist = m_pTPSCtrller->GetTargetDist();
		m_pTPSCtrller->SetTargetDist( fTargetDist + zDelta );
	}

	void CCameraManager::LButtonDblClk()
	{
		m_pTPSCtrller->SetXRotateDegree(CMath::qtr_pi);
		m_pTPSCtrller->SetYRotateDegree(CMath::qtr_pi + CMath::half_pi);
		m_pTPSCtrller->SetTargetDist(640.0f);
		m_pTPSCtrller->SetTargetPos(CVector3f::ZERO);
	}

	void CCameraManager::Move(const POINT point)
	{
		switch( m_eCtrlState )
		{
		case CS_ROTATE:
			{
				m_pTPSCtrller->SetXRotateDegree( m_pTPSCtrller->GetXRotateDegree() + float(point.y - m_CurPoint.y)/100.0f );
				m_pTPSCtrller->SetYRotateDegree( m_pTPSCtrller->GetYRotateDegree() + float(point.x - m_CurPoint.x)/100.0f );
			}
			break;
		case CS_MOVE:
			{
				m_pTPSCtrller->MoveTargetPos(  CVector3f((point.x - m_CurPoint.x)*0.5f,(m_CurPoint.y - point.y)*0.5f,0.0f) );
			}
			break;
		default:
			break;
		}
	}
}