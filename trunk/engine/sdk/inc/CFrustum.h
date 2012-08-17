#pragma once
//------------------------------------------------------------------------------
/**
    @class CFrustum

	摄像机的视锥体.

    (C) 2009 ThreeOGCMan
*/
#include "CAxisAlignedBox.h"
#include "ClipStatus.h"
#include "CGraphicMallocObject.h"
namespace sqr
{

class CFrustum : public CGraphicMallocObject
{
public:
	/// 视锥体的6个面
	enum EFrustumPlane
	{
		FP_Left,
		FP_Right,
		FP_Bottom,
		FP_Top,
		FP_Near,
		FP_Far,

		FrustumPlaneNum
	};

	enum
	{
		FrustumCornerNum = 8
	};

public:
	CFrustum();

	CFrustum(const CMatrix& viewProj);

	ClipStatus::Type Cull(const CAxisAlignedBox& box);
	ClipStatus::Type Cull(const CSphere& sphere);
	ClipStatus::Type Cull(const CVector3f& point);

	bool	IsVisible( const CAxisAlignedBox& box );
	bool	IsVisible( const CSphere& sphere );
	bool	IsVisible( const CVector3f& point );

	const CPlane& GetPlane(EFrustumPlane index);
	const CVector3f& GetCorner(index_t index);

private:
	void Update(const CMatrix& viewProj);
	static bool PlaneIntersection( CVector3f* intersectPt, const CPlane* p0,
	                               const CPlane* p1, const CPlane* p2 );
private:
	friend class CCamera;

	float fovY;
	float zNear;
	float zFar;
	float aspect;
	float orthoWidth;
	float orthoHeight;
	bool isOrtho;

	CPlane m_CamPlanes[FrustumPlaneNum];
	CVector3f m_PntList[FrustumCornerNum];
};

//------------------------------------------------------------------------------
inline const CPlane&
CFrustum::GetPlane( CFrustum::EFrustumPlane index )
{
	return m_CamPlanes[index];
}

//------------------------------------------------------------------------------
inline const CVector3f&
CFrustum::GetCorner( index_t index )
{
	return m_PntList[index];
}

}//namespace sqr
