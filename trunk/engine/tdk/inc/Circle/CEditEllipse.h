#pragma once
#include "CVector3.h"

namespace sqr
{
	class CEditEllipse
	{
	protected:
		CVector3f	m_vCenter;
		float		m_fRadiusX;
		float		m_fRadiusY;
		size_t		m_fSlices;
		DWORD		m_dwColor;
		
		vector<CVector3f> m_spherePoints;

	private:
		void		CreateEllipsePoints();
	public:
		CEditEllipse();
		~CEditEllipse();

		///operation
		void		SetEllipseCenter(const CVector3f& center);
		void		SetEllipseRadiusX(const float  radius);
		void		SetEllipseRadiusY(const float  radius);
		void		SetEllipseSlices(const size_t slices);
		void		SetEllipseColor(const DWORD color);

		void		Render() const;

	};
}