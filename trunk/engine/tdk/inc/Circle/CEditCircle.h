#pragma once
#include "CVector3.h"

namespace sqr
{
	class CEditCircle
	{
	protected:
		CVector3f	m_vCenter;
		float		m_fRadius;
		size_t		m_fSlices;
		DWORD		m_dwColor;
		
		vector<CVector3f> m_spherePoints;

	private:
		void		CreateSpherePoints();
	public:
		CEditCircle();
		~CEditCircle();

		///operation
		void		SetCircleCenter(const CVector3f& center);
		void		SetCircleRadius(const float  radius);
		void		SetCircleSlices(const size_t slices);
		void		SetSphereColor(const DWORD color);

		void		Render() const;

	};
}