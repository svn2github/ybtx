#pragma once

#define Max_Far_Dist 20000.0f
#include "CCtrlNode.h"
#include "CFrustum.h"

namespace sqr
{
class CCamera :	public CCtrlNode
{
public:
	static const DWORD st_CameraType;

	CCamera(void);
	virtual ~CCamera(void);

	virtual void						GetBillboardNoneX( CMatrix& Billboard ) const;
	virtual void						GetBillboardAll( CMatrix& Billboard )	const;
	virtual void						GetCameraInvert( CMatrix& ViewInvert )	const;
public:
	//设置Y方向上的视野
	void setFOVy(float fov);
	//获取当前视野
	float getFOVy(void);

	//设置摄像机近端距离
	void setNearDist(float nd);
	//获得摄像机近端距离
	float getNearDist(void)						const;

	//设置摄像机远端距离
	void setFarDist(float fd);
	//获得摄像机远端距离
	float getFarDist(void)						const;

	//设置显示比例
	void setAspectRatio(float ratio);
	//获取当前显示比例
	float getAspectRatio(void)					const;

	const CMatrix& getViewProjectMatrix(void)	const;
	const CMatrix& getProjectionMatrix(void)	const;
	const CMatrix& getViewMatrix(void)			const;

	/// 平行投影
	bool IsOrthoProject() const;
	void SetOrthoProject(bool b);
	void SetOrthoSize(float width,float height);
	void SetDeltaViewMatrix(CMatrix& matDView);
	const CFrustum& GetFrustum() const;
	DWORD GetNodeType()	{ return CCamera::st_CameraType; };
public:

	//设置观察点
	void lookAt( const CVector3f& fPoint );
	void lookAt( float x, float y, float z );

	//设置观察方向
	void lookTo( const CVector3f& fVector );

	ClipStatus::Type cull( const CAxisAlignedBox& box ) const;
	ClipStatus::Type cull( const CSphere& sph )			const;
	ClipStatus::Type cull( const CVector3f& vec )		const;

	bool	IsVisible( const CAxisAlignedBox& box )		const;
	bool	IsVisible( const CSphere& sphere )			const;
	bool	IsVisible( const CVector3f& point )			const;

	void setFixedYawAxis( bool useFixed,const CVector3f& fixedYaw );
	/*void updateView(void);*/
protected:
	//更新矩阵
	void _UpdateEx(void)		const;
	void updateFrustum(void)	const;
protected:

	//顺序不能变
	mutable	CMatrix			m_matProj;
	mutable	CMatrix			m_matView;
	mutable	CMatrix			m_matViewProject;

	mutable	CFrustum		m_Frustum;
	mutable	float			m_rAspectScr;
	mutable	float			m_rCos[2];
	mutable	float			m_rSin[2];

	//投影矩阵是否被改变
	mutable	bool			m_bRecalcFrustum;

	//摄像机Y方向
	CVector3f			m_vYawFixed;
	//Y方向是否固定
	bool				m_bYawFixed;
	mutable	CMatrix		m_matDeltaView;
	mutable	CMatrix		m_matBillboardNoneX;
	mutable	CMatrix		m_matBillboardAll;
};
//------------------------------------------------------------------------------
inline bool
CCamera::IsOrthoProject() const
{
	return m_Frustum.isOrtho;
}

//------------------------------------------------------------------------------
inline void
CCamera::SetOrthoProject( bool b )
{
	if (m_Frustum.isOrtho != b)
	{
		m_Frustum.isOrtho = b;
		m_bRecalcFrustum = true;
		m_bNeedUpdate = true;
	}
}

inline void
CCamera::SetOrthoSize( float width, float height )
{
	m_bNeedUpdate		= true;
	m_bRecalcFrustum	= true;
	m_Frustum.orthoWidth	= width;
	m_Frustum.orthoHeight	= height;
}

inline void CCamera::setFOVy(float fov)
{
	if (m_Frustum.fovY != fov)
	{
		m_Frustum.fovY = fov;
		m_bRecalcFrustum = true;
		m_bNeedUpdate = true;
	}
}

inline float CCamera::getFOVy(void)
{
	return m_Frustum.fovY;
}

inline void CCamera::setNearDist(float nd)
{
	if (m_Frustum.zNear != nd)
	{
		m_Frustum.zNear = nd;
		m_bRecalcFrustum = true;
		m_bNeedUpdate = true;
	}
}

inline float CCamera::getNearDist(void) const
{
	return m_Frustum.zNear;
}

inline void CCamera::setFarDist(float fd)
{
	if (m_Frustum.zFar != fd)
	{
		m_Frustum.zFar = fd;
		m_bRecalcFrustum = true;
		m_bNeedUpdate = true;
	}
}

inline float CCamera::getFarDist(void)  const
{
	return m_Frustum.zFar;
}

inline void CCamera::setAspectRatio(float ratio)
{
	if (m_Frustum.aspect != ratio)
	{
		m_Frustum.aspect = ratio;
		m_bRecalcFrustum = true;
		m_bNeedUpdate = true;
	}
}

inline float CCamera::getAspectRatio(void) const
{
	return m_Frustum.aspect;
}

inline const CMatrix& CCamera::getViewProjectMatrix(void) const
{
	Update();
	return m_matViewProject;
}

inline const CMatrix& CCamera::getProjectionMatrix(void) const
{
	updateFrustum();
	return m_matProj;
}
inline const CMatrix& CCamera::getViewMatrix(void) const
{
	Update();
	return m_matView;
}

inline ClipStatus::Type CCamera::cull(const CVector3f& vec) const
{
	//updateView();
	return m_Frustum.Cull(vec);
}

inline ClipStatus::Type CCamera::cull(const CSphere& sph) const
{
	//updateView();
	return m_Frustum.Cull(sph);
}

inline ClipStatus::Type CCamera::cull(const CAxisAlignedBox& box) const
{
	//updateView();
	if (box.isNull())
		return ClipStatus::Outside;
	return m_Frustum.Cull(box);
}

inline bool CCamera::IsVisible( const CAxisAlignedBox& box ) const
{
	return m_Frustum.IsVisible(box);
}

inline bool CCamera::IsVisible( const CSphere& sphere ) const
{
	return m_Frustum.IsVisible(sphere);
}

inline bool CCamera::IsVisible( const CVector3f& point ) const
{
	return m_Frustum.IsVisible(point);
}

inline void CCamera::lookAt(float x, float y, float z)
{
	lookAt(CVector3f(x,y,z));
}

inline void CCamera::GetBillboardNoneX( CMatrix& Billboard ) const
{
	Update();
	Billboard = m_matBillboardNoneX;
}

inline void CCamera::GetBillboardAll( CMatrix& Billboard ) const
{
	Update();
	Billboard = m_matBillboardAll;
}

inline void CCamera::GetCameraInvert( CMatrix& ViewInvert ) const
{
	Update();
	ViewInvert = m_matCached;
}

//------------------------------------------------------------------------------
inline const CFrustum&
CCamera::GetFrustum() const
{
	return m_Frustum;
}

inline void CCamera::SetDeltaViewMatrix(CMatrix& matDView)
{
	m_matDeltaView = matDView;
}

}

