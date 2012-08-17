#pragma once
#include "IRenderTime.h"

#include "IDistortedTime.h"

namespace sqr
{
	class CRenderTime 
		: public IRenderTime
	{
	public:
		CRenderTime( IDistortedTime* pDtObj);
		~CRenderTime();

		uint32	GetCurTime(void);
	protected:
		IDistortedTime*	m_pDtObj;
	};
}