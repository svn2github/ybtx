#pragma once
#include "CRTSCameraCtrller.h"
#include "CEventDelegate.h"

namespace sqr_tools
{
	class CEditContext;
	class CRTSCamDelegate 
		: public CEventDelegate
		, public CRTSCameraCtrller
	{
	public:
		CRTSCamDelegate(void);
		~CRTSCamDelegate(void);

		bool MsgProc(const CMsg& msg);
		
	protected:
		CtrlState	_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery );
		void		_Init(CEventOwner* pOwner);
		void		_InitCameraInfo(void);

		float	m_fMouseRadian;
		float	m_fMoveXDist;
		float	m_fMoveZDist;
	
	protected:
		float	m_fRadianSpeed;
		float	m_fDistSpeed;
		float	m_fMoveSpeed;

		bool	m_bAutoHight;
		float	m_fCameraHight;
	protected:
		CEditContext* m_pContext;
	};
}