#pragma once
#include "CCameraController.h"

namespace sqr
{
	class CRTSCameraCtrller : public CCameraController
	{	
	public:
		CRTSCameraCtrller();
		void	SetDist( float dist );
		void	SetYRadian( float degree );
		void	SetResetCtrlTime( uint32 time );
		void	SetDistSpeed( float Speed );
		//void	SetCameraSpeed( float Speed );
		void	SetYRotateDegree( float dgree );

		//属性设置
		void	SetMaxCameraSpeed( float MaxSpeed );
		void	SetMaxXZ_Dist( float MaxXZDist );
		void	SetMinDist(float MinDist);
		void	SetMaxDist(float MaxDist);

		void	SetMaxFov( float fov );
		void	SetMinFov( float fov );

		void	SetMaxRadian( float degree );
		void	SetMinRadian( float degree );

		void	SetMaxNearPlan( float NearPlan);
		void	SetMinNearPlan( float NearPlan);

		void	SetMinYOffset( float offset );
		void	SetMaxYOffset( float offset );
		void	SetMaxYDegree( float dgree );

		//获取参数
		float	GetDist(void);
		float	GetYRadian(void);
		uint32	GetResetCtrlTime(void);
		float	GetDistSpeed(void);
		//float	GetCameraSpeed( void );
		float	GetYRotateDegree( void );

		float	GetMaxCameraSpeed( void);

		float	GetMaxXZ_Dist( void );
		float 	GetMinDist( void );
		float	GetMaxDist( void );

		float	GetMaxNearPlan( void ) const;
		float	GetMinNearPlan( void ) const;

		float	GetMaxFov( void );
		float	GetMinFov( void );

		float	GetMaxRadian( void );
		float	GetMinRadian( void );

		float	GetMinYOffset( void );
		float	GetMaxYOffset( void );
		float	GetMaxYDegree(void);
		void	ReSetPosNow();
	protected:
		CtrlState	_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery );
		void		_InitBind( void );

		//当前值
		float	m_fDistSpeed;
		//float	m_fSpeed;	
		float	m_fDist;
		float	m_fFov;
		float	m_fNearPlan;
		float	m_fRadian;
		float	m_fYOffset;
		float	m_fYRadian;
		float	m_fYDegree;
		float	m_fCurYDegree;
		uint32	m_uResetCtrlTime;

		//限制值
		float	m_MaxSpeed;
		float	m_MaxXZDist;
		float	m_MaxDist;
		float	m_MinDist;
		float	m_MinFov;
		float	m_MaxFov;
		float	m_MinRadian;
		float	m_MaxRadian;
		float	m_MaxYOffset;
		float	m_MinYOffset;
		float	m_MaxYDegree;
		float	m_MinNearPlan;
		float	m_MaxNearPlan;

		//计算值
		CVector3f	m_TargetNow;
		CVector3f	m_PosNow;
		CVector3f	m_DirNow;
		float		m_fDistNow;
		float		m_CtrlTime;
		bool		m_bFollow;
		float		m_fMoveDist;
	protected:
		//CVector3f	m_MaxDir;
		//CVector3f	m_MinDir;
		CVector3f	m_TarDir;

		bool		m_NeedReQuery;
		//void		_UpdateMaxDir(void);
		//void		_UpdateMinDir(void);
		void		_UpdateTarDir(void);
	};

	inline void	CRTSCameraCtrller::SetYRadian( float degree )
	{
		m_fYRadian = degree;
		//_UpdateMaxDir();
		//_UpdateMinDir();
		_UpdateTarDir();
	}

	inline void CRTSCameraCtrller::SetResetCtrlTime( uint32 time )
	{
		m_uResetCtrlTime = time;
	}

	inline void CRTSCameraCtrller::SetDistSpeed( float Speed )
	{
		m_fDistSpeed = Speed;
	}

	//inline void	CRTSCameraCtrller::SetCameraSpeed( float Speed )
	//{
	//	m_fSpeed = Speed;
	//}

	inline void	CRTSCameraCtrller::SetMaxXZ_Dist(float MaxXZDist)
	{
		m_MaxXZDist = MaxXZDist;
	}

	inline void	CRTSCameraCtrller::SetMaxRadian( float degree )
	{
		m_MaxRadian = degree;
		//_UpdateMaxDir();
	}

	inline void	CRTSCameraCtrller::SetMinRadian( float degree )
	{
		m_MinRadian = degree;
		//_UpdateMinDir();
	}

	inline void	CRTSCameraCtrller::SetMinYOffset( float offset )
	{
		m_MinYOffset = offset;
		//_UpdateMinDir();
	}

	inline void	CRTSCameraCtrller::SetMaxYOffset( float offset )
	{
		m_MaxYOffset = offset;
		//_UpdateMaxDir();
	}

	inline void CRTSCameraCtrller::SetMinDist( float MinDist )
	{
		m_MinDist = MinDist;
		//_UpdateMinDir();
	}

	inline void	CRTSCameraCtrller::SetMaxDist( float MaxDist )
	{
		m_MaxDist = MaxDist;
		//_UpdateMaxDir();
	}

	inline void	CRTSCameraCtrller::SetMaxNearPlan( float NearPlan )
	{
		m_MaxNearPlan = NearPlan;
	}

	inline void	CRTSCameraCtrller::SetMinNearPlan( float NearPlan )
	{
		m_MinNearPlan = NearPlan;
	}

	inline float	CRTSCameraCtrller::GetDist(void)
	{
		return m_fDistNow;
	}

	inline float	CRTSCameraCtrller::GetYRadian(void)
	{
		return m_fYRadian;
	}

	inline uint32	CRTSCameraCtrller::GetResetCtrlTime(void)
	{
		return m_uResetCtrlTime;
	}

	inline float	CRTSCameraCtrller::GetDistSpeed( void )
	{
		return m_fDistSpeed;
	}

	//inline float	CRTSCameraCtrller::GetCameraSpeed( void )
	//{
	//	return m_fSpeed;
	//}

	inline float	CRTSCameraCtrller::GetMaxCameraSpeed( void)
	{
		return m_MaxSpeed;
	}

	inline float	CRTSCameraCtrller::GetMaxXZ_Dist( void )
	{
		return m_MaxXZDist;
	}

	inline float	CRTSCameraCtrller::GetMaxDist( void )
	{
		return m_MaxDist;
	}

	inline float	CRTSCameraCtrller::GetMinDist(void)
	{
		return m_MinDist;
	}

	inline float	CRTSCameraCtrller::GetMaxFov( void )
	{
		return m_MaxFov;
	}

	inline float	CRTSCameraCtrller::GetMinFov( void )
	{
		return m_MinFov;
	}

	inline float	CRTSCameraCtrller::GetMaxRadian( void )
	{
		return m_MaxRadian;
	}

	inline float	CRTSCameraCtrller::GetMinRadian( void )
	{
		return m_MinRadian;
	}

	inline float	CRTSCameraCtrller::GetMinYOffset( void )
	{
		return m_MinYOffset;
	}

	inline float	CRTSCameraCtrller::GetMaxYOffset( void )
	{
		return m_MaxYOffset;
	}

	inline float	CRTSCameraCtrller::GetMaxNearPlan( void )	const
	{
		return m_MaxNearPlan;
	}

	inline float	CRTSCameraCtrller::GetMinNearPlan( void )	const
	{
		return m_MinNearPlan;
	}

	inline float	CRTSCameraCtrller::GetMaxYDegree( void )
	{
		return m_MaxYDegree;
	}

	inline float	CRTSCameraCtrller::GetYRotateDegree( void )
	{
		return m_fYDegree;
	}
}
