#pragma once
#include "CGraphic.h"
//------------------------------------------------------------------------------
/**
    @struct ClipPlane

	裁剪面.
	只是为了方便而把Plane封装了下, 免得拷贝好几份代码

    (C) 2009 ThreeOGCMan
*/
struct ClipPlane
{
	CPlane originPlane;
	CPlane transformedPlane;

	void SetTransform(const CMatrix& matrix);
	void RenderDebug() const;
};


//------------------------------------------------------------------------------
inline void
ClipPlane::SetTransform( const CMatrix& mat )
{
	if (!originPlane.IsValid())
		return;
	CMatrix transform = mat;
	transform.Invert();
	transform.Transpose();
	transformedPlane = originPlane;
	transformedPlane.Transform(transform);
}

//------------------------------------------------------------------------------
inline void
ClipPlane::RenderDebug() const
{
	if (!originPlane.IsValid())
		return;
	VerColor3D pos[4];
	const float PLANE_RANGE = 1000;
	pos[0].diffuse = pos[1].diffuse = pos[2].diffuse = pos[3].diffuse = 0x800000ff;
	float normalDir = max(abs(transformedPlane.m_Normal.x), abs(transformedPlane.m_Normal.y));
	normalDir = max(abs(transformedPlane.m_Normal.z), normalDir);
	if (normalDir == abs(transformedPlane.m_Normal.x))
	{
		//x
		pos[0].p = transformedPlane.Split(CVector3f(-1, -PLANE_RANGE, -PLANE_RANGE), CVector3f(1, -PLANE_RANGE, -PLANE_RANGE));
		pos[1].p = transformedPlane.Split(CVector3f(-1, -PLANE_RANGE, PLANE_RANGE), CVector3f(1, -PLANE_RANGE, PLANE_RANGE));
		pos[2].p = transformedPlane.Split(CVector3f(-1, PLANE_RANGE, -PLANE_RANGE), CVector3f(1, PLANE_RANGE, -PLANE_RANGE));
		pos[3].p = transformedPlane.Split(CVector3f(-1, PLANE_RANGE, PLANE_RANGE), CVector3f(1, PLANE_RANGE, PLANE_RANGE));
		CGraphic::GetInst()->DrawPrimitive(PT_TRIANGLESTRIP, pos, 4);
	}
	else if (normalDir == abs(transformedPlane.m_Normal.y))
	{
		//y
		pos[0].p = transformedPlane.Split(CVector3f(-PLANE_RANGE, -1, -PLANE_RANGE), CVector3f(-PLANE_RANGE, 1, -PLANE_RANGE));
		pos[1].p = transformedPlane.Split(CVector3f(-PLANE_RANGE, -1, PLANE_RANGE), CVector3f(-PLANE_RANGE, 1, PLANE_RANGE));
		pos[2].p = transformedPlane.Split(CVector3f(PLANE_RANGE, -1, -PLANE_RANGE), CVector3f(PLANE_RANGE, 1, -PLANE_RANGE));
		pos[3].p = transformedPlane.Split(CVector3f(PLANE_RANGE, -1, PLANE_RANGE), CVector3f(PLANE_RANGE, 1, PLANE_RANGE));
		CGraphic::GetInst()->DrawPrimitive(PT_TRIANGLESTRIP, pos, 4);
	}
	else
	{
		//z
		pos[0].p = transformedPlane.Split(CVector3f(-PLANE_RANGE, -PLANE_RANGE, -1), CVector3f(-PLANE_RANGE, -PLANE_RANGE, 1));
		pos[1].p = transformedPlane.Split(CVector3f(PLANE_RANGE, -PLANE_RANGE, -1), CVector3f(PLANE_RANGE, -PLANE_RANGE, 1));
		pos[2].p = transformedPlane.Split(CVector3f(-PLANE_RANGE, PLANE_RANGE, -1), CVector3f(-PLANE_RANGE, PLANE_RANGE, 1));
		pos[3].p = transformedPlane.Split(CVector3f(PLANE_RANGE, PLANE_RANGE, -1), CVector3f(PLANE_RANGE, PLANE_RANGE, 1));
		CGraphic::GetInst()->DrawPrimitive(PT_TRIANGLESTRIP, pos, 4);
	}
}