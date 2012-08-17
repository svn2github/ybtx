#include "stdafx.h"
#include "CCameraBaseImpl.h"


namespace sqr
{
	CCameraBaseImpl::CCameraBaseImpl( CCameraManager *pManagerIn )
		: pManager(pManagerIn)
		, m_NewTarget(0.0f, 0.0f, 1.0f)
		, m_NewPosition(0,0,0)
		, m_fFarClip(STDFARPLANE)
		, m_fNearClip(STDNEARPLANE)
		, m_fNewFov(0)
		, m_fPitch(0.0f)
		, m_fYaw(0.0f)
		, m_fMovSpeed(64.0f)
	{
	}
}