#include "stdafx.h"
#include "CCameraBase.h"
#include "CCameraManager.h"
#include "BaseHelper.h"
#include "MEGraphic.h"

namespace sqr
{
	CCameraBase::CCameraBase(CCameraManager* camMgr)
		: m_pCamMgr(camMgr)
		, m_NewTarget(0.0f, 0.0f, 1.0f)
		, m_NewPosition(0,0,0)
		, m_fFarClip(10000.0f)
		, m_fNearClip(STDNEARPLANE)
		, m_fNewFov(0)
		, m_fNewPitch(0.0f)
		, m_fNewYaw(0.0f)
		, m_fMovSpeed(64.0f)
		, m_bInitBind(false)
		, m_camType(eCT_None)
	{
	}

	void CCameraBase::_InitBind()
	{
		m_pCamMgr->SetActiveCamera(this);
		m_bInitBind = true;
	}

	CCameraBase::~CCameraBase()
	{
	}

	CVector3f CCameraBase::GetDirection() const 
	{
		CVector3f dir = GetTarget() - GetPosition();
		dir.normalize();
		return dir;
	}
}