#pragma once

#include "CCameraController.h"
#include "CEventDelegate.h"
#include "CRecordStruct.h"

namespace sqr_tools
{
class CEditContext;
class CRecordCamDelegate: public CEventDelegate, public CCameraController
{
public:
	CRecordCamDelegate(void);
	~CRecordCamDelegate(void);
	
	void SetAnimate(RecordAnimate _animate);
	void SetPlay(bool _play);

	bool MsgProc(const CMsg& msg);
	CtrlState _DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery );

protected:
	CEditContext* m_pContext;
	void		_Init(CEventOwner* pOwner);
	void		_InitCameraInfo(void);
	void		_LineLerpCam();
	//时间
	int			m_TotalTime;	//总时长 毫秒
	int			m_PassTime;		//播放时长 毫秒
	
	//还是在这里在存放一定量的数据吧
	RecordAnimate	m_CurAnimate;
	
	//当前数据
	CVector3f	curPos;
	CVector3f	curDir;
	CVector3f	curTarget;

	bool		m_Play;

};

//!namespace
}
