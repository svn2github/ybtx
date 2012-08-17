#pragma once
#include "CCameraController.h"

namespace sqr
{
	class CTPSCameraCtrller : public CCameraController
	{	
	public:
		CTPSCameraCtrller();
	public://属性接口
		void	SetMaxXDegree( float dgree );
		void	SetMinXDegree( float dgree );
		void	SetMaxYDegree( float dgree );
		void	SetMinTargetDist( float dist );
		void	SetMaxTargetDist( float dist );
		void	SetMaxFov( float fov );
		void	SetMinFov( float fov );
		void	SetResetCtrlTime( uint32 time );
		//void	SetRotateSpeed( float speed );
		//void	SetMaxMoveDist( float dist ); 
		//void	SetMoveDistSpeed( float speed );

		float	GetMaxXDegree(void);
		float	GetMinXDegree(void);
		float	GetMaxYDegree(void);
		float	GetMaxTargetDist(void);
		float	GetMinTargetDist(void);
		float	GetMaxFov(void);
		float	GetMinFov(void);
		uint32	GetResetCtrlTime(void);
		//float	GetRotateSpeed(void);
		//float	GetMaxMoveDist(void);
		//float	GetMoveDistSpeed(void);


	public: //控制接口
		void	SetXRotateDegree( float dgree );
		void	SetYRotateDegree( float dgree );
		void	SetTargetDist( float dist );

		float	GetXRotateDegree(void);
		float	GetYRotateDegree(void);
		float	GetTargetDist(void);

		void	MoveTargetPos(CVector3f vec);
	protected:
		CtrlState	_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery );

		//属性
		float	m_fMaxXDegree;
		float	m_fMinXDegree;
		float	m_fMaxYDegree;
		float	m_fMaxTargetDist;
		float	m_fMinTargetDist;
		float	m_MinFov;
		float	m_MaxFov;

		//float	m_fRotateSpeed;
		//float	m_fMaxMoveDist;
		//float	m_fMoveDistSpeed;

		//控制接口
		float	m_fCurDist;
		float	m_fCurXDegree;
		float	m_fCurYDegree;
		//float	m_fMoveDist;
		float	m_fTargetDist;
		CQuaternion	m_CurQuat;

		CVector3f	m_PosNow;
		CVector3f	m_DirNow;
		float		m_fFov;
		float		m_CtrlTime;
		uint32		m_uResetCtrlTime;
	};

	inline float CTPSCameraCtrller::GetMaxXDegree(void)
	{ 
		return m_fMaxXDegree;
	}

	inline float CTPSCameraCtrller::GetMinXDegree(void)
	{
		return m_fMinXDegree;
	}

	inline float CTPSCameraCtrller::GetMaxYDegree(void)
	{
		return m_fMaxYDegree;
	}

	inline float CTPSCameraCtrller::GetMaxTargetDist(void)
	{
		return m_fMaxTargetDist;
	}

	inline float CTPSCameraCtrller::GetMinTargetDist(void)
	{
		return m_fMinTargetDist;
	}

	//inline float CTPSCameraCtrller::GetRotateSpeed(void)
	//{
	//	return m_fRotateSpeed;
	//}

	//inline float CTPSCameraCtrller::GetMaxMoveDist(void)
	//{
	//	return m_fMaxMoveDist;
	//}

	//inline float CTPSCameraCtrller::GetMoveDistSpeed(void)
	//{
	//	return m_fMoveDistSpeed;
	//}

	inline float CTPSCameraCtrller::GetXRotateDegree(void)
	{
		return m_fCurXDegree;
	}

	inline float CTPSCameraCtrller::GetYRotateDegree(void)
	{
		return m_fCurYDegree;
	}

	inline float CTPSCameraCtrller::GetTargetDist(void)
	{
		return m_fTargetDist;
	}

	inline float CTPSCameraCtrller::GetMaxFov(void)
	{
		return m_MaxFov;
	}

	inline float CTPSCameraCtrller::GetMinFov(void)
	{
		return m_MinFov;
	}

	inline uint32 CTPSCameraCtrller::GetResetCtrlTime(void)
	{
		return m_uResetCtrlTime;
	}
}