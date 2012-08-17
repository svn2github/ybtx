#include "StdAfx.h"
#include "CFPSCameraCtrller.h"
#include "BaseHelper.h"
#include "CCamera.h"
#include "CMath.h"
#include "CVector2.h"

CFPSCameraCtrller::CFPSCameraCtrller(void)
:m_XAngle(0.0f)
,m_YAngle(0.0f)
,m_MaxYAngle(179.0f)
,m_MinYAngle(1.0)
,m_MoveSpeed(1200.0f)
,m_XRate(0.2f) //大概每像素0.2度
,m_YRate(0.2f)
,m_XPixel(0)
,m_YPixel(0)
,m_State(STATE_STOP)
,m_UpdateUnit(true)
{
	m_Dir.x=0;
	m_Dir.y=0;
	m_Dir.z=0;
	m_CenterX=400;
	m_CenterY=300;
	m_ViewWidth=800.0f;
	m_ViewHeight=600.0f;
}

CFPSCameraCtrller::~CFPSCameraCtrller(void)
{
}

CtrlState CFPSCameraCtrller::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
{
	float pi=3.14159265357f;
	float fDeltaTime = float(uCurTime - m_uCurTime) * 0.001f;
	float MoveDis=fDeltaTime*m_MoveSpeed;
	m_uCurTime=uCurTime; 

	//像素需要和窗口的大小做个转换

	m_Dir.x=cos(m_XAngle*pi/180.0f);
	m_Dir.y=cos(m_YAngle*pi/180.0f);
	m_Dir.z=sin(m_XAngle*pi/180.0f);

	CVector3f UpVec(0,1,0);
	CVector3f xAliax=UpVec.Cross(m_Dir);
	xAliax.normalize();

	//处理平移
	switch(m_State)
	{
	case STATE_MOVE_RIGHT:
		{
			m_Pos+=xAliax*MoveDis;
			m_State=STATE_STOP;
			break;
		}
	case STATE_MOVE_LEFT:
		{
			m_Pos-=xAliax*MoveDis;
			m_State=STATE_STOP;
			break;
		}
	case STATE_MOVE_FORWAR:
		{
			m_Dir.y=0;
			m_Dir.normalize();
			m_Pos+=m_Dir*MoveDis;
			m_State=STATE_STOP;
			break;
		}
	case STATE_MOVE_BACK:
		{
			m_Dir.y=0;
			m_Dir.normalize();
			m_Pos-=m_Dir*MoveDis;
			m_State=STATE_STOP;
			break;
		}
	case STATE_PULL:
		{
			m_Pos-=m_Dir*MoveDis;
			m_State=STATE_STOP;
			break;
		}
	case STATE_PUSH:
		{
			m_Pos+=m_Dir*MoveDis;
			m_State=STATE_STOP;
			break;
		}
	case STATE_ROT:
		{
			//处理旋转
			m_XAngle+=m_XPixel*m_XRate;
			if(m_XAngle>360) m_XAngle-=360;
			if(m_XAngle<0) m_XAngle+=360;

			m_YAngle+=m_YPixel*m_YRate;
			m_YAngle=m_YAngle>m_MinYAngle?(m_YAngle<m_MaxYAngle?m_YAngle:m_MaxYAngle):m_MinYAngle;
			m_State=STATE_STOP;
		//	::SetCursorPos(m_CenterX,m_CenterY);
			break;
		}
	case STATE_STOP:
		{
		}
	}

	m_Dir.x=cos(m_XAngle*pi/180.0f);
	m_Dir.y=cos(m_YAngle*pi/180.0f);
	m_Dir.z=sin(m_XAngle*pi/180.0f);

	pCamera->SetPosition(m_Pos);
	pCamera->lookTo(m_Dir);

	return CS_CTRL;
}

void CFPSCameraCtrller::SetState(FPS_STATE _state)
{
	m_State=_state;
}