#include "StdAfx.h"
#include "CameraCtrl\CRecordCamDelegate.h"
#include "CEventState.h"
#include "CMsg.h"
#include "CDataScene.h"
#include "CLogicDataMgr.h"
#include "CEditContext.h"
#include "CWindowTarget.h"

CRecordCamDelegate::CRecordCamDelegate(void)
{
	m_PassTime=100000000;
	m_Play=true;
}

CRecordCamDelegate::~CRecordCamDelegate(void)
{
	m_CurAnimate.keylist.clear();
}
void CRecordCamDelegate::_LineLerpCam()
{
	float		curFovy=45.0f;
	KeyInfo		forwardKey,backKey;
	forwardKey=m_CurAnimate.keylist[0];
	backKey=m_CurAnimate.keylist[0];
	for(unsigned int i=0;i<m_CurAnimate.keylist.size();i++)
	{
		if(m_CurAnimate.keylist[i].time*1000>m_PassTime)
		{
			if(	i>=1)
			{
				backKey=m_CurAnimate.keylist[i];
				forwardKey=m_CurAnimate.keylist[i-1];
				break;
			}
		}
	}
	//插值计算
	if(backKey.time!=forwardKey.time)
	{
		float lerpNum=(m_PassTime*0.001f-forwardKey.time)*1.0f/(backKey.time-forwardKey.time);

		curPos.x=forwardKey.pos.x*(1-lerpNum)+backKey.pos.x*lerpNum;
		curPos.y=forwardKey.pos.y*(1-lerpNum)+backKey.pos.y*lerpNum;
		curPos.z=forwardKey.pos.z*(1-lerpNum)+backKey.pos.z*lerpNum;

		curDir.x=forwardKey.dir.x*(1-lerpNum)+backKey.dir.x*lerpNum;
		curDir.y=forwardKey.dir.y*(1-lerpNum)+backKey.dir.y*lerpNum;
		curDir.z=forwardKey.dir.z*(1-lerpNum)+backKey.dir.z*lerpNum;

		//角度 不应该用线行差值 所以暂时不做

	}
	else
	{
		curPos=backKey.pos;
		curDir=backKey.dir;
		curFovy=backKey.fov;
	}
}

CtrlState CRecordCamDelegate::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
{
	if(!m_Play)
		return CS_CTRL;
	//计算位置
	if(m_PassTime<m_TotalTime)
	{
		int deltaTime= int(uCurTime - m_uCurTime);
		m_PassTime+=deltaTime;
		_LineLerpCam();
		if(pCamera)
		{
			pCamera->SetPosition(curPos);
			pCamera->lookTo(curDir);
		}
	}
	return CS_CTRL;
}

void CRecordCamDelegate::_Init(CEventOwner* pOwner)
{
	m_pContext = dynamic_cast<CEditContext*>(pOwner);
	if(m_pContext)
	{
		_InitCameraInfo();
		m_pContext->SetCameraController(this);
		Active();
	}
}

void CRecordCamDelegate::_InitCameraInfo(void)
{
	m_PassTime=0;
}

void CRecordCamDelegate::SetPlay(bool _play)
{
	m_Play=_play;
}

void CRecordCamDelegate::SetAnimate(RecordAnimate _animate)
{
	m_CurAnimate.keylist.clear();
	m_CurAnimate.id=_animate.id;
	m_CurAnimate.timelong=_animate.timelong;
	m_CurAnimate.tip=_animate.tip;
	for(unsigned int i=0;i<_animate.keylist.size();i++)
	{
		m_CurAnimate.keylist.push_back(_animate.keylist[i]);
	}
	m_TotalTime=m_CurAnimate.timelong*1000;
	m_PassTime=0;
}

bool CRecordCamDelegate::MsgProc(const CMsg& msg)
{
	return CEventDelegate::MsgProc(msg);
}
