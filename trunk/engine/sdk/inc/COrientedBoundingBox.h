#ifndef _Oriented_Bounding_Box_
#define _Oriented_Bounding_Box_

#include "CPlane.h"
#include "CSphere.h"
#include "CQuaternion.h"
#include "CAxisAlignedBox.h"
#include "CMathMallocObject.h"


namespace sqr
{
	class CAxisAlignedBox;

	class COrientedBoundingBox : public CMathMallocObject
	{
	protected:
		/*ÓÐÏòºÐ×Ó*/
		CVector3f m_vCenter;
		CVector3f m_vExtent;
		CQuaternion	m_qRotation;
		bool m_bNull;
	public:
		COrientedBoundingBox();
		COrientedBoundingBox(CVector3f fCenter,CVector3f ExVector);
		~COrientedBoundingBox();

		void setNull(void);
		bool isNull(void) const;

		const CVector3f& getCenter(void);
		const CVector3f& getExtent(void);
		const CQuaternion getRotation(void);

		void SetCenter(const CVector3f& fCenter);
		void SetExtent(const CVector3f& fExtent);
		void SetRotation(const CQuaternion& fRot);

		void SetCenter(float x,float y,float z);
		void SetExtent(float x,float y,float z);

		void mergeBox( const COrientedBoundingBox& rhs );
		void mergeBox( const CAxisAlignedBox& rhs );
		void mergeBox( const CVector3f& point );
		void transformBox( const CMatrix& matrix);

		void move( float x, float y, float z );
		void move( const CVector3f& v3 );
		void rotate(const CVector3f& axis,float angle);
		void rotate(CQuaternion quat);
		void pitch( float alpha );
		void yaw( float beta );
		void roll( float gamma );
		void scaleBox(const CVector3f& s);

		bool intersects(const COrientedBoundingBox& b2) const;
		bool intersects(const CAxisAlignedBox& b2) const;
		bool intersects(const CSphere& s) const;
		bool intersects(const CPlane& p) const;
		bool intersects(const CVector3f& v) const;

		inline bool operator != (const COrientedBoundingBox& box) const;
		inline bool operator == (const COrientedBoundingBox& box) const;
	};

	inline bool COrientedBoundingBox::operator != (const COrientedBoundingBox& box) const
	{
		return m_vCenter!=box.m_vCenter||m_vExtent!=box.m_vExtent||m_qRotation!=box.m_qRotation;
	}

	inline bool COrientedBoundingBox::operator == (const COrientedBoundingBox& box) const
	{
		return m_vCenter==box.m_vCenter&&m_vExtent==box.m_vExtent&&m_qRotation==box.m_qRotation;
	}
}

#endif

