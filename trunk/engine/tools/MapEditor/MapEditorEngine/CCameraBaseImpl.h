#pragma once


namespace sqr
{
	class CCameraBaseImpl
	{
		friend class CCameraBase;

		CCameraManager		*pManager;
		float				 m_fNearClip;
		float				 m_fFarClip;
		float				 m_fNewFov;
		float				 m_fPitch;
		float				 m_fYaw;
		float				 m_fMovSpeed;
		CVector3f			 m_NewTarget;
		CVector3f			 m_NewPosition;

	private:
		CCameraBaseImpl& operator = ( const CCameraBaseImpl &);
		CCameraBaseImpl( const CCameraBaseImpl & );
		
	private:
		CCameraBaseImpl( CCameraManager *pManagerIn );
	};
}