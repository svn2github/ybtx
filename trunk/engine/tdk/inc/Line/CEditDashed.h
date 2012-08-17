#pragma once
#include "CVector3.h"
#include "Line/CEditLine.h"

namespace sqr
{
	class CEditDashed : public CEditLine
	{
	protected:
		float		m_fDashedStep;///dashed interval
	public:
		CEditDashed();
		~CEditDashed();

		///operation
		void		SetDashedStep(const float fStep);

		void		Render() const;
	};
}