#pragma once
#include "CVector3.h"

namespace sqr
{
	class CEditLine
	{
	protected:
		CVector3f	m_vStart;
		CVector3f	m_vEnd;
		DWORD		m_dwColor;
		int			m_nWeight;

	public:
		CEditLine();
		~CEditLine();

		///operation
		void		SetLineStartPoint(const CVector3f& start);
		void		SetLineEndPoint(const CVector3f& end);
		CVector3f	GetLineStartPoint() const;
		CVector3f	GetLineEndPoint() const;

		void		SetLineColor(const DWORD color);
		void		SetLineWeight(const int weight);

		void		Render() const;
	};
}