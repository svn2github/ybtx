#include "StdAfx.h"
#include "CameraCtrl\CFPSCamDelegate.h"
#include "CEventState.h"
#include "CMsg.h"
#include "CDataScene.h"
#include "CLogicDataMgr.h"
#include "CEditContext.h"
#include "CWindowTarget.h"

CFPSCamDelegate::CFPSCamDelegate(void)
{
	m_mouseX=0;
	m_mouseY=0;
}

CFPSCamDelegate::~CFPSCamDelegate(void)
{
}

void CFPSCamDelegate::_Init( CEventOwner* pOwner )
{
	m_pContext = dynamic_cast<CEditContext*>(pOwner);
	if(m_pContext)
	{
		_InitCameraInfo();
		m_pContext->SetCameraController(this);
		Active();
	}
}

void CFPSCamDelegate::_InitCameraInfo(void)
{
	if(m_pContext)
	{
		CIRect rtClient;
		CWindowTarget* pTarget = m_pContext->GetWindowTarget();
		if(pTarget==NULL)
			return;

		pTarget->GetRect(rtClient);
		if (rtClient.Width() == 0 || rtClient.Height() == 0)
			return;

		int	idx	= (int)(ECam_Mode16_10);
		if (rtClient.Width() != 0.0f)
		{
			float ratio[4] = { 9.0f/16, 10.0f/16, 3.0f/4, 4.0f/5 };
			float dis	= 100.0f;
			float cur_ratio =  ratio[0];
			cur_ratio = float(rtClient.Height()) / float(rtClient.Width());
			for (int i = 0; i < 4; i++)
			{
				if ( dis > abs(cur_ratio - ratio[i]) )
				{
					dis = abs(cur_ratio - ratio[i]);
					idx = i;
				}
			}
		}
		ECamMode cm = (ECamMode)idx;

		this->SetPostion(1000,1000,1000);
		this->SetXAngle(0.0f);
		this->SetYAngle(145.0f);
		//this->SetMaxXZ_Dist( CCameraConfig::GetInst()->GetCamMaxHOffset(cm) * eGridSpan );
		//this->SetMinDist( CCameraConfig::GetInst()->GetCamMinDist(cm) * eGridSpan );
		//this->SetMaxDist( CCameraConfig::GetInst()->GetCamMaxDist(cm) * eGridSpan );
		//this->SetDist(	CCameraConfig::GetInst()->GetCamDefDist(cm)* eGridSpan );
		//this->SetMaxRadian( CCameraConfig::GetInst()->GetCamMaxXRadian(cm) );
		//this->SetMinRadian( CCameraConfig::GetInst()->GetCamMinXRadian(cm) );
		//this->SetMinFov( CCameraConfig::GetInst()->GetCamMinFovRadian(cm) );
		//this->SetMaxFov( CCameraConfig::GetInst()->GetCamMaxFovRadian(cm) );
		//this->SetMinYOffset( CCameraConfig::GetInst()->GetCamMinYOffset(cm) * eGridSpan );
		//this->SetMaxYOffset( CCameraConfig::GetInst()->GetCamMaxYOffset(cm) * eGridSpan );
		//this->SetYRadian( CCameraConfig::GetInst()->GetCamYRadian(cm) );
		//this->SetMaxNearPlan( CCameraConfig::GetInst()->GetCamMaxNearPlan(cm) * eGridSpan );
		//this->SetMinNearPlan( CCameraConfig::GetInst()->GetCamMinNearPlan(cm) * eGridSpan );
		//this->SetDistSpeed( CCameraConfig::GetInst()->GetCamSpeed(cm) * eGridSpan );
		//this->ReSetPosNow();
	}
}

bool CFPSCamDelegate::MsgProc(const CMsg& msg)
{
	switch( msg.msgSource )
	{
	case MGS_SYSMSG:
		{
			switch(msg.msgType.sys_type)
			{
			case MGT_SYS_RESIZE:
				_InitCameraInfo();
				break;
			}
		}
		break;
	case MGS_MOUSEMSG:
		{
			switch(msg.msgType.mouse_type)
			{
			case MGT_MOUSE_LBNDOWN:
				{
 					m_mouseX=msg.msgInfo.mouse_info.X;
 					m_mouseY=msg.msgInfo.mouse_info.Y;
					break;
				}
			case MGT_MOUSE_LBNUP:
				{
					this->SetState(STATE_STOP);
					break;
				}
			case MGT_MOUSE_MOVE:
				if(CEventState::GetInst()->GetState(MGT_MOUSE_LBUTTON) == MGT_MOUSE_LBNDOWN)
				{
					this->SetState(STATE_ROT);
					unsigned short tmpMouseX=msg.msgInfo.mouse_info.X;
					unsigned short tmpMouseY=msg.msgInfo.mouse_info.Y;
					this->SetXPixel(float(tmpMouseX-m_mouseX));
					this->SetYPixel(float(tmpMouseY-m_mouseY));
					m_mouseX=tmpMouseX;
					m_mouseY=tmpMouseY;
				}
				break;
			case MGT_MOUSE_WHEEL:
				{
				}
				break;
			default:
				break;
			}
			return true;
		}
		break;
	case MGS_KEYMSG:
		{
			switch(msg.msgType.key_type)
			{
			case MGT_KEY_DOWN:
				{
					switch(msg.msgInfo.key_info)
					{
					case MGI_KEY_W:
						{
							this->SetState(STATE_MOVE_FORWAR);
						}
						break;
					case MGI_KEY_S:
						{
							this->SetState(STATE_MOVE_BACK);
						}
						break;
					case MGI_KEY_D:
						{
							this->SetState(STATE_MOVE_RIGHT);
						}
						break;
					case MGI_KEY_A:
						{
							this->SetState(STATE_MOVE_LEFT);
						}
						break;
					case MGI_KEY_Q:
						{
							this->SetState(STATE_PULL);
						}
						break;
					case MGI_KEY_E:
						{
							this->SetState(STATE_PUSH);
						}
						break;
					default:
						break;
					}
				}
				break;
			case MGT_KEY_UP:
				{
					switch(msg.msgInfo.key_info)
					{
					case MGI_KEY_A:
					case MGI_KEY_D:
					case MGI_KEY_W:
					case MGI_KEY_S:
					case MGI_KEY_Q:
					case MGI_KEY_E:
						{
							this->SetState(STATE_STOP);
						}	
						break;
					default:
						break;
					}
					break;	
				}
				break;
			default:
				break;
			}
			return false;
		}
	case MGS_UIMSG:
		{
			switch(msg.msgType.ui_type)
			{
			case MGT_UI_SIZE:
				{
					int a=0;
				}
				break;
			}
		}
	default:
		break;
	}
	return CEventDelegate::MsgProc(msg);
}

CtrlState CFPSCamDelegate::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
{
	return CFPSCameraCtrller::_DoCameraCtrl( uCurTime, pCamera, pQuery );
}
