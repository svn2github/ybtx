#pragma once
#include "CVector3.h"
#include "CNodeController.h"
#include "CCamera.h"
#include "CSceneQuery.h"

enum ECamMode
{
	ECam_Mode16_9	= 0,
	ECam_Mode16_10	= 1,
	ECam_Mode4_3	= 2,
	ECam_Mode5_4	= 3,
	ECam_ModeDefault,
};

namespace sqr
{
	class CCameraController : public CNodeController
	{
	public:
		 void SetTargetPos(const CVector3f& vec);
		 void SetTargetPos(float x, float y,float z);
		 CVector3f	GetTargetPos( void );
		 CtrlState	DoQueryCtrl( uint32 uCurTime, CSceneQuery* pQuery = NULL );
	protected:
		 bool _IsValid( CCtrlNode* pNode );
		 CtrlState	_DoCtrl( uint32 uCurTime );
		
		 CVector3f	m_TargetPos;
	protected:
		 virtual CtrlState	_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery ) = 0;
	};

	inline CVector3f CCameraController::GetTargetPos( void )
	{
		return m_TargetPos;
	}

	inline  CtrlState CCameraController::_DoCtrl( uint32 uCurTime )
	{
		CtrlState cs = _DoCameraCtrl(uCurTime,(CCamera*)m_pCtrlNode, NULL);
		return cs;
	}

	inline  CtrlState	 CCameraController::DoQueryCtrl( uint32 uCurTime, CSceneQuery* pQuery)
	{
		if(NULL==m_pCtrlNode || !m_isInCtrl)
			return CS_STOP;

		CtrlState ret = _DoCameraCtrl(uCurTime,(CCamera*)m_pCtrlNode, pQuery);
		m_uCurTime = uCurTime;
		return ret;
	}
}
