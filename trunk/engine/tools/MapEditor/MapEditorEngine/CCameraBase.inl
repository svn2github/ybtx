#pragma once
#include "CCameraBase.h"

namespace sqr
{
	inline void CCameraBase::SetCamType(const ECamType& type)
	{
		m_camType=type;
	}

	inline ECamType CCameraBase::GetCamType() const
	{
		return m_camType;
	}

	inline void CCameraBase::SetNewPosition( const CVector3f& vecPos )
	{
		m_NewPosition = vecPos;
	}

	inline void CCameraBase::SetNewTarget( const CVector3f& vecTarget )
	{
		m_NewTarget = vecTarget;
	}

	inline void CCameraBase::SetPosition( const CVector3f& vPos )
	{
		if(m_pCtrlNode)
		{
			m_NewPosition = vPos;
			m_pCtrlNode->SetPosition(vPos);
		}
	}

	inline void CCameraBase::SetTarget( const CVector3f& vTarget )
	{
		if(m_pCtrlNode)
		{
			m_NewTarget = vTarget; 
			m_TargetPos = vTarget;
			(dynamic_cast<CCamera*>(m_pCtrlNode))->lookAt(vTarget);
		}
	}

	inline CVector3f CCameraBase::GetNewPosition()		const	
	{ 
		return m_NewPosition; 
	}

	inline CVector3f CCameraBase::GetNewTarget()		const
	{ 
		return m_NewTarget; 
	}

	inline CVector3f CCameraBase::GetPosition()			const
	{
		if(m_pCtrlNode)
			return m_pCtrlNode->GetPosition();

		return CVector3f(0,0,0);
	}

	inline CVector3f CCameraBase::GetTarget()			const
	{
		return m_TargetPos;
	}

	inline float CCameraBase::GetPitch()				const
	{
		return CMath::ToDegree(m_fNewPitch);
	}
	
	inline float CCameraBase::GetYaw()					const
	{
		return CMath::ToDegree(m_fNewYaw);
	}

	inline float CCameraBase::GetDistance()				const
	{
		return (m_NewTarget - m_NewPosition).Mag();
	}

	inline void	CCameraBase::SetFovY( float fFOVY )
	{
		m_fNewFov = CMath::ToRadian(fFOVY);
	}

	inline float	CCameraBase::GetFovY()				const
	{
		return CMath::ToDegree(m_fNewFov);
	}

	inline void CCameraBase::IncreaseTheFOV()
	{
		m_fNewFov += CMath::ToRadian(5);
		if ( m_fNewFov > 3.14f )
		{
			m_fNewFov = 3.14f;
		}
	}

	inline void CCameraBase::DecreaseTheFOV()
	{
		m_fNewFov -= CMath::ToRadian(5);
		if ( m_fNewFov < 0.1f )
		{
			m_fNewFov = 0.1f;
		}
	}

	inline void CCameraBase::SetMovSpeed( float fSpeed )
	{
		m_fMovSpeed = fSpeed;		
	}

	inline float CCameraBase::GetMovSpeed() const
	{
		return m_fMovSpeed;
	}

	inline bool CCameraBase::IsValid() const
	{
		if( m_pCtrlNode == NULL )
			return false;
		else
			return true;
	}
}