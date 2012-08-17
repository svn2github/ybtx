#include "CMapEditCamera.h"

namespace sqr
{
	inline void CMapEditCamera::MoveLeft()
	{
		CVector3f vecDir = -((CCamera*)m_pCtrlNode)->getRIGHT();
		MoveRTS(vecDir);
	}

	inline void CMapEditCamera::MoveRight()
	{
		CVector3f vecDir = ((CCamera*)m_pCtrlNode)->getRIGHT();
		MoveRTS(vecDir);
	}

	inline void CMapEditCamera::MoveFront()
	{
		CVector3f vecDir = ((CCamera*)m_pCtrlNode)->getDIRECTION();
		MoveRTS(vecDir);
	}

	inline void CMapEditCamera::MoveBack()
	{
		CVector3f vecDir = -((CCamera*)m_pCtrlNode)->getDIRECTION();
		MoveRTS(vecDir);
	}

	inline void CMapEditCamera::MoveToRTSByView(CVector3f v)
	{
		v = v - m_NewTarget;
		m_NewPosition += v;
		m_NewTarget	  += v;
	}
	
	inline bool CMapEditCamera::GetIsOrthogonal()
	{
		return m_bOrthogonal;
	}

	inline void CMapEditCamera::SwitchToStandard( const CVector3f& vTarget )
	{
		if( m_bYBTXCamera )
			m_fNewFov	 = CMath::ToRadian(YBTX_STDFOV);
		else
			m_fNewFov	 = CMath::ToRadian(STDFOV);

		m_NewTarget	 = vTarget;
		SetPitchAndYaw(STDPITCH,STDYAW);
	}

	inline void CMapEditCamera::IncreaseTheView()
	{
		m_fFarClip += 200.0f;
	}

	inline void CMapEditCamera::DecreaseTheView()
	{
		m_fFarClip -= 200.0f;
	}

	inline void CMapEditCamera::SetFarClip(const float fFarClip)
	{
		m_fFarClip = fFarClip;
	}

	inline float CMapEditCamera::GetFarClip() const
	{
		return m_fFarClip;
	}

	inline void CMapEditCamera::SwitchToMiniMapStandard(const CVector3f& vTarget, float fPitch)
	{
		m_fNewFov	 = CMath::ToRadian(STDFOV);
		m_NewTarget  = vTarget;
		SetPitchAndYaw(fPitch,STDYAW);
	}

	inline void CMapEditCamera::SwitchToTopViewMapStandard(const CVector3f& vTarget, float fPitch)
	{
		m_fNewFov	 = CMath::ToRadian(STDFOV);
		m_NewTarget  = vTarget;
		SetPitchAndYaw(fPitch, 0.0f );
	}

	inline void CMapEditCamera::SwitchToLeftMapStandard(const CVector3f& vTarget)
	{
		m_fNewFov		= CMath::ToRadian(STDFOV);
		m_NewPosition	= CVector3f(vTarget.x, -100, vTarget.y);
		m_NewTarget		= vTarget;
	}

	inline void CMapEditCamera::SwitchToFrontMapStandard(const CVector3f& vTarget)
	{
		m_fNewFov		= CMath::ToRadian(STDFOV);
		m_NewPosition	= CVector3f(STDMINIDISTANCE, vTarget.y, vTarget.z);
		m_NewTarget		= vTarget;
	}

	inline void CMapEditCamera::SwitchToSkyStandard()
	{
		m_fNewFov		= CMath::ToRadian(STDFOV);
		m_NewTarget		= CVector3f::ZERO;
		this->SetPitchAndYaw(0,0);
	}

	inline void CMapEditCamera::SwitchToLogicStandard(const CVector3f& vTarget)
	{
		m_fNewFov		= CMath::ToRadian(STDFOV);
		m_NewTarget		= vTarget;
		this->SetPitchAndYaw(STDPITCH,STDYAW,true);
	}

	inline void CMapEditCamera::SetGameDistanceRange( float max, float min )
	{
		//m_pImpl->m_fMaxDistance = max;
		//m_pImpl->m_fMinDistance = min;
	}

	inline void CMapEditCamera::SetPitch( float pitch )
	{
		pitch	 = CMath::ToRadian(pitch);
		m_fNewPitch = pitch - m_fNewPitch;
		RotatePosition(0.0f, m_fNewPitch);
	}

	inline void CMapEditCamera::SetYaw( float yaw )
	{
		yaw		= CMath::ToRadian(yaw);
		m_fNewYaw	= yaw - m_fNewYaw;
		RotatePosition(m_fNewYaw, 0.0f);
	}

	inline void CMapEditCamera::SetPatchWidth( float f )
	{
		m_fPatchWidth = f;
	}

	inline void CMapEditCamera::SetPatchHeight( float f )
	{
		m_fPatchHeight = f;
	}

	inline float CMapEditCamera::GetPatchWidth()
	{
		return m_fPatchWidth;
	}

	inline float CMapEditCamera::GetPatchHeight()
	{
		return m_fPatchHeight;
	}

	inline void CMapEditCamera::SetIsGameView( bool b )
	{
		m_bYBTXCamera = b;
	}
	inline CMapEditCameraNode* CMapEditCamera::GetEventPorcess() const
	{
		return EventProc;
	}
}