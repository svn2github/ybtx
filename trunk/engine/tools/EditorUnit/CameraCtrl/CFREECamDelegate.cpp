#include "StdAfx.h"
#include "CameraCtrl\CFREECamDelegate.h"
#include "CEventState.h"
#include "CMsg.h"
#include "CDataScene.h"
#include "CLogicDataMgr.h"
#include "CEditContext.h"
#include "CWindowTarget.h"


CFREECamDelegate::CFREECamDelegate(void)
{
}

CFREECamDelegate::~CFREECamDelegate(void)
{
}

void CFREECamDelegate::_Init( CEventOwner* pOwner )
{
	m_pContext = dynamic_cast<CEditContext*>(pOwner);
	if(m_pContext)
	{
		_InitCameraInfo();
		m_pContext->SetCameraController(this);
		Active();
	}
}

void CFREECamDelegate::_InitCameraInfo(void)
{
	if(m_pContext)
	{
		CWindowTarget* pTarget = m_pContext->GetWindowTarget();
		this->SetNewTargetPos(1000,0,1000);
		this->SetXAngle(0.0f);
		this->SetYAngle(145.0f);
	}
}

CtrlState CFREECamDelegate::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
{
	float pi=3.14159265357f;
	float fDeltaTime = float(uCurTime - m_uCurTime) * 0.001f;
	float MoveDis=fDeltaTime*m_MoveSpeed;
	float PullDis=fDeltaTime*m_PullSpeed;
	m_uCurTime=uCurTime;

	m_Dir.x=cos(m_XAngle*pi/180.0f);
	m_Dir.y=cos(m_YAngle*pi/180.0f);
	m_Dir.z=sin(m_XAngle*pi/180.0f);
	m_Dir.normalize();

	CVector3f dir_xoz;
	dir_xoz.y=0;
	dir_xoz.x=m_Dir.x;
	dir_xoz.z=m_Dir.z;
	dir_xoz.normalize();
	
	CVector3f UpVec(0,1,0);
	CVector3f xAliax=UpVec.Cross(m_Dir);
	xAliax.y=0;
	xAliax.normalize();

	//处理平移
	switch(m_State)
	{
	case FREECAM_MOVE_RIGHT:
		{
			m_NewTargetPos+=xAliax*MoveDis;
			m_State=FREECAM_STOP;
			break;
		}
	case FREECAM_MOVE_LEFT:
		{
			m_NewTargetPos-=xAliax*MoveDis;
			m_State=FREECAM_STOP;
			break;
		}
	case FREECAM_MOVE_FORWAR:
		{
			m_NewTargetPos.x+=dir_xoz.x*MoveDis;
			m_NewTargetPos.z+=dir_xoz.z*MoveDis;
			m_State=FREECAM_STOP;
			break;
		}
	case FREECAM_MOVE_BACK:
		{
			m_NewTargetPos.x-=dir_xoz.x*MoveDis;
			m_NewTargetPos.z-=dir_xoz.z*MoveDis;
			m_State=FREECAM_STOP;
			break;
		}
	case FREECAM_PULL:
		{
			m_Length+=PullDis;
			m_State=FREECAM_STOP;
			break;
		}
	case FREECAM_PUSH:
		{
			m_Length-=PullDis;
			if(m_Length<10.0f)	//最近10米
				m_Length=10.0f;	
			m_State=FREECAM_STOP;
			break;
		}
	case FREECAM_PULL_MOUSE:
	case FREECAM_PUSH_MOUSE:
		{
			m_Length+=(m_YPixel*5.0f);
			if(m_Length<50.0f)	//最近10米
				m_Length=50.0f;	
			m_State=FREECAM_STOP;
			break;
		}
	case FREECAM_ROT:
		{
			//处理旋转
			m_XAngle-=m_XPixel*m_XRate;
			if(m_XAngle>360) m_XAngle-=360;
			if(m_XAngle<0) m_XAngle+=360;
			m_YAngle+=m_YPixel*m_YRate;
			m_YAngle=m_YAngle>m_MinYAngle?(m_YAngle<m_MaxYAngle?m_YAngle:m_MaxYAngle):m_MinYAngle;
			m_State=FREECAM_STOP;
			break;
		}
	case FREECAM_STOP:
		{
		}
	}
	m_Dir.x=cos(m_XAngle*pi/180.0f);
	m_Dir.y=cos(m_YAngle*pi/180.0f);
	m_Dir.z=sin(m_XAngle*pi/180.0f);
	m_Dir.normalize();

	CVector3f m_Pos;
	m_Pos=m_NewTargetPos-m_Length*m_Dir;
	SetNewTargetPos(m_NewTargetPos.x,m_NewTargetPos.y,m_NewTargetPos.z);
	pCamera->SetPosition(m_Pos);
	pCamera->lookTo(m_Dir);
	return CFreeCameraCtrller::_DoCameraCtrl( uCurTime, pCamera, pQuery );
}

bool CFREECamDelegate::MsgProc(const CMsg& msg)
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
			case MGT_MOUSE_RBNDOWN:
				{
					m_mouseX=msg.msgInfo.mouse_info.X;
 					m_mouseY=msg.msgInfo.mouse_info.Y;
					this->SetState(FREECAM_ROT);
					break;
				}
			case MGT_MOUSE_RBNUP:
				{
					this->SetState(FREECAM_STOP);
					break;
				}
			case MGT_MOUSE_MBNDOWN:
				{
					m_mouseX=msg.msgInfo.mouse_info.X;
 					m_mouseY=msg.msgInfo.mouse_info.Y;
					this->SetState(FREECAM_ROT);
					break;
				}
			case MGT_MOUSE_MBNUP:
				{
					this->SetState(FREECAM_STOP);
					break;
				}
			case MGT_MOUSE_MOVE:
				{
					unsigned short tmpMouseX=msg.msgInfo.mouse_info.X;
					unsigned short tmpMouseY=msg.msgInfo.mouse_info.Y;
					if(CEventState::GetInst()->GetState(MGT_MOUSE_RBUTTON) == MGT_MOUSE_RBNDOWN)
					{
						this->SetXPixel(float(tmpMouseX-m_mouseX));
						this->SetYPixel(float(tmpMouseY-m_mouseY));
						this->SetState(FREECAM_ROT);
					}
					if(CEventState::GetInst()->GetState(MGT_MOUSE_MBUTTON) == MGT_MOUSE_MBNDOWN)
					{
						this->SetYPixel(float(tmpMouseY-m_mouseY));
						if((tmpMouseY-m_mouseY)<0)
							this->SetState(FREECAM_PUSH_MOUSE);
						else
							this->SetState(FREECAM_PULL_MOUSE);
					}
					m_mouseX=tmpMouseX;
					m_mouseY=tmpMouseY;
					break;
				}
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
							this->SetState(FREECAM_MOVE_FORWAR);
						}
						break;
					case MGI_KEY_S:
						{
							this->SetState(FREECAM_MOVE_BACK);
						}
						break;
					case MGI_KEY_D:
						{
							this->SetState(FREECAM_MOVE_RIGHT);
						}
						break;
					case MGI_KEY_A:
						{
							this->SetState(FREECAM_MOVE_LEFT);
						}
						break;
					case MGI_KEY_Q:
						{
							this->SetState(FREECAM_PULL);
						}
						break;
					case MGI_KEY_E:
						{
							this->SetState(FREECAM_PUSH);
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
							this->SetState(FREECAM_STOP);
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
