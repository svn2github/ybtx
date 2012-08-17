#pragma once
#include "CRefObject.h"
#include "CAxisAlignedBox.h"
/**
	@class CShadowMatrixBuilder

	阴影矩阵生成算法.

	派生子类来生成对应算法
 */

namespace sqr
{

class CCamera;
class CShadowClipper;
SmartPointer(CShadowMatrixBuilder);
class CShadowMatrixBuilder : public CRefObject
{
public:
	CShadowMatrixBuilder();
	virtual ~CShadowMatrixBuilder();

	/// 设置场景摄像机
	void SetCamera(CCamera* camera);
	/// 取得场景摄像机
	CCamera* GetCamera() const;
	/// 设置光源方向
	void SetLightDir(const CVector3f& dir);
	/// 设置shadow纹理大小
	void SetShadowMapSize(uint size);
	/// 取得纹理大小
	uint GetShadowMapSize() const;

	/// 取得light空间视图矩阵
	const CMatrix& GetViewMatrix() const;
	/// 取得light空间投影矩阵
	const CMatrix& GetProjectMatrix() const;
	/// 取得阴影纹理坐标生成矩阵
	const CMatrix& GetShadowMatrix() const;

	/// 计算矩阵信息
	virtual void BuildMatrix(CShadowClipper* clipper) = 0;

	//计算扩展矩阵
	virtual float BuildMatrixEx( CShadowClipper* clipper)
	{
		return 0;
	};

protected:
	CCamera* m_Camera;
	CVector3f m_LightDir;
	uint m_ShadowMapSize;

	CMatrix m_ViewMatrix;
	CMatrix m_ProjMatrix;
	CMatrix m_ShadowMatrix;
};

/// 简单的平行投影.
class COrthoMatrixBuilder : public CShadowMatrixBuilder
{
public:
	/// 计算矩阵信息
	void BuildMatrix(CShadowClipper* clipper);
	float BuildMatrixEx( CShadowClipper* clipper);
};

/// Trapezoidal Shadow Map.
class CTsmMatrixBuilder : public CShadowMatrixBuilder
{
public:
	CTsmMatrixBuilder();
	/// 设置焦点位置(0~1)
	void SetFocusRegion(float focus);
	/// 计算矩阵信息
	void BuildMatrix(CShadowClipper* clipper);

private:
	float m_FocusRegion;
};


//------------------------------------------------------------------------------
inline void
CShadowMatrixBuilder::SetCamera( CCamera* cam )
{
	m_Camera = cam;
}

//------------------------------------------------------------------------------
inline void
CShadowMatrixBuilder::SetLightDir( const CVector3f& dir )
{
	m_LightDir = -dir;
}

//------------------------------------------------------------------------------
inline void
CShadowMatrixBuilder::SetShadowMapSize( uint size )
{
	m_ShadowMapSize = size;
}

//------------------------------------------------------------------------------
inline  CCamera*
CShadowMatrixBuilder::GetCamera() const
{
	return m_Camera;
}

//------------------------------------------------------------------------------
inline uint
CShadowMatrixBuilder::GetShadowMapSize() const
{
	return m_ShadowMapSize;
}

//------------------------------------------------------------------------------
inline const CMatrix&
CShadowMatrixBuilder::GetViewMatrix() const
{
	return m_ViewMatrix;
}

//------------------------------------------------------------------------------
inline const CMatrix&
CShadowMatrixBuilder::GetProjectMatrix() const
{
	return m_ProjMatrix;
}

//------------------------------------------------------------------------------
inline const CMatrix&
CShadowMatrixBuilder::GetShadowMatrix() const
{
	return m_ShadowMatrix;
}

//------------------------------------------------------------------------------
inline void
CTsmMatrixBuilder::SetFocusRegion( float focus )
{
	m_FocusRegion = focus;
}
}// namespace sqr

